//
//      Created by Stephens Nunnally on 7/18/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef ENGINE_THING_WATER_H
#define ENGINE_THING_WATER_H

#include <QOpenGLFramebufferObject>

#include "chipmunk/chipmunk.h"
#include "engine_thing.h"
#include "enums.h"
#include "enums_engine.h"


//####################################################################################
//##    DrEngineWater
//##        2D Water / Reflection
//############################
class DrEngineWater : public DrEngineThing
{
public:
    // Water Settings
    QPointF         water_size;                             // Size of water
    QColor          water_color = QColor(128, 128, 255);    // Color of water
    float           water_tint = 0.5f;                      // Color tint percentage     0 to 1
    float           reflection_opacity = 0.5f;              // Reflection opacity        0 to 1

    float           ripple_frequency = 100.0;               // Ripple frequency         25 to 100 is pretty good
    float           ripple_speed =      50.0;               // Ripple speed              0 to 100 is pretty good
    float           ripple_amplitude =  25.0;               // Ripple amplitude          0 to 100 is pretty good
    float           ripple_stretch =    25.0;               // Ripple stretch            0 to 100 is pretty good

    float           wave_frequency =    20.0;               // Wave frequency
    float           wave_speed =        20.0;               // Wave frequency
    float           wave_amplitude =    20.0;               // Wave frequency

    QColor          surface_color = QColor(255, 255, 255);  // Surface color
    float           surface_tint = 0.5f;                    // Surface tint percentage
    float           surface_height = 5.0f;                  // Surface height

    float           refract_reflection = 1.0f;
    float           refract_underwater = 1.0f;
    float           refract_texture = 1.0f;
    float           refract_foam = 1.0f;

    float           movement_speed = 0.0f;                  // Moves texture left / right

public:
    // Constructor / Destructor
    DrEngineWater(DrEngineWorld *world, long unique_key, double x, double y, double z, QPointF size, QColor color, float tint, float reflection,
                  float rip_length,  float rip_speed, float rip_amplitude, float rip_stretch,
                  float w_length,    float w_speed,   float w_amplitude,
                  QColor foam_color, float foam_tint, float foam_height,
                  float r_reflection,  float r_underwater, float r_texture, float r_foam, float texture_movement,
                  float opacity);
    virtual ~DrEngineWater() override;

    // Abstract Engine Thing Overrides
    virtual void        addToWorld() override;
    virtual DrThingType getThingType() override { return DrThingType::Water; }
    virtual bool        update(double time_passed, double time_warp, QRectF &area) override;

};


#endif // ENGINE_THING_WATER_H













