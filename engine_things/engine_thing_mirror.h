//
//      Created by Stephens Nunnally on 8/19/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef ENGINE_THING_MIRROR_H
#define ENGINE_THING_MIRROR_H

#include "3rd_party_chipmunk/chipmunk.h"
#include "engine_thing.h"
#include "enums.h"
#include "enums_engine.h"


//####################################################################################
//##    DrEngineMirror
//##        Mirror Effect
//############################
class DrEngineMirror : public DrEngineThing
{
public:
    // Mirror Settings
    QColor          start_color =   QColor(192, 192, 192);      // Color tint of top of mirror
    QColor          end_color =     QColor(128, 128, 255);      // Color tint of bottom of mirror
    float           tint_percent = 0.5f;                        // Color tint percentage     0 to  1
    float           blur = 0.0f;                                // Scatter amount
    float           blur_stretch = 0.0f;                        // Scatter stretch away from start
    float           scale = 1.0f;                               // Noise (scatter) scale

public:
    // Constructor / Destructor
    DrEngineMirror(DrEngineWorld *world, long unique_key, double x, double y, double z, double angle, float opacity, DrPointF size,
                   QColor color_1, QColor color_2, float tint_amount, float blur, float blur_stretch, float scale);
    virtual ~DrEngineMirror() override;

    // Abstract Engine Thing Overrides
    virtual DrThingType getThingType() override { return DrThingType::Mirror; }



};


#endif // ENGINE_THING_MIRROR_H







