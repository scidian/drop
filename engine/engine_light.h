//
//      Created by Stephens Nunnally on 6/19/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef ENGINE_LIGHT_H
#define ENGINE_LIGHT_H

#include <QOpenGLFramebufferObject>

#include "chipmunk/chipmunk.h"
#include "enums_engine.h"
#include "helper.h"

class DrEngineLight
{
public:

    float           light_size = 1500;                      // Diameter of light
    QPointF         position = QPointF(0.0, 0.0);           // Light position in the world
    QColor          color = QColor(192, 64, 192);           // Color of light
    QPointF         cone = QPointF(0.0, 360.0);             // Start / end angle of light
    double          shadows = 1.0;                          // Visibility of shadows, 0 to 100
    double          intensity = 1.0;                        // Intensity of light, 0 to 100



    // ***** For use by engine
    QPointF         screen_pos = QPointF(0,0);              // Position of the center of the light as rendered to screen
    int             light_radius = 0;                       // Size of light frame buffer object
    int             light_radius_fitted = 0;                // Size fit to screen size * 2
    bool            visible = false;                        // Tracks if light is in view

    QOpenGLFramebufferObject *occluder_fbo = nullptr;       // Off screen Frame Buffer Object for Occluder Map
    QOpenGLFramebufferObject *shadow_fbo = nullptr;         // Off screen Frame Buffer Object for 1D Shadow Map


public:
    // Constructor / Destructor
    DrEngineLight();
    ~DrEngineLight();

    // Getters / Setters


};




#endif // ENGINE_LIGHT_H








