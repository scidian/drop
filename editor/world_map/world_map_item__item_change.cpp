//
//      Created by Stephens Nunnally on 5/7/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QDebug>
#include <QGraphicsItem>

#include "editor/interface_editor_relay.h"
#include "editor/preferences.h"
#include "editor/world_map/world_map_item.h"
#include "editor/world_map/world_map_scene.h"
#include "editor/world_map/world_map_view.h"
#include "project/settings/settings.h"


//####################################################################################
//##    Item Change Event - Allows for auto updating on property changes
//####################################################################################
QVariant WorldMapItem::itemChange(GraphicsItemChange change, const QVariant &value) {

    // If havent implicitly turned on changes, do not process
    if (m_item_change_flags_enabled == false)   return QGraphicsPixmapItem::itemChange(change, value);

    // Load any new data stored in item
    double  angle = 0.0;
    QPointF scale = QPointF(1.0, 1.0);


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

        // ***** Calculate new center location based on starting center of item and difference between starting pos() and new passed in new_pos
        QPointF old_center = QPointF(0, 0);
        QPointF new_center = old_center - (pos() - new_pos);

        // Align new desired center to grid
        QPointF rounded_center = m_editor_relay->roundPointToGrid( new_center );

        // Adjust new position based on adjustment to grid we just performed
        QPointF adjust_by = new_center - rounded_center;
        QPointF adjusted_pos = new_pos - adjust_by;
        return  adjusted_pos;
    }

    // ********** If item position has changed, update it
    if (change == ItemScenePositionHasChanged) {
        // Value is new scene position (of upper left corner)
        QPointF new_pos =    value.toPointF();

        QPointF new_center = mapToScene( boundingRect().center() );
        m_entity->setComponentPropertyValue(Comps::Thing_Transform, Props::Thing_Position, DrPointF(new_center.x(), new_center.y()), false);

        m_editor_relay->updateEditorWidgetsAfterItemChange(Editor_Widgets::Map_View, { m_entity }, { std::make_pair(Comps::Thing_Transform, Props::Thing_Position) });
        return new_pos;
    }

    // ********** If item transform has changed, update it
    if (change == ItemTransformHasChanged) {
        // Value is new item QTransform
        QTransform new_transform = value.value<QTransform>();

        double size_x = m_width  * (scale.x());
        double size_y = m_height * (scale.y());

        m_entity->setComponentPropertyValue(Comps::Thing_Transform, Props::Thing_Rotation, angle, false);
        m_entity->setComponentPropertyValue(Comps::Thing_Transform, Props::Thing_Scale, DrPointF(scale.x(), scale.y()), false);
        m_entity->setComponentPropertyValue(Comps::Thing_Transform, Props::Thing_Size, DrPointF(size_x, size_y), false);

        m_editor_relay->updateEditorWidgetsAfterItemChange(Editor_Widgets::Map_View, { m_entity },
                        {   std::make_pair(Comps::Thing_Transform, Props::Thing_Size),
                            std::make_pair(Comps::Thing_Transform, Props::Thing_Scale),
                            std::make_pair(Comps::Thing_Transform, Props::Thing_Rotation) });
        return new_transform;
    }

    // ********** If item z value has changed, update it
    if (change == ItemZValueHasChanged) {
        // Value is new double z value
        double new_z = value.toDouble();
///        m_entity->setZOrderWithSub(new_z, Z_Insert::Front);

        m_editor_relay->updateEditorWidgetsAfterItemChange(Editor_Widgets::Map_View, { m_entity }, { std::make_pair(Comps::Thing_Layering, Props::Thing_Z_Order) });
        return new_z;
    }

    return QGraphicsPixmapItem::itemChange(change, value);

}















