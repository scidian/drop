//
//      Created by Stephens Nunnally on 5/17/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef ENGINE_CAMERA_H
#define ENGINE_CAMERA_H

#include <QVector>
#include <QVector3D>

// Forward Declarations
class DrEngine;
struct SceneObject;

//####################################################################################
//##    DrEngineCamera
//##        Holds a Camera for use in the Engine
//############################
class DrEngineCamera
{
private:
    QVector3D       m_position = QVector3D(0, 0, 0);            // Current camera position
    QVector3D       m_speed = QVector3D(0, 0, 0);               // Current camera speed

    QVector3D       m_target;                                   // Calculated point this camera is moving towards

    SceneObject    *m_follow;                                   // Object this camera should follow
    QVector<double> m_avg_speed_x;                              // Average x speed of object this camera is following
    QVector<double> m_avg_speed_y;



public:
    // Constructor
    DrEngineCamera(float x = 0, float y = 0, float z = 0);

    // Function Calls
    void        followObject(SceneObject *object) { m_follow = object; }
    void        moveCamera(const double& milliseconds);
    void        updateCamera();

    // Getters / Setters
    const QVector3D&    getPosition()   { return m_position; }
    const QVector3D&    getSpeed()      { return m_speed; }
    void                setPosition(QVector3D new_position) {       m_position = new_position; }
    void                setPosition(double x, double y, double z) { m_position = QVector3D(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)); }
    void                setPosition(float x, float y, float z) {    m_position = QVector3D(x, y, z); }
    void                setPositionX(float x) { m_position.setX(x); }
    void                setPositionY(float y) { m_position.setY(y); }
    void                setPositionZ(float z) { m_position.setZ(z); }
    void                setSpeed(QVector3D new_speed) {          m_speed = new_speed; }
    void                setSpeed(double x, double y, double z) { m_speed = QVector3D(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)); }
    void                setSpeed(float x, float y, float z) {    m_speed = QVector3D(x, y, z); }
    void                setSpeedX(float x) { m_speed.setX(x); }
    void                setSpeedY(float y) { m_speed.setY(y); }
    void                setSpeedZ(float z) { m_speed.setZ(z); }

};

#endif // ENGINE_CAMERA_H










