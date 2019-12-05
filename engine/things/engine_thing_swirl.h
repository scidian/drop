//
//      Created by Stephens Nunnally on 8/26/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef ENGINE_THING_SWIRL_H
#define ENGINE_THING_SWIRL_H

#include "engine_thing.h"
#include "globals_engine.h"


//####################################################################################
//##    DrEngineSwirl
//##        Swirl Effect
//############################
class DrEngineSwirl : public DrEngineThing
{
public:
    // Swirl Settings
    QColor          start_color = QColor(128, 128, 255);        // Color tint of lens
    float           color_tint = 0.5f;                          // Color tint percentage     0 to  1
    float           rotation;                                   // Amount of rotation for Swirl

public:
    // Constructor / Destructor
    DrEngineSwirl(DrEngineWorld *world, long unique_key, long original_key, double x, double y, double z, double angle, float opacity, DrPointF size,
                  QColor color, float tint, float rotation_amount);
    virtual ~DrEngineSwirl() override;

    // Abstract Engine Thing Overrides
    virtual DrThingType getThingType() override { return DrThingType::Swirl; }


};

#endif // ENGINE_THING_SWIRL_H
