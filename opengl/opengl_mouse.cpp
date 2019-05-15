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

#include "engine/engine.h"
#include "engine/engine_texture.h"
#include "opengl/opengl.h"
#include "helper.h"

//####################################################################################
//##        Mouse Events
//####################################################################################
void OpenGL::mousePressEvent(QMouseEvent *event) {

    if (m_engine->has_scene == false) return;

    QVector3D vec = mapFromScreen( QPointF(event->pos().x(), event->pos().y()) );

    double x = static_cast<double>(vec.x());
    double y = static_cast<double>(vec.y());
    double z = 0.0; ///static_cast<double>(vec.z());
    ///if (m_engine->render_type == Render_Type::Orthographic) z = 0.0;
    ///m_engine->info = "X: " + QString::number(x) + " " + "Y: " + QString::number(y) + " " + "Z: " + QString::number(z);



    if (m_engine->demo_player == Demo_Player::Spawn) {
        if (event->button() & Qt::LeftButton) {
            for (int i = 0; i < 25; i++ ) {
                double vel_x = QRandomGenerator::global()->bounded(-100, 100);
                double vel_y = QRandomGenerator::global()->bounded( 100, 500);
                m_engine->addCircle(Body_Type::Dynamic, Test_Textures::Ball, x, y, z, 1, .7, .5, 2, QPointF(vel_x, vel_y) );
            }
        } else if (event->button() & Qt::MiddleButton) {
            m_engine->addBlock(Body_Type::Dynamic, Test_Textures::Block, x, y, z, 0, QPointF(1, 1), 1, 1, 0, 3, QPointF(0, 0));
        } else if (event->button() & Qt::RightButton) {

            // Polygon shape points should be counter-clockwise
            QVector<QPointF> points;
            points.append( QPointF( 20, -55) );     // Right Bottom
            points.append( QPointF( 45,  15) );     // Right Middle
            points.append( QPointF(  5,  60) );     // Top
            points.append( QPointF(-10, -30) );     // Left Mid Middle Concave <-- point is ignored by Chipmunk
            points.append( QPointF(-46, -10) );     // Left Middle
            points.append( QPointF(-38, -55) );     // Left Bottom
            m_engine->addPolygon(Body_Type::Dynamic, Test_Textures::Plant, x, y, z, 1, points, .15, .4, 2.5, QPointF(0, 0));
        }

    } else if (m_engine->demo_player == Demo_Player::Car) {
        if (event->button() & Qt::LeftButton) {
            m_engine->gas_pedal = Pedal::Clockwise;
        } else if (event->button() & Qt::RightButton) {
            m_engine->gas_pedal = Pedal::CounterClockwise;
        }

    } else if (m_engine->demo_player == Demo_Player::Jump) {
        if (event->button() & Qt::LeftButton) {
            m_engine->keyboard_y = 1;
            m_engine->jump_state = Jump_State::Need_To_Jump;
        } else if (event->button() & Qt::RightButton) {
            m_engine->addBlock(Body_Type::Dynamic, Test_Textures::Block, x, y, z, 0, QPointF(1, 1), 1, .6, .3, 25, QPointF(0, 0));
        }
    }

}

void OpenGL::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_engine->demo_player == Demo_Player::Car) {
        if (event->buttons() == Qt::MouseButton::NoButton)
            m_engine->gas_pedal = Pedal::None;

    } else if (m_engine->demo_player == Demo_Player::Jump) {
        if (event->buttons() == Qt::MouseButton::NoButton)
            m_engine->keyboard_y = 0;
    }
}



//####################################################################################
//##        Wheel Event / Zoom Functions
//####################################################################################

// Handles zooming in / out of view with mouse wheel
#if QT_CONFIG(wheelevent)
void OpenGL::wheelEvent(QWheelEvent *event) {
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


void OpenGL::zoomInOut(int level) {
    m_zoom += level;
    if (m_zoom > 500) m_zoom = 500;
    if (m_zoom < -40) m_zoom = -40;
    m_scale = static_cast<float>(qPow(qreal(2), (m_zoom - 250) / qreal(50)));
}










