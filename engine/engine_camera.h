//
//      Created by Stephens Nunnally on 5/17/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef ENGINE_CAMERA_H
#define ENGINE_CAMERA_H

#include "engine/globals_engine.h"


//####################################################################################
//##    DrEngineCamera
//##        Holds a Camera for use in the Engine
//############################
class DrEngineCamera
{
private:
    // External Borrowed Pointers
    DrEngineWorld  *m_world;                                    // Pointer to the parent DrEngineWorld

    // Local Variables
    long            m_key;                                      // Unique DrEngineWorld Key of for this camera in the DrEngineWorld EngineCameraMap

    glm::vec3       m_position = glm::vec3(0, 0, 0);            // Current camera position

    glm::vec3       m_speed =    glm::vec3(0, 0, 0);            // Current camera speed

    glm::vec3       m_rotation = glm::vec3(0, 0, 0);            // Current camera rotation
                                                                //              X Rotation, controls up / down
                                                                //              Y Rotation, controls left / right
                                                                //              Z Rotation, tilts head

    DrPointF        m_lag = { 0.0, 0.0 };                       // Current camera lag when following object
    bool            m_match_angle = false;                      // Does this camera match angle of following object
    double          m_distance = c_default_camera_z;            // Distance to look at target
    double          m_zoom = 1.0;                               // Current camera zoom level

    glm::vec3       m_target;                                   // Calculated point this camera is moving towards

    long            m_follow_key = 0;                           // Unique DrEngineWorld key this camera should follow
    bool            m_follow_lost = false;                      // Set to true by engine when was following and now is not following
    int             m_buffer_size = c_slop_buffer_size;         // Number of past object speeds to average together for camera follow
    bool            m_wants_active = false;                     // If set to true in Editor, this camera tries to take Active when in Start Stage

    Up_Vector       m_up_vector = Up_Vector::Y;                 // Up Vector of Camera, usually Y, Z for first person

    std::deque<double>  m_avg_speed_x;                          // Average x speed of object this camera is following
    std::deque<double>  m_avg_speed_y;                          // Average y speed of object this camera is following
    std::deque<double>  m_avg_speed_z;                          // Average z speed of object this camera is following


public:
    // Constructor
    DrEngineCamera(DrEngineWorld *engine_world, long unique_key, float x = 0, float y = 0, float z = 0, int buffer_size = c_slop_buffer_size);

    // Function Calls
    void            moveCamera(const double& milliseconds);
    void            updateCamera();

    // Getters / Setters
    DrEngineWorld*      getEngineWorld()    { return m_world; }
    long                getKey()            { return m_key; }

    DrEngineThing*      getThingFollowing();
    double              getThingFollowingRotation();
    float               getThingFollowingZOrder();
    long                getThingFollowingKey() { return m_follow_key; }
    void                setFollowThing(long follow_key) { m_follow_key = follow_key; }
    bool                wasFollowLost() { return m_follow_lost; }

    const int&          getBufferSize()     { return m_buffer_size; }
    const DrPointF&     getLag()            { return m_lag; }
    const bool&         getMatchAngle()     { return m_match_angle; }
    const glm::vec3&    getPosition()       { return m_position; }
    const glm::vec3&    getRotation()       { return m_rotation; }
    const glm::vec3&    getSpeed()          { return m_speed; }
    Up_Vector           getUpVector()       { return m_up_vector; }
    const bool&         getWantsActive()    { return m_wants_active; }
    const double&       getDistance()       { return m_distance; }
    const double&       getZoom()           { return m_zoom; }

    void                setBufferSize(int slop) { m_buffer_size = (slop < 1) ? 1 : slop; }
    void                setLag(DrPointF lag) { m_lag.x = (lag.x <= 0) ? 0 : lag.x;
                                               m_lag.y = (lag.y <= 0) ? 0 : lag.y; }
    void                setMatchAngle(bool match) { m_match_angle = match; }

    void                setPosition(glm::vec3 new_position) {       m_position = new_position; }
    void                setPosition(double x, double y, double z) { m_position = glm::vec3(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)); }
    void                setPosition(float x, float y, float z) {    m_position = glm::vec3(x, y, z); }
    void                setPosition(int x, int y, int z) {          m_position = glm::vec3(x, y, z); }
    void                setPositionX(float x) { m_position.x = x; }
    void                setPositionY(float y) { m_position.y = y; }
    void                setPositionZ(float z) { m_position.z = z; }

    void                setRotation(glm::vec3 new_rotation) {       m_rotation = new_rotation; }
    void                setRotation(double x, double y, double z) { m_rotation = glm::vec3(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)); }
    void                setRotation(float x, float y, float z) {    m_rotation = glm::vec3(x, y, z); }
    void                setRotation(int x, int y, int z) {          m_rotation = glm::vec3(x, y, z); }
    void                setRotationX(float x) { m_rotation.x = x; }
    void                setRotationY(float y) { m_rotation.y = y; }
    void                setRotationZ(float z) { m_rotation.z = z; }

    void                setSpeed(glm::vec3 new_speed) {          m_speed = new_speed; }
    void                setSpeed(double x, double y, double z) { m_speed = glm::vec3(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)); }
    void                setSpeed(float x, float y, float z) {    m_speed = glm::vec3(x, y, z); }
    void                setSpeedX(float x) { m_speed.x = x; }
    void                setSpeedY(float y) { m_speed.y = y; }
    void                setSpeedZ(float z) { m_speed.z = z; }

    void                setTarget(glm::vec3 target) { m_target = target; }
    void                setUpVector(Up_Vector up) { m_up_vector = up; }
    void                setWantActive(bool wants) { m_wants_active = wants; }
    void                setDistance(double d) { m_distance = d; }
    void                setZoom(double zoom) { m_zoom = zoom; }

};

#endif // ENGINE_CAMERA_H










