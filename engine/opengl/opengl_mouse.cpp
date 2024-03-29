//
//      Created by Stephens Nunnally on 5/3/2019, (c) 2019 Scidian Studios, All Rights Reserved
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
#include "engine/thing/components/thing_comp_3d.h"
#include "engine/thing/components/thing_comp_physics.h"
#include "engine/thing/engine_thing.h"
#include "engine/world/engine_world.h"
#include "project/dr_project.h"
#include "project/entities_physics_2d/dr_asset.h"

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
    DrPointF  screen_position = DrPointF(event->pos().x(), event->pos().y());
    glm::vec3 vec = mapFromScreen( screen_position );
    double x = static_cast<double>(vec.x);
    double y = static_cast<double>(vec.y);
    double z = static_cast<double>(vec.z);

    // ***** If not running, don't process mousePressEvent
    if (m_form_engine->isTimerActive() == false) return;

    // ***** Push Messages onto Signal Stack
    if        (event->button() & Qt::LeftButton) {
        m_engine->pushMessage(Messages::MouseLeftDown, screen_position);
    } else if (event->button() & Qt::RightButton) {
        m_engine->pushMessage(Messages::MouseRightDown, screen_position);
    } else if (event->button() & Qt::MiddleButton) {
        m_engine->pushMessage(Messages::MouseMiddleDown, screen_position);
    }

    // ***** Handle Demo Specific functions
    if (m_form_engine->demo_player == Demo_Player::Spawn) {
        if (event->button() & Qt::LeftButton) {
            for (int i = 0; i < 50; i++ ) {
                double vel_x = Dr::RandomDouble(-100, 100);
                double vel_y = Dr::RandomDouble( 100, 500);

                double ball_radius =  m_engine->getTexture(Asset_Textures::Ball)->width()  / 2.0;

                DrEngineThing *circle;
                if (Dr::RandomBool()) {
                    circle = new DrEngineThing(world, world->getNextKey(), c_no_key);
                    circle->setComponentPhysics(new ThingCompPhysics(world, circle, Body_Type::Dynamic, Asset_Textures::Ball, x, y, z));
                } else {
                    circle = new DrEngineThing(world, world->getNextKey(), c_no_key);
                    circle->setComponentPhysics(new ThingCompPhysics(world, circle, Body_Type::Dynamic, Asset_Textures::Ball, x, y, z, DrPointF(2, 1)));
                }
                circle->compPhysics()->addShapeCircle(ball_radius, c_center);
                circle->compPhysics()->setOriginalVelocityX(vel_x);
                circle->compPhysics()->setOriginalVelocityX(vel_y);
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

            DrEngineThing *plant = new DrEngineThing(world, world->getNextKey(), c_no_key);
            plant->setComponentPhysics(new ThingCompPhysics(world, plant, Body_Type::Dynamic, Asset_Textures::Plant, x, y, z, DrPointF(2, .5)));
            plant->compPhysics()->addShapePolygon(points);
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
            DrEngineThing   *touch = nullptr;
            cpPointQueryInfo point_query;
            cpFloat          max_distance = 10.0;
            cpShape         *nearest = cpSpacePointQueryNearest(world->getSpace(), cpv(x, y), max_distance, CP_SHAPE_FILTER_ALL, &point_query);

            if (nearest != nullptr) {
                touch = static_cast<DrEngineThing*>(cpShapeGetUserData(nearest));
                if (touch != nullptr) {
                    ThingCompPhysics *physics = touch->compPhysics();
                    if (physics != nullptr) {

                        if (physics->hasTouchDamage()) {
                            physics->takeDamage(physics->getTouchDamagePoints(), true);
                            should_jump = false;
                        }

                        if (physics->hasTouchDrag()) {
                            if (physics->body_type == Body_Type::Kinematic) {
                                l_drag_body = physics->body;
                                l_drag_offset = cpvsub(cpBodyGetPosition(l_drag_body), cpv(x, y));
                                should_jump = false;

                            } else if (physics->body_type == Body_Type::Dynamic) {
                                m_engine->mouse_joint = cpPivotJointNew(m_engine->mouse_body, physics->body, cpBodyGetPosition(physics->body));
                                double max_force = 10000;
                                double body_mass = cpBodyGetMass(physics->body);
                                if (Dr::RealDouble(body_mass)) max_force *= body_mass * (physics->getTouchDragForce() / 100.0);
                                if (physics->body_style == Body_Style::Mesh_Blob) max_force *= 20.0;
                                cpConstraintSetMaxForce(m_engine->mouse_joint, max_force);
                                cpSpaceAddConstraint(world->getSpace(), m_engine->mouse_joint);
                                should_jump = false;
                            }
                        }

                    }
                }
            }

            if (should_jump) g_jump_button = true;


        } else if (event->button() & Qt::RightButton) {
            if (m_form_engine->demo_player == Demo_Player::Jump) {


                //###########################################
                //##
                //##    Soft Body Demo
                //##
                //###########################################
                DrEngineThing *soft_body = nullptr;
                long   asset_texture;
                double friction = 0.25;
                double bounce =   0.5;

                if (Dr::RandomBool()) {
                    double scale_x = Dr::RandomDouble(0.8, 3.0);
                    double scale_y = Dr::RandomDouble(0.8, 3.0);

                    DrAsset *asset = world->getProject()->findAssetFromKey(1092);
                    if (Dr::RandomBool() && asset != nullptr) {
                        asset_texture = asset->getIdleAnimationFirstFrameImageKey();
                        scale_x /= 5.0;
                        scale_y /= 5.0;
                        DrPointF size(asset->getWidth() * scale_x, asset->getHeight() * scale_y);
                        soft_body = world->addSoftBodyMesh(c_no_key, asset_texture, x, y, 0, size, DrPointF(scale_x, scale_y), g_double, friction, bounce, true);
                    } else {
                        DrPointF size(61 * scale_x, 61.0 * scale_y);
                        soft_body = world->addSoftBodySquare(c_no_key, Asset_Textures::Block, x, y, 0, size, DrPointF(scale_x, scale_y), g_double, friction, bounce, true);
                    }

                } else {
                    double scale_x = Dr::RandomDouble(1.0, 4.0);
                    double scale_y = Dr::RandomDouble(1.0, 4.0);

                    if (Dr::RandomInt(0, 10) == 1) {
                        asset_texture = Asset_Textures::Plant;
                        DrPointF size(91.0 * scale_x, 112.0 * scale_y);
                        soft_body = world->addSoftBodyCircle(c_no_key, asset_texture, x, y, 0, size, DrPointF(scale_x, scale_y), g_double, friction, bounce, true);
                    } else {
                        asset_texture = Asset_Textures::Ball;
                        DrPointF size(46.0 * scale_x, 45.0 * scale_y);
                        soft_body = world->addSoftBodyCircle(c_no_key, asset_texture, x, y, 0, size, DrPointF(scale_x, scale_y), g_double, friction, bounce, true);
                    }
                }
                soft_body->saturation = 255.0;
                soft_body->hue = static_cast<float>(Dr::RandomDouble(0, 360));
                soft_body->compPhysics()->setDamageRecoil(1000.0);
                soft_body->compPhysics()->setTouchDrag(true);

            } else {


                //###########################################
                //##
                //##    Foliage Demo
                //##
                //###########################################
//                long foliage = 0;
//                DrEngineObject *grass = nullptr;
//                if (Dr::RandomInt(0, 4) == 1) {
//                    double scale_x = Dr::RandomDouble(0.5, 1.0);
//                    double scale_y = Dr::RandomDouble(0.8, 1.5);
//                    foliage = Asset_Textures::Flower;
//                    grass = new DrEngineObject(world, world->getNextKey(), c_no_key, Body_Type::Dynamic, foliage, x, y, z, DrPointF(scale_x, scale_y));
//                } else {
//                    double scale_x = Dr::RandomDouble(0.8, 1.5);
//                    double scale_y = Dr::RandomDouble(0.4, 1.0);
//                    foliage = Asset_Textures::Grass;
//                    grass = new DrEngineObject(world, world->getNextKey(), c_no_key, Body_Type::Dynamic, foliage, x, y, z, DrPointF(scale_x, scale_y));
//                }
//                grass->addShapeBoxFromTexture(foliage);
//                grass->setComponentFoliage(new ThingCompFoliage(world, grass, 1.0));
//                world->addThing(grass);


                //###########################################
                //##
                //##    Color Blocks
                //##
                //###########################################
                for (int i = 0; i < 25; i++ ) {
                    DrEngineThing *block = new DrEngineThing(world, world->getNextKey(), c_no_key);
                    block->setComponentPhysics(new ThingCompPhysics(world, block, Body_Type::Dynamic, Asset_Textures::Block, x, y, z));
                    block->compPhysics()->addShapeBoxFromTexture(Asset_Textures::Block);
                    block->comp3D()->set3DType(Convert_3D_Type::Cube);
                    block->comp3D()->setDepth(61);
                    block->compPhysics()->setTouchDrag(true);
                    block->compPhysics()->setTouchDragForce(5000.0);
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
    DrPointF screen_position = DrPointF(event->pos().x(), event->pos().y());

    // ***** Push Messages onto Signal Stack
    if        (event->button() & Qt::LeftButton) {
        m_engine->pushMessage(Messages::MouseLeftUp, screen_position);
    } else if (event->button() & Qt::RightButton) {
        m_engine->pushMessage(Messages::MouseRightUp, screen_position);
    } else if (event->button() & Qt::MiddleButton) {
        m_engine->pushMessage(Messages::MouseMiddleUp, screen_position);
    }

    // ***** Process mouse up for current demo mode
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
    DrPointF  screen_position = DrPointF(event->pos().x(), event->pos().y());
    glm::vec3 vec = mapFromScreen( DrPointF(event->pos().x(), event->pos().y()) );
    double x = static_cast<double>(vec.x);
    double y = static_cast<double>(vec.y);
    ///double z = static_cast<double>(vec.z());

    // ***** Push Messages onto Signal Stack
    m_engine->pushMessage(Messages::MouseMove, screen_position);

    // Store mouse position as global for convienience
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

    // ***** Push Messages onto Signal Stack
    m_engine->pushMessage(Messages::MouseScroll, event->angleDelta().y());

    if (event->angleDelta().y() > 0) { zoomInOut( 10); }
    else                             { zoomInOut(-10); }
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








