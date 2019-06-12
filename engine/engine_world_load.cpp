//
//      Created by Stephens Nunnally on 5/15/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "engine.h"
#include "engine_object.h"
#include "engine_world.h"
#include "project/project_world_stage.h"
#include "project/project_world_stage_object.h"


//######################################################################################################
//##    Load DrProject Stage to Space
//######################################################################################################
void DrEngineWorld::loadStageToSpace(DrStage *stage, double offset_x, double offset_y) {

    // Load objects
    for (auto object_pair : stage->getObjectMap()) {

        // Grab current object
        DrObject *object = object_pair.second;

        // Check if Object type
        if (object->getType() != DrType::Object) continue;
        if (object->getObjectType() != DrObjectType::Object) continue;

        // ***** Load Object properties
        long        asset_key =     object->getAssetKey();
        QPointF     position =      object->getComponentPropertyValue(Components::Object_Transform, Properties::Object_Position).toPointF();
        QPointF     scale =         object->getComponentPropertyValue(Components::Object_Transform, Properties::Object_Scale).toPointF();
        double      angle =         object->getComponentPropertyValue(Components::Object_Transform, Properties::Object_Rotation).toDouble();
        double      z_order =       object->getComponentPropertyValue(Components::Object_Layering,  Properties::Object_Z_Order).toDouble();
        double      alpha =         object->getComponentPropertyValue(Components::Object_Layering,  Properties::Object_Opacity).toDouble() / 100;
        bool        collide =       object->getComponentPropertyValue(Components::Object_Settings,  Properties::Object_Collide).toBool();
        int         physics =       object->getComponentPropertyValue(Components::Object_Settings, Properties::Object_Physics_Type).toInt();

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
        long           damage_type = object->getComponentPropertyValue(Components::Object_Settings, Properties::Object_Damage).toInt();
        switch (damage_type) {
            case 0: collision_type = Collision_Type::Damage_None;   break;
            case 1: collision_type = Collision_Type::Damage_Player; break;
            case 2: collision_type = Collision_Type::Damage_Enemy;  break;
            case 3: collision_type = Collision_Type::Damage_All;    break;
        }
        block->setCollisionType(collision_type);

        QPointF pixelation = object->getComponentPropertyValue(Components::Object_Appearance, Properties::Object_Filter_Pixelation).toPointF();
        float   brightness = object->getComponentPropertyValue(Components::Object_Appearance, Properties::Object_Filter_Brightness).toInt() / 255.f;
        float   contrast =   object->getComponentPropertyValue(Components::Object_Appearance, Properties::Object_Filter_Contrast).toInt() / 255.f;
        float   saturation = object->getComponentPropertyValue(Components::Object_Appearance, Properties::Object_Filter_Saturation).toInt() / 255.f;
        float   hue =        object->getComponentPropertyValue(Components::Object_Appearance, Properties::Object_Filter_Hue).toInt() / 360.f;
        bool    grayscale =  object->getComponentPropertyValue(Components::Object_Appearance, Properties::Object_Filter_Grayscale).toBool();
        bool    negative =   object->getComponentPropertyValue(Components::Object_Appearance, Properties::Object_Filter_Negative).toBool();

        block->pixel_x =    static_cast<float>(pixelation.x());
        block->pixel_y =    static_cast<float>(pixelation.y());
        block->brightness = brightness;
        block->contrast =   contrast;
        block->saturation = saturation;
        block->hue =        hue;
        block->grayscale =  grayscale;
        block->negative =   negative;
    }

    // Update distance we've loaded scenes to
    m_loaded_to += stage->getComponentPropertyValue(Components::Stage_Settings, Properties::Stage_Size).toInt();

}











