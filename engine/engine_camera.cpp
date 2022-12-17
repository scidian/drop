//
//      Created by Stephens Nunnally on 5/17/2019, (c) 2019 Scidian Studios, All Rights Reserved
//
//  File:
//      DrEngineCamera - Class Functions
//
//
#include <numeric>

#include "engine/engine.h"
#include "engine/engine_camera.h"
#include "engine/form_engine.h"
#include "engine/opengl/opengl.h"
#include "engine/thing/components/thing_comp_camera.h"
#include "engine/thing/components/thing_comp_physics.h"
#include "engine/thing/engine_thing.h"
#include "engine/world/engine_world.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
DrEngineCamera::DrEngineCamera(DrEngineWorld *world, long unique_key, float x, float y, float z, int buffer_size) : m_world(world) {
    m_key = unique_key;

    setPosition(glm::vec3(x, y, z) );
    setTarget(  glm::vec3(x, y, z) );
    setSpeed(   glm::vec3(0, 0, 0) );
    setRotation(c_default_camera_rot );

    // Lag slop buffer size
    setBufferSize(buffer_size);

    // Initialize Frame Edges
    m_camera_edges[Edge_Location::Top] =    Frame_Edge::Normal;
    m_camera_edges[Edge_Location::Right] =  Frame_Edge::Normal;
    m_camera_edges[Edge_Location::Bottom] = Frame_Edge::Normal;
    m_camera_edges[Edge_Location::Left] =   Frame_Edge::Normal;
}


//####################################################################################
//##    Get Thing Following / Info
//####################################################################################
DrEngineThing* DrEngineCamera::getThingFollowing() {
    DrEngineThing *follow = m_world->findThingByKey(m_follow_key);
    if (follow == nullptr) {
        if (m_follow_key != c_no_key) {
            m_follow_key = c_no_key;
            m_follow_lost = true;
        }
        return nullptr;
    }
    if (follow->compPhysics() == nullptr) {
        m_follow_key = c_no_key;
        m_follow_lost = true;
        return nullptr;
    }
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
//##    Sets new Zoom setting
//##        Updates related variables if requested, generally upon camera creation
//####################################################################################
void DrEngineCamera::setZoom(double zoom, bool update_speed_zoom) {
    m_zoom = zoom;
    if (update_speed_zoom) {
        m_speed_adjusted_zoom = zoom;
        m_avg_zoom = zoom;
    }
}


//####################################################################################
//##    Moves camera based on current speed / settings
//####################################################################################
void DrEngineCamera::moveCamera(const double& milliseconds) {

    // ********** Update Speed / Position
    // ***** Update by fixed speed if not following an object
    if (m_follow_key == c_no_key) {
        m_position.x = m_position.x + (m_speed.x * static_cast<float>((milliseconds*m_world->getTimeWarp())/1000.0));
        m_position.y = m_position.y + (m_speed.y * static_cast<float>((milliseconds*m_world->getTimeWarp())/1000.0));

    // ***** Otherwise Lerp to new Target Position
    } else {
        double lerp_move = 0.01 * milliseconds;
        m_position.x = static_cast<float>( Dr::Lerp(static_cast<double>(m_position.x), static_cast<double>(m_target.x), lerp_move) );
        m_position.y = static_cast<float>( Dr::Lerp(static_cast<double>(m_position.y), static_cast<double>(m_target.y), lerp_move) );
        m_position.z = static_cast<float>( Dr::Lerp(static_cast<double>(m_position.z), static_cast<double>(m_target.z), lerp_move) );
        ///m_position.x = m_target.x();
        ///m_position.y = m_target.y();
        ///m_position.z = m_target.z();
    }


    // ********** Update Speed Adjusted Zoom
    if (this->getEngineWorld()->zoom_from_movement) {

        // Get average speed if following an object, and that object is alive
        double average_speed = 0.0;
        if (m_follow_key != c_no_key) {
            DrEngineThing *follow = getThingFollowing();
            if (follow != nullptr) {
                ThingCompPhysics *physics = follow->compPhysics();
                if (physics != nullptr) {
                    if (physics->isDying() == false && physics->isDead() == false) {
                        average_speed = static_cast<double>(Dr::Max(abs(m_average_speed.x), abs(m_average_speed.y)));
                    }
                }
            }
        }

        // ***** Calculate average speed per xxx seconds
        double avg_speed_wait_time =    16.0;                                               // 16 milliseconds * 30 == 1/2 second average
        size_t avg_speed_elements =     30;
        m_last_speed_clock += milliseconds;
        while (m_last_speed_clock > avg_speed_wait_time) {
            while (m_avg_speed.size() <= avg_speed_elements) m_avg_speed.push_back( average_speed );
            while (m_avg_speed.size() >  avg_speed_elements) m_avg_speed.pop_front();
            m_last_speed = std::accumulate(m_avg_speed.begin(), m_avg_speed.end(), 0.0) / m_avg_speed.size();
            m_last_speed_clock -= avg_speed_wait_time;
        }

        // ***** Calculate max speed over last xxx seconds
        double max_average_wait_time =  200.0;                                              // xxx is 200 milliseconds * 20 == 4 second average
        size_t max_average_elements =   20;
        m_max_speed_clock += milliseconds;
        while (m_max_speed_clock > max_average_wait_time) {
            // Max speed
            double next_max_speed = *std::max_element(m_avg_speed.begin(), m_avg_speed.end());
            while (m_max_speed.size() <= max_average_elements) m_max_speed.push_back( next_max_speed );
            while (m_max_speed.size() >  max_average_elements) m_max_speed.pop_front();
            double max_speed = *std::max_element(m_max_speed.begin(), m_max_speed.end());
            // Average of max speeds
            while (m_max_average.size() <= max_average_elements) m_max_average.push_back( max_speed );
            while (m_max_average.size() >  max_average_elements) m_max_average.pop_front();
            m_max_speed_average = Dr::Max(std::accumulate(m_max_average.begin(), m_max_average.end(), 0.0) / m_max_average.size(), 1.0);
            m_max_speed_clock -= max_average_wait_time;
        }

        // ***** Figure out percentage of current speed vs max speed, apply percentage to multiplier for target zoom
        double speed_adjust = Dr::Clamp(m_last_speed / m_max_speed_average, 0.0, 1.0);
        double multiplier = this->getEngineWorld()->zoom_multiplier;
        if (this->getEngineWorld()->zoom_type == Auto_Zoom::Zoom_Out) multiplier = 1.0 / multiplier;
        multiplier = Dr::Lerp(1.0, multiplier, speed_adjust);
        double target_zoom = m_zoom * multiplier;

        // ***** Calculate average target zoom per xxx seconds
        double avg_zoom_wait_time =     10.0 * this->getEngineWorld()->zoom_damping;        // 10 to 100 milliseconds * 60 == 3/5 of a second to 6 second average
        size_t avg_zoom_elements =      60;
        m_avg_zoom_clock += milliseconds;
        while (m_avg_zoom_clock > avg_zoom_wait_time) {
            while (m_target_zoom.size() <= avg_zoom_elements) m_target_zoom.push_back( target_zoom );
            while (m_target_zoom.size() >  avg_zoom_elements) m_target_zoom.pop_front();
            m_avg_zoom = std::accumulate(m_target_zoom.begin(), m_target_zoom.end(), 0.0) / m_target_zoom.size();
            m_avg_zoom_clock -= avg_zoom_wait_time;
        }

        // Debug Output
        ///g_info = "Target Zoom: " + std::to_string(m_avg_zoom) + ", Average Speed: " + std::to_string(m_last_speed) +
        ///       ", Max Average: " + std::to_string(m_max_speed_average);

        // Lerp as power for smoother tweening
        double lerp_zoom = (0.01 / this->getEngineWorld()->zoom_damping) * milliseconds;
        double target_zoom_as_pow = DrOpenGL::zoomScaleToPow( m_avg_zoom );
        double new_zoom_as_pow =    Dr::Lerp(DrOpenGL::zoomScaleToPow(m_speed_adjusted_zoom), target_zoom_as_pow, lerp_zoom);
        m_speed_adjusted_zoom =     DrOpenGL::zoomPowToScale(new_zoom_as_pow);
        // Alternate no power conversion
        ///m_speed_adjusted_zoom =     Dr::Lerp(m_speed_adjusted_zoom, avg_zoom, lerp_zoom);
    } else {
        m_speed_adjusted_zoom = m_zoom;
    }
}


//####################################################################################
//##    DrEngineCamera - Update Camera Position
//####################################################################################
void DrEngineCamera::updateCamera() {
    // Movement is based on following an object stored in m_follow
    if (m_follow_key == c_no_key) return;
    DrEngineThing *follow = getThingFollowing();                        if (follow == nullptr) return;
    ThingCompPhysics *physics = follow->compPhysics();                  if (physics == nullptr) return;
    ThingCompCamera *comp_camera = follow->compCamera();                if (comp_camera == nullptr) return;
    if (physics->isDying() || physics->isDead()) return;

    double follow_pos_x = follow->getPosition().x + static_cast<double>(comp_camera->getCameraPosition().x);
    double follow_pos_y = follow->getPosition().y + static_cast<double>(comp_camera->getCameraPosition().y);
    double follow_pos_z = follow->getZOrder() +     static_cast<double>(comp_camera->getCameraPosition().z);
    double follow_previous_pos_x = static_cast<double>(physics->getPreviousPosition().x) + static_cast<double>(comp_camera->getCameraPosition().x);
    double follow_previous_pos_y = static_cast<double>(physics->getPreviousPosition().y) + static_cast<double>(comp_camera->getCameraPosition().y);
    double follow_previous_pos_z = static_cast<double>(physics->getPreviousPosition().z) + static_cast<double>(comp_camera->getCameraPosition().z);

    // Check for Lag Bounding Box
    bool update_x = false;
    bool update_y = false;
    bool update_z = true;
    if (abs(static_cast<double>(m_position.x) - follow_pos_x) > (m_lag.x / 2.0)) update_x = true;
    if (abs(static_cast<double>(m_position.y) - follow_pos_y) > (m_lag.y / 2.0)) update_y = true;

    // Calculate the average object Speed
    m_average_speed.x = 0;
    m_average_speed.y = 0;
    m_average_speed.z = 0;
    if (update_x) {
        while (m_avg_speed_x.size() < static_cast<unsigned long>(m_buffer_size + 1)) m_avg_speed_x.push_back( follow_pos_x - follow_previous_pos_x );
        while (m_avg_speed_x.size() > m_buffer_size) m_avg_speed_x.pop_front();
        m_avg_speed_x.pop_front();
        if (m_avg_speed_x.size() > 0) m_average_speed.x = static_cast<float>(std::accumulate(m_avg_speed_x.begin(), m_avg_speed_x.end(), 0.0) / m_avg_speed_x.size());
    }
    if (update_y) {
        while (m_avg_speed_y.size() < static_cast<unsigned long>(m_buffer_size + 1)) m_avg_speed_y.push_back( follow_pos_y - follow_previous_pos_y );
        while (m_avg_speed_y.size() > m_buffer_size) m_avg_speed_y.pop_front();
        if (m_avg_speed_y.size() > 0) m_average_speed.y = static_cast<float>(std::accumulate(m_avg_speed_y.begin(), m_avg_speed_y.end(), 0.0) / m_avg_speed_y.size());
    }
    if (update_z) {
        while (m_avg_speed_z.size() < static_cast<unsigned long>(m_buffer_size + 1)) m_avg_speed_z.push_back( follow_pos_z - follow_previous_pos_z );
        while (m_avg_speed_z.size() > m_buffer_size) m_avg_speed_z.pop_front();
        if (m_avg_speed_z.size() > 0) m_average_speed.z = static_cast<float>(std::accumulate(m_avg_speed_z.begin(), m_avg_speed_z.end(), 0.0) / m_avg_speed_z.size());
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
    pos_x += static_cast<double>(m_target.x + m_average_speed.x) * 3.0;
    pos_y += static_cast<double>(m_target.y + m_average_speed.y) * 3.0;
    pos_z += static_cast<double>(m_target.z + m_average_speed.z) * 3.0;
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

    if (update_x) m_target.x = static_cast<float>(pos_x);
    if (update_y) m_target.y = static_cast<float>(pos_y);
    if (update_z) m_target.z = static_cast<float>(pos_z);
}



















