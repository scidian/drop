//
//      Created by Stephens Nunnally on 5/15/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QtMath>
#include <QRandomGenerator>

#include "engine.h"
#include "engine_thing_light.h"
#include "engine_thing_object.h"
#include "engine_thing_water.h"
#include "engine_world.h"
#include "project/project_world_stage.h"
#include "project/project_world_stage_thing.h"

// Local Constants
const double    c_light_size_adjuster = 1.15;           // Multiplier to equal out the slight rendering difference between shader and pixmap drawing functions


//####################################################################################
//##    Load DrProject Stage to World / Space
//####################################################################################
void DrEngineWorld::loadStageToWorld(DrStage *stage, double offset_x, double offset_y) {

    // ***** Load Things
    for (auto thing_pair : stage->getThingMap()) {

        // Grab current Thing
        DrThing *thing = thing_pair.second;

        // Check if Thing type, if it is, call appropriate Thing Loader
        if (thing->getType() != DrType::Thing) continue;

        if (thing->getThingType() == DrThingType::Object) {
            loadObjectToWorld(thing, offset_x, offset_y);
        } else if (thing->getThingType() == DrThingType::Light) {
            loadLightToWorld(thing, offset_x, offset_y);
        } else if (thing->getThingType() == DrThingType::Water) {
            loadWaterToWorld(thing, offset_x, offset_y);
        }

    }

    // ***** Update distance we've loaded scenes to
    m_loaded_to += stage->getComponentPropertyValue(Components::Stage_Settings, Properties::Stage_Size).toInt();
}


//####################################################################################
//##    Loads one DrProject DrThingType::Object to World / Space
//####################################################################################
void DrEngineWorld::loadObjectToWorld(DrThing *thing, double offset_x, double offset_y) {

    // ***** Load Object Thing Properties
    long        asset_key =     thing->getAssetKey();
    QPointF     position =      thing->getComponentPropertyValue(Components::Thing_Transform, Properties::Thing_Position).toPointF();
    QPointF     scale =         thing->getComponentPropertyValue(Components::Thing_Transform, Properties::Thing_Scale).toPointF();
    double      angle =         thing->getComponentPropertyValue(Components::Thing_Transform, Properties::Thing_Rotation).toDouble();
    double      z_order =       thing->getComponentPropertyValue(Components::Thing_Layering,  Properties::Thing_Z_Order).toDouble();
    float       alpha =         thing->getComponentPropertyValue(Components::Thing_Layering,  Properties::Thing_Opacity).toFloat() / 100.0f;
    bool        collide =       thing->getComponentPropertyValue(Components::Thing_Settings_Object,  Properties::Thing_Object_Collide).toBool();
    int         physics =       thing->getComponentPropertyValue(Components::Thing_Settings_Object,  Properties::Thing_Object_Physics_Type).toInt();

    Body_Type body_type = Body_Type::Static;
    switch (physics) {
        case 0: body_type = Body_Type::Static;       break;
        case 1: body_type = Body_Type::Kinematic;    break;
        case 2: body_type = Body_Type::Dynamic;      break;
    }

    // ***** Add the block to the cpSpace
    DrEngineObject *block = new DrEngineObject(this, getNextKey(), body_type, asset_key, position.x() + offset_x, -position.y() + offset_y, z_order,
                                               scale, c_friction, c_bounce, collide, true, angle, alpha);
    block->addShapeBoxFromTexture(asset_key);
    addThing(block);

    // ***** Set collision type
    Collision_Type collision_type = Collision_Type::Damage_None;
    long           damage_type = thing->getComponentPropertyValue(Components::Thing_Settings_Object, Properties::Thing_Object_Damage).toInt();
    switch (damage_type) {
        case 0: collision_type = Collision_Type::Damage_None;   break;
        case 1: collision_type = Collision_Type::Damage_Player; break;
        case 2: collision_type = Collision_Type::Damage_Enemy;  break;
        case 3: collision_type = Collision_Type::Damage_All;    break;
    }
    block->setCollisionType(collision_type);

    // ***** Velocity settings
    QPointF vel_x = thing->getComponentPropertyValue(Components::Thing_Movement, Properties::Thing_Velocity_X).toPointF();
    QPointF vel_y = thing->getComponentPropertyValue(Components::Thing_Movement, Properties::Thing_Velocity_Y).toPointF();
    QPointF rotation_vel =  thing->getComponentPropertyValue(Components::Thing_Movement, Properties::Thing_Spin_Velocity).toPointF();
    bool    angle_velocty = thing->getComponentPropertyValue(Components::Thing_Movement, Properties::Thing_Angle_Velocity).toBool();

    cpVect velocity;
    velocity.x = vel_x.x() + (QRandomGenerator::global()->bounded(vel_x.y() * 2.0) - vel_x.y());
    velocity.y = vel_y.x() + (QRandomGenerator::global()->bounded(vel_y.y() * 2.0) - vel_y.y());
    double deg_angular = rotation_vel.x() + (QRandomGenerator::global()->bounded(rotation_vel.y() * 2.0) - rotation_vel.y());
    double rad_angular = qDegreesToRadians( deg_angular );
    if (body_type != Body_Type::Static) {
        cpBodySetVelocity( block->body, velocity );
        cpBodySetAngularVelocity( block->body, rad_angular );

        // Attach KinematicUpdateVelocity callback function
        if (body_type == Body_Type::Kinematic) {
            block->setOriginalVelocityX( velocity.x );
            block->setOriginalVelocityY( velocity.y );
            block->setOriginalSpinVelocity( rad_angular );
            block->setUseAngleVelocity( angle_velocty );
            cpBodySetVelocityUpdateFunc(block->body, KinematicUpdateVelocity);
        }
    }

    // ***** Appearance settings
    bool    cast_shadows =  thing->getComponentPropertyValue(Components::Thing_Lighting,   Properties::Thing_Lighting_Cast_Shadows).toBool();
    QPointF pixelation =    thing->getComponentPropertyValue(Components::Thing_Appearance, Properties::Thing_Filter_Pixelation).toPointF();
    float   brightness =    thing->getComponentPropertyValue(Components::Thing_Appearance, Properties::Thing_Filter_Brightness).toInt() / 255.f;
    float   contrast =      thing->getComponentPropertyValue(Components::Thing_Appearance, Properties::Thing_Filter_Contrast).toInt() / 255.f;
    float   saturation =    thing->getComponentPropertyValue(Components::Thing_Appearance, Properties::Thing_Filter_Saturation).toInt() / 255.f;
    float   hue =           thing->getComponentPropertyValue(Components::Thing_Appearance, Properties::Thing_Filter_Hue).toInt() / 360.f;
    bool    grayscale =     thing->getComponentPropertyValue(Components::Thing_Appearance, Properties::Thing_Filter_Grayscale).toBool();
    bool    negative =      thing->getComponentPropertyValue(Components::Thing_Appearance, Properties::Thing_Filter_Negative).toBool();
    block->cast_shadows =   cast_shadows;
    block->pixel_x =        static_cast<float>(pixelation.x());
    block->pixel_y =        static_cast<float>(pixelation.y());
    block->brightness =     brightness;
    block->contrast =       contrast;
    block->saturation =     saturation;
    block->hue =            hue;
    block->grayscale =      grayscale;
    block->negative =       negative;
}


//####################################################################################
//##    Loads one DrProject DrThingType::Light to World / Space
//####################################################################################
void DrEngineWorld::loadLightToWorld(DrThing *thing, double offset_x, double offset_y) {

    // Load Light Thing Properties
    QPointF     position =      thing->getComponentPropertyValue(Components::Thing_Transform, Properties::Thing_Position).toPointF();
    double      z_order =       thing->getComponentPropertyValue(Components::Thing_Layering,  Properties::Thing_Z_Order).toDouble();
    float       alpha =         thing->getComponentPropertyValue(Components::Thing_Layering,  Properties::Thing_Opacity).toFloat() / 100.0f;
    double      light_size =    thing->getComponentPropertyValue(Components::Thing_Transform, Properties::Thing_Size).toPointF().x();
    QColor      light_color =   QColor::fromRgba(thing->getComponentPropertyValue(Components::Thing_Settings_Light, Properties::Thing_Light_Color).toUInt());
    int         light_type =    thing->getComponentPropertyValue(Components::Thing_Settings_Light, Properties::Thing_Light_Type).toInt();

    double      cone_start =    thing->getComponentPropertyValue(Components::Thing_Settings_Light, Properties::Thing_Light_Cone_Start).toDouble();
    double      cone_end =      thing->getComponentPropertyValue(Components::Thing_Settings_Light, Properties::Thing_Light_Cone_End).toDouble();
    float       intensity =     thing->getComponentPropertyValue(Components::Thing_Settings_Light, Properties::Thing_Light_Intensity).toFloat();
    float       blur =          thing->getComponentPropertyValue(Components::Thing_Settings_Light, Properties::Thing_Light_Blur).toFloat();
    float       shadows =       thing->getComponentPropertyValue(Components::Thing_Settings_Light, Properties::Thing_Light_Shadows).toFloat();
    bool        draw_shadows =  thing->getComponentPropertyValue(Components::Thing_Settings_Light, Properties::Thing_Light_Draw_Shadows).toBool();

    float       pulse =         thing->getComponentPropertyValue(Components::Thing_Settings_Light, Properties::Thing_Light_Pulse).toFloat();
    float       pulse_speed =   thing->getComponentPropertyValue(Components::Thing_Settings_Light, Properties::Thing_Light_Pulse_Speed).toFloat();

    light_size *= c_light_size_adjuster;

    addThing( new DrEngineLight(this, getNextKey(), position.x() + offset_x, -position.y() + offset_y, z_order, static_cast<Light_Type>(light_type),
                                light_color, static_cast<float>(light_size),
                                QPointF(cone_start, cone_end), intensity, shadows, draw_shadows, blur, pulse, pulse_speed, alpha));
}


//####################################################################################
//##    Loads one DrProject DrThingType::Light to World / Space
//####################################################################################
void DrEngineWorld::loadWaterToWorld(DrThing *thing, double offset_x, double offset_y) {

    // Load Light Thing Properties
    QPointF     position =      thing->getComponentPropertyValue(Components::Thing_Transform, Properties::Thing_Position).toPointF();
    double      z_order =       thing->getComponentPropertyValue(Components::Thing_Layering,  Properties::Thing_Z_Order).toDouble();
    float       alpha =         thing->getComponentPropertyValue(Components::Thing_Layering,  Properties::Thing_Opacity).toFloat() / 100.0f;
    QPointF     size =          thing->getComponentPropertyValue(Components::Thing_Transform, Properties::Thing_Size).toPointF();

    QColor      water_color =   QColor::fromRgba(thing->getComponentPropertyValue(Components::Thing_Settings_Water, Properties::Thing_Water_Color).toUInt());
    float       water_tint =    thing->getComponentPropertyValue(Components::Thing_Settings_Water, Properties::Thing_Water_Color_Tint).toFloat() / 100.0f;
    float       reflection =    thing->getComponentPropertyValue(Components::Thing_Settings_Water, Properties::Thing_Water_Reflection).toFloat() / 100.0f;
    float       ripple_length = thing->getComponentPropertyValue(Components::Thing_Settings_Water, Properties::Thing_Water_Ripple_Length).toFloat();
    float       ripple_speed =  thing->getComponentPropertyValue(Components::Thing_Settings_Water, Properties::Thing_Water_Ripple_Speed).toFloat();

    addThing( new DrEngineWater(this, getNextKey(), position.x() + offset_x, -position.y() + offset_y, z_order,
                                size, water_color, water_tint, reflection, ripple_length, ripple_speed,
                                alpha ) );
}











