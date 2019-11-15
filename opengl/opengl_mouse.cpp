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
#include "engine/engine_world.h"
#include "engine_things/engine_thing_light.h"
#include "engine_things/engine_thing_object.h"
#include "engine/form_engine.h"
#include "opengl/opengl.h"
#include "helper.h"

//####################################################################################
//##    Mouse Events
//####################################################################################
void DrOpenGL::mousePressEvent(QMouseEvent *event) {
    if (m_engine->getCurrentWorld()->has_scene == false) return;

    // ***** Convert mouse click to world coordinates
    QVector3D vec = mapFromScreen( QPointF(event->pos().x(), event->pos().y()) );
    double x = static_cast<double>(vec.x());
    double y = static_cast<double>(vec.y());
    double z = static_cast<double>(vec.z());

    // ***** Process click
    DrEngineWorld *world = m_engine->getCurrentWorld();
    if (m_form_engine->demo_player == Demo_Player::Spawn) {
        if (event->button() & Qt::LeftButton) {
            for (int i = 0; i < 50; i++ ) {
                double vel_x = QRandomGenerator::global()->bounded(-100, 100);
                double vel_y = QRandomGenerator::global()->bounded( 100, 500);

                double ball_radius =  m_engine->getTexture(Asset_Textures::Ball)->width()  / 2.0;

                DrEngineObject *circle;
                if (QRandomGenerator::global()->bounded(0, 2) == 0) {
                    circle = new DrEngineObject(world, world->getNextKey(), c_no_key, Body_Type::Dynamic, Asset_Textures::Ball, x, y, z);
                } else {
                    ///circle = new DrEngineObject(world, world->getNextKey(), Body_Type::Dynamic, Asset_Textures::Ball, x, y, z);
                    circle = new DrEngineObject(world, world->getNextKey(), c_no_key, Body_Type::Dynamic, Asset_Textures::Ball, x, y, z, DrPointF(2, 1));
                }
                circle->addShapeCircle(ball_radius, c_center);
                circle->setOriginalVelocityX(vel_x);
                circle->setOriginalVelocityX(vel_y);
                world->addThing(circle);
            }
        } else if (event->button() & Qt::MiddleButton) {            
            DrEngineObject *block = new DrEngineObject(world, world->getNextKey(), c_no_key, Body_Type::Dynamic, Asset_Textures::Block, x, y, z);
            block->addShapeBoxFromTexture(Asset_Textures::Block);
            world->addThing(block);

        } else if (event->button() & Qt::RightButton) {

            // Polygon shape points should be counter-clockwise
            QVector<DrPointF> points;
            points.append( DrPointF( 20, -55) );    // Right Bottom
            points.append( DrPointF( 45,  15) );    // Right Middle
            points.append( DrPointF(  5,  60) );    // Top
            points.append( DrPointF(-10, -30) );    // Left Mid Middle Concave <-- point is ignored by Chipmunk
            points.append( DrPointF(-46, -10) );    // Left Middle
            points.append( DrPointF(-38, -55) );    // Left Bottom

            DrEngineObject *plant = new DrEngineObject(world, world->getNextKey(), c_no_key, Body_Type::Dynamic, Asset_Textures::Plant, x, y, z, DrPointF(2, .5));
            plant->addShapePolygon(points);
            world->addThing(plant);
        }

    } else if (m_form_engine->demo_player == Demo_Player::Car) {
        if (event->button() & Qt::LeftButton) {
            if (event->pos().x() < width() / 2)
                g_pedal = Pedal::Clockwise;
            else
                g_pedal = Pedal::CounterClockwise;
        } else if (event->button() & Qt::RightButton)
            g_pedal = Pedal::CounterClockwise;

    } else if (m_form_engine->demo_player == Demo_Player::Jump ||
               m_form_engine->demo_player == Demo_Player::Light ||
               m_form_engine->demo_player == Demo_Player::Player) {
        if (event->button() & Qt::LeftButton) {
            g_jump_button = true;
        } else if (event->button() & Qt::RightButton) {
            for (int i = 0; i < 25; i++ ) {
                DrEngineObject *block = new DrEngineObject(world, world->getNextKey(), c_no_key, Body_Type::Dynamic, Asset_Textures::Block, x, y, z);
                block->addShapeBoxFromTexture(Asset_Textures::Block);
                block->set3DType(Convert_3D_Type::Cube);
                block->setDepth(61);
                world->addThing(block);

                double hue = QRandomGenerator::global()->bounded(1.0);
                block->hue = static_cast<float>(hue);

                ///double saturation = QRandomGenerator::global()->bounded(0.5) - 0.125;
                ///block->saturation = static_cast<float>(saturation);
                ///double contrast = QRandomGenerator::global()->bounded(0.5) - 0.125;
                ///block->contrast = static_cast<float>(contrast);
            }
        }
    }

}

void DrOpenGL::mouseReleaseEvent(QMouseEvent *event) {
    if (m_form_engine->demo_player == Demo_Player::Car) {
        if (event->buttons() == Qt::MouseButton::NoButton)
            g_pedal = Pedal::None;

    } else if (m_form_engine->demo_player == Demo_Player::Jump ||
               m_form_engine->demo_player == Demo_Player::Light ||
               m_form_engine->demo_player == Demo_Player::Player) {
        if (event->buttons() == Qt::MouseButton::NoButton) {
            g_jump_button = false;
            g_pedal = Pedal::None;
        }
    }
}

void DrOpenGL::mouseMoveEvent(QMouseEvent *event) {
    if (m_engine->getCurrentWorld()->has_scene == false) return;

    QPointF mouse_position = QPointF(event->pos().x() * devicePixelRatio(), event->pos().y() * devicePixelRatio());
    g_mouse_position = DrPointF(mouse_position.x(), mouse_position.y());
}

//####################################################################################
//##    Wheel Event / Zoom Functions
//####################################################################################

// Handles zooming in / out of view with mouse wheel
#if QT_CONFIG(wheelevent)
void DrOpenGL::wheelEvent(QWheelEvent *event) {
    // Allow for scene scrolling if ctrl (cmd) is down
    if (event->modifiers() & Qt::KeyboardModifier::ControlModifier) {
        QOpenGLWidget::wheelEvent(event);
        return;
    }

    if (event->delta() > 0) { zoomInOut( 10); }
    else                    { zoomInOut(-10); }
    event->accept();
}
#endif

// Converts back and forth between magnification scale and power zoom level
double DrOpenGL::zoomScaleToPow(double scale) {
    double solve_for_zoom  = std::log(scale) / std::log(2.0);
           solve_for_zoom *= 50.0;
           solve_for_zoom += 250.0;
    return solve_for_zoom;
}
double DrOpenGL::zoomPowToScale(double pow) {
   return std::pow(2.0, (pow - 250.0) / 50.0);
}


void DrOpenGL::zoomInOut(int level) {
    m_zoom += level;
    if (m_zoom > 500) m_zoom = 500;
    if (m_zoom < -40) m_zoom = -40;
    m_zoom_scale = zoomPowToScale(m_zoom);
}

void DrOpenGL::zoomToScale(double scale) {
    m_zoom_scale = scale;
    m_zoom = static_cast<int>(zoomScaleToPow(m_zoom_scale));
}

float DrOpenGL::combinedZoomScale() {
    double zoom = 1.0;
    if (m_engine->getCurrentWorld() != nullptr)
        zoom = m_engine->getCurrentWorld()->getCameraZoom();

    return static_cast<float>(zoom * m_zoom_scale);
}








