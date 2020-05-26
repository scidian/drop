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
#include "editor/view_node_map/node_map_item.h"
#include "editor/view_node_map/node_map_scene.h"
#include "editor/view_node_map/node_map_view.h"


//####################################################################################
//##
//##    Mouse Wheel Event / Zoom Functions
//##
//####################################################################################
// Handles zooming in / out of view with mouse wheel
#if QT_CONFIG(wheelevent)
void NodeMapView::wheelEvent(QWheelEvent *event) {
    // ********** Allow for scene scrolling if ctrl (cmd) is down
    if (event->modifiers() & Qt::KeyboardModifier::ControlModifier) {
        QGraphicsView::wheelEvent(event);
        return;
    }

    // ********** Force View to know mouse position in Interactive Mode (good if in Hand mode)
    Mouse_Mode before_zoom = m_mouse_mode;
    m_mouse_mode = Mouse_Mode::None;
    spaceBarUp();
    QMouseEvent temp_event(QMouseEvent::Type::Move, event->position().toPoint(), Qt::MouseButton::NoButton, { }, { });
    mouseMoveEvent(&temp_event);

    // ********** Process Zoom
    if (event->angleDelta().y() > 0) zoomInOut( 10);                // In
    else                             zoomInOut(-10);                // Out
    event->accept();

    // Restore Mouse Mode
    m_mouse_mode = before_zoom;
}
#endif

void NodeMapView::zoomInOut(int level) {
    m_zoom += level;
    if (m_zoom > 350) m_zoom = 350;
    if (m_zoom <   0) m_zoom =   0;
    ///Dr::PrintDebug("Zoom: " + std::to_string(m_zoom));
    zoomToScale(std::pow(2.0, (m_zoom - 250) / 50.0), false);
}

void NodeMapView::zoomToPower(int level) {
    m_zoom = level;
    if (m_zoom > 350) m_zoom = 350;
    if (m_zoom <   0) m_zoom =   0;
    zoomToScale(std::pow(2.0, (m_zoom - 250) / 50.0), false);
}

void NodeMapView::zoomToScale(double scale, bool recalculate_level) {
    m_zoom_scale = scale;

    if (recalculate_level) {
        double solve_for_zoom  = std::log(m_zoom_scale) / std::log(2);
               solve_for_zoom *= 50;
               solve_for_zoom += 250;
        m_zoom = static_cast<int>(solve_for_zoom);
    }

    // QT515
    //QMatrix matrix;
    //matrix.scale(m_zoom_scale, m_zoom_scale);
    //matrix.rotate(m_rotate);
    //this->setMatrix(matrix);
    // {
    QTransform transform;
    transform.scale(m_zoom_scale, m_zoom_scale);
    transform.rotate(m_rotate);
    this->setTransform(transform);
    // }

    emit updateZoomSlider(static_cast<int>(m_zoom / 10.0));
    emit updateZoomSpin(static_cast<int>(m_zoom_scale * 100.0));
}

// Fits contents of NodeMapScene into View, called first time World Map is shown and DrProject::m_just_loaded is true
void NodeMapView::zoomToContents() {    
    this->fitInView( scene()->itemsBoundingRect() );

    // QT515
    //QMatrix fit_matrix = this->matrix();
    //double  scale_x = fit_matrix.m11();
    //double  scale_y = fit_matrix.m22();
    //double  min = (scale_x < scale_y) ? scale_x : scale_y;
    //        min = (min > 1.0) ? 1.0 : min;
    //zoomToScale(min);
    // {
    QTransform transform = this->transform();
    double  scale_x = transform.m11();
    double  scale_y = transform.m22();
    double  min = (scale_x < scale_y) ? scale_x : scale_y;
            min = (min > 1.0) ? 1.0 : min;
    zoomToScale(min);
    // }

    m_editor_relay->viewCenterOnPoint( scene()->itemsBoundingRect().center() );
}
















