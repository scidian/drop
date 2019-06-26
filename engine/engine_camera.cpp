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
#include "engine_thing_object.h"
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
//##    DrEngineWorld - Camera Functions
//######################################################################################################
// Default parameters: nullptr, 0, 0, 800
long DrEngineWorld::addCamera(long thing_key_to_follow, float x, float y, float z) {
    long new_key = getNextKey();
    DrEngineCamera *camera = new DrEngineCamera(this, new_key, x, y, z);

    m_cameras[new_key] = camera;

    // If an object was passed in, attach camera to that object
    DrEngineThing *follow = findThingByKey(thing_key_to_follow);
    if (follow) {
        camera->followObject( thing_key_to_follow );
        camera->setPositionX( static_cast<float>(follow->getPosition().x()) );
        camera->setPositionY( static_cast<float>(follow->getPosition().y()) );
        camera->setTarget( QVector3D(camera->getPosition().x(), camera->getPosition().y(), z));
        follow->setActiveCameraKey(new_key);
    }
    return new_key;
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
//##    DrEngineWorld - Returns camera position, also takes into handle camera switching
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
DrEngineCamera::DrEngineCamera(DrEngineWorld *world, long unique_key, float x, float y, float z) : m_world(world) {
    m_key = unique_key;
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
    // Movement is based on following an object stored in m_follow
    if (m_follow_key == 0) return;
    DrEngineThing *follow = m_world->findThingByKey(m_follow_key);
    if (follow == nullptr) {                                m_follow_key = 0;   return;     }
    if (follow->getThingType() != DrThingType::Object) {    m_follow_key = 0;   return;     }
    DrEngineObject *object = dynamic_cast<DrEngineObject*>(follow);
    QPointF follow_pos = object->getPosition();
    QPointF follow_previous_pos = object->getPreviousPosition();

    // Calculate the average object Speed
    m_avg_speed_x.push_back( follow_pos.x() - follow_previous_pos.x() );
    m_avg_speed_y.push_back( follow_pos.y() - follow_previous_pos.y() );
    m_avg_speed_x.pop_front();
    m_avg_speed_y.pop_front();
    double average_x = 0;
    double average_y = 0;
    if (m_avg_speed_x.size() > 0) average_x = std::accumulate( m_avg_speed_x.begin(), m_avg_speed_x.end(), 0.0) / m_avg_speed_x.size();
    if (m_avg_speed_y.size() > 0) average_y = std::accumulate( m_avg_speed_y.begin(), m_avg_speed_y.end(), 0.0) / m_avg_speed_y.size();

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
    double pos_x, pos_y;
    pos_x = ((follow_pos.x()) + ((static_cast<double>(m_target.x()) + average_x)*3.0) + (follow_previous_pos.x() + average_x)) / 5.0;
    pos_y = ((follow_pos.y()) + ((static_cast<double>(m_target.y()) + average_y)*3.0) + (follow_previous_pos.y() + average_y)) / 5.0;
    m_target.setX( static_cast<float>(pos_x) );
    m_target.setY( static_cast<float>(pos_y) );
}















