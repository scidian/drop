//
//      Created by Stephens Nunnally on 5/17/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef ENGINE_CAMERA_H
#define ENGINE_CAMERA_H

#include "constants_engine.h"
#include "globals_engine.h"

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

    QVector3D       m_position = QVector3D(0, 0, 0);            // Current camera position

    QVector3D       m_speed =    QVector3D(0, 0, 0);            // Current camera speed

    QVector3D       m_rotation = QVector3D(0, 0, 0);            // Current camera rotation
                                                                //              X Rotation, controls up / down
                                                                //              Y Rotation, controls left / right
                                                                //              Z Rotation, tilts head

    DrPointF        m_lag = { 0.0, 0.0 };                       // Current camera lag when following object
    double          m_zoom = 1.0;                               // Current camera zoom level

    QVector3D       m_target;                                   // Calculated point this camera is moving towards

    long            m_follow_key = 0;                           // Unique DrEngineWorld key this camera should follow
    int             m_buffer_size = c_slop_buffer_size;         // Number of past object speeds to average together for camera follow
    bool            m_wants_active = false;                     // If set to true in Editor, this camera tries to take Active when in Start Stage

    Up_Vector       m_up_vector = Up_Vector::Y;                 // Up Vector of Camera, usually Y, Z for first person

    QVector<double> m_avg_speed_x;                              // Average x speed of object this camera is following
    QVector<double> m_avg_speed_y;                              // Average y speed of object this camera is following
    QVector<double> m_avg_speed_z;                              // Average z speed of object this camera is following


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

    const int&          getBufferSize()     { return m_buffer_size; }
    const DrPointF&     getLag()            { return m_lag; }
    const QVector3D&    getPosition()       { return m_position; }
    const QVector3D&    getRotation()       { return m_rotation; }
    const QVector3D&    getSpeed()          { return m_speed; }
    Up_Vector           getUpVector()       { return m_up_vector; }
    const bool&         getWantsActive()    { return m_wants_active; }
    const double&       getZoom()           { return m_zoom; }

    void                setBufferSize(int slop) { m_buffer_size = (slop < 1) ? 1 : slop; }
    void                setLag(DrPointF lag) { m_lag.x = (lag.x <= 0) ? 0 : lag.x;
                                               m_lag.y = (lag.y <= 0) ? 0 : lag.y; }

    void                setPosition(QVector3D new_position) {       m_position = new_position; }
    void                setPosition(double x, double y, double z) { m_position = QVector3D(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)); }
    void                setPosition(float x, float y, float z) {    m_position = QVector3D(x, y, z); }
    void                setPosition(int x, int y, int z) {          m_position = QVector3D(x, y, z); }
    void                setPositionX(float x) { m_position.setX(x); }
    void                setPositionY(float y) { m_position.setY(y); }
    void                setPositionZ(float z) { m_position.setZ(z); }

    void                setRotation(QVector3D new_rotation) {       m_rotation = new_rotation; }
    void                setRotation(double x, double y, double z) { m_rotation = QVector3D(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)); }
    void                setRotation(float x, float y, float z) {    m_rotation = QVector3D(x, y, z); }
    void                setRotation(int x, int y, int z) {          m_rotation = QVector3D(x, y, z); }
    void                setRotationX(float x) { m_rotation.setX(x); }
    void                setRotationY(float y) { m_rotation.setY(y); }
    void                setRotationZ(float z) { m_rotation.setZ(z); }

    void                setSpeed(QVector3D new_speed) {          m_speed = new_speed; }
    void                setSpeed(double x, double y, double z) { m_speed = QVector3D(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)); }
    void                setSpeed(float x, float y, float z) {    m_speed = QVector3D(x, y, z); }
    void                setSpeedX(float x) { m_speed.setX(x); }
    void                setSpeedY(float y) { m_speed.setY(y); }
    void                setSpeedZ(float z) { m_speed.setZ(z); }

    void                setTarget(QVector3D target) { m_target = target; }
    void                setUpVector(Up_Vector up) { m_up_vector = up; }
    void                setWantActive(bool wants) { m_wants_active = wants; }
    void                setZoom(double zoom) { m_zoom = zoom; }

};

#endif // ENGINE_CAMERA_H










