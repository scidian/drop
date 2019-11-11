//
//      Created by Stephens Nunnally on 10/20/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "3rd_party/hull_finder.h"
#include "3rd_party/polyline_simplification.h"
#include "debug.h"
#include "engine_mesh/engine_vertex_data.h"
#include "forms/form_main.h"
#include "globals.h"
#include "helper.h"
#include "helper_qt.h"
#include "imaging/imaging.h"
#include "project.h"
#include "project_asset.h"
#include "project_effect.h"
#include "project_font.h"
#include "project_image.h"
#include "project_world.h"
#include "project_stage.h"
#include "project_thing.h"
#include "project/thing_shape_list.h"
#include "settings/settings.h"
#include "settings/settings_component.h"
#include "settings/settings_component_property.h"


//####################################################################################
//##    Character Asset Components
//####################################################################################
void DrAsset::initializeAssetSettingsCharacter(QString new_name) {
    DrProperty *property_name = getComponentProperty(Components::Entity_Settings, Properties::Entity_Name);
    property_name->setDisplayName("Character Name");
    property_name->setDescription("Name of this Character Asset.");
    property_name->setValue(new_name);

    addComponent(Components::Asset_Settings_Character, "Character Settings", "Settings for this Character.", Component_Colors::White_Snow, true);
    getComponent(Components::Asset_Settings_Character)->setIcon(Component_Icons::Character);

    addPropertyToComponent(Components::Asset_Settings_Character, Properties::Asset_Character_Max_Speed, Property_Type::PointF, QPointF(1000, 1000),
                           "Max Speed", "Maximum movement speed of this Character in the x and y direction.");
    addPropertyToComponent(Components::Asset_Settings_Character, Properties::Asset_Character_Forced_Speed, Property_Type::PointF, QPointF(0, 0),
                           "Forced Speed", "Forced movement speed of this Character in the x and y direction.");
    addPropertyToComponent(Components::Asset_Settings_Character, Properties::Asset_Character_Move_Speed, Property_Type::PointF, QPointF(400, 400),
                           "Move Speed", "Button / Joystick movement speed of this Character in the x and y direction.");
    addPropertyToComponent(Components::Asset_Settings_Character, Properties::Asset_Character_Angle_Movement, Property_Type::Bool, false,
                           "Relative Angle?", "Should the rotation of this Character affect the movement speed forces? Good for moving in first person.");
    addPropertyToComponent(Components::Asset_Settings_Character, Properties::Asset_Character_Jump_Force, Property_Type::PointF, QPointF(0, 250),
                           "Jump Force", "Force of jump button in the x and y direction");

    addPropertyToComponent(Components::Asset_Settings_Character, Properties::Asset_Character_Jump_Timeout, Property_Type::Double, 800,
                           "Jump Timeout", "Time, in milliseconds, Character should continue to gain jump force when jump button is held down. For "
                                           "jump to never timeout use any negative number (like -1), useful for Character glide ability.");
    addPropertyToComponent(Components::Asset_Settings_Character, Properties::Asset_Character_Jump_Counter, Property_Type::Int, 1,
                           "Jump Count", "Number of jumps Character can make before having to touch the ground or wall. For unlimited jumps "
                                         "use any negative number (like -1). A Jump Count of 0 disables jumping for this Character.");

    addPropertyToComponent(Components::Asset_Settings_Character, Properties::Asset_Character_Jump_Air, Property_Type::Bool, true,
                           "Air Jump?", "Can this Character start jumping while falling in the air? (for example, if the Character "
                                        "fell off a platform)");
    addPropertyToComponent(Components::Asset_Settings_Character, Properties::Asset_Character_Jump_Wall, Property_Type::Bool, false,
                           "Wall Jump?", "Can this Character jump off of walls?");

    addPropertyToComponent(Components::Asset_Settings_Character, Properties::Asset_Character_Acceleration, Property_Type::PositiveDouble, 1.0,
                           "Acceleration", "Multiplier affecting how fast Character starts to move / switch directions. A value of 1.0 is default, a higher "
                                           "number like 5.0 will cause slower acceleration, a value of 0.0 will cause instant acceleration.");
    addPropertyToComponent(Components::Asset_Settings_Character, Properties::Asset_Character_Air_Drag, Property_Type::PositiveDouble, 1.0,
                           "Air Drag", "Affects acceleration and decceleration in air. Usually ranging from 0.0 to 1.0 or higher.");
    addPropertyToComponent(Components::Asset_Settings_Character, Properties::Asset_Character_Ground_Drag, Property_Type::PositiveDouble, 1.0,
                           "Ground Drag", "Affects acceleration and decceleration on the ground. Usually ranging from 0.0 to 1.0 or higher.");
    addPropertyToComponent(Components::Asset_Settings_Character, Properties::Asset_Character_Rotation_Drag, Property_Type::PositiveDouble, 0.25,
                           "Rotate Drag", "Affects rotation acceleration and decceleration. Usually ranging from 0.0 to 1.0 or higher.");

    addPropertyToComponent(Components::Asset_Settings_Character, Properties::Asset_Character_Flip_Image_X, Property_Type::Bool, false,
                           "Flip Image X?", "Should this characters image flip left and right depending on movement?");
    addPropertyToComponent(Components::Asset_Settings_Character, Properties::Asset_Character_Flip_Image_Y, Property_Type::Bool, false,
                           "Flip Image Y?", "Should this characters image flip up and down depending on movement?");
}


//####################################################################################
//##    Object Asset Components
//####################################################################################
void DrAsset::initializeAssetSettingsObject(QString new_name) {
    DrProperty *property_name = getComponentProperty(Components::Entity_Settings, Properties::Entity_Name);
    property_name->setDisplayName("Object Name");
    property_name->setDescription("Name of this Object Asset.");
    property_name->setValue(new_name);

//    addComponent(Components::Asset_Settings_Object, "Object Settings", "Settings for this Object.", Component_Colors::White_Snow, true);
//    getComponent(Components::Asset_Settings_Object)->setIcon(Component_Icons::Object);

//    addPropertyToComponent(Components::Asset_Settings_Object, Properties::Asset_Object_One_Way_Type, Property_Type::List, 0,
//                           "One Way Type", "Type of One Way collision for this object. <b></b>");
}


//####################################################################################
//##    Device Asset Components
//####################################################################################
void DrAsset::initializeAssetSettingsDevice(QString new_name) {
    DrProperty *property_name = getComponentProperty(Components::Entity_Settings, Properties::Entity_Name);
    property_name->setDisplayName("Device Name");
    property_name->setDescription("Name of this Device Asset.");
    property_name->setValue(new_name);
}


//####################################################################################
//##    Effect Asset Components
//####################################################################################
void DrAsset::initializeAssetSettingsEffect(QString new_name) {
    DrProperty *property_name = getComponentProperty(Components::Entity_Settings, Properties::Entity_Name);
    property_name->setDisplayName("Effect Name");
    property_name->setDescription("Name of this Effect Asset.");
    property_name->setValue(new_name);
}


//####################################################################################
//##    Font Asset Components
//####################################################################################
void DrAsset::initializeAssetSettingsFont(DrFont *font) {
    DrProperty *property_name = getComponentProperty(Components::Entity_Settings, Properties::Entity_Name);
    property_name->setDisplayName("Font Name");
    property_name->setDescription("Name of this Font Asset.");
    property_name->setValue(font->getName());

    addComponent(Components::Asset_Settings_Font, "Font Settings", "Font settings for this Text Asset.", Component_Colors::Orange_Medium, true);
    getComponent(Components::Asset_Settings_Font)->setIcon(Component_Icons::Font);

    addPropertyToComponent(Components::Asset_Settings_Font, Properties::Asset_Font_Family, Property_Type::String, font->getPropertyFontFamily(),
                           "Font Family", "Font used for this text asset.", false, false);
    addPropertyToComponent(Components::Asset_Settings_Font, Properties::Asset_Font_Size, Property_Type::Int, font->getPropertyFontSize(),
                           "Font Size", "Font size of this text asset.", false, false);
}













