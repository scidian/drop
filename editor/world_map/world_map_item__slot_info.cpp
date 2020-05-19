//
//      Created by Stephens Nunnally on 5/19/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QDebug>
#include <QWidget>

#include "core/colors/colors.h"
#include "core/dr_debug.h"
#include "core/dr_math.h"
#include "editor/helper_library.h"
#include "editor/interface_editor_relay.h"
#include "editor/pixmap/pixmap.h"
#include "editor/preferences.h"
#include "editor/world_map/world_map_item.h"
#include "editor/world_map/world_map_view.h"
#include "engine/debug_flags.h"
#include "project/constants_component_info.h"
#include "project/dr_project.h"
#include "project/settings/settings.h"
#include "project/settings/settings_component.h"
#include "project/settings/settings_component_slot.h"


//####################################################################################
//##    Returns Slot Circle Rect for a particular slot, slot_number should start at 0
//####################################################################################
QRectF WorldMapItem::slotRect(DrSlotType slot_type, int slot_number) {
    int left = 0;
    int top  = (boundingRect().top() + c_node_buffer) + (c_row_height * (1.5 + static_cast<double>(slot_number)));
        top -= (c_slot_size/2);
    switch (slot_type) {
        case DrSlotType::Signal:    left = (boundingRect().top()   + c_node_buffer) - (c_slot_size*0.5);    break;
        case DrSlotType::Output:    left = (boundingRect().width() - c_node_buffer) - (c_slot_size*0.5);    break;
    }
    return QRectF(left, top, c_slot_size, c_slot_size);
}


//####################################################################################
//##    Returns DrSlot that is under scene_point
//####################################################################################
DrSlot* WorldMapItem::slotAtPoint(QPointF scene_point) {
    int signal = 0;
    for (auto &signal_pair : m_component->getSignalMap()) {
        ///QRectF    signal_rect = this->slotRect(DrSlotType::Signal, signal);
        QRectF    signal_rect = getSignalRects()[signal];
        QPolygonF scene_rect =  this->mapToScene(signal_rect);
        if (scene_rect.containsPoint(scene_point, Qt::FillRule::OddEvenFill)) return signal_pair.second;
        ++signal;
    }

    int output = 0;
    for (auto &output_pair : m_component->getOutputMap()) {
        ///QRectF    slot_rect =   this->slotRect(DrSlotType::Output, slot);
        QRectF    output_rect = getOutputRects()[output];
        QPolygonF scene_rect =  this->mapToScene(output_rect);
        if (scene_rect.containsPoint(scene_point, Qt::FillRule::OddEvenFill)) return output_pair.second;
        ++output;
    }
    return nullptr;
}


//####################################################################################
//##    Returns center point of DrSlot in Scene
//####################################################################################
QPointF WorldMapItem::slotLocationInScene(DrSlot *slot) {
    if (slot == nullptr) return QPointF(0, 0);

    int signal = 0;
    for (auto &signal_pair : m_component->getSignalMap()) {
        if (signal_pair.second == slot) {
            QRectF    signal_rect = this->slotRect(DrSlotType::Signal, signal);
            QPolygonF scene_rect =  this->mapToScene(signal_rect);
            return scene_rect.boundingRect().center();
        }
        ++signal;
    }

    int output = 0;
    for (auto &output_pair : m_component->getOutputMap()) {
        if (output_pair.second == slot) {
            QRectF    output_rect = this->slotRect(DrSlotType::Output, output);
            QPolygonF scene_rect =  this->mapToScene(output_rect);
            return scene_rect.boundingRect().center();
        }
        ++output;
    }
    return QPointF(0, 0);
}






















