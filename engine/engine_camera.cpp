//
//      Created by Stephens Nunnally on 5/17/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "engine.h"
#include "engine_camera.h"

//######################################################################################################
//##    Constructor / Destructor
//######################################################################################################
DrEngineCamera::DrEngineCamera(DrEngine *engine, float x, float y, float z) : m_engine(engine) {
    m_position = QVector3D(x, y, z);
    m_avg_speed_x.clear();
    m_avg_speed_y.clear();
    m_speed = QVector3D(0, 0, 0);
}


//######################################################################################################
//##    Update Camera Position
//######################################################################################################
void DrEngineCamera::updateCamera() {
    if (m_follow != nullptr) {
        updateFollow();
    }
}

void DrEngineCamera::updateFollow() {

    // ***** Camera updates
    ///info = ", " + QString::number( pos.x - getCameraPos().x() );
    ///setCameraPos( static_cast<float>(pos.x), static_cast<float>(pos.y), getCameraPos().z() );


    m_avg_speed_x.push_back( m_follow->position.x() - m_follow->last_position_x );
    m_avg_speed_y.push_back( m_follow->position.y() - m_follow->last_position_y );
    if (m_avg_speed_x.size() > 30) m_avg_speed_x.pop_front();
    if (m_avg_speed_y.size() > 30) m_avg_speed_y.pop_front();

    double average_x = std::accumulate( m_avg_speed_x.begin(), m_avg_speed_x.end(), 0.0) / m_avg_speed_x.size();
    double average_y = std::accumulate( m_avg_speed_y.begin(), m_avg_speed_y.end(), 0.0) / m_avg_speed_y.size();

    m_speed = QVector3D( static_cast<float>(average_x), static_cast<float>(average_y), 0);

    m_engine->info = ", " + QString::number( static_cast<double>(m_speed.x()) );

}










