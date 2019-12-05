//
//      Created by Stephens Nunnally on 6/19/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      2D Light w/ Pixel Perfect Shadows for use in the Physics World Engine
//
//
#ifndef ENGINE_LIGHT_H
#define ENGINE_LIGHT_H

#include "engine_thing_object.h"
#include "globals_engine.h"


//####################################################################################
//##    DrEngineLight
//##        Holds a 2D Light w/ Pixel Perfect Shadows for use in the Engine
//############################
class DrEngineLight : public DrEngineThing
{
public:
    // Light Settings
    Light_Type      light_type = Light_Type::Opaque;        // Type of Light: Opaque or Glow
    float           light_size = 1500.0f;                   // Diameter of light
    QColor          color = QColor(192, 64, 192);           // Color of light
    DrPointF        cone = DrPointF(0.0, 360.0);             // Start / end angle of light
                                                            //      float cone_1 = qDegreesToRadians( 30.0f);    // Pac-man
                                                            //      float cone_2 = qDegreesToRadians(330.0f);
                                                            //      float cone_1 = qDegreesToRadians(330.0f);    // Small cone
                                                            //      float cone_2 = qDegreesToRadians( 30.0f);
                                                            //      float cone_1 = qDegreesToRadians(  0.0f);   // Whole Circle
                                                            //      float cone_2 = qDegreesToRadians(360.0f);
    float           shadows = 1.0f;                         // Visibility of shadows,   0 to 100, no light      to  barely visible
    float           intensity = 1.0f;                       // Increase intensity,      0 to 100, none / normal to  very bright
    float           blur = 1.0f;                            // Blur of shadows,         0 to 100, no blur       to  very blurry
    bool            draw_shadows = true;                    // Should this light produc shadows?

    float           pulse = 0.0f;                           // -/+ amount starting from intensity to pulse between
    float           pulse_speed = 0.0f;                     // Amound to change per second when pulsing

private:
    // ***** For use by engine
    QPointF         m_screen_pos = QPointF(0, 0);           // Position of the center of the light as rendered to screen
    int             m_light_diameter = 0;                   // Size of light frame buffer object
    int             m_light_diameter_fitted = 0;            // Size fit to screen size * 2
    float           m_start_intensity = 1.0f;               // Saves start intensity for use with pulsing
    float           m_pulse_direction = 0.0f;               // Used to pulse light
    float           m_pulse_target;                         // Pulse target intensity
    float           m_perspective_scale = 1.0f;             // Scales light at a far away zorder for Perspective Rendering
    bool            m_visible = false;                      // Tracks if light is in view
    DrPointF        m_rotated_cone = DrPointF(0, 360);       // Cone set to current angle

public:
    // Constructor / Destructor
    DrEngineLight(DrEngineWorld *world, long unique_key, long original_key,
                  double x, double y, double z, float opacity, Light_Type type, QColor color, float diameter, DrPointF cone, float intensity,
                  float shadows, bool draw_shadows, float blur, float pulse, float pulse_speed);
    virtual ~DrEngineLight() override;

    // Abstract Engine Thing Overrides
    virtual void        addToWorld() override;
    virtual DrThingType getThingType() override { return DrThingType::Light; }
    virtual bool        update(double time_passed, double time_warp, QRectF &area) override;

    // Non-Abstract Engine Thing Overrides
    virtual void        setAngle(double new_angle) override;


    // Getters / Setters
    QPointF     getScreenPos() { return m_screen_pos; }
    int         getLightDiameter() { return m_light_diameter; }
    int         getLightDiameterFitted() { return m_light_diameter_fitted; }
    float       getStartIntensity() { return m_start_intensity; }
    float       getPulseDirection() { return m_pulse_direction; }
    float       getPerspectiveScale() { return m_perspective_scale; }
    bool        isInView() { return m_visible; }
    DrPointF    getRotatedCone() { return m_rotated_cone; }

    void        setScreenPos(QPointF new_pos) { m_screen_pos = new_pos; }
    void        setLightDiameter(int new_diameter) { m_light_diameter = new_diameter; }
    void        setLightDiameterFitted(int new_diameter) { m_light_diameter_fitted = new_diameter; }
    void        setStartIntensity(float intensity) { m_start_intensity = intensity; }
    void        setPulseDirection(float new_direction) { m_pulse_direction = new_direction; }
    void        setPerspectiveScale(float scale) { m_perspective_scale = scale; }
    void        setIsInView(bool in_view) { m_visible = in_view; }


};


#endif // ENGINE_LIGHT_H











