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
#include "editor/view_node_map/node_map_item.h"
#include "editor/view_node_map/node_map_scene.h"
#include "editor/view_node_map/node_map_view.h"
#include "engine/debug_flags.h"
#include "project/constants_component_info.h"
#include "project/dr_project.h"
#include "project/settings/settings.h"
#include "project/settings/settings_component.h"
#include "project/settings/settings_component_slot.h"


//####################################################################################
//##    Returns Slot Circle Rect for a particular slot
//####################################################################################
// Calculates Slot rect by slot number, slot_number should start at 0
QRectF NodeMapItem::slotRect(DrSlotType slot_type, int slot_number) {
    int left = 0;
    int top  = (boundingRect().top() + c_node_buffer) + (c_node_row_height * (1.5 + static_cast<double>(slot_number)));
        top -= (c_slot_size/2);
    switch (slot_type) {
        case DrSlotType::Signal:    left = (boundingRect().top()   + c_node_buffer) - (c_slot_size*0.5);    break;
        case DrSlotType::Output:    left = (boundingRect().width() - c_node_buffer) - (c_slot_size*0.5);    break;
    }
    return QRectF(left, top, c_slot_size, c_slot_size);
}

// Slot rect by DrSlot*
QRectF NodeMapItem::slotRect(DrSlot *slot) {
    if (slot == nullptr) return QRectF();

    if (slot->getSlotType() == DrSlotType::Signal) {
        int signal = 0;
        for (auto &signal_pair : m_component->getSignalMap()) {
            if (signal_pair.second == slot) return this->slotRect(DrSlotType::Signal, signal);
            ++signal;
        }
    } else if (slot->getSlotType() == DrSlotType::Output) {
        int output = 0;
        for (auto &output_pair : m_component->getOutputMap()) {
            if (output_pair.second == slot) return this->slotRect(DrSlotType::Output, output);
            ++output;
        }
    }
    return QRectF();
}

QRectF NodeMapItem::slotSceneRect(DrSlot *slot) {
    if (slot == nullptr) return QRectF();
    QRectF    signal_rect = this->slotRect(slot);
    QPolygonF scene_rect =  this->mapToScene(signal_rect);
    return scene_rect.boundingRect();
}


//####################################################################################
//##    Returns DrSlot that is under scene_point
//####################################################################################
DrSlot* NodeMapItem::slotAtPoint(QPointF scene_point) {
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























