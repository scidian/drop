//
//      Created by Stephens Nunnally on 8/19/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef ENGINE_THING_MIRROR_H
#define ENGINE_THING_MIRROR_H

#include "engine/globals_engine.h"
#include "engine/thing/engine_thing.h"


//####################################################################################
//##    DrEngineMirror
//##        Mirror Effect
//############################
class DrEngineMirror : public DrEngineThing
{
public:
    // Mirror Settings
    DrColor         start_color =   DrColor(192, 192, 192);     // Color tint of top of mirror
    DrColor         end_color =     DrColor(128, 128, 255);     // Color tint of bottom of mirror
    float           tint_percent = 0.5f;                        // Color tint percentage     0 to  1
    float           blur = 0.0f;                                // Scatter amount
    float           blur_stretch = 0.0f;                        // Scatter stretch away from start
    float           scale = 1.0f;                               // Noise (scatter) scale

public:
    // Constructor / Destructor
    DrEngineMirror(DrEngineWorld *world, long unique_key, long original_key, double x, double y, double z, double angle, float opacity, DrPointF size,
                   DrColor color_1, DrColor color_2, float tint_amount, float blur, float blur_stretch, float scale);
    virtual ~DrEngineMirror() override;

    // Abstract Engine Thing Overrides
    virtual DrThingType getThingType() override     { return DrThingType::Mirror; }



};


#endif // ENGINE_THING_MIRROR_H







