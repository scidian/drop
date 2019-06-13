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
#include "engine/engine_object.h"
#include "engine/engine_texture.h"
#include "engine/engine_world.h"
#include "opengl/opengl.h"
#include "helper.h"

//####################################################################################
//##        Mouse Events
//####################################################################################
void OpenGL::mousePressEvent(QMouseEvent *event) {

    if (m_engine->getCurrentWorld()->has_scene == false) return;

    // Convert mouse click to world coordinates
    QVector3D vec = mapFromScreen( QPointF(event->pos().x(), event->pos().y()) );
    double x = static_cast<double>(vec.x());
    double y = static_cast<double>(vec.y());
    double z = static_cast<double>(vec.z());
    if (m_engine->getCurrentWorld()->render_type == Render_Type::Orthographic) z = 0.0;

    // Process click
    if (m_engine->demo_player == Demo_Player::Spawn) {
        if (event->button() & Qt::LeftButton) {
            for (int i = 0; i < 50; i++ ) {
                double vel_x = QRandomGenerator::global()->bounded(-100, 100);
                double vel_y = QRandomGenerator::global()->bounded( 100, 500);

                double ball_radius =  m_engine->getTexture(Asset_Textures::Ball)->width()  / 2.0;

                if (QRandomGenerator::global()->bounded(0, 2) == 0) {
                    m_engine->getCurrentWorld()->addCircle(Body_Type::Dynamic, Asset_Textures::Ball, x, y, z, c_norotate, QPointF(1, 1), c_opaque,
                                                           ball_radius, c_center, c_friction, c_bounce, QPointF(vel_x, vel_y) );
                } else {
                    m_engine->getCurrentWorld()->addCircle(Body_Type::Dynamic, Asset_Textures::Ball, x, y, z, c_norotate, c_scale1x1, c_opaque,
                                                           ball_radius, c_center, c_friction, c_bounce, QPointF(vel_x, vel_y) );
                }
            }
        } else if (event->button() & Qt::MiddleButton) {
            m_engine->getCurrentWorld()->addBlock(Body_Type::Dynamic, Asset_Textures::Block, x, y, z, 0, QPointF(1, 1), 1, c_friction, c_bounce, QPointF(0, 0));
        } else if (event->button() & Qt::RightButton) {

            // Polygon shape points should be counter-clockwise
            QVector<QPointF> points;
            points.append( QPointF( 20, -55) );     // Right Bottom
            points.append( QPointF( 45,  15) );     // Right Middle
            points.append( QPointF(  5,  60) );     // Top
            points.append( QPointF(-10, -30) );     // Left Mid Middle Concave <-- point is ignored by Chipmunk
            points.append( QPointF(-46, -10) );     // Left Middle
            points.append( QPointF(-38, -55) );     // Left Bottom
            m_engine->getCurrentWorld()->addPolygon(Body_Type::Dynamic, Asset_Textures::Plant, x, y, z, c_norotate, QPointF(2, .5), c_opaque,
                                                    points, c_friction, c_bounce, QPointF(0, 0));
        }

    } else if (m_engine->demo_player == Demo_Player::Car) {
        if (event->button() & Qt::LeftButton) {
            if (event->pos().x() < width() / 2)
                m_engine->gas_pedal = Pedal::Clockwise;
            else
                m_engine->gas_pedal = Pedal::CounterClockwise;
        } else if (event->button() & Qt::RightButton)
            m_engine->gas_pedal = Pedal::CounterClockwise;

    } else if (m_engine->demo_player == Demo_Player::Jump) {
        if (m_engine->demo_jumper_1->hasLostControl() == false) {
            if (event->button() & Qt::LeftButton) {
                m_engine->jump_button = true;
            } else if (event->button() & Qt::RightButton) {
                for (int i = 0; i < 25; i++ ) {
                    DrEngineObject *block = m_engine->getCurrentWorld()->addBlock(Body_Type::Dynamic, Asset_Textures::Block, x, y, z, 0, QPointF(1, 1), 1,
                                                                                  c_friction, c_bounce, QPointF(0, 0));
                    double hue = QRandomGenerator::global()->bounded(1.0);
                    block->hue = static_cast<float>(hue);

                    ///double saturation = QRandomGenerator::global()->bounded(0.5) - 0.125;
                    ///block->saturation = static_cast<float>(saturation);
                    ///double contrast = QRandomGenerator::global()->bounded(0.5) - 0.125;
                    ///block->contrast = static_cast<float>(contrast);
                }
            }
        } else {
            if (event->button() & Qt::LeftButton)
                m_engine->gas_pedal = Pedal::CounterClockwise;
            else if (event->button() & Qt::RightButton)
                m_engine->gas_pedal = Pedal::Clockwise;
            else if (event->button() & Qt::MiddleButton)
                m_engine->jump_button = true;
        }
    }

}

void OpenGL::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_engine->demo_player == Demo_Player::Car) {
        if (event->buttons() == Qt::MouseButton::NoButton)
            m_engine->gas_pedal = Pedal::None;

    } else if (m_engine->demo_player == Demo_Player::Jump) {
        if (event->buttons() == Qt::MouseButton::NoButton) {
            m_engine->jump_button = false;
            m_engine->gas_pedal = Pedal::None;
        }
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










