//
//      Created by Stephens Nunnally on 8/15/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef ENGINE_THING_FIRE_H
#define ENGINE_THING_FIRE_H

#include "chipmunk/chipmunk.h"
#include "engine_thing.h"
#include "enums.h"
#include "enums_engine.h"

const float x_scale_modifier = 2.100f;
const float y_scale_modifier = 1.565f;


//####################################################################################
//##    DrEngineFire
//##        Fire Effect
//############################
class DrEngineFire : public DrEngineThing
{
private:
    float           m_scale_x = 1.0f;                       // Scale of fire in world
    float           m_scale_y = 1.0f;                       // Scale of fire in world

public:
    // Lens Settings
    QPointF         fire_size;                              // Size of fire
    QColor          start_color = QColor(0, 0, 0);          // Color 1 of fire
    QColor          end_color = QColor(0, 0, 0);            // Color 2 of fire
    float           intensity = 0.5f;                       // Color tint percentage     0 to  1

public:
    // Constructor / Destructor
    DrEngineFire(DrEngineWorld *world, long unique_key, double x, double y, double z, QPointF scale, double angle, QPointF size,
                 QColor color_1, QColor color_2, float tint,
                 float opacity);
    virtual ~DrEngineFire() override;

    // Abstract Engine Thing Overrides
    virtual void        addToWorld() override;
    virtual DrThingType getThingType() override { return DrThingType::Fire; }
    virtual bool        update(double time_passed, double time_warp, QRectF &area) override;


    // Fire Basic Settings
    const float&    getScaleX() { return m_scale_x; }
    const float&    getScaleY() { return m_scale_y; }
    void            setScaleX(float new_scale_x)  { m_scale_x = new_scale_x * x_scale_modifier; }
    void            setScaleX(double new_scale_x) { m_scale_x = static_cast<float>(new_scale_x) * x_scale_modifier; }
    void            setScaleY(float new_scale_y)  { m_scale_y = new_scale_y * y_scale_modifier; }
    void            setScaleY(double new_scale_y) { m_scale_y = static_cast<float>(new_scale_y) * y_scale_modifier; }

};

#endif // ENGINE_THING_FIRE_H











