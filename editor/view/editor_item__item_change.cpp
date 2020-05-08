//
//      Created by Stephens Nunnally on 3/9/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Handles ItemChange event of custom EditorItem QGraphicsPixmapItem
//
//

/* ---------- Following changes Post Event when GraphicsItemFlag::ItemSendsGeometryChanges is set to true

    ItemPositionChange          Position is about to change         Value is the new position, PointF               Return a different PointF to change it
    ItemPositionHasChanged      Position has changed                                                                Return does nothing

    ItemTransformChange         Transform is about to change        Value is the new transform, QTransform          Return a different QTransform to change it
    ItemTransformHasChanged     Transform has changed                                                               Return does nothing

    ----------

    ItemEnabledChange           Enabled state is about to change    Value is the new state, bool                    Return a different bool to stop it
    ItemEnabledHasChanged       Enabled state has changed                                                           Return does nothing

    ItemSelectedChange          Selected state is about to change   Value is if selected or not, bool               Return a different bool to stop it
    ItemSelectedHasChanged      Selected state has changed                                                          Return does nothing

    ItemVisibleChange           Visible state about to change       Value is Visible or not, bool                   Return a different bool to stop it
    ItemVisibleHasChanged       Visible state has changed                                                           Return does nothing

    ItemParentChange            Parent is about to change           Value is new parent, QGraphicsItem*             Return a different pointer to change it
    ItemParentHasChanged        Parent has changed                                                                  Return does nothing

    ItemChildAddedChange        Child is added to this item         Value is new child, QGraphicsItem*              Return value is unused
    ItemChildRemovedChange      Child is removed from this item     Value is child to be removed, QGraphicsItem*    Return value is unused

    ItemSceneChange             Parent scene is about to change     Value is the new scene, QGraphicsScene*
    ItemSceneHasChanged

    ItemFlagsChange             quint_32
    ItemFlagsHaveChanged

    ItemZValueChange            double
    ItemZValueHasChanged

    ItemOpacityChange           double
    ItemOpacityHasChanged

    ItemScenePositionHasChanged

---------- */
#include <QDebug>

#include "core/colors/colors.h"
#include "editor/helper_library.h"
#include "editor/interface_editor_relay.h"
#include "editor/preferences.h"
#include "editor/view/editor_item.h"
#include "editor/view/editor_scene.h"
#include "editor/view/editor_view.h"
#include "engine/debug_flags.h"
#include "project/enums_entity_types.h"
#include "project/entities/dr_stage.h"
#include "project/entities/dr_thing.h"


//####################################################################################
//##    Item Change Event - Allows for auto updating on property changes
//####################################################################################
QVariant EditorItem::itemChange(GraphicsItemChange change, const QVariant &value) {
    // If this is a temporary item, or not attached to a scene, do not process change
    if (m_temp_only || !m_editor_relay)         return QGraphicsPixmapItem::itemChange(change, value);

    // If havent implicitly turned on changes, do not process
    if (m_item_change_flags_enabled == false)   return QGraphicsPixmapItem::itemChange(change, value);

    // Load any new data stored in item
    double  angle = data(User_Roles::Rotation).toDouble();
    QPointF scale = data(User_Roles::Scale).toPointF();


    // ********** Intercepts item position change and limits new location if Snap to Grid is on
    if (change == ItemPositionChange) {
        // If not coming from an interactive view mode, return event position
        QPointF new_pos = value.toPointF();
        if (m_editor_relay->currentViewMode() != View_Mode::Translating &&
            m_editor_relay->currentViewMode() != View_Mode::Holding_Keys) {
            return new_pos;
        }

        // Not snapping to grid? Go ahead and return event position
        if (Dr::GetPreference(Preferences::World_Editor_Snap_To_Grid).toBool() == false) return new_pos;

        // ***** Calculate new center based on SelectionBox starting center and difference between starting scenePos() and new passed in new_pos
        if (Dr::GetPreference(Preferences::World_Editor_Snap_To_Center_Of_Selection_Box).toBool() == true) {
            EditorScene *editor_scene = dynamic_cast<EditorScene*>(this->scene());
            QPointF new_select_center, rounded_select_center;
            QPointF adjust_by;

            if (editor_scene->getHasCalculatedAdjustment() == false) {
                new_select_center = editor_scene->getPreMoveSelectionCenter() - (scenePos() - new_pos);
                rounded_select_center = m_editor_relay->roundPointToGrid( new_select_center );
                adjust_by = new_select_center - rounded_select_center;
                editor_scene->setMoveAdjustment( adjust_by );
                editor_scene->setHasCalculatedAdjustment(true);
            } else {
                adjust_by = editor_scene->getMoveAdjustment();
            }
            QPointF adjusted_pos = new_pos - adjust_by;
            return  adjusted_pos;

        // ***** Calculate new center location based on starting center of item and difference between starting scenePos() and new passed in new_pos
        } else {
            QPointF old_center = scenePos();
            if (m_thing != nullptr) {
                old_center = Dr::ToQPointF(m_thing->getComponentPropertyValue(Comps::Thing_Transform, Props::Thing_Position).toPointF());
            }
            QPointF new_center = old_center - (scenePos() - new_pos);

            // Align new desired center to grid
            QPointF rounded_center = m_editor_relay->roundPointToGrid( new_center );

            // Adjust new position based on adjustment to grid we just performed
            QPointF adjust_by = new_center - rounded_center;
            QPointF adjusted_pos = new_pos - adjust_by;
            return  adjusted_pos;
        }
    }

    // ********** If item position has changed, update it
    if (change == ItemScenePositionHasChanged) {
        // Value is new scene position (of upper left corner)
        QPointF new_pos =    value.toPointF();

        // Following works with ItemPositionHasChanged, but we like using ItemScenePositionHasChanged better, works better for some reason
        ///// Create a transform so we can find new center position of item
        ///QTransform t = QTransform().rotate(angle).scale(scale.x(), scale.y());
        ///QPointF new_center = t.map( boundingRect().center() ) + new_pos;

        QPointF new_center = mapToScene( boundingRect().center() );
        m_thing->setComponentPropertyValue(Comps::Thing_Transform, Props::Thing_Position, DrPointF(new_center.x(), new_center.y()));

        m_editor_relay->updateEditorWidgetsAfterItemChange(Editor_Widgets::Stage_View, { m_thing }, { std::make_pair(Comps::Thing_Transform, Props::Thing_Position) });

        return new_pos;
    }

    // ********** If item transform has changed, update it
    if (change == ItemTransformHasChanged) {
        // Value is new item QTransform
        QTransform new_transform = value.value<QTransform>();

        double size_x = m_asset_width *  (scale.x());
        double size_y = m_asset_height * (scale.y());

        m_thing->setComponentPropertyValue(Comps::Thing_Transform, Props::Thing_Rotation, angle);
        m_thing->setComponentPropertyValue(Comps::Thing_Transform, Props::Thing_Scale, DrPointF(scale.x(), scale.y()) );
        m_thing->setComponentPropertyValue(Comps::Thing_Transform, Props::Thing_Size, DrPointF(size_x, size_y));

        m_editor_relay->updateEditorWidgetsAfterItemChange(Editor_Widgets::Stage_View, { m_thing },
                        {   std::make_pair(Comps::Thing_Transform, Props::Thing_Size),
                            std::make_pair(Comps::Thing_Transform, Props::Thing_Scale),
                            std::make_pair(Comps::Thing_Transform, Props::Thing_Rotation) });
        return new_transform;
    }

    // ********** If item z value has changed, update it
    if (change == ItemZValueHasChanged) {
        // Value is new double z value
        double new_z = value.toDouble();
        m_thing->setZOrderWithSub(new_z, Z_Insert::Front);

        m_editor_relay->updateEditorWidgetsAfterItemChange(Editor_Widgets::Stage_View, { m_thing }, { std::make_pair(Comps::Thing_Layering, Props::Thing_Z_Order) });
        return new_z;
    }

    return QGraphicsPixmapItem::itemChange(change, value);
}












