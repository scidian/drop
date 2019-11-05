//
//      Created by Stephens Nunnally on 12/7/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrThing Class Definitions
//
//
#include "debug.h"
#include "forms/form_main.h"
#include "globals.h"
#include "helper.h"
#include "helper_qt.h"
#include "imaging/imaging.h"
#include "opengl/opengl.h"
#include "project.h"
#include "project_asset.h"
#include "project_world.h"
#include "project_stage.h"
#include "project_thing.h"
#include "settings/settings.h"
#include "settings/settings_component.h"
#include "settings/settings_component_property.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
DrThing::DrThing(DrProject *parent_project, DrWorld *parent_world, DrStage *parent_stage,
                 long new_thing_key, QString new_thing_name, DrThingType new_thing_type,
                 long from_asset_key,
                 double x, double y, double z,
                 bool should_collide) : DrSettings(parent_project) {
    m_parent_world = parent_world;                  // pointer to parent World
    m_parent_stage = parent_stage;                  // pointer to parent Stage

    this->setKey(new_thing_key);                    // assign key passed in from key generator, this key matches key in parent Stage map container

    m_thing_type = new_thing_type;                  // assign thing type
    m_asset_key =  from_asset_key;                  // associated asset key

    getComponentProperty(Components::Entity_Settings, Properties::Entity_Name)->setEditable(false);
    getComponentProperty(Components::Entity_Settings, Properties::Entity_Key)->setHidden(true);
    addPropertyToComponent(Components::Entity_Settings, Properties::Entity_Asset_Key, Property_Type::Int, QVariant::fromValue(from_asset_key),
                           "Asset ID Key", "ID Key of Asset this item represents.", false, false);

    DrAsset *asset = getParentProject()->findAssetFromKey(from_asset_key);


    // Call to load in all the components / properties for this Stage thing
    switch (new_thing_type) {
        case DrThingType::Character:
            addComponentSettingsCharacter(new_thing_name);
            addComponentTransform(asset->getWidth(), asset->getHeight(), x, -y, DrThingType::Character);
            addComponentLayering(z);
            addComponentLighting();
            addComponent3D();
            addComponentAppearance();
            addComponentSpecialEffects();
            break;

        case DrThingType::Object:
            addComponentSettingsObject(new_thing_name, should_collide);
            addComponentTransform(asset->getWidth(), asset->getHeight(), x, -y, DrThingType::Object);
            addComponentLayering(z);
            addComponentMovement();
            addComponentLighting();
            addComponent3D();
            addComponentAppearance();
            addComponentSpecialEffects();
            break;

        case DrThingType::Camera:
            addComponentSettingsCamera("Camera 1");
            addComponentTransform(static_cast<int>(c_image_size * 0.625), c_image_size, x, -y, DrThingType::Fire);
            addComponentLayering(z, 100.0);
            addComponentAppearance(true);
            ///setComponentPropertyValue(Components::Size_Settings, Properties::Size_Keep_Square, true);
            setComponentPropertyValue(Components::Size_Settings, Properties::Size_Max_Size, QPointF(100, 100));
            setComponentPropertyValue(Components::Size_Settings, Properties::Size_Min_Size, QPointF(100, 100));
            break;

        case DrThingType::Text:
            addComponentSettingsText(new_thing_name);
            addComponentTransform(asset->getWidth(), asset->getHeight(), x, -y, DrThingType::Text);
            addComponentLayering(z);
            break;

        // ***** DrEffectType DrThing's
        case DrThingType::Fire:
            addComponentSettingsFire();
            addComponentTransform(static_cast<int>(c_image_size * 0.625), c_image_size, x, -y, DrThingType::Fire);
            addComponentLayering(z, 100.0);
            addComponentAppearance(true);
            break;
        case DrThingType::Fisheye:
            addComponentSettingsFisheye();
            addComponentTransform(c_image_size, c_image_size, x, -y, DrThingType::Fisheye);
            addComponentLayering(z, 100.0);
            addComponentAppearance(true);
            break;
        case DrThingType::Light:
            addComponentSettingsLight(Qt::white);
            addComponentTransform(c_image_size, c_image_size, x, -y, DrThingType::Light);
            addComponentLayering(z);
            setComponentPropertyValue(Components::Size_Settings, Properties::Size_Keep_Square, true);
            setComponentPropertyValue(Components::Size_Settings, Properties::Size_Max_Size, QPointF( c_desired_light_fbo_size,  c_desired_light_fbo_size));
            setComponentPropertyValue(Components::Size_Settings, Properties::Size_Min_Size, QPointF(-c_desired_light_fbo_size, -c_desired_light_fbo_size));
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
            setComponentPropertyValue(Components::Size_Settings, Properties::Size_Keep_Square, true);
            break;
        case DrThingType::Water:
            addComponentSettingsWater();
            addComponentTransform(c_image_size, c_image_size, x, -y, DrThingType::Water);
            addComponentLayering(z, 75.0);
            addComponentAppearance(true);
            break;

        default:
            Dr::ShowMessageBox("Error in DrThing Constructor, DrThingType not handled! Type: " + Dr::StringFromThingType(new_thing_type),
                               QMessageBox::Icon::Critical, "Error!", Dr::GetActiveFormMain());
    }

}

DrThing::~DrThing() { }




















