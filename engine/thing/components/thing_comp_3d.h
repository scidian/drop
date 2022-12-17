//
//      Created by Stephens Nunnally on 6/19/2019, (c) 2019 Scidian Studios, All Rights Reserved
//
//  File:
//      Abstract base class that represents one DrThing in the physics engine
//
//
#ifndef THING_COMP_3D_H
#define THING_COMP_3D_H

#include "engine/globals_engine.h"
#include "engine/thing/engine_thing_component.h"


//####################################################################################
//##    ThingComp3D
//##        Built-In Component for DrEngineThing dealing with 3D Properties
//############################
class ThingComp3D : public DrThingComponent
{
public:
    // Constructor / Destructor
    ThingComp3D(DrEngineWorld *engine_world, DrEngineThing *parent_thing);
    virtual ~ThingComp3D() override;


    // #################### VARIABLES ####################
private:
    // Thing Properties - 3D
    Convert_3D_Type     m_3d_type           { Convert_3D_Type::Extrusion };
    double              m_angle_x           { 0.0 };                                // X axis rotation
    double              m_angle_y           { 0.0 };                                // Y axis rotation
    bool                m_billboard         { false };                              // Keeps object facing camera
    double              m_depth             { 0.0 };                                // Desired 3D Depth of 2D Objects
    double              m_rotate_x_speed    { 0.0 };                                // X axis rotation speed
    double              m_rotate_y_speed    { 0.0 };                                // Y axis rotation speed


    // #################### FUNCTIONS TO BE EXPOSED TO API ####################
public:
    // Basic Component Events
    ///virtual void        draw() override;                                                // Called when it is time to Render Thing
    ///virtual bool        update(double time_passed, double time_warp) override;          // Called during DrEngineWorld->updateWorld() step


    // #################### INTERNAL FUNCTIONS ####################
public:
    // Getters / Setters
    Convert_3D_Type         get3DType()             { return m_3d_type; }
    const double&           getAngleX()             { return m_angle_x; }                       // Returns Thing X Axis Rotation (in degrees)
    const double&           getAngleY()             { return m_angle_y; }                       // Returns Thing Y Axis Rotation (in degrees)
    const bool&             getBillboard()          { return m_billboard; }                     // True / False should render facing camera
    const double&           getDepth()              { return m_depth; }                         // Returns Thing 3D Extrusion Depth
    const double&           getRotateSpeedX()       { return m_rotate_x_speed; }                // Returns Thing X Axis Rotational Speed
    const double&           getRotateSpeedY()       { return m_rotate_y_speed; }                // Returns Thing Y Axis Rotational Speed

    void                    set3DType(Convert_3D_Type type)         { m_3d_type = type; }
    void                    setAngleX(double new_angle_x)           { m_angle_x = new_angle_x; }
    void                    setAngleY(double new_angle_y)           { m_angle_y = new_angle_y; }
    void                    setBillboard(bool billboard)            { m_billboard = billboard; }
    void                    setDepth(double new_depth)              { m_depth = new_depth; }
    void                    setRotateSpeedX(double new_speed)       { m_rotate_x_speed = new_speed; }
    void                    setRotateSpeedY(double new_speed)       { m_rotate_y_speed = new_speed; }

};

#endif // THING_COMP_3D_H



















