//
//      Created by Stephens Nunnally on 5/1/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QKeyEvent>

#include "editor/interface_editor_relay.h"
#include "editor/world_map/world_map_view.h"


//####################################################################################
//##    Key Events
//####################################################################################
void WorldMapView::spaceBarDown() {
    m_flag_key_down_spacebar = true;
    setDragMode(QGraphicsView::DragMode::ScrollHandDrag);
    setInteractive(false);
}

void WorldMapView::spaceBarUp() {
    m_flag_key_down_spacebar = false;
    if (m_mouse_mode != Mouse_Mode::Hand) {
        setDragMode(QGraphicsView::DragMode::NoDrag);
        setInteractive(true);
    }
}
