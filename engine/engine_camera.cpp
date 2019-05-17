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



// ********** Update camera position and load more stages if necessary
DrEngineCamera::updateCamera() {

    if (m_follow != nullptr) {
        updateFollow();
    }


    // ***** Calculate distance and load new stage if we need to
    if (demo_space == Demo_Space::Project && has_scene == true) {
        QTransform t = QTransform().translate(m_game_start.x(), m_game_start.y())
                                   .rotate(m_game_direction)
                                   .translate(-m_game_start.x(), -m_game_start.y());
        QPointF rotated = t.map( QPointF( static_cast<double>(m_camera_pos.x()), static_cast<double>(m_camera_pos.y()) ));
        m_game_distance = rotated.x() - m_game_start.x();
//                info = "Distance: " + QString::number(m_game_distance) + ", Loaded To: " + QString::number(m_loaded_to);

        if (m_loaded_to - m_game_distance < m_load_buffer)
            should_add_stage = true;
    }

}


DrEngineCamera::updateFollow() {

    // ***** Camera updates
    ///info = ", " + QString::number( pos.x - getCameraPos().x() );
    ///setCameraPos( static_cast<float>(pos.x), static_cast<float>(pos.y), getCameraPos().z() );

    m_avg_speed_x.push_back( pos.x - object->last_position_x );
    m_avg_speed_y.push_back( pos.y - object->last_position_y );
    if (avg_speed_x.size() > 30) avg_speed_x.pop_front();
    if (avg_speed_y.size() > 30) avg_speed_y.pop_front();
    object->last_position_x = pos.x;
    object->last_position_y = pos.y;

    double average_x = std::accumulate( avg_speed_x.begin(), avg_speed_x.end(), 0.0) / avg_speed_x.size();
    double average_y = std::accumulate( avg_speed_y.begin(), avg_speed_y.end(), 0.0) / avg_speed_y.size();

    m_camera_speed = QVector3D( static_cast<float>(average_x), static_cast<float>(average_y), 0);

    info = ", " + QString::number( static_cast<double>(m_camera_speed.x()) );

}










