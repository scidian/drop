//
//      Created by Stephens Nunnally on 5/17/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <numeric>

#include "engine.h"
#include "engine_camera.h"
#include "engine_things/engine_thing_object.h"
#include "engine_world.h"
#include "form_engine.h"
#include "opengl/opengl.h"

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
//##    Equalizes x, y, and z angles to within 0 to 360
//####################################################################################
static float EqualizeAngle0to360(const float &angle) {
    float equalized = angle;
    while (equalized <   0) { equalized += 360; }
    while (equalized > 360) { equalized -= 360; }
    return equalized;
}

//####################################################################################
//##    Finds closest angle within 180 degrees of angle (both angles must be between 0 to 360)
//####################################################################################
static float FindClosestAngle180(const float &start, const float &angle) {
    float closest = angle;
    if (closest - start > 180)
        closest -= 360;
    else if (start - closest > 180)
        closest += 360;
    return closest;
}


//####################################################################################
//##    DrEngineWorld - Camera Functions
//##        Default parameters: nullptr, 0, 0, c_default_camera_z
//####################################################################################
long DrEngineWorld::addCamera(long thing_key_to_follow, float x, float y, float z) {
    long new_key = getNextKey();
    DrEngineCamera *camera = new DrEngineCamera(this, new_key, x, y, z);

    m_cameras[new_key] = camera;

    // If an object was passed in, attach camera to that object
    DrEngineThing *follow = findThingByKey(thing_key_to_follow);
    if (follow) {
        camera->followObject( thing_key_to_follow );
        camera->setPositionX( static_cast<float>(follow->getPosition().x) + follow->getCameraPosition().x() );
        camera->setPositionY( static_cast<float>(follow->getPosition().y) + follow->getCameraPosition().y() );
        camera->setPositionZ( static_cast<float>(follow->getZOrder() )    + follow->getCameraPosition().z() );
        camera->setRotation(  follow->getCameraRotation() );
        camera->setZoom(      follow->getCameraZoom() );
        camera->setLag(       follow->getCameraLag() );
        camera->setTarget(    camera->getPosition() );
        follow->setActiveCameraKey(new_key);
    }
    return new_key;
}

// Updates all cameras based on the objects they're following
void DrEngineWorld::moveCameras(double milliseconds) {
    for (auto camera_pair : m_cameras) {
        camera_pair.second->moveCamera(milliseconds);

        if (m_switching_cameras) {
            m_switch_milliseconds += milliseconds;

            DrEngineCamera *target_camera = getCamera(m_active_camera);

            // Lerp position
            QVector3D target_position = target_camera->getPosition();
            m_temp_position = m_switch_position;
            SmoothMove(m_temp_position, target_position, 0.001f * static_cast<float>(cam_switch_speed), static_cast<float>(m_switch_milliseconds) );

            // Lerp Rotation
            QVector3D target_rotation = target_camera->getRotation();
                      target_rotation.setX( EqualizeAngle0to360(target_rotation.x()) );
                      target_rotation.setY( EqualizeAngle0to360(target_rotation.y()) );
                      target_rotation.setZ( EqualizeAngle0to360(target_rotation.z()) );
                      target_rotation.setX( FindClosestAngle180(m_temp_rotation.x(), target_rotation.x()) );
                      target_rotation.setY( FindClosestAngle180(m_temp_rotation.y(), target_rotation.y()) );
                      target_rotation.setZ( FindClosestAngle180(m_temp_rotation.z(), target_rotation.z()) );
            m_temp_rotation = m_switch_rotation;
            SmoothMove(m_temp_rotation, target_rotation, 0.001f * static_cast<float>(cam_switch_speed), static_cast<float>(m_switch_milliseconds) );

            m_temp_zoom = m_switch_zoom;
            double target_zoom_as_pow = DrOpenGL::zoomScaleToPow( target_camera->getZoom() );
            double temp_zoom_as_pow =   DrOpenGL::zoomScaleToPow( m_temp_zoom );
            SmoothMove(temp_zoom_as_pow, target_zoom_as_pow, 0.001 * cam_switch_speed, m_switch_milliseconds);
            m_temp_zoom = DrOpenGL::zoomPowToScale( temp_zoom_as_pow );

            if (m_temp_position.distanceToPoint(target_position) < (0.0001f) &&
                abs(temp_zoom_as_pow - target_zoom_as_pow) < 10.0) {
                m_temp_position = target_position;
                m_temp_rotation = target_rotation;
                m_temp_zoom =     DrOpenGL::zoomPowToScale( target_zoom_as_pow );
                m_switching_cameras = false;
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
        m_switch_rotation.setX( EqualizeAngle0to360(m_switch_rotation.x()) );
        m_switch_rotation.setY( EqualizeAngle0to360(m_switch_rotation.y()) );
        m_switch_rotation.setZ( EqualizeAngle0to360(m_switch_rotation.z()) );
    m_switch_zoom = getCameraZoom();

    m_temp_position = m_switch_position;
    m_temp_rotation = m_switch_rotation;
    m_temp_zoom =     m_switch_zoom;

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

        if (only_switch_to_character_cameras) {
            if ((*it).second->getThingFollowing() != 0) {
                DrEngineThing *thing = findThingByKey((*it).second->getThingFollowing());
                if (thing != nullptr && thing->getThingType() == DrThingType::Object) {
                    found_camera = true;
                }
            }
        } else {
            found_camera = true;
        }
    } while (found_camera == false);

    // If not active camera already, switch cameras
    long new_key = (*it).second->getKey();
    if (new_key != m_active_camera) {
        if (m_cameras[m_active_camera]->getThingFollowing() != 0) {
            DrEngineThing *thing = findThingByKey(m_cameras[m_active_camera]->getThingFollowing());
            if (thing != nullptr && thing->getThingType() == DrThingType::Object) {
                DrEngineObject *object = dynamic_cast<DrEngineObject*>(thing);
                if (switch_player_controls) object->setLostControl(true);
            }
        }
        if (m_cameras[new_key]->getThingFollowing() != 0) {
            DrEngineThing *thing = findThingByKey(m_cameras[new_key]->getThingFollowing());
            if (thing != nullptr && thing->getThingType() == DrThingType::Object) {
                DrEngineObject *object = dynamic_cast<DrEngineObject*>(thing);
                if (switch_player_controls) object->setLostControl(false);
            }
        }
        switchCameras(new_key);
    }
}


//####################################################################################
//##    DrEngineWorld - Returns Camera Position, also takes into handle camera switching
//####################################################################################
QVector3D DrEngineWorld::getCameraPosition() {
    if (m_active_camera == 0) {
        return c_default_camera_pos;
    } else if (m_switching_cameras == false) {
        return m_cameras[m_active_camera]->getPosition();
    } else {
        return m_temp_position;
    }
}
double DrEngineWorld::getCameraPositionX() { return static_cast<double>(getCameraPosition().x()); }
double DrEngineWorld::getCameraPositionY() { return static_cast<double>(getCameraPosition().y()); }
double DrEngineWorld::getCameraPositionZ() { return static_cast<double>(getCameraPosition().z()); }


//####################################################################################
//##    DrEngineWorld - Returns Camera Rotation, also takes into handle camera switching
//####################################################################################
QVector3D DrEngineWorld::getCameraRotation() {
    if (m_active_camera == 0) {
        return c_default_camera_rot;
    } else if (m_switching_cameras == false) {
        return m_cameras[m_active_camera]->getRotation();
    } else {
        return m_temp_rotation;
    }
}
double DrEngineWorld::getCameraRotationX() { return static_cast<double>(getCameraRotation().x()); }
double DrEngineWorld::getCameraRotationY() { return static_cast<double>(getCameraRotation().y()); }
double DrEngineWorld::getCameraRotationZ() { return static_cast<double>(getCameraRotation().z()); }

//####################################################################################
//##    DrEngineWorld - Returns Camera Zoom
//####################################################################################
double DrEngineWorld::getCameraZoom() {
    if (m_active_camera == 0) {
        return 1.0;
    } else if (m_switching_cameras == false) {
        return m_cameras[m_active_camera]->getZoom();
    } else {
        return m_temp_zoom;
    }
}

//####################################################################################
//####################################################################################


//####################################################################################
//##    DrEngineCamera - Constructor / Destructor
//####################################################################################
DrEngineCamera::DrEngineCamera(DrEngineWorld *world, long unique_key, float x, float y, float z) : m_world(world) {
    m_key = unique_key;
    m_position = QVector3D(x, y, z);
    m_target = m_position;
    m_avg_speed_x.clear();  m_avg_speed_x.fill(0, 20);          // Zero out average speed vector
    m_avg_speed_y.clear();  m_avg_speed_y.fill(0, 20);          // Zero out average speed vector
    m_avg_speed_z.clear();  m_avg_speed_z.fill(0, 20);          // Zero out average speed vector
    m_speed = QVector3D(0, 0, 0);
    m_rotation = c_default_camera_rot;
}

//####################################################################################
//##    Moves camera based on current speed / settings
//####################################################################################
void DrEngineCamera::moveCamera(const double& milliseconds) {
    double lerp = 0.01 * milliseconds;
    m_position.setX( static_cast<float>( Dr::Lerp( static_cast<double>(m_position.x()), static_cast<double>(m_target.x()), lerp)) );
    m_position.setY( static_cast<float>( Dr::Lerp( static_cast<double>(m_position.y()), static_cast<double>(m_target.y()), lerp)) );
    m_position.setZ( static_cast<float>( Dr::Lerp( static_cast<double>(m_position.z()), static_cast<double>(m_target.z()), lerp)) );
    ///m_position.setX( m_target.x() );
    ///m_position.setY( m_target.y() );
    ///m_position.setY( m_target.z() );
}

//####################################################################################
//##    DrEngineCamera - Update Camera Position
//####################################################################################
void DrEngineCamera::updateCamera() {
    // Movement is based on following an object stored in m_follow
    if (m_follow_key == 0) return;
    DrEngineThing *follow = m_world->findThingByKey(m_follow_key);
    if (follow == nullptr) {                                m_follow_key = 0;   return;     }
    if (follow->getThingType() != DrThingType::Object) {    m_follow_key = 0;   return;     }
    DrEngineObject *object = dynamic_cast<DrEngineObject*>(follow);
    if (object->isDying() || object->isDead()) return;

    double follow_pos_x = object->getPosition().x + static_cast<double>(object->getCameraPosition().x());
    double follow_pos_y = object->getPosition().y + static_cast<double>(object->getCameraPosition().y());
    double follow_pos_z = object->getZOrder() +     static_cast<double>(object->getCameraPosition().z());
    double follow_previous_pos_x = static_cast<double>(object->getPreviousPosition().x) + static_cast<double>(object->getCameraPosition().x());
    double follow_previous_pos_y = static_cast<double>(object->getPreviousPosition().y) + static_cast<double>(object->getCameraPosition().y());
    double follow_previous_pos_z = static_cast<double>(object->getPreviousPosition().z) + static_cast<double>(object->getCameraPosition().z());

    // Calculate the average object Speed
    m_avg_speed_x.push_back( follow_pos_x - follow_previous_pos_x );
    m_avg_speed_y.push_back( follow_pos_y - follow_previous_pos_y );
    m_avg_speed_z.push_back( follow_pos_z - follow_previous_pos_z );
    m_avg_speed_x.pop_front();
    m_avg_speed_y.pop_front();
    m_avg_speed_z.pop_front();
    double average_x = 0;
    double average_y = 0;
    double average_z = 0;
    if (m_avg_speed_x.size() > 0) average_x = std::accumulate( m_avg_speed_x.begin(), m_avg_speed_x.end(), 0.0) / m_avg_speed_x.size();
    if (m_avg_speed_y.size() > 0) average_y = std::accumulate( m_avg_speed_y.begin(), m_avg_speed_y.end(), 0.0) / m_avg_speed_y.size();
    if (m_avg_speed_z.size() > 0) average_z = std::accumulate( m_avg_speed_z.begin(), m_avg_speed_z.end(), 0.0) / m_avg_speed_z.size();

    // Basic Camera = Object Position
    ///double pos_x = follow_pos.x();
    ///double pos_y = follow_pos.y();

    // Move based on Last Camera Position + Average
    ///double pos_x = m_target.x() + average_x;
    ///double pos_y = m_target.y() + average_y;

    // Move based on Last Object Position + Average
    ///double pos_x = follow_previous_pos.x() + average_x;
    ///double pos_y = follow_previous_pos.y() + average_y;

    // Average of all three options, use try catch in case something happens to m_follow during this routine
    double pos_x, pos_y, pos_z;
    pos_x = ((follow_pos_x) + ((static_cast<double>(m_target.x()) + average_x)*3.0) + (follow_previous_pos_x + average_x)) / 5.0;
    pos_y = ((follow_pos_y) + ((static_cast<double>(m_target.y()) + average_y)*3.0) + (follow_previous_pos_y + average_y)) / 5.0;
    pos_z = ((follow_pos_z) + ((static_cast<double>(m_target.z()) + average_z)*3.0) + (follow_previous_pos_z + average_z)) / 5.0;

    // Check for lag
    ///DrPointF cam_pos( static_cast<double>(m_position.x()), static_cast<double>(m_position.y()) );
    ///DrPointF target ( pos_x, pos_y);
    ///if (target.Distance(cam_pos) < m_lag) return;

    m_target.setX( static_cast<float>(pos_x) );
    m_target.setY( static_cast<float>(pos_y) );
    m_target.setZ( static_cast<float>(pos_z) );
}















