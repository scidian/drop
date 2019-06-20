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
#include "engine_object.h"
#include "engine_world.h"
#include "project/project_world_stage.h"
#include "project/project_world_stage_thing.h"


//######################################################################################################
//##    Load DrProject Stage to Space
//######################################################################################################
void DrEngineWorld::loadStageToSpace(DrStage *stage, double offset_x, double offset_y) {

    // Load Things
    for (auto thing_pair : stage->getThingMap()) {

        // ***** Grab current Thing
        DrThing *thing = thing_pair.second;

        // ***** Check if Thing type
        if (thing->getType() != DrType::Thing) continue;
        if (thing->getThingType() != DrThingType::Object) continue;

        // ***** Load Thing properties
        long        asset_key =     thing->getAssetKey();
        QPointF     position =      thing->getComponentPropertyValue(Components::Object_Transform, Properties::Object_Position).toPointF();
        QPointF     scale =         thing->getComponentPropertyValue(Components::Object_Transform, Properties::Object_Scale).toPointF();
        double      angle =         thing->getComponentPropertyValue(Components::Object_Transform, Properties::Object_Rotation).toDouble();
        double      z_order =       thing->getComponentPropertyValue(Components::Object_Layering,  Properties::Object_Z_Order).toDouble();
        double      alpha =         thing->getComponentPropertyValue(Components::Object_Layering,  Properties::Object_Opacity).toDouble() / 100;
        bool        collide =       thing->getComponentPropertyValue(Components::Object_Settings,  Properties::Object_Collide).toBool();
        int         physics =       thing->getComponentPropertyValue(Components::Object_Settings,  Properties::Object_Physics_Type).toInt();

        Body_Type body = Body_Type::Static;
        switch (physics) {
            case 0: body = Body_Type::Static;       break;
            case 1: body = Body_Type::Kinematic;    break;
            case 2: body = Body_Type::Dynamic;      break;
        }

        // ***** Add the block to the cpSpace
        DrEngineObject *block =     addBlock(body, asset_key, position.x() + offset_x, -position.y() + offset_y, z_order,
                                             angle, scale, alpha, c_friction, c_bounce, QPointF(0, 0), collide);

        // ***** Set collision type
        Collision_Type collision_type = Collision_Type::Damage_None;
        long           damage_type = thing->getComponentPropertyValue(Components::Object_Settings, Properties::Object_Damage).toInt();
        switch (damage_type) {
            case 0: collision_type = Collision_Type::Damage_None;   break;
            case 1: collision_type = Collision_Type::Damage_Player; break;
            case 2: collision_type = Collision_Type::Damage_Enemy;  break;
            case 3: collision_type = Collision_Type::Damage_All;    break;
        }
        block->setCollisionType(collision_type);

        // ***** Velocity settings
        QPointF vel_x = thing->getComponentPropertyValue(Components::Object_Movement, Properties::Object_Velocity_X).toPointF();
        QPointF vel_y = thing->getComponentPropertyValue(Components::Object_Movement, Properties::Object_Velocity_Y).toPointF();
        QPointF rotation_vel =  thing->getComponentPropertyValue(Components::Object_Movement, Properties::Object_Spin_Velocity).toPointF();
        bool    angle_velocty = thing->getComponentPropertyValue(Components::Object_Movement, Properties::Object_Angle_Velocity).toBool();

        cpVect velocity;
        velocity.x = vel_x.x() + (QRandomGenerator::global()->bounded(vel_x.y() * 2.0) - vel_x.y());
        velocity.y = vel_y.x() + (QRandomGenerator::global()->bounded(vel_y.y() * 2.0) - vel_y.y());
        double deg_angular = rotation_vel.x() + (QRandomGenerator::global()->bounded(rotation_vel.y() * 2.0) - rotation_vel.y());
        double rad_angular = qDegreesToRadians( deg_angular );
        if (body != Body_Type::Static) {
            cpBodySetVelocity( block->body, velocity );
            cpBodySetAngularVelocity( block->body, rad_angular );

            // Attach KinematicUpdateVelocity callback function
            if (body == Body_Type::Kinematic) {
                block->setOriginalVelocityX( velocity.x );
                block->setOriginalVelocityY( velocity.y );
                block->setOriginalSpinVelocity( rad_angular );
                block->setUseAngleVelocity( angle_velocty );
                cpBodySetVelocityUpdateFunc(block->body, KinematicUpdateVelocity);
            }
        }

        // ***** Appearance settings
        QPointF pixelation = thing->getComponentPropertyValue(Components::Object_Appearance, Properties::Object_Filter_Pixelation).toPointF();
        float   brightness = thing->getComponentPropertyValue(Components::Object_Appearance, Properties::Object_Filter_Brightness).toInt() / 255.f;
        float   contrast =   thing->getComponentPropertyValue(Components::Object_Appearance, Properties::Object_Filter_Contrast).toInt() / 255.f;
        float   saturation = thing->getComponentPropertyValue(Components::Object_Appearance, Properties::Object_Filter_Saturation).toInt() / 255.f;
        float   hue =        thing->getComponentPropertyValue(Components::Object_Appearance, Properties::Object_Filter_Hue).toInt() / 360.f;
        bool    grayscale =  thing->getComponentPropertyValue(Components::Object_Appearance, Properties::Object_Filter_Grayscale).toBool();
        bool    negative =   thing->getComponentPropertyValue(Components::Object_Appearance, Properties::Object_Filter_Negative).toBool();
        block->pixel_x =    static_cast<float>(pixelation.x());
        block->pixel_y =    static_cast<float>(pixelation.y());
        block->brightness = brightness;
        block->contrast =   contrast;
        block->saturation = saturation;
        block->hue =        hue;
        block->grayscale =  grayscale;
        block->negative =   negative;
    }


    // ***** Update distance we've loaded scenes to
    m_loaded_to += stage->getComponentPropertyValue(Components::Stage_Settings, Properties::Stage_Size).toInt();
}











