//
//      Created by Stephens Nunnally on 5/1/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QApplication>
#include <QMouseEvent>
#include <QScrollBar>
#include <QTimer>

#include "editor/interface_editor_relay.h"
#include "editor/world_map/world_map_view.h"


//####################################################################################
//##
//##    Mouse Wheel Event / Zoom Functions
//##
//####################################################################################
// Handles zooming in / out of view with mouse wheel
#if QT_CONFIG(wheelevent)
void WorldMapView::wheelEvent(QWheelEvent *event) {
    // ********** Allow for scene scrolling if ctrl (cmd) is down
    if (event->modifiers() & Qt::KeyboardModifier::ControlModifier) {
        QGraphicsView::wheelEvent(event);
        return;
    }

    // ********** Force View to know mouse position in Interactive Mode (good if in Hand mode)
    Mouse_Mode before_zoom = m_mouse_mode;
    m_mouse_mode = Mouse_Mode::None;
    spaceBarUp();
    QMouseEvent temp_event(QMouseEvent::Type::Move, event->pos(), Qt::MouseButton::NoButton, { }, { });
    mouseMoveEvent(&temp_event);

    // ********** Process Zoom
    if (event->delta() > 0) zoomInOut( 10);                 // In
    else                    zoomInOut(-10);                 // Out
    event->accept();

    // Restore Mouse Mode
    m_mouse_mode = before_zoom;
}
#endif

void WorldMapView::zoomInOut(int level) {
    m_zoom += level;
    if (m_zoom > 350) m_zoom = 350;
    if (m_zoom < -50) m_zoom = -50;
    ///Dr::PrintDebug("Zoom: " + std::to_string(m_zoom));
    zoomToScale(std::pow(2.0, (m_zoom - 250) / 50.0), false);
}

void WorldMapView::zoomToPower(int level) {
    m_zoom = level;
    if (m_zoom > 350) m_zoom = 350;
    if (m_zoom < -50) m_zoom = -50;
    zoomToScale(std::pow(2.0, (m_zoom - 250) / 50.0), false);
}

void WorldMapView::zoomToScale(double scale, bool recalculate_level) {
    m_zoom_scale = scale;

    if (recalculate_level) {
        double solve_for_zoom  = std::log(m_zoom_scale) / std::log(2);
               solve_for_zoom *= 50;
               solve_for_zoom += 250;
        m_zoom = static_cast<int>(solve_for_zoom);
    }

    QMatrix matrix;
    matrix.scale(m_zoom_scale, m_zoom_scale);
    matrix.rotate(m_rotate);
    this->setMatrix(matrix);
}











