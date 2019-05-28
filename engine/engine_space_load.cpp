//
//      Created by Stephens Nunnally on 5/15/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "engine.h"
#include "project/project_world_stage.h"
#include "project/project_world_stage_object.h"

void DrEngine::loadStageToSpace(DrStage *stage, double offset_x, double offset_y) {

    // Load objects
    for (auto object_pair : stage->getObjectMap()) {

        DrObject *object = object_pair.second;

        if (object->getType() != DrType::Object) continue;
        if (object->getObjectType() != DrObjectType::Object) continue;

        QPointF     position =      object->getComponentPropertyValue(Components::Object_Transform, Properties::Object_Position).toPointF();
        QPointF     scale =         object->getComponentPropertyValue(Components::Object_Transform, Properties::Object_Scale).toPointF();
        double      angle =         object->getComponentPropertyValue(Components::Object_Transform, Properties::Object_Rotation).toDouble();
        double      z_order =       object->getComponentPropertyValue(Components::Object_Layering,  Properties::Object_Z_Order).toDouble();
        double      alpha =         object->getComponentPropertyValue(Components::Object_Layering,  Properties::Object_Opacity).toDouble() / 100;
        bool        collide =       object->getComponentPropertyValue(Components::Object_Settings,  Properties::Object_Collide).toBool();
        bool        physics =       object->getComponentPropertyValue(Components::Object_Settings,  Properties::Object_Physics).toBool();
        Body_Type   body =          physics? Body_Type::Dynamic : Body_Type::Kinematic;

        long        asset_key = object->getAssetKey();

        SceneObject *block = addBlock(body, asset_key, position.x() + offset_x, -position.y() + offset_y, z_order,
                                      angle, scale, alpha, c_friction, c_bounce, QPointF(0, 0), collide);

        Collision_Type collision_type = Collision_Type::Damage_None;
        long           damage_type = object->getComponentPropertyValue(Components::Object_Settings, Properties::Object_Damage).toInt();
        switch (damage_type) {
            case 0: collision_type = Collision_Type::Damage_None;   break;
            case 1: collision_type = Collision_Type::Damage_Player; break;
            case 2: collision_type = Collision_Type::Damage_Enemy;  break;
            case 3: collision_type = Collision_Type::Damage_All;    break;
        }

        block->collision_type = collision_type;
        for (auto shape : block->shapes)
            cpShapeSetCollisionType(shape, static_cast<cpCollisionType>(collision_type));

    }

    // Update distance we've loaded scenes to
    m_loaded_to += stage->getComponentPropertyValue(Components::Stage_Settings, Properties::Stage_Size).toInt();

}







