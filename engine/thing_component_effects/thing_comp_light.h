//
//      Created by Stephens Nunnally on 3/16/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef THING_COMP_LIGHT_H
#define THING_COMP_LIGHT_H

#include "3rd_party/chipmunk/chipmunk.h"
#include "engine/globals_engine.h"
#include "engine/thing/engine_thing_component.h"


//####################################################################################
//##    ThingCompLight
//##        Built-In Component for DrEngineThing acting as a 2D Light
//############################
class ThingCompLight : public DrThingComponent
{
public:
    // Constructor / Destructor
    ThingCompLight(DrEngineWorld *engine_world, DrEngineThing *parent_thing,
                   Light_Type type, DrColor color, float diameter, DrPointF cone, float intensity,
                   float shadows, bool draw_shadows, float blur, float pulse, float pulse_speed);
    virtual ~ThingCompLight() override;


    // #################### VARIABLES ####################
public:
    // Light Variables
    Light_Type      light_type = Light_Type::Opaque;        // Type of Light: Opaque or Glow
    float           light_size = 1500.0f;                   // Diameter of light
    DrColor         color = DrColor(192, 64, 192);          // Color of light
    DrPointF        cone = DrPointF(0.0, 360.0);            // Start / end angle of light
                                                            //      float cone_1 = Dr::DegreesToRadians( 30.0f);    // Pac-man
                                                            //      float cone_2 = Dr::DegreesToRadians(330.0f);
                                                            //      float cone_1 = Dr::DegreesToRadians(330.0f);    // Small cone
                                                            //      float cone_2 = Dr::DegreesToRadians( 30.0f);
                                                            //      float cone_1 = Dr::DegreesToRadians(  0.0f);    // Whole Circle
                                                            //      float cone_2 = Dr::DegreesToRadians(360.0f);
    float           shadows = 1.0f;                         // Visibility of shadows,   0 to 100, no light      to  barely visible
    float           intensity = 1.0f;                       // Increase intensity,      0 to 100, none / normal to  very bright
    float           blur = 1.0f;                            // Blur of shadows,         0 to 100, no blur       to  very blurry
    bool            draw_shadows = true;                    // Should this light produc shadows?

    float           pulse = 0.0f;                           // -/+ amount starting from intensity to pulse between
    float           pulse_speed = 0.0f;                     // Amound to change per second when pulsing

private:
    // ***** For use by engine
    DrPointF        m_screen_pos = DrPointF(0, 0);          // Position of the center of the light as rendered to screen
    int             m_light_diameter = 0;                   // Size of light frame buffer object
    int             m_light_diameter_fitted = 0;            // Size fit to screen size * 2
    float           m_start_intensity = 1.0f;               // Saves start intensity for use with pulsing
    float           m_pulse_direction = 0.0f;               // Used to pulse light
    float           m_pulse_target;                         // Pulse target intensity
    float           m_perspective_scale = 1.0f;             // Scales light at a far away zorder for Perspective Rendering
    bool            m_visible = false;                      // Tracks if light is in view
    DrPointF        m_rotated_cone = DrPointF(0, 360);      // Cone set to current angle



    // #################### FUNCTIONS TO BE EXPOSED TO API ####################
public:
    // Basic Component Events
    virtual void        init() override;                                                // Called when component is first created
    virtual void        addToWorld() override;                                          // Called when Thing is added to m_things DrEngineWorld vector
    virtual void        draw() override;                                                // Called when it is time to Render Thing
    virtual void        update(double time_passed, double time_warp) override;          // Called during DrEngineWorld->updateWorld() step
    virtual void        destroy() override;                                             // Called when component is destroyed


    // #################### INTERNAL FUNCTIONS ####################
public:
    // Light Functions
    void        setAngle(double new_angle);


    // Getters / Setters
    DrPointF    getScreenPos()              { return m_screen_pos; }
    int         getLightDiameter()          { return m_light_diameter; }
    int         getLightDiameterFitted()    { return m_light_diameter_fitted; }
    float       getStartIntensity()         { return m_start_intensity; }
    float       getPulseDirection()         { return m_pulse_direction; }
    float       getPerspectiveScale()       { return m_perspective_scale; }
    bool        isInView()                  { return m_visible; }
    DrPointF    getRotatedCone()            { return m_rotated_cone; }

    void        setScreenPos(DrPointF new_pos) { m_screen_pos = new_pos; }
    void        setLightDiameter(int new_diameter) { m_light_diameter = new_diameter; }
    void        setLightDiameterFitted(int new_diameter) { m_light_diameter_fitted = new_diameter; }
    void        setStartIntensity(float intensity) { m_start_intensity = intensity; }
    void        setPulseDirection(float new_direction) { m_pulse_direction = new_direction; }
    void        setPerspectiveScale(float scale) { m_perspective_scale = scale; }
    void        setIsInView(bool in_view) { m_visible = in_view; }

};


#endif // THING_COMP_LIGHT_H

















