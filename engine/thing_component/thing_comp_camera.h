//
//      Created by Stephens Nunnally on 2/2/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef THING_COMP_CAMERA_H
#define THING_COMP_CAMERA_H

#include "engine/globals_engine.h"
#include "engine/thing/engine_thing_component.h"


//####################################################################################
//##    ThingCompCamera
//##        Built-In Component for DrEngineThing dealing with Camera Properties
//############################
class ThingCompCamera : public DrThingComponent
{
public:
    // Constructor / Destructor
    ThingCompCamera(DrEngineWorld *engine_world, DrEngineThing *parent_thing);
    virtual ~ThingCompCamera();


    // #################### VARIABLES ####################
private:
    // Thing Properties - Camera
    long        m_active_camera         { c_no_key };                   // Set to ID of last camera that followed this object, c_no_key == no camera
    glm::vec3   m_camera_position       { c_default_camera_pos };       // X, Y, and Z Camera Position Offset for this objects Active (Follow) Camera
    glm::vec3   m_camera_rotation       { c_default_camera_rot };       // X, Y, and Z Camera Rotation        for this objects Active (Follow) Camera
    double      m_camera_zoom           { 1.0 };                        // Camera Magnification Level         for this objects Active (Follow) Camera
    DrPointF    m_camera_lag            { 0.0, 0.0 };                   // Camera Movement Lag Amount         for this objects Active (Follow) Camera
    Up_Vector   m_camera_up             { Up_Vector::Y };               // Camera Up Vector                   usually Y, Z for first person
    bool        m_camera_match_angle    { false };                      // Should Camera Match Angle of Object Following

    std::map<Edge_Location, Frame_Edge> m_camera_edges;                 // Describes edges of camera frame


    // #################### FUNCTIONS TO BE EXPOSED TO API ####################
public:
    // Basic Component Events
    virtual void    init();                                                         // Called when component is first created
    virtual void    update(double time_passed, double time_warp);                   // Called during DrEngineWorld->updateWorld() step
    virtual void    destroy();


    // #################### INTERNAL FUNCTIONS ####################
public:
    // Getters / Setters
    const long&             getActiveCameraKey()    { return m_active_camera; }
    bool                    hasActiveCamera()       { return (m_active_camera == c_no_key) ? false : true; }
    void                    setActiveCameraKey(const long &new_camera_key) { m_active_camera = new_camera_key; }

    glm::vec3&              getCameraPosition()                             { return m_camera_position; }
    void                    setCameraPosition(glm::vec3 position)           { m_camera_position = position; }
    void                    setCameraPosition(float x, float y, float z)    { m_camera_position = glm::vec3(x, y, z); }
    void                    setCameraPositionXY(DrPointF point)             { m_camera_position.x = float(point.x); m_camera_position.y = float(point.y); }
    void                    setCameraPositionZ(float z)                     { m_camera_position.z = z; }
    void                    setCameraPositionZ(double z)                    { m_camera_position.z = static_cast<float>(z); }

    glm::vec3&              getCameraRotation()             { return m_camera_rotation; }
    void                    setCameraRotation(glm::vec3 rotation);
    void                    setCameraRotation(float x_up_down, float y_left_right, float z_rotate);

    DrPointF                getCameraLag()                  { return m_camera_lag; }
    void                    setCameraLag(DrPointF lag)      { m_camera_lag.x = (lag.x <= 0) ? 0 : lag.x;
                                                              m_camera_lag.y = (lag.y <= 0) ? 0 : lag.y; }
    bool                    getCameraMatch()                { return m_camera_match_angle; }
    void                    setCameraMatch(bool match)      { m_camera_match_angle = match; }
    Up_Vector               getCameraUpVector()             { return m_camera_up; }
    void                    setCameraUpVector(Up_Vector up) { m_camera_up = up; }
    double                  getCameraZoom()                 { return m_camera_zoom; }
    void                    setCameraZoom(double zoom)      { m_camera_zoom = zoom; }

    Frame_Edge              getCameraEdge(Edge_Location edge_location) { return m_camera_edges[edge_location]; }
    void                    setCameraEdge(Edge_Location edge_location, Frame_Edge edge_type) { m_camera_edges[edge_location] = edge_type; }

};

#endif // THING_COMP_CAMERA_H






















