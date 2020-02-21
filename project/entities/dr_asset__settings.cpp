//
//      Created by Stephens Nunnally on 10/20/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "project/constants_component_info.h"
#include "project/dr_project.h"
#include "project/entities/dr_asset.h"
#include "project/entities/dr_stage.h"
#include "project/entities/dr_thing.h"
#include "project/entities/dr_world.h"
#include "project/settings/settings.h"
#include "project/settings/settings_component.h"
#include "project/settings/settings_component_property.h"


//####################################################################################
//##    Character Asset Components
//####################################################################################
void DrAsset::initializeAssetSettingsCharacter(std::string new_name) {
    DrProperty *property_name = getComponentProperty(Comps::Entity_Settings, Props::Entity_Name);
    property_name->setDisplayName("Character Name");
    property_name->setDescription("Name of this Character Asset.");
    property_name->setValue(new_name);

    addComponent(Comps::Asset_Settings_Character, "Character Settings", "Settings for this Character.", Component_Colors::RGB_20_Tan, true);
    getComponent(Comps::Asset_Settings_Character)->setIcon(Component_Icons::Character);

    addPropertyToComponent(Comps::Asset_Settings_Character, Props::Asset_Character_Max_Speed, Property_Type::PointF, DrPointF(2000, 2000),
                           "Max Speed", "Maximum movement speed of this Character in the x and y direction.");
    addPropertyToComponent(Comps::Asset_Settings_Character, Props::Asset_Character_Forced_Speed, Property_Type::PointF, DrPointF(0, 0),
                           "Forced Speed", "Forced movement speed of this Character in the x and y direction.");
    addPropertyToComponent(Comps::Asset_Settings_Character, Props::Asset_Character_Move_Speed, Property_Type::PointF, DrPointF(400, 400),
                           "Move Speed", "Button / Joystick movement speed of this Character in the x and y direction.");
    addPropertyToComponent(Comps::Asset_Settings_Character, Props::Asset_Character_Angle_Movement, Property_Type::Bool, false,
                           "Relative Angle?", "Should the rotation of this Character affect the movement speed forces? Good for moving in first person.");
    addPropertyToComponent(Comps::Asset_Settings_Character, Props::Asset_Character_Jump_Force, Property_Type::PointF, DrPointF(0, 250),
                           "Jump Force", "Force of jump button in the x and y direction");

    addPropertyToComponent(Comps::Asset_Settings_Character, Props::Asset_Character_Jump_Timeout, Property_Type::Double, 800,
                           "Jump Timeout", "Time, in milliseconds, Character should continue to gain jump force when jump button is held down. For "
                                           "jump to never timeout use any negative number (like -1), useful for Character glide ability.");
    addPropertyToComponent(Comps::Asset_Settings_Character, Props::Asset_Character_Jump_Counter, Property_Type::Int, 1,
                           "Jump Count", "Number of jumps Character can make before having to touch the ground or wall. For unlimited jumps "
                                         "use any negative number (like -1). A Jump Count of 0 disables jumping for this Character.");

    addPropertyToComponent(Comps::Asset_Settings_Character, Props::Asset_Character_Jump_Air, Property_Type::Bool, true,
                           "Air Jump?", "Can this Character start jumping while falling in the air? (for example, if the Character "
                                        "fell off a platform)");
    addPropertyToComponent(Comps::Asset_Settings_Character, Props::Asset_Character_Jump_Wall, Property_Type::Bool, false,
                           "Wall Jump?", "Can this Character jump off of walls?");

    addPropertyToComponent(Comps::Asset_Settings_Character, Props::Asset_Character_Acceleration, Property_Type::PositiveDouble, 1.0,
                           "Acceleration", "Multiplier affecting how fast Character starts to move / switch directions. A value of 1.0 is default, a higher "
                                           "number like 5.0 will cause slower acceleration, a value of 0.0 will cause instant acceleration.");
    addPropertyToComponent(Comps::Asset_Settings_Character, Props::Asset_Character_Air_Drag, Property_Type::PositiveDouble, 1.0,
                           "Air Drag", "Affects acceleration and decceleration in air. Usually ranging from 0.0 to 1.0 or higher.");
    addPropertyToComponent(Comps::Asset_Settings_Character, Props::Asset_Character_Ground_Drag, Property_Type::PositiveDouble, 1.0,
                           "Ground Drag", "Affects acceleration and decceleration on the ground. Usually ranging from 0.0 to 1.0 or higher.");
    addPropertyToComponent(Comps::Asset_Settings_Character, Props::Asset_Character_Rotation_Drag, Property_Type::PositiveDouble, 1.0,
                           "Rotate Drag", "Affects rotation acceleration and decceleration. Usually ranging from 0.0 to 1.0 or higher.");

    addPropertyToComponent(Comps::Asset_Settings_Character, Props::Asset_Character_Flip_Image_X, Property_Type::Bool, false,
                           "Flip Image X?", "Should this characters image flip left and right depending on movement?");
    addPropertyToComponent(Comps::Asset_Settings_Character, Props::Asset_Character_Flip_Image_Y, Property_Type::Bool, false,
                           "Flip Image Y?", "Should this characters image flip up and down depending on movement?");
    addPropertyToComponent(Comps::Asset_Settings_Character, Props::Asset_Character_Mouse_Rotate, Property_Type::Bool, false,
                           "Mouse Rotate", "Should this character rotate to the mouse cursor?");
}


//####################################################################################
//##    Object Asset Components
//####################################################################################
void DrAsset::initializeAssetSettingsObject(std::string new_name) {
    DrProperty *property_name = getComponentProperty(Comps::Entity_Settings, Props::Entity_Name);
    property_name->setDisplayName("Object Name");
    property_name->setDescription("Name of this Object Asset.");
    property_name->setValue(new_name);

//    addComponent(Components::Asset_Settings_Object, "Object Settings", "Settings for this Object.", Component_Colors::RGB_18_Gray, true);
//    getComponent(Components::Asset_Settings_Object)->setIcon(Component_Icons::Object);



}













