//
//      Created by Stephens Nunnally on 5/17/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <numeric>

#include "engine/engine.h"
#include "engine/engine_camera.h"
#include "engine/form_engine.h"
#include "engine/opengl/opengl.h"
#include "engine/things/engine_thing_object.h"
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
static inline void SmoothMove(QVector3D &start, const QVector3D &target, const float &lerp, const float &milliseconds) {
    float lerp_amount = Dr::Clamp(lerp * milliseconds, 0.001f, 1.0f);
    start.setX( Dr::Lerp(start.x(), target.x(), lerp_amount) );
    start.setY( Dr::Lerp(start.y(), target.y(), lerp_amount) );
    start.setZ( Dr::Lerp(start.z(), target.z(), lerp_amount) );

    ///start.setX( Dr::LerpConst(start.x(), target.x(), lerp_amount * 100.0f) );
    ///start.setY( Dr::LerpConst(start.y(), target.y(), lerp_amount * 100.0f) );
    ///start.setZ( Dr::LerpConst(start.z(), target.z(), lerp_amount * 100.0f) );
}

static inline void SmoothMove(double &start, const double &target, const double &lerp, const double &milliseconds) {
    double  lerp_amount = Dr::Clamp(lerp * milliseconds, 0.001, 1.0);
    start = Dr::Lerp(start, target, lerp_amount);
    ///start = Dr::LerpConst(start, target, lerp_amount * 100.0);
}


//####################################################################################
//####################################################################################
//##
//##    DrEngineWorld - Camera Functions
//##
//####################################################################################
//####################################################################################
// Adds Camera to World, Default parameters: nullptr, 0, 0, c_default_camera_z
DrEngineCamera* DrEngineWorld::addCamera(long thing_key_to_follow, float x, float y, float z, int buffer_size) {
    long new_key = getNextKey();
    DrEngineCamera *camera = new DrEngineCamera(this, new_key, x, y, z, buffer_size);

    m_cameras[new_key] = camera;

    // If an object was passed in, attach camera to that object
    DrEngineThing *follow = findThingByKey(thing_key_to_follow);
    if (thing_key_to_follow != 0 && follow != nullptr) {
        camera->setFollowThing( thing_key_to_follow );
        camera->setPositionX(   static_cast<float>(follow->getPosition().x) + follow->getCameraPosition().x() );
        camera->setPositionY(   static_cast<float>(follow->getPosition().y) + follow->getCameraPosition().y() );
        camera->setPositionZ(   static_cast<float>(follow->getZOrder() )    + follow->getCameraPosition().z() );
        camera->setRotation(    follow->getCameraRotation() );
        camera->setZoom(        follow->getCameraZoom() );
        camera->setLag(         follow->getCameraLag() );
        camera->setMatchAngle(  follow->getCameraMatch() );
        camera->setTarget(      camera->getPosition() );
        camera->setUpVector(    follow->getCameraUpVector() );
        follow->setActiveCameraKey(new_key);
    }
    return camera;
}

// Updates all cameras based on the objects they're following
void DrEngineWorld::moveCameras(double milliseconds) {
    for (auto camera_pair : m_cameras) {
        camera_pair.second->moveCamera(milliseconds);

        if (m_switching_cameras) {
            m_switch_milliseconds += milliseconds;

            DrEngineCamera *target_camera = getCamera(m_active_camera);           

            // Linear Interpolation of temporary values
            QVector3D target_position = target_camera->getPosition();
            QVector3D target_rotation = target_camera->getRotation();
                      target_rotation.setX( Dr::EqualizeAngle0to360(target_rotation.x()) );
                      target_rotation.setY( Dr::EqualizeAngle0to360(target_rotation.y()) );
                      target_rotation.setZ( Dr::EqualizeAngle0to360(target_rotation.z()) );
                      target_rotation.setX( Dr::FindClosestAngle180(m_temp_rotation.x(), target_rotation.x()) );
                      target_rotation.setY( Dr::FindClosestAngle180(m_temp_rotation.y(), target_rotation.y()) );
                      target_rotation.setZ( Dr::FindClosestAngle180(m_temp_rotation.z(), target_rotation.z()) );
            QVector3D target_up_vector = (target_camera->getUpVector() == Up_Vector::Y) ? c_up_vector_y : c_up_vector_z;
            double    target_following_rotation = target_camera->getThingFollowingRotation();
                      target_following_rotation = Dr::EqualizeAngle0to360(target_following_rotation);
                      target_following_rotation = Dr::FindClosestAngle180(m_temp_follow_angle, target_following_rotation);
            double    target_z_order = static_cast<double>(target_camera->getThingFollowingZOrder());
            double    target_zoom_as_pow = DrOpenGL::zoomScaleToPow( target_camera->getZoom() );
            double    temp_zoom_as_pow =   DrOpenGL::zoomScaleToPow( m_temp_zoom );
            m_temp_follow_angle =   m_switch_follow_angle;
            m_temp_position =       m_switch_position;
            m_temp_rotation =       m_switch_rotation;
            m_temp_up_vector =      m_switch_up_vector;
            m_temp_z_order =        m_switch_z_order;
            m_temp_zoom =           m_switch_zoom;
            m_temp_match =          target_camera->getMatchAngle();

            SmoothMove(m_temp_follow_angle, target_following_rotation,  0.001 * cam_switch_speed, m_switch_milliseconds );
            SmoothMove(m_temp_z_order,      target_z_order,             0.001 * cam_switch_speed, m_switch_milliseconds );
            SmoothMove(m_temp_position,     target_position,    0.001f * static_cast<float>(cam_switch_speed), static_cast<float>(m_switch_milliseconds) );
            SmoothMove(m_temp_rotation,     target_rotation,    0.001f * static_cast<float>(cam_switch_speed), static_cast<float>(m_switch_milliseconds) );
            SmoothMove(m_temp_up_vector,    target_up_vector,   0.001f * static_cast<float>(cam_switch_speed), static_cast<float>(m_switch_milliseconds) );
            SmoothMove(temp_zoom_as_pow, target_zoom_as_pow, 0.001 * cam_switch_speed, m_switch_milliseconds);
            m_temp_zoom = DrOpenGL::zoomPowToScale( temp_zoom_as_pow );

            if (m_temp_position.distanceToPoint(target_position) < (0.0001f) &&
                abs(temp_zoom_as_pow - target_zoom_as_pow) < 10.0) {
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
    m_switch_position = getCameraPosition();
    m_switch_rotation = getCameraRotation();
        m_switch_rotation.setX( Dr::EqualizeAngle0to360(m_switch_rotation.x()) );
        m_switch_rotation.setY( Dr::EqualizeAngle0to360(m_switch_rotation.y()) );
        m_switch_rotation.setZ( Dr::EqualizeAngle0to360(m_switch_rotation.z()) );
    m_switch_zoom = getCameraZoom();
    m_switch_follow_angle = getCameraFollowingRotation();
        m_switch_follow_angle = Dr::EqualizeAngle0to360(m_switch_follow_angle);
    m_switch_up_vector = getCameraUpVector();
    m_switch_z_order = static_cast<double>(getCameraFollowingZ());
    m_switch_match = getCameraMatching();

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
    bool found_camera = false;
    do {
        it++;
        if (it == m_cameras.end()) it = m_cameras.begin();              // End of map, loop back around
        if ((*it).second->getKey() == m_active_camera) return;          // Made it back to active camera, cancel switching

        if ((*it).second->wasFollowLost() == false) {
            if ((*it).second->getThingFollowingKey() != 0) {
                DrEngineThing *following = findThingByKey((*it).second->getThingFollowingKey());
                if (following != nullptr) {
                    if (only_switch_to_character_cameras == false || following->getThingType() == DrThingType::Object)
                        found_camera = true;
                }
            } else if (only_switch_to_character_cameras == false) {
                found_camera = true;
            }
        }
    } while (found_camera == false);

    g_info = "Cam Following Key: " + QString::number((*it).second->getThingFollowingKey());

    // If not active camera already, switch cameras
    long new_key = (*it).second->getKey();
    if (new_key != m_active_camera) {

        DrEngineThing  *thing_new =  nullptr;
        DrEngineObject *object_new = nullptr;

        // Switch on new player if there is one
        bool switched = false;
        if (m_cameras[new_key]->getThingFollowingKey() != 0) {
            thing_new = findThingByKey(m_cameras[new_key]->getThingFollowingKey());
            if (thing_new != nullptr && thing_new->getThingType() == DrThingType::Object) {
                object_new = dynamic_cast<DrEngineObject*>(thing_new);
                if (switch_player_controls && object_new != nullptr) {
                    object_new->setLostControl(false);
                    switched = true;
                }
            }
        }

        // Switch off other players if we switched to new player
        if (switched) {
            for (auto &thing : m_things) {
                if (thing->getKey() != thing_new->getKey()) {
                    if (thing_new->getThingType() == DrThingType::Object) {
                        DrEngineObject *object = dynamic_cast<DrEngineObject*>(thing);
                        if (object != nullptr) object->setLostControl(true);
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
QVector3D DrEngineWorld::getCameraPosition() {
    if (m_active_camera == 0) {                     return c_default_camera_pos;
    } else if (m_switching_cameras == false) {      return m_cameras[m_active_camera]->getPosition();
    } else {                                        return m_temp_position;
    }
}
double DrEngineWorld::getCameraPositionX() { return static_cast<double>(getCameraPosition().x()); }
double DrEngineWorld::getCameraPositionY() { return static_cast<double>(getCameraPosition().y()); }
double DrEngineWorld::getCameraPositionZ() { return static_cast<double>(getCameraPosition().z()); }

// Returns Camera Rotation, also takes into handle camera switching
QVector3D DrEngineWorld::getCameraRotation() {
    if (m_active_camera == 0) {                     return c_default_camera_rot;
    } else if (m_switching_cameras == false) {      return m_cameras[m_active_camera]->getRotation();
    } else {                                        return m_temp_rotation;
    }
}
double DrEngineWorld::getCameraRotationX() { return static_cast<double>(getCameraRotation().x()); }
double DrEngineWorld::getCameraRotationY() { return static_cast<double>(getCameraRotation().y()); }
double DrEngineWorld::getCameraRotationZ() { return static_cast<double>(getCameraRotation().z()); }

// Tries to return Rotation of Thing camera is following, if not following returns 0
double DrEngineWorld::getCameraFollowingRotation() {
    if (m_active_camera == 0) {                     return 0.0;
    } else if (m_switching_cameras == false) {      return m_cameras[m_active_camera]->getThingFollowingRotation();
    } else {                                        return m_temp_follow_angle;
    }
}

// Tries to return Z Order of Thing camera is following, if not following returns 0
float DrEngineWorld::getCameraFollowingZ() {
    if (m_active_camera == 0) {                     return 0.0f;
    } else if (m_switching_cameras == false) {      return m_cameras[m_active_camera]->getThingFollowingZOrder();
    } else {                                        return static_cast<float>(m_temp_z_order);
    }
}

// Returns Camera Zoom
double DrEngineWorld::getCameraZoom() {
    if (m_active_camera == 0) {                     return 1.0;
    } else if (m_switching_cameras == false) {      return m_cameras[m_active_camera]->getZoom();
    } else {                                        return m_temp_zoom;
    }
}

// Returns Camera Up Vector
QVector3D DrEngineWorld::getCameraUpVector() {
    if (m_active_camera == 0) {                     return c_up_vector_y;
    } else if (m_switching_cameras == false) {      return (m_cameras[m_active_camera]->getUpVector() == Up_Vector::Y) ? c_up_vector_y : c_up_vector_z;
    } else {                                        return m_temp_up_vector;
    }
}

// Returns Camera Matching Object Angle
bool DrEngineWorld::getCameraMatching() {
    if (m_active_camera == 0) {                     return false;
    } else if (m_switching_cameras == false) {      return (m_cameras[m_active_camera]->getMatchAngle());
    } else {                                        return (m_temp_match || m_switch_match);
    }
}

//####################################################################################
//####################################################################################
//##
//##    DrEngineCamera - Class Functions
//##
//####################################################################################
//####################################################################################

//####################################################################################
//##    Constructor / Destructor
//####################################################################################
DrEngineCamera::DrEngineCamera(DrEngineWorld *world, long unique_key, float x, float y, float z, int buffer_size) : m_world(world) {
    m_key = unique_key;

    setPosition(    QVector3D(x, y, z) );
    setTarget(      QVector3D(x, y, z) );
    setSpeed(       QVector3D(0, 0, 0) );
    setRotation(    c_default_camera_rot );

    setBufferSize(  buffer_size);
    m_avg_speed_x.clear();  m_avg_speed_x.fill(0, buffer_size);                 // Zero out average speed vector
    m_avg_speed_y.clear();  m_avg_speed_y.fill(0, buffer_size);                 // Zero out average speed vector
    m_avg_speed_z.clear();  m_avg_speed_z.fill(0, buffer_size);                 // Zero out average speed vector
}


//####################################################################################
//##    Get Thing Following / Info
//####################################################################################
DrEngineThing* DrEngineCamera::getThingFollowing() {
    DrEngineThing *follow = m_world->findThingByKey(m_follow_key);
    if (follow == nullptr) {
        if (m_follow_key != 0) {
            m_follow_key = 0;
            m_follow_lost = true;
        }
        return nullptr;
    }
    if (follow->getThingType() != DrThingType::Object) {    m_follow_key = 0;   m_follow_lost = true;   return nullptr;     }
    return follow;
}

double DrEngineCamera::getThingFollowingRotation() {
    DrEngineThing *follow = getThingFollowing();
    if (follow == nullptr) return 0;
    return follow->getAngle();
}

float DrEngineCamera::getThingFollowingZOrder() {
    DrEngineThing *follow = getThingFollowing();
    if (follow == nullptr) return 0.0;
    return static_cast<float>(follow->getZOrder());
}

//####################################################################################
//##    Moves camera based on current speed / settings
//####################################################################################
void DrEngineCamera::moveCamera(const double& milliseconds) {
    // Update by fixed speed if not following an object
    if (m_follow_key == 0) {
        m_position.setX( m_position.x() + (m_speed.x() * static_cast<float>((milliseconds*m_world->getTimeWarp())/1000.0)) );
        m_position.setY( m_position.y() + (m_speed.y() * static_cast<float>((milliseconds*m_world->getTimeWarp())/1000.0)) );

    // Otherwise Lerp to new Target
    } else {
        double lerp = 0.01 * milliseconds;
        m_position.setX( static_cast<float>( Dr::Lerp( static_cast<double>(m_position.x()), static_cast<double>(m_target.x()), lerp)) );
        m_position.setY( static_cast<float>( Dr::Lerp( static_cast<double>(m_position.y()), static_cast<double>(m_target.y()), lerp)) );
        m_position.setZ( static_cast<float>( Dr::Lerp( static_cast<double>(m_position.z()), static_cast<double>(m_target.z()), lerp)) );
        ///m_position.setX( m_target.x() );
        ///m_position.setY( m_target.y() );
        ///m_position.setY( m_target.z() );
    }
}

//####################################################################################
//##    DrEngineCamera - Update Camera Position
//####################################################################################
void DrEngineCamera::updateCamera() {        
    // Movement is based on following an object stored in m_follow
    if (m_follow_key == 0) return;
    DrEngineThing *follow = getThingFollowing();
    if (follow == nullptr) return;
    DrEngineObject *object = dynamic_cast<DrEngineObject*>(follow);
    if (object->isDying() || object->isDead()) return;

    double follow_pos_x = object->getPosition().x + static_cast<double>(object->getCameraPosition().x());
    double follow_pos_y = object->getPosition().y + static_cast<double>(object->getCameraPosition().y());
    double follow_pos_z = object->getZOrder() +     static_cast<double>(object->getCameraPosition().z());
    double follow_previous_pos_x = static_cast<double>(object->getPreviousPosition().x) + static_cast<double>(object->getCameraPosition().x());
    double follow_previous_pos_y = static_cast<double>(object->getPreviousPosition().y) + static_cast<double>(object->getCameraPosition().y());
    double follow_previous_pos_z = static_cast<double>(object->getPreviousPosition().z) + static_cast<double>(object->getCameraPosition().z());

    // Check for Lag Bounding Box
    bool update_x = false;
    bool update_y = false;
    bool update_z = true;
    if (abs(static_cast<double>(m_position.x()) - follow_pos_x) > m_lag.x) update_x = true;
    if (abs(static_cast<double>(m_position.y()) - follow_pos_y) > m_lag.y) update_y = true;

    // Calculate the average object Speed
    double average_x = 0;
    double average_y = 0;
    double average_z = 0;
    if (update_x) {
        while (m_avg_speed_x.count() < m_buffer_size + 1)
            m_avg_speed_x.push_back( follow_pos_x - follow_previous_pos_x );
        m_avg_speed_x.pop_front();
        if (m_avg_speed_x.size() > 0) average_x = std::accumulate( m_avg_speed_x.begin(), m_avg_speed_x.end(), 0.0) / m_avg_speed_x.size();
    }
    if (update_y) {
        while (m_avg_speed_y.count() < m_buffer_size + 1)
            m_avg_speed_y.push_back( follow_pos_y - follow_previous_pos_y );
        m_avg_speed_y.pop_front();
        if (m_avg_speed_y.size() > 0) average_y = std::accumulate( m_avg_speed_y.begin(), m_avg_speed_y.end(), 0.0) / m_avg_speed_y.size();
    }
    if (update_z) {
        while (m_avg_speed_z.count() < m_buffer_size + 1)
            m_avg_speed_z.push_back( follow_pos_z - follow_previous_pos_z );
        m_avg_speed_z.pop_front();
        if (m_avg_speed_z.size() > 0) average_z = std::accumulate( m_avg_speed_z.begin(), m_avg_speed_z.end(), 0.0) / m_avg_speed_z.size();
    }

    // Average
    double pos_x = 0.0;
    double pos_y = 0.0;
    double pos_z = 0.0;
    double total = 0.0;

    // Basic Camera = Object Position    
    ///pos_x += follow_pos_x;
    ///pos_y += follow_pos_y;
    ///pos_z += follow_pos_z;
    ///total += 1.0;

    // Move based on Last Camera Position + Average
    pos_x += (static_cast<double>(m_target.x()) + average_x) * 3.0;
    pos_y += (static_cast<double>(m_target.y()) + average_y) * 3.0;
    pos_z += (static_cast<double>(m_target.z()) + average_z) * 3.0;
    total += 3.0;

    // Move based on Last Object Position + Average
    ///pos_x += follow_previous_pos_x + average_x;
    ///pos_y += follow_previous_pos_y + average_y;
    ///pos_z += follow_previous_pos_z + average_z;
    ///total += 1.0;

    // Average of all three options, use try catch in case something happens to m_follow during this routine
    pos_x /= total;
    pos_y /= total;
    pos_z /= total;

    if (update_x) m_target.setX( static_cast<float>(pos_x) );
    if (update_y) m_target.setY( static_cast<float>(pos_y) );
    if (update_z) m_target.setZ( static_cast<float>(pos_z) );
}















