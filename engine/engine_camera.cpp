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
#include "engine_object.h"
#include "engine_world.h"
#include "form_engine.h"
#include "opengl/opengl.h"

//######################################################################################################
//##    Clamps and Flerps
//######################################################################################################
// Linearly interpolate (or extrapolate) between f1 and f2 by t percent
static inline float  Drflerp(float f1, float f2, float t) { return f1*(1.0f - t) + f2*t; }
static inline double Drdlerp(double f1, double f2, double t) { return f1*(1.0 - t) + f2*t; }

static inline void SmoothMove(QVector3D& start, const QVector3D &target, const float& lerp, const float& milliseconds) {
    start.setX( Drflerp( start.x(), target.x(), lerp * milliseconds) );
    start.setY( Drflerp( start.y(), target.y(), lerp * milliseconds) );
    start.setZ( Drflerp( start.z(), target.z(), lerp * milliseconds) );
}

///static inline void smoothMoveMaxSpeed(QVector3D& start, const QVector3D &target, const float& max) {
///    start.setX( drflerpconst( start.x(), target.x(), max ));
///    start.setY( drflerpconst( start.y(), target.y(), max ));
///    start.setZ( drflerpconst( start.z(), target.z(), max ));
///}


//######################################################################################################
//##    DrEngine - Camera Functions
//######################################################################################################
// Default parameters: nullptr, 0, 0, 800
long DrEngineWorld::addCamera(DrEngineObject* object_to_follow, float x, float y, float z) {
    DrEngineCamera *camera = new DrEngineCamera(this, x, y, z);
    camera->setCameraKey(m_camera_keys);

    m_cameras[m_camera_keys] = camera;

    // If an object was passed in, attach camera to that object
    if (object_to_follow != nullptr) {
        camera->followObject(object_to_follow);
        object_to_follow->setActiveCameraKey(m_camera_keys);
        camera->setPositionX( static_cast<float>(object_to_follow->getBodyPosition().x()) );
        camera->setPositionY( static_cast<float>(object_to_follow->getBodyPosition().y()) );
        camera->setTarget( QVector3D(camera->getPosition().x(), camera->getPosition().y(), z));
    }
    // Increment camera ID generator, return current camera ID
    m_camera_keys++;
    return (m_camera_keys - 1);
}

// Removes all cameras from engine
void DrEngineWorld::clearCameras() {
    for (auto camera_pair : m_cameras)
        delete camera_pair.second;
    m_cameras.clear();
}

// Updates all cameras based on the objects they're following
void DrEngineWorld::moveCameras(double milliseconds) {
    for (auto camera_pair : m_cameras) {
        camera_pair.second->moveCamera(milliseconds);

        if (m_switching_cameras) {
            QVector3D target = getCamera(m_active_camera)->getPosition();
            SmoothMove(m_temp_position, target, .0050f, static_cast<float>(milliseconds) );
            ///smoothMoveMaxSpeed(m_temp_position, target, 4.0f);
            if ( m_temp_position.distanceToPoint(target) < 1 ) m_switching_cameras = false;
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
    m_temp_position = getCameraPos();
    m_switching_cameras = true;
    m_active_camera = new_camera;
}


//######################################################################################################
//##    DrEngine - Returns camera position, also takes into handle camera switching
//######################################################################################################
QVector3D DrEngineWorld::getCameraPos() {
    if (m_active_camera == 0) {
        return c_default_camera_pos;
    } else if (m_switching_cameras == false) {
        return m_cameras[m_active_camera]->getPosition();
    } else {
        return m_temp_position;
    }
}
double DrEngineWorld::getCameraPosXD() { return static_cast<double>(getCameraPos().x()); }
double DrEngineWorld::getCameraPosYD() { return static_cast<double>(getCameraPos().y()); }
double DrEngineWorld::getCameraPosZD() { return static_cast<double>(getCameraPos().z()); }

//######################################################################################################
//######################################################################################################


//######################################################################################################
//##    DrEngineCamera - Constructor / Destructor
//######################################################################################################
DrEngineCamera::DrEngineCamera(DrEngineWorld *world, float x, float y, float z) : m_world(world) {
    m_position = QVector3D(x, y, z);
    m_target = m_position;
    m_avg_speed_x.clear();  m_avg_speed_x.fill(0, 20);
    m_avg_speed_y.clear();  m_avg_speed_y.fill(0, 20);
    m_speed = QVector3D(0, 0, 0);
}

//######################################################################################################
//##    Moves camera based on current speed / settings
//######################################################################################################
void DrEngineCamera::moveCamera(const double& milliseconds) {
    double lerp = 0.01 * milliseconds;
    m_position.setX( static_cast<float>(Drdlerp( static_cast<double>(m_position.x()), static_cast<double>(m_target.x()), lerp)) );
    m_position.setY( static_cast<float>(Drdlerp( static_cast<double>(m_position.y()), static_cast<double>(m_target.y()), lerp)) );
    m_position.setZ( static_cast<float>(Drdlerp( static_cast<double>(m_position.z()), static_cast<double>(m_target.z()), lerp)) );
    ///m_position.setX( m_target.x() );
    ///m_position.setY( m_target.y() );
}

//######################################################################################################
//##    DrEngineCamera - Update Camera Position
//######################################################################################################
void DrEngineCamera::updateCamera() {
    // Movement is based on following an object
    if (m_follow != nullptr) {

        // Calculate the average object Speed
        m_avg_speed_x.push_back( m_follow->getBodyPosition().x() - m_follow->getBodyPreviousPosition().x() );
        m_avg_speed_y.push_back( m_follow->getBodyPosition().y() - m_follow->getBodyPreviousPosition().y() );
        m_avg_speed_x.pop_front();
        m_avg_speed_y.pop_front();
        double average_x = 0;
        double average_y = 0;
        if (m_avg_speed_x.size() > 0) average_x = std::accumulate( m_avg_speed_x.begin(), m_avg_speed_x.end(), 0.0) / m_avg_speed_x.size();
        if (m_avg_speed_y.size() > 0) average_y = std::accumulate( m_avg_speed_y.begin(), m_avg_speed_y.end(), 0.0) / m_avg_speed_y.size();

        // Basic Camera = Object Position
        ///double pos_x = m_follow->getBodyPosition().x();
        ///double pos_y = m_follow->getBodyPosition().y();
        // Move based on Last Camera Position + Average
        ///double pos_x = m_target.x() + average_x;
        ///double pos_y = m_target.y() + average_y;
        // Move based on Last Object Position + Average
        ///double pos_x = m_follow->getBodyPreviousPosition().x() + average_x;
        ///double pos_y = m_follow->getBodyPreviousPosition().y() + average_y;

        // Average of all three options
        double pos_x = ((m_follow->getBodyPosition().x()) + ((static_cast<double>(m_target.x()) + average_x)*3.0) +
                        (m_follow->getBodyPreviousPosition().x() + average_x)) / 5.0;
        double pos_y = ((m_follow->getBodyPosition().y()) + ((static_cast<double>(m_target.y()) + average_y)*3.0) +
                        (m_follow->getBodyPreviousPosition().y() + average_y)) / 5.0;

        m_target.setX( static_cast<float>(pos_x) );
        m_target.setY( static_cast<float>(pos_y) );

    }
}















