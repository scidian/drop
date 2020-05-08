//
//      Created by Stephens Nunnally on 5/7/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QDebug>
#include <QGraphicsItem>

#include "core/dr_math.h"
#include "editor/helper_library.h"
#include "editor/interface_editor_relay.h"
#include "editor/preferences.h"
#include "editor/world_map/world_map_item.h"
#include "editor/world_map/world_map_scene.h"
#include "editor/world_map/world_map_view.h"
#include "project/entities/dr_node.h"
#include "project/settings/settings.h"


//####################################################################################
//##    Item Change Event - Allows for auto updating on property changes
//####################################################################################
QVariant WorldMapItem::itemChange(GraphicsItemChange change, const QVariant &value) {

    // If havent implicitly turned on changes, do not process
    if (m_item_change_flags_enabled == false)   return QGraphicsPixmapItem::itemChange(change, value);

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

        // Round new position to grid
        QPointF rounded_center = m_editor_relay->roundPointToGrid(new_pos);

        // Adjust new position based on rounding we just performed
        QPointF adjust_by =     new_pos - rounded_center;
        QPointF adjusted_pos =  new_pos - adjust_by;
        ///qDebug() << "Last Pos: " << lastPosition() << ", New Pos: " << new_pos << ", Rounded: " << rounded_center << ", Adjusted: " << adjusted_pos;
        return  adjusted_pos;
    }


    // ********** If item position has changed, update it
    if (change == ItemScenePositionHasChanged) {
        // Value is new scene position (of upper left corner)
        QPointF new_pos = value.toPointF();
        DrNode* node = dynamic_cast<DrNode*>(m_entity);
        if (node != nullptr) {
            QPointF new_center = mapToScene( boundingRect().center() );
            node->setNodePosition( Dr::FromQPointF(new_center) );
        }
        return new_pos;
    }

    // ********** If item has been selected, raise to top of z order
    if (change == ItemSelectedHasChanged) {
        // Value is new scene position (of upper left corner)
        bool new_is_selected = value.toBool();

        // Set selected item to top of z order
        if (new_is_selected) {
            setZValue(1);

        // Return non selected item back to 0 with other nodes, ensure it stays on top of those around it
        } else {
            setZValue(0);
            for (auto item : this->collidingItems(Qt::IntersectsItemBoundingRect)) { item->stackBefore(this); }
        }
        return new_is_selected;
    }

    // ********** If item transform has changed, update it
    if (change == ItemTransformHasChanged) {
        // Value is new item QTransform
        QTransform new_transform = value.value<QTransform>();

        return new_transform;
    }

    // ********** If item z value has changed, update it
    if (change == ItemZValueHasChanged) {
        // Value is new double z value
        double new_z = value.toDouble();

        return new_z;
    }

    return QGraphicsPixmapItem::itemChange(change, value);

}















