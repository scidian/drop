//
//      Created by Stephens Nunnally on 1/21/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QtMath>
#include <QMouseEvent>
#include <QScrollBar>
#include <QTimer>

#include "editor/interface_editor_relay.h"
#include "editor/view/editor_view.h"
#include "project/entities/dr_thing.h"


//####################################################################################
//##
//##    Mouse Wheel Event / Zoom Functions
//##
//####################################################################################
// Handles zooming in / out of view with mouse wheel
#if QT_CONFIG(wheelevent)
void DrView::wheelEvent(QWheelEvent *event) {
    // ********** Allow for scene scrolling if ctrl (cmd) is down
    if (event->modifiers() & Qt::KeyboardModifier::ControlModifier) {
        QGraphicsView::wheelEvent(event);
        return;
    }

    // ********** If over a Camera, need to zoom it In / Out
    if (m_over_handle == Position_Flags::Over_Camera && m_cam_mouse_over != nullptr) {
        if (m_cam_mouse_over->isLocked() == false) {
            setInspectorClearSelection(m_cam_mouse_over);
            double change = (event->delta() > 0) ? 0.25 : -0.25;
            if (m_cam_mouse_over->getThingType() == DrThingType::Character) {
                double cam_zoom = m_cam_mouse_over->getComponentPropertyValue(Comps::Thing_Settings_Character, Props::Thing_Character_Camera_Zoom).toDouble() + change;
                if (cam_zoom < 0) cam_zoom = 0;
                m_cam_mouse_over->setComponentPropertyValue(Comps::Thing_Settings_Character, Props::Thing_Character_Camera_Zoom, cam_zoom);
                m_editor_relay->updateEditorWidgetsAfterItemChange( Editor_Widgets::Stage_View, { m_cam_mouse_over },
                                { std::make_pair(Comps::Thing_Settings_Character, Props::Thing_Character_Camera_Zoom) });
            } else if (m_cam_mouse_over->getThingType() == DrThingType::Camera) {
                double cam_zoom = m_cam_mouse_over->getComponentPropertyValue(Comps::Thing_Settings_Camera, Props::Thing_Camera_Zoom).toDouble() + change;
                if (cam_zoom < 0) cam_zoom = 0;
                m_cam_mouse_over->setComponentPropertyValue(Comps::Thing_Settings_Camera, Props::Thing_Camera_Zoom, cam_zoom);
                m_editor_relay->updateEditorWidgetsAfterItemChange( Editor_Widgets::Stage_View, { m_cam_mouse_over },
                                { std::make_pair(Comps::Thing_Settings_Camera, Props::Thing_Camera_Zoom) });
            }
            update();
            return;
        }
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

    // Show tool tip with zoom percentage, if first time start tooltip, otherwise update it
    if (m_tool_tip->getTipType() != View_Mode::Zooming)
        m_tool_tip->startToolTip(View_Mode::Zooming, event->pos(), static_cast<int>(m_zoom_scale * 100) );
    else
        m_tool_tip->updateToolTipData( static_cast<int>(m_zoom_scale * 100) );

    // Reset tool tip timeout, post a single shot timer to eventually hide tool tip
    m_zoom_timer.restart();
    QTimer::singleShot(300, this, SLOT(stoppedZooming()));
}
#endif

// SLOT: Handles hiding tool tip after done zooming
void DrView::stoppedZooming() {
    // If over 1.2 seconds have passed since last time mouse wheel was activated, stop tool tip
    if (m_tool_tip->getTipType() == View_Mode::Zooming) {
        if (m_zoom_timer.elapsed() > 1200) {
            m_tool_tip->stopToolTip();
            this->viewport()->repaint(this->viewport()->rect());
        } else {
            QTimer::singleShot(300, this, SLOT(stoppedZooming()));
        }
    }
}

void DrView::zoomInOut(int level) {
    m_zoom += level;
    if (m_zoom > 500) m_zoom = 500;
    if (m_zoom < -50) m_zoom = -50;
    ///Dr::SetLabelText(Label_Names::Label_1, "Zoom: " + QString::number(m_zoom));
    zoomToScale(std::pow(2.0, (m_zoom - 250) / 50.0), false);
}

void DrView::zoomToPower(int level) {
    m_zoom = level;
    if (m_zoom > 500) m_zoom = 500;
    if (m_zoom < -50) m_zoom = -50;
    zoomToScale(std::pow(2.0, (m_zoom - 250) / 50.0), false);
}

void DrView::zoomToScale(double scale, bool recalculate_level) {
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

    updateSelectionBoundingBox(5);
    if (horizontalScrollBar()->value() == 0 && verticalScrollBar()->value() == 0) {
        updateGrid();
    }

    emit updateZoomSlider(static_cast<int>(m_zoom / 10.0));
    emit updateZoomSpin(static_cast<int>(m_zoom_scale * 100.0));
}

// Fits contents of stage into View, called from button on View Toolbar
void DrView::zoomToContents() {
    this->fitInView( this->scene()->sceneRect() );

    QMatrix fit_matrix = this->matrix();
    double  scale_x = fit_matrix.m11();
    double  scale_y = fit_matrix.m22();
    double  min = (scale_x < scale_y) ? scale_x : scale_y;
    zoomToScale(min);

    m_editor_relay->viewCenterOnPoint( this->scene()->sceneRect().center() );
}
















