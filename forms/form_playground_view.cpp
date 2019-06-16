//
//      Created by Stephens Nunnally on 6/15/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QtMath>
#include <QDebug>
#include <QWheelEvent>

#include "form_playground.h"


//####################################################################################
//##        Wheel Event / Zoom Functions
//####################################################################################
// Handles zooming in / out of view with mouse wheel
#if QT_CONFIG(wheelevent)
void DrPlaygroundView::wheelEvent(QWheelEvent *event) {
    // Allow for scene scrolling if ctrl (cmd) is down
    if (event->modifiers() & Qt::KeyboardModifier::ControlModifier) {
        QGraphicsView::wheelEvent(event);
        return;
    }

    if (event->delta() > 0)
        zoomInOut(10);
    else
        zoomInOut(-10);
    event->accept();
}
#endif

void DrPlaygroundView::zoomInOut(int level) {
    m_zoom += level;
    if (m_zoom > 500) m_zoom = 500;
    if (m_zoom < -40) m_zoom = -40;
    m_zoom_scale = qPow(qreal(2), (m_zoom - 250) / qreal(50));

    QMatrix matrix;
    matrix.scale(m_zoom_scale, m_zoom_scale);
    this->setMatrix(matrix);
}










