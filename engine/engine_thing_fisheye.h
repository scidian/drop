//
//      Created by Stephens Nunnally on 8/10/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef ENGINE_THING_FISHEYE_H
#define ENGINE_THING_FISHEYE_H

#include "chipmunk/chipmunk.h"
#include "engine_thing.h"
#include "enums.h"
#include "enums_engine.h"


//####################################################################################
//##    DrEngineFisheye
//##        Fisheye Lens
//############################
class DrEngineFisheye : public DrEngineThing
{
public:
    // Lens Settings
    QPointF         lens_size;                              // Size of lens
    QColor          start_color = QColor(128, 128, 255);    // Color tint of lens
    float           color_tint = 0.5f;                      // Color tint percentage     0 to  1
    float           lens_zoom = 2.2f;                       // Lens Zoom                 0 to 10

public:
    // Constructor / Destructor
    DrEngineFisheye(DrEngineWorld *world, long unique_key, double x, double y, double z, double angle, QPointF size,
                    QColor start_color, float tint, float zoom, float opacity);
    virtual ~DrEngineFisheye() override;

    // Abstract Engine Thing Overrides
    virtual void        addToWorld() override;
    virtual DrThingType getThingType() override { return DrThingType::Fisheye; }
    virtual bool        update(double time_passed, double time_warp, QRectF &area) override;

};


#endif // ENGINE_THING_FISHEYE_H





