//
//      Created by Stephens Nunnally on 5/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QMouseEvent>

#include "core/dr_random.h"
#include "engine/engine.h"
#include "engine/engine_texture.h"
#include "engine/form_engine.h"
#include "engine/opengl/opengl.h"
#include "engine/things/engine_thing_light.h"
#include "engine/things/engine_thing_object.h"
#include "engine/world/engine_world.h"
#include "project/dr_project.h"


// Local File Scope Globals
static cpBody *l_drag_body =    nullptr;
static cpVect  l_drag_offset =  cpv(0, 0);


//####################################################################################
//##    Mouse Events
//####################################################################################
void DrOpenGL::mousePressEvent(QMouseEvent *event) {
    if (m_engine->getCurrentWorld()->has_scene == false) return;
    DrEngineWorld *world = m_engine->getCurrentWorld();

    // ***** Convert mouse click to world coordinates
    glm::vec3 vec = mapFromScreen( DrPointF(event->pos().x(), event->pos().y()) );
    double x = static_cast<double>(vec.x);
    double y = static_cast<double>(vec.y);
    double z = static_cast<double>(vec.z);

    // ***** If running, process click
    if (m_form_engine->isTimerActive() == false)
        return;
    if (m_form_engine->demo_player == Demo_Player::Spawn) {
        if (event->button() & Qt::LeftButton) {
            for (int i = 0; i < 50; i++ ) {
                double vel_x = Dr::RandomDouble(-100, 100);
                double vel_y = Dr::RandomDouble( 100, 500);

                double ball_radius =  m_engine->getTexture(Asset_Textures::Ball)->width()  / 2.0;

                DrEngineObject *circle;
                if (Dr::RandomBool()) {
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
        } else if (event->button() & Qt::RightButton) {
            // Polygon shape points should be counter-clockwise
            std::vector<DrPointF> points;
            points.push_back( DrPointF( 20, -55) );     // Right Bottom
            points.push_back( DrPointF( 45,  15) );     // Right Middle
            points.push_back( DrPointF(  5,  60) );     // Top
            points.push_back( DrPointF(-10, -30) );     // Left Mid Middle Concave <-- point is ignored by Chipmunk
            points.push_back( DrPointF(-46, -10) );     // Left Middle
            points.push_back( DrPointF(-38, -55) );     // Left Bottom

            DrEngineObject *plant = new DrEngineObject(world, world->getNextKey(), c_no_key, Body_Type::Dynamic, Asset_Textures::Plant, x, y, z, DrPointF(2, .5));
            plant->addShapePolygon(points);
            world->addThing(plant);
        }

    } else if (m_form_engine->demo_player == Demo_Player::Car) {
        if (event->button() & Qt::LeftButton) {
            if (event->pos().x() < width() / 2) g_pedal = Pedal::Clockwise;
            else                                g_pedal = Pedal::CounterClockwise;
        } else if (event->button() & Qt::RightButton) {
            g_pedal = Pedal::CounterClockwise;
        }

    } else if (m_form_engine->demo_player == Demo_Player::Jump ||
               m_form_engine->demo_player == Demo_Player::Light ||
               m_form_engine->demo_player == Demo_Player::Player) {

        if (event->button() & Qt::LeftButton) {
            bool should_jump = true;

            // struct cpPointQueryInfo {
            //      const cpShape *shape;           // The nearest shape, NULL if no shape was within range.
            //      cpVect point;                   // The closest point on the shape's surface. (in world space coordinates)
            //      cpFloat distance;               // The distance to the point. The distance is negative if the point is inside the shape.
            //      cpVect gradient;                // The gradient of the signed distance function. The value should be similar to info.p/info.d,
            //                                      //      but accurate even for very small values of info.d
            DrEngineObject  *touch = nullptr;
            cpPointQueryInfo point_query;
            cpFloat          max_distance = 10.0;
            cpShape         *nearest = cpSpacePointQueryNearest(world->getSpace(), cpv(x, y), max_distance, CP_SHAPE_FILTER_ALL, &point_query);

            if (nearest != nullptr) {
                touch = static_cast<DrEngineObject*>(cpShapeGetUserData(nearest));

                if (touch != nullptr) {
                    if (touch->hasTouchDamage()) {
                        touch->takeDamage(touch->getTouchDamagePoints(), true);
                        should_jump = false;
                    }

                    if (touch->hasTouchDrag()) {
                        if (touch->body_type == Body_Type::Kinematic) {
                            l_drag_body = touch->body;
                            l_drag_offset = cpvsub(cpBodyGetPosition(l_drag_body), cpv(x, y));
                            should_jump = false;

                        } else if (touch->body_type == Body_Type::Dynamic) {
                            m_engine->mouse_joint = cpPivotJointNew(m_engine->mouse_body, touch->body, cpBodyGetPosition(touch->body));
                            double max_force = 10000;
                            double body_mass = cpBodyGetMass(touch->body);
                            if (isinf(body_mass) == false && isnan(body_mass) == false) max_force *= body_mass * (touch->getTouchDragForce() / 100.0);
                            cpConstraintSetMaxForce(m_engine->mouse_joint, max_force);
                            cpSpaceAddConstraint(world->getSpace(), m_engine->mouse_joint);
                            should_jump = false;
                        }
                    }
                }
            }

            if (should_jump) g_jump_button = true;


        } else if (event->button() & Qt::RightButton) {
            if (m_form_engine->demo_player == Demo_Player::Jump) {
                world->addSoftBodyCircle(DrPointF(x, y), Dr::RandomInt(30, 200));

            } else {
                for (int i = 0; i < 25; i++ ) {
                    DrEngineObject *block = new DrEngineObject(world, world->getNextKey(), c_no_key, Body_Type::Dynamic, Asset_Textures::Block, x, y, z);
                    block->addShapeBoxFromTexture(Asset_Textures::Block);
                    block->set3DType(Convert_3D_Type::Cube);
                    block->setDepth(61);
                    world->addThing(block);

                    double hue = Dr::RandomDouble(0.0, 1.0);
                    block->hue = static_cast<float>(hue);

                    ///double saturation = Dr::RandomDouble(0.0, 0.5) - 0.125;
                    ///block->saturation = static_cast<float>(saturation);
                }
            }
        }
    }

}

void DrOpenGL::mouseReleaseEvent(QMouseEvent *event) {
    if (m_engine->getCurrentWorld()->has_scene == false) return;
    DrEngineWorld *world = m_engine->getCurrentWorld();

    if (m_form_engine->demo_player == Demo_Player::Car) {
        if (event->buttons() == Qt::MouseButton::NoButton)
            g_pedal = Pedal::None;

    } else if (m_form_engine->demo_player == Demo_Player::Jump ||
               m_form_engine->demo_player == Demo_Player::Light ||
               m_form_engine->demo_player == Demo_Player::Player) {
        // On mouse up if there was an object linked to the mouse_body, destroy the mouse_joint
        if (m_engine->mouse_joint != nullptr) {
            cpSpaceRemoveConstraint(world->getSpace(), m_engine->mouse_joint);
            cpConstraintFree(m_engine->mouse_joint);
            m_engine->mouse_joint = nullptr;
        }
        if (l_drag_body != nullptr) l_drag_body = nullptr;

        // Reset buttons
        if (event->buttons() == Qt::MouseButton::NoButton) {
            g_jump_button =  false;
            g_shoot_button = false;
            g_pedal = Pedal::None;
        }
    }
}

void DrOpenGL::mouseMoveEvent(QMouseEvent *event) {
    if (m_engine->getCurrentWorld()->has_scene == false) return;
    ///DrEngineWorld *world = m_engine->getCurrentWorld();

    // ***** Convert mouse click to world coordinates
    glm::vec3 vec = mapFromScreen( DrPointF(event->pos().x(), event->pos().y()) );
    double x = static_cast<double>(vec.x);
    double y = static_cast<double>(vec.y);
    ///double z = static_cast<double>(vec.z());

    g_mouse_position = DrPointF(event->pos().x() * devicePixelRatio(), event->pos().y() * devicePixelRatio());

    // If running, process mouse move
    if (m_form_engine->isTimerActive()) {
        cpVect pos = cpv(x, y);
        cpBodySetPosition(m_engine->mouse_body, pos);
        if (l_drag_body != nullptr) {
            cpBodySetPosition(l_drag_body, cpvadd(pos, l_drag_offset));
        }
    }
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
    if (m_engine->getCurrentWorld() != nullptr) {
        zoom = m_engine->getCurrentWorld()->getCameraZoom();
        if ((zoom < 0.001) && (zoom > -0.001)) zoom = 0.001;
    }
    return static_cast<float>(zoom * m_zoom_scale);
}








