//
//      Created by Stephens Nunnally on 5/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QtMath>
#include <QMouseEvent>
#include <QRandomGenerator>

#include "forms/form_engine.h"
#include "physics/physics_opengl.h"

//####################################################################################
//##        Mouse Events
//####################################################################################
void PhysicsOpenGL::mousePressEvent(QMouseEvent *event)
{
    if (m_parent->has_scene == false) return;

    double x = ((event->pos().x() - m_center_of_widget.x()) * 2 );
    double y = ((event->pos().y() - m_center_of_widget.y()) * 3 );

    if (m_parent->demo == Demo::Spawn) {
        if (event->button() & Qt::LeftButton) {
            for (int i = 0; i < 25; i++ ) {
                double vel_x = QRandomGenerator::global()->bounded(-100, 100);
                double vel_y = QRandomGenerator::global()->bounded( 100, 500);
                m_parent->addCircle(BodyType::Dynamic, m_parent->t_ball, x, y, .7, .5, 2, QPointF(vel_x, vel_y) );
            }
        } else if (event->button() & Qt::RightButton) {
            m_parent->addBlock(BodyType::Dynamic, m_parent->t_metal_block, x, y, 1, 0, 3, QPointF(0, 0));
        } else if (event->button() & Qt::MiddleButton) {
            // Polygon shape points should be counter-clockwise
            QVector<QPointF> points;
            points.append( QPointF( 20, -55) );
            points.append( QPointF( 45,  15) );
            points.append( QPointF(  5,  60) );
            points.append( QPointF(-46, -10) );
            points.append( QPointF(-38, -55) );
            m_parent->addPolygon(BodyType::Dynamic, m_parent->t_moon_plant, x, y, points, .15, .4, 2.5, QPointF(0, 0));
        }


    } else if (m_parent->demo == Demo::Car) {
        if (event->button() & Qt::LeftButton) {
            m_parent->gas_pedal = Pedal::Clockwise;
        } else if (event->button() & Qt::RightButton) {
            m_parent->gas_pedal = Pedal::CounterClockwise;
        }
    }

}

void PhysicsOpenGL::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_parent->demo == Demo::Car)
        if (event->buttons() == Qt::MouseButton::NoButton)
            m_parent->gas_pedal = Pedal::None;
}



//####################################################################################
//##        Wheel Event / Zoom Functions
//####################################################################################

// Handles zooming in / out of view with mouse wheel
#if QT_CONFIG(wheelevent)
void PhysicsOpenGL::wheelEvent(QWheelEvent *event)
{
    // Allow for scene scrolling if ctrl (cmd) is down
    if (event->modifiers() & Qt::KeyboardModifier::ControlModifier) {
        QOpenGLWidget::wheelEvent(event);
        return;
    }

    if (event->delta() > 0) { zoomInOut(10);  }
    else                    { zoomInOut(-10); }
    event->accept();
}
#endif


void PhysicsOpenGL::zoomInOut(int level)
{
    m_zoom += level;
    if (m_zoom > 500) m_zoom = 500;
    if (m_zoom < -40) m_zoom = -40;
    m_scale = static_cast<float>(qPow(qreal(2), (m_zoom - 250) / qreal(50)));
}





