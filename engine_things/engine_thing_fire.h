//
//      Created by Stephens Nunnally on 8/15/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef ENGINE_THING_FIRE_H
#define ENGINE_THING_FIRE_H

#include "3rd_party_chipmunk/chipmunk.h"
#include "engine_thing.h"
#include "enums.h"
#include "enums_engine.h"


//####################################################################################
//##    DrEngineFire
//##        Fire Effect
//############################
class DrEngineFire : public DrEngineThing
{

public:
    // Lens Settings
    Fire_Mask       fire_mask =   Fire_Mask::Torch;         // Shape mask to use for this fire
    QColor          start_color = QColor(0, 0, 0);          // Color 1 of fire
    QColor          end_color =   QColor(0, 0, 0);          // Color 2 of fire
    QColor          smoke_color = QColor(0, 0, 0);          // Background Color of fire
    float           intensity =   0.5f;                     // How much percent wise to add in alpha noise
    float           smoothness =  0.2f;                     // How smooth to make the flames
    float           wavy =        0.5f;                     // How wavyness to make the flames
    float           flame_speed = 7.0f;                     // Flame speed


public:
    // Constructor / Destructor
    DrEngineFire(DrEngineWorld *world, long unique_key, double x, double y, double z, QPointF scale, double angle, float opacity, QPointF size,
                 Fire_Mask mask, QColor color_1, QColor color_2, QColor smoke, float tint, float smooth, float wave, float speed);
    virtual ~DrEngineFire() override;

    // Abstract Engine Thing Overrides
    virtual void        addToWorld() override;
    virtual DrThingType getThingType() override { return DrThingType::Fire; }
    virtual bool        update(double time_passed, double time_warp, QRectF &area) override;

    // Engine Thing Overrides
    virtual double      getAngle() override { return -(DrEngineThing::getAngle()); }

};

#endif // ENGINE_THING_FIRE_H











