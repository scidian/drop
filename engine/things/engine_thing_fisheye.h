//
//      Created by Stephens Nunnally on 8/10/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef ENGINE_THING_FISHEYE_H
#define ENGINE_THING_FISHEYE_H

#include "engine/globals_engine.h"
#include "engine/things/engine_thing.h"


//####################################################################################
//##    DrEngineFisheye
//##        Fisheye Lens
//############################
class DrEngineFisheye : public DrEngineThing
{
public:
    // Lens Settings
    QColor          start_color = QColor(128, 128, 255);    // Color tint of lens
    float           color_tint = 0.5f;                      // Color tint percentage     0 to  1
    float           lens_zoom = 2.2f;                       // Lens Zoom                 0 to 10

public:
    // Constructor / Destructor
    DrEngineFisheye(DrEngineWorld *world, long unique_key, long original_key, double x, double y, double z, double angle, float opacity, DrPointF size,
                    QColor start_color_, float tint_, float zoom_);
    virtual ~DrEngineFisheye() override;

    // Abstract Engine Thing Overrides
    virtual DrThingType getThingType() override { return DrThingType::Fisheye; }


};


#endif // ENGINE_THING_FISHEYE_H





