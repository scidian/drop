//
//      Created by Stephens Nunnally on 1/16/2020, (c) 2020 Scidian Studios, All Rights Reserved
//
//  File:
//
//
//
#include "core/dr_debug.h"
#include "engine/engine.h"
#include "engine/engine_camera.h"
#include "engine/form_engine.h"
#include "engine/opengl/opengl.h"
#include "engine/thing/components/thing_comp_camera.h"
#include "engine/thing/components/thing_comp_physics.h"
#include "engine/thing/components/thing_comp_player.h"
#include "engine/thing/engine_thing.h"
#include "engine/world/engine_world.h"


//####################################################################################
//####################################################################################
//##
//##    Local Camera Functions
//##
//####################################################################################
//####################################################################################

//####################################################################################
//##    Lerps an X,Y,Z triplet
//####################################################################################
static inline void SmoothMove(glm::vec3 &start, const glm::vec3 &target, const float &lerp, const float &milliseconds) {
    float lerp_amount = Dr::Clamp(lerp * milliseconds, 0.001f, 1.0f);
    start.x = Dr::Lerp(start.x, target.x, lerp_amount);
    start.y = Dr::Lerp(start.y, target.y, lerp_amount);
    start.z = Dr::Lerp(start.z, target.z, lerp_amount);

    ///float lerp_amount = lerp * milliseconds * 10.f;
    ///start.x = Dr::LerpConst(start.x, target.x, lerp_amount);
    ///start.y = Dr::LerpConst(start.y, target.y, lerp_amount);
    ///start.z = Dr::LerpConst(start.z, target.z, lerp_amount);
}

static inline void SmoothMove(double &start, const double &target, const double &lerp, const double &milliseconds) {
    double  lerp_amount = Dr::Clamp(lerp * milliseconds, 0.001, 1.0);
    start = Dr::Lerp(start, target, lerp_amount);

    ///double lerp_amount = lerp * milliseconds * 10.0;
    ///start = Dr::LerpConst(start, target, lerp_amount);
}


//####################################################################################
//####################################################################################
//##
//##    DrEngineWorld - Camera Functions
//##
//####################################################################################
//####################################################################################
// Finds Camera with camera_id
DrEngineCamera* DrEngineWorld::getCamera(long camera_id) {
    for (auto camera : m_cameras) {
        if (camera.second->getKey() == camera_id) return camera.second;
    }
    return nullptr;
}

// Finds Cameras following thing_id
EngineCameraList DrEngineWorld::getCamerasFollowThing(long thing_id) {
    std::vector<DrEngineCamera*> cameras;
    cameras.clear();
    for (auto camera : m_cameras) {
        if (camera.second->getThingFollowingKey() == thing_id) cameras.push_back(camera.second);
    }
    return cameras;
}

// Adds Camera to World, Default parameters: nullptr, 0, 0, c_default_camera_z
DrEngineCamera* DrEngineWorld::addCamera(long thing_key_to_follow, float x, float y, float z, int buffer_size) {
    long new_key = getNextKey();
    DrEngineCamera *camera = new DrEngineCamera(this, new_key, x, y, z, buffer_size);

    m_cameras[new_key] = camera;

    // If an object was passed in, attach camera to that object
    DrEngineThing *follow = findThingByKey(thing_key_to_follow);
    if (thing_key_to_follow != 0 && follow != nullptr) {
        if (follow->compCamera() == nullptr) {
            follow->setComponentCamera(new ThingCompCamera(this, follow));
        }

        camera->setThingFollowingKey( thing_key_to_follow );
        camera->setPositionX(   static_cast<float>(follow->getPosition().x) + follow->compCamera()->getCameraPosition().x );
        camera->setPositionY(   static_cast<float>(follow->getPosition().y) + follow->compCamera()->getCameraPosition().y );
        camera->setPositionZ(   static_cast<float>(follow->getZOrder() )    + follow->compCamera()->getCameraPosition().z );
        camera->setRotation(    follow->compCamera()->getCameraRotation() );
        camera->setZoom(        follow->compCamera()->getCameraZoom(), true );
        camera->setLag(         follow->compCamera()->getCameraLag() );
        camera->setMatchAngle(  follow->compCamera()->getCameraMatch() );
        camera->setTarget(      camera->getPosition() );
        camera->setUpVector(    follow->compCamera()->getCameraUpVector() );
        camera->setEdge(        Edge_Location::Top,     follow->compCamera()->getCameraEdge(Edge_Location::Top));
        camera->setEdge(        Edge_Location::Right,   follow->compCamera()->getCameraEdge(Edge_Location::Right));
        camera->setEdge(        Edge_Location::Bottom,  follow->compCamera()->getCameraEdge(Edge_Location::Bottom));
        camera->setEdge(        Edge_Location::Left,    follow->compCamera()->getCameraEdge(Edge_Location::Left));
        follow->compCamera()->setActiveCameraKey(new_key);
    }
    return camera;
}

// Updates all cameras based on the objects they're following
void DrEngineWorld::moveCameras(double milliseconds) {
    // ***** Move Cameras
    for (auto camera_pair : m_cameras) {
        camera_pair.second->moveCamera(milliseconds);
    }

    // ***** Handle Switching Cameras
    if (m_switching_cameras) {
        m_switch_milliseconds += milliseconds;

        if (m_active_camera == c_no_key) return;
        DrEngineCamera *target_camera = getCamera(m_active_camera);

        // Linear Interpolation of Temporary Values
        glm::vec3 target_position = target_camera->getPosition();
        glm::vec3 target_rotation = target_camera->getRotation();
                  target_rotation.x = Dr::EqualizeAngle0to360(target_rotation.x);
                  target_rotation.y = Dr::EqualizeAngle0to360(target_rotation.y);
                  target_rotation.z = Dr::EqualizeAngle0to360(target_rotation.z);
                  target_rotation.x = Dr::FindClosestAngle180(m_temp_rotation.x, target_rotation.x);
                  target_rotation.y = Dr::FindClosestAngle180(m_temp_rotation.y, target_rotation.y);
                  target_rotation.z = Dr::FindClosestAngle180(m_temp_rotation.z, target_rotation.z);
        glm::vec3 target_up_vector = (target_camera->getUpVector() == Up_Vector::Y) ? c_up_vector_y : c_up_vector_z;
        double    target_following_rotation = target_camera->getThingFollowingRotation();
                  target_following_rotation = Dr::EqualizeAngle0to360(target_following_rotation);
                  target_following_rotation = Dr::FindClosestAngle180(m_temp_follow_angle, target_following_rotation);
        double    target_z_order = static_cast<double>(target_camera->getThingFollowingZOrder());

        m_temp_follow_angle =   m_switch_follow_angle;
        m_temp_position =       m_switch_position;
        m_temp_rotation =       m_switch_rotation;
        m_temp_up_vector =      m_switch_up_vector;
        m_temp_z_order =        m_switch_z_order;
        m_temp_match =          target_camera->getMatchAngle();

        SmoothMove(m_temp_follow_angle, target_following_rotation,  0.001 * cam_switch_speed, m_switch_milliseconds );
        SmoothMove(m_temp_z_order,      target_z_order,             0.001 * cam_switch_speed, m_switch_milliseconds );
        SmoothMove(m_temp_position,     target_position,    0.001f * static_cast<float>(cam_switch_speed), static_cast<float>(m_switch_milliseconds) );
        SmoothMove(m_temp_rotation,     target_rotation,    0.001f * static_cast<float>(cam_switch_speed), static_cast<float>(m_switch_milliseconds) );
        SmoothMove(m_temp_up_vector,    target_up_vector,   0.001f * static_cast<float>(cam_switch_speed), static_cast<float>(m_switch_milliseconds) );

        // Linear Interpolation of Zoom Values
        double  target_zoom_as_pow = DrOpenGL::zoomScaleToPow( target_camera->getZoom() );
        double  temp_zoom_as_pow =   DrOpenGL::zoomScaleToPow( m_temp_zoom );
        bool    done_with_zoom = false;
        m_temp_zoom = m_switch_zoom;

        if (m_switch_zoom < target_camera->getZoom()) {
            if (m_temp_zoom >= target_camera->getZoom()) done_with_zoom = true;
        } else {
            if (m_temp_zoom <= target_camera->getZoom()) done_with_zoom = true;
        }
        if (done_with_zoom == false) {
            SmoothMove(temp_zoom_as_pow, target_zoom_as_pow, 0.00017 * cam_switch_speed, m_switch_milliseconds);
            m_temp_zoom = DrOpenGL::zoomPowToScale( temp_zoom_as_pow );
        }

        // If distance and zoom are close to target, officially done switching cameras
        if (glm::distance(m_temp_position, target_position) < (0.0001f) && done_with_zoom) {
            m_temp_position =       target_position;
            m_temp_rotation =       target_rotation;
            m_temp_zoom =           DrOpenGL::zoomPowToScale( target_zoom_as_pow );
            m_temp_follow_angle =   target_following_rotation;
            m_temp_up_vector =      target_up_vector;
            m_temp_match =          false;
            m_switching_cameras =   false;
        }
    }
}

// Moves all cameras to their new locations based on their speed, etc
void DrEngineWorld::updateCameras() {
    for (auto camera_pair : m_cameras) {
        camera_pair.second->updateCamera();
    }
}

// Initiates a move to a new camera
void DrEngineWorld::switchCameras(long new_camera) {
    m_switch_milliseconds = 0;
    m_switch_position =     getCameraPosition();
    m_switch_rotation =     getCameraRotation();
        m_switch_rotation.x =   Dr::EqualizeAngle0to360(m_switch_rotation.x);
        m_switch_rotation.y =   Dr::EqualizeAngle0to360(m_switch_rotation.y);
        m_switch_rotation.z =   Dr::EqualizeAngle0to360(m_switch_rotation.z);
    m_switch_zoom =         getCameraZoom();
    m_switch_follow_angle = Dr::EqualizeAngle0to360(getCameraFollowingRotation());
    m_switch_up_vector =    getCameraUpVector();
    m_switch_z_order =      static_cast<double>(getCameraFollowingZ());
    m_switch_match =        getCameraMatching();

    m_temp_position =       m_switch_position;
    m_temp_rotation =       m_switch_rotation;
    m_temp_zoom =           m_switch_zoom;
    m_temp_follow_angle =   m_switch_follow_angle;
    m_temp_up_vector =      m_switch_up_vector;
    m_temp_z_order =        m_switch_z_order;
    m_temp_match =          m_switch_match;

    m_switching_cameras = true;
    m_active_camera = new_camera;
}

void DrEngineWorld::switchCameraToNext(bool only_switch_to_character_cameras, bool switch_player_controls) {
    // Only one camera, cancel switching
    if (m_cameras.size() <= 1) return;


    // Find next available camera
    auto it = m_cameras.find(m_active_camera);
    if (it == m_cameras.end()) it = m_cameras.begin();
    bool found_camera = false;
    do {
        it++;
        if (it == m_cameras.end()) it = m_cameras.begin();              // End of map, loop back around
        if ((*it).second->getKey() == m_active_camera) return;          // Made it back to active camera, cancel switching

        if ((*it).second->wasFollowLost() == false) {
            if ((*it).second->getThingFollowingKey() != c_no_key) {
                DrEngineThing *following = findThingByKey((*it).second->getThingFollowingKey());
                if (following != nullptr) {
                    if (only_switch_to_character_cameras == false || following->compPlayer() != nullptr)
                        found_camera = true;
                }
            } else if (only_switch_to_character_cameras == false) {
                found_camera = true;
            }
        }
    } while (found_camera == false);

    // If not active camera already, switch cameras
    long new_key = (*it).second->getKey();
    if (new_key != m_active_camera) {
        DrEngineThing *thing_new =  nullptr;

        // Switch on new player if there is one
        if (switch_player_controls) {
            if (m_cameras[new_key]->getThingFollowingKey() != c_no_key) {
                thing_new = findThingByKey(m_cameras[new_key]->getThingFollowingKey());
                if (thing_new != nullptr) {

                    // Switch on new player, off other players
                    for (auto &thing : m_things) {
                        if (thing->compPhysics() != nullptr) {
                            if (thing->getKey() != thing_new->getKey()) {
                                thing->compPhysics()->setLostControl(true);
                            } else {
                                thing->compPhysics()->setLostControl(false);
                            }
                        }
                    }
                }
            }
        }

        switchCameras(new_key);
    }
}


//####################################################################################
//##    DrEngineWorld - Camera Property Getters
//##                    #NOTE: Takes into account camera switching
//####################################################################################
// Returns Camera Position
glm::vec3 DrEngineWorld::getCameraPosition() {
    if (m_active_camera == c_no_key) {              return c_default_camera_pos;
    } else if (m_switching_cameras == false) {      return m_cameras[m_active_camera]->getPosition();
    } else {                                        return m_temp_position;
    }
}
double DrEngineWorld::getCameraPositionX() { return static_cast<double>(getCameraPosition().x); }
double DrEngineWorld::getCameraPositionY() { return static_cast<double>(getCameraPosition().y); }
double DrEngineWorld::getCameraPositionZ() { return static_cast<double>(getCameraPosition().z); }

// Returns Camera Rotation, also takes into handle camera switching
glm::vec3 DrEngineWorld::getCameraRotation() {
    if (m_active_camera == c_no_key) {              return c_default_camera_rot;
    } else if (m_switching_cameras == false) {      return m_cameras[m_active_camera]->getRotation();
    } else {                                        return m_temp_rotation;
    }
}
double DrEngineWorld::getCameraRotationX() { return static_cast<double>(getCameraRotation().x); }
double DrEngineWorld::getCameraRotationY() { return static_cast<double>(getCameraRotation().y); }
double DrEngineWorld::getCameraRotationZ() { return static_cast<double>(getCameraRotation().z); }

// Tries to return Rotation of Thing camera is following, if not following returns 0
double DrEngineWorld::getCameraFollowingRotation() {
    if (m_active_camera == c_no_key) {              return 0.0;
    } else if (m_switching_cameras == false) {      return m_cameras[m_active_camera]->getThingFollowingRotation();
    } else {                                        return m_temp_follow_angle;
    }
}

// Tries to return Z Order of Thing camera is following, if not following returns 0
float DrEngineWorld::getCameraFollowingZ() {
    if (m_active_camera == c_no_key) {              return 0.0f;
    } else if (m_switching_cameras == false) {      return m_cameras[m_active_camera]->getThingFollowingZOrder();
    } else {                                        return static_cast<float>(m_temp_z_order);
    }
}

// Returns Camera Zoom
double DrEngineWorld::getCameraZoom() {
    if (m_active_camera == c_no_key) {              return 1.0;
    } else if (m_switching_cameras == false) {      return m_cameras[m_active_camera]->getZoom();
    } else {                                        return m_temp_zoom;
    }
}

// Returns Camera Up Vector
glm::vec3 DrEngineWorld::getCameraUpVector() {
    if (m_active_camera == c_no_key) {              return c_up_vector_y;
    } else if (m_switching_cameras == false) {      return (m_cameras[m_active_camera]->getUpVector() == Up_Vector::Y) ? c_up_vector_y : c_up_vector_z;
    } else {                                        return m_temp_up_vector;
    }
}

// Returns Camera Matching Object Angle
bool DrEngineWorld::getCameraMatching() {
    if (m_active_camera == c_no_key) {              return false;
    } else if (m_switching_cameras == false) {      return (m_cameras[m_active_camera]->getMatchAngle());
    } else {                                        return (m_temp_match || m_switch_match);
    }
}





















