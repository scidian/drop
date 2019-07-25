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


    float           wave_length =  200.0f;                  // 50 is good for big waves, 200 is good for small ripples      0.0 to 400.0
    float           wave_speed =     5.0f;                  //  1 is good for big waves,   5 is good for small ripples      0.0 to  10.0

    float           wave_min_width = 0.25f;                 // Minimum wave starting width          0.0 to  1.0     good start =  0.25
    float           wave_stretch =   3.00f;                 // Stretches away from the start        0.0 to 10.0     good start =  3.00

    float           bob_speed =      2.00f;                 // Between 0.0 and 5.0                                  good = 2.00
    float           bob_amount =     0.01f;                 // Between 0.0 and 0.05                                 good = 0.01


public:
    // Constructor / Destructor
    DrEngineWater(DrEngineWorld *world, long unique_key, double x, double y, double z, QPointF size, QColor color, float opacity);
    virtual ~DrEngineWater() override;

    // Abstract Engine Thing Overrides
    virtual void        addToWorld() override;
    virtual DrThingType getThingType() override { return DrThingType::Water; }
    virtual bool        update(double time_passed, double time_warp, QRectF &area) override;

};




#endif // ENGINE_THING_WATER_H




