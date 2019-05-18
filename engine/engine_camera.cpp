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

//######################################################################################################
//##    Constructor / Destructor
//######################################################################################################
DrEngineCamera::DrEngineCamera(DrEngine *engine, float x, float y, float z) : m_engine(engine) {
    m_position = QVector3D(x, y, z);
    m_target = m_position;
    m_avg_speed_x.clear();
    m_avg_speed_y.clear();
    m_speed = QVector3D(0, 0, 0);
}


//######################################################################################################
//##    Moves camera based on current speed / settings
//######################################################################################################
// Linearly interpolate (or extrapolate) between f1 and f2 by t percent.
static inline float flerp(float f1, float f2, float t) { return f1*(1.0f - t) + f2*t; }

void DrEngineCamera::moveCamera(float milliseconds) {
    float lerp = .01f;
    m_position.setX( flerp( m_position.x(), m_target.x(), lerp * milliseconds) );
    m_position.setY( flerp( m_position.y(), m_target.y(), lerp * milliseconds) );
    m_position.setZ( flerp( m_position.z(), m_target.z(), lerp * milliseconds) );
}


//######################################################################################################
//##    Update Camera Position
//######################################################################################################
void DrEngineCamera::updateCamera() {

    // Movement is based on following an object
    if (m_follow != nullptr) {
        // Basic Camera = Position
        ///m_engine->info = ", " + QString::number( pos.x - getCameraPos().x() );
        ///setPosition( static_cast<float>(m_follow->position.x()), static_cast<float>(m_follow->position.y()), m_position.z() );

        // Calculate the average object speed
        ///m_avg_speed_x.push_back( m_follow->position.x() - m_follow->last_position_x );
        ///m_avg_speed_y.push_back( m_follow->position.y() - m_follow->last_position_y );
        ///if (m_avg_speed_x.size() > 20) m_avg_speed_x.pop_front();
        ///if (m_avg_speed_y.size() > 20) m_avg_speed_y.pop_front();
        ///double average_x = std::accumulate( m_avg_speed_x.begin(), m_avg_speed_x.end(), 0.0) / m_avg_speed_x.size();
        ///double average_y = std::accumulate( m_avg_speed_y.begin(), m_avg_speed_y.end(), 0.0) / m_avg_speed_y.size();

        float pos_x = static_cast<float>(m_follow->position.x());// + average_x);
        float pos_y = static_cast<float>(m_follow->position.y());// + average_y);

        m_target.setX( pos_x );
        m_target.setY( pos_y );

        ///m_engine->info = ", AX: " + QString::number(average_x);
    }


}

























