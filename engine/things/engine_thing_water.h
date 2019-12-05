//
//      Created by Stephens Nunnally on 7/18/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef ENGINE_THING_WATER_H
#define ENGINE_THING_WATER_H

#include "engine/globals_engine.h"
#include "engine/things/engine_thing.h"


//####################################################################################
//##    DrEngineWater
//##        2D Water / Reflection
//############################
class DrEngineWater : public DrEngineThing
{
public:
    // Water Settings
    Water_Texture   water_texture = Water_Texture::Ripples; // Texture to use
    QColor          start_color = QColor(128, 128, 255);    // Color of top of water
    QColor          end_color = QColor(128, 128, 255);      // Color of bottom of water
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
    bool            surface_keep_flat = false;              // Keep top of water flat?

    float           refract_reflection = 1.0f;
    float           refract_underwater = 1.0f;
    float           refract_texture = 1.0f;
    float           refract_foam = 1.0f;

    float           movement_speed = 0.0f;                  // Moves texture left / right

public:
    // Constructor / Destructor
    DrEngineWater(DrEngineWorld *world, long unique_key, long original_key, double x, double y, double z, double angle, float opacity, DrPointF size,
                  Water_Texture texture, QColor start_color, QColor end_color,
                  float tint, float reflection,
                  float rip_length,     float rip_speed, float rip_amplitude, float rip_stretch,
                  float w_length,       float w_speed,   float w_amplitude,
                  QColor foam_color,    float foam_tint, float foam_height, bool foam_flat,
                  float r_reflection,   float r_underwater, float r_texture, float r_foam, float texture_movement);
    virtual ~DrEngineWater() override;

    // Abstract Engine Thing Overrides
    virtual DrThingType getThingType() override { return DrThingType::Water; }



};


#endif // ENGINE_THING_WATER_H













