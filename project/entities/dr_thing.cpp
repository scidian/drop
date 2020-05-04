//
//      Created by Stephens Nunnally on 12/7/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrThing Class Definitions
//
//
#include "core/dr_debug.h"
#include "engine/enums_engine.h"
#include "engine/opengl/opengl.h"
#include "project/constants_component_info.h"
#include "project/dr_project.h"
#include "project/entities/dr_asset.h"
#include "project/entities/dr_font.h"
#include "project/entities/dr_stage.h"
#include "project/entities/dr_thing.h"
#include "project/entities/dr_world.h"
#include "project/settings/settings.h"
#include "project/settings/settings_component.h"
#include "project/settings/settings_component_property.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
DrThing::DrThing(DrProject *parent_project, DrWorld *parent_world, DrStage *parent_stage,
                 long new_thing_key, std::string new_thing_name, DrThingType new_thing_type,
                 DrSettings *from_entity,
                 double x, double y, double z,
                 bool should_collide)
    : DrSettings(parent_project) {

    m_parent_world = parent_world;                  // pointer to parent World
    m_parent_stage = parent_stage;                  // pointer to parent Stage

    this->setKey(new_thing_key);                    // assign key passed in from key generator, this key matches key in parent Stage map container

    m_thing_type = new_thing_type;                  // assign thing type
    m_asset_key =  from_entity->getKey();           // associated asset key

    getComponentProperty(Comps::Entity_Settings, Props::Entity_Name)->setEditable(false);
    getComponentProperty(Comps::Entity_Settings, Props::Entity_Key)->setHidden(true);
    addPropertyToComponent(Comps::Entity_Settings, Props::Entity_Asset_Key, Property_Type::Int, m_asset_key,
                           "Asset ID Key", "ID Key of Asset this Thing represents.", false, false);

    // ********** Call to load in all the Components / Properties for this Thing
    switch (new_thing_type) {
        case DrThingType::Character: {
            DrAsset *asset = dynamic_cast<DrAsset*>(from_entity);
            getComponentProperty(Comps::Entity_Settings, Props::Entity_Key)->setHidden(false);
            getComponentProperty(Comps::Entity_Settings, Props::Entity_Key)->setDisplayName("Character ID Key");
            addComponentSettingsCharacter(new_thing_name);
            addComponentTransform(asset->getWidth(), asset->getHeight(), x, -y, DrThingType::Character);
            addComponentLayering(z);
            addComponentLighting();
            addComponent3D();
            addComponentAppearance();
            addComponentSpecialEffects();
            break;
        }
        case DrThingType::Object: {
            DrAsset *asset = dynamic_cast<DrAsset*>(from_entity);
            getComponentProperty(Comps::Entity_Settings, Props::Entity_Key)->setHidden(false);
            getComponentProperty(Comps::Entity_Settings, Props::Entity_Key)->setDisplayName("Object ID Key");
            addComponentSettingsObject(new_thing_name, should_collide);            
            addComponentTransform(asset->getWidth(), asset->getHeight(), x, -y, DrThingType::Object);
            addComponentLayering(z);
            addComponentMovement();
            addComponentLighting();
            addComponentSpawn();
            addComponent3D();
            addComponentAppearance();
            addComponentSpecialEffects();
            break;
        }
        case DrThingType::Text: {
            DrFont *font = dynamic_cast<DrFont*>(from_entity);
            DrRect rect = font->getCharRect('A');
            addComponentSettingsText(new_thing_name);
            addComponentTransform(rect.width * 4 /* "Test" */, rect.height, x, -y, DrThingType::Text);
            addComponentLayering(z);
            break;
        }


        // ***** Device Things
        case DrThingType::Camera:
            addComponentSettingsCamera("Camera");
            addComponentTransform(c_device_size, c_device_size, x, -y, DrThingType::Camera);
            addComponentLayering(z, 100.0, false);
            addComponentAppearance(true, false);
            setComponentPropertyValue(Comps::Size_Settings, Props::Size_Max_Size, DrPointF(100, 100));
            setComponentPropertyValue(Comps::Size_Settings, Props::Size_Min_Size, DrPointF(100, 100));
            break;            


        // ***** Effect Things
        case DrThingType::Fire:
            addComponentSettingsFire();
            addComponentTransform(static_cast<int>(c_image_size * 0.625), c_image_size, x, -y, DrThingType::Fire);
            addComponentLayering(z, 100.0);
            addComponent3D();
            getComponentProperty(Comps::Thing_3D, Props::Thing_3D_Type)->setHidden(true);
            getComponentProperty(Comps::Thing_3D, Props::Thing_3D_Depth)->setHidden(true);
            addComponentAppearance(true);
            break;
        case DrThingType::Fisheye:
            addComponentSettingsFisheye();
            addComponentTransform(c_image_size, c_image_size, x, -y, DrThingType::Fisheye);
            addComponentLayering(z, 100.0);
            addComponentAppearance(true);
            break;
        case DrThingType::Light:
            addComponentSettingsLight(Dr::white);
            addComponentTransform(c_image_size, c_image_size, x, -y, DrThingType::Light);
            addComponentLayering(z);
            setComponentPropertyValue(Comps::Size_Settings, Props::Size_Keep_Square, true);
            setComponentPropertyValue(Comps::Size_Settings, Props::Size_Max_Size, DrPointF( c_desired_light_fbo_size,  c_desired_light_fbo_size));
            setComponentPropertyValue(Comps::Size_Settings, Props::Size_Min_Size, DrPointF(-c_desired_light_fbo_size, -c_desired_light_fbo_size));
            break;
        case DrThingType::Mirror:
            addComponentSettingsMirror();
            addComponentTransform(c_image_size, c_image_size, x, -y, DrThingType::Mirror);
            addComponentLayering(z);
            addComponentAppearance(true);
            break;
        case DrThingType::Swirl:
            addComponentSettingsSwirl();
            addComponentTransform(c_image_size, c_image_size, x, -y, DrThingType::Swirl);
            addComponentLayering(z);
            addComponentAppearance(true);
            setComponentPropertyValue(Comps::Size_Settings, Props::Size_Keep_Square, true);
            break;
        case DrThingType::Water:
            addComponentSettingsWater();
            addComponentTransform(c_image_size, c_image_size, x, -y, DrThingType::Water);
            addComponentLayering(z, 75.0);
            addComponentAppearance(true);
            break;


        // ***** Item Things
        case DrThingType::Tile:
            addComponentSettingsTile("Tile");
            addComponentTransform(c_device_size, c_device_size, x, -y, DrThingType::Tile);
            addComponentLayering(z, 100.0);
            addComponentAppearance(true);
            break;

        default:
            Dr::PrintDebug("Error in DrThing Constructor, DrThingType not handled! Type: " + Dr::StringFromThingType(new_thing_type));
    }

}

DrThing::~DrThing() { }




















