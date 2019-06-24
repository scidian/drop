//
//      Created by Stephens Nunnally on 6/19/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      2D Light w/ Pixel Perfect Shadows for use in the Physics World Engine
//
//
#ifndef ENGINE_LIGHT_H
#define ENGINE_LIGHT_H

#include <QOpenGLFramebufferObject>

#include "chipmunk/chipmunk.h"
#include "engine_object.h"
#include "enums_engine.h"
#include "helper.h"

//####################################################################################
//##    DrEngineLight
//##        Holds a 2D Light w/ Pixel Perfect Shadows for use in the Engine
//############################
class DrEngineLight : public DrEngineObject
{
public:
    // Light Settings
    float           light_size = 1500.0f;                   // Diameter of light
    QColor          color = QColor(192, 64, 192);           // Color of light
    QPointF         cone = QPointF(0.0, 360.0);             // Start / end angle of light
                                                            //      float cone_1 = qDegreesToRadians( 30.0f);    // Pac-man
                                                            //      float cone_2 = qDegreesToRadians(330.0f);
                                                            //      float cone_1 = qDegreesToRadians(330.0f);    // Small cone
                                                            //      float cone_2 = qDegreesToRadians( 30.0f);
                                                            //      float cone_1 = qDegreesToRadians(  0.0f);   // Whole Circle
                                                            //      float cone_2 = qDegreesToRadians(360.0f);
    float           shadows = 1.0f;                         // Visibility of shadows, 0 to 100
    float           intensity = 1.0f;                       // Intensity of light, 0 to 100
    float           blur = 1.0f;                            // Blur of shadows, 0 to 100
    bool            draw_shadows = true;                    // Should this light produc shadows?

    // Frame Buffer Objects for use during Render
    QOpenGLFramebufferObject *occluder_fbo = nullptr;       // Off screen Frame Buffer Object for Occluder Map
    QOpenGLFramebufferObject *shadow_fbo = nullptr;         // Off screen Frame Buffer Object for 1D Shadow Map


private:
    // ***** For use by engine
    QPointF         m_screen_pos = QPointF(0,0);              // Position of the center of the light as rendered to screen
    int             m_light_diameter = 0;                     // Size of light frame buffer object
    int             m_light_diameter_fitted = 0;              // Size fit to screen size * 2
    bool            m_visible = false;                        // Tracks if light is in view


public:
    // Constructor / Destructor
    DrEngineLight();
    virtual ~DrEngineLight() override;

    // Virtual Functions
    virtual bool    isLight() override { return true; }

    // Getters / Setters
    QPointF     getScreenPos() { return m_screen_pos; }
    int         getLightDiameter() { return m_light_diameter; }
    int         getLightDiameterFitted() { return m_light_diameter_fitted; }
    bool        isInView() { return m_visible; }

    void        setScreenPos(QPointF new_pos) { m_screen_pos = new_pos; }
    void        setLightDiameter(int new_diameter) { m_light_diameter = new_diameter; }
    void        setLightDiameterFitted(int new_diameter) { m_light_diameter_fitted = new_diameter; }
    void        setIsInView(bool in_view) { m_visible = in_view; }

};


#endif // ENGINE_LIGHT_H











