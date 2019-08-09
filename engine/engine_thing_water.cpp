//
//      Created by Stephens Nunnally on 7/18/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "engine_thing_water.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
DrEngineWater::DrEngineWater(DrEngineWorld *world, long unique_key, double x, double y, double z, double angle, QPointF size, Water_Texture texture,
                             QColor start_color, QColor end_color,
                             float tint, float reflection,
                             float rip_frequency, float rip_speed, float rip_amplitude, float rip_stretch,
                             float w_length,      float w_speed,   float w_amplitude,
                             QColor foam_color,   float foam_tint, float foam_height,
                            float r_reflection,  float r_underwater, float r_texture, float r_foam, float texture_movement,
                             float opacity) :
    DrEngineThing(world, unique_key) {

    this->setPosition( QPointF(x, y) );
    this->z_order = z;
    this->water_size = size;
    this->setAngle(angle);

    this->water_texture = texture;
    this->start_color = start_color;
    this->end_color = end_color;
    this->water_tint = tint;
    this->reflection_opacity = reflection;

    this->ripple_frequency = rip_frequency;
    this->ripple_speed = rip_speed;
    this->ripple_amplitude = rip_amplitude;
    this->ripple_stretch = rip_stretch;

    this->wave_frequency = w_length;
    this->wave_speed = w_speed;
    this->wave_amplitude = w_amplitude;

    this->surface_color = foam_color;
    this->surface_tint = foam_tint;
    this->surface_height = foam_height;

    this->refract_reflection = r_reflection;
    this->refract_underwater = r_underwater;
    this->refract_texture = r_texture;
    this->refract_foam = r_foam;
    this->movement_speed = texture_movement;

    this->setOpacity( opacity );
}

DrEngineWater::~DrEngineWater() {

}



//####################################################################################
//##    Override for DrEngineThing::addToWorld()
//####################################################################################
void DrEngineWater::addToWorld() {

}



//####################################################################################
//##    Override for DrEngineThing::update() - Pulses Light
//####################################################################################
bool DrEngineWater::update(double time_passed, double time_warp, QRectF &area) {
    Q_UNUSED( time_passed )
    Q_UNUSED( time_warp )

    bool remove = false;

    // ***** Delete object if ends up outside the deletion threshold
    if (area.contains(getPosition()) == false) remove = true;
    return remove;
}







