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
void DrAsset::initializeAssetSettingsCharacter(QString new_name, QPixmap pixmap) {
    DrProperty *property_name = getComponentProperty(Components::Entity_Settings, Properties::Entity_Name);
    property_name->setDisplayName("Character Name");
    property_name->setDescription("Name of the current Character Asset.");
    property_name->setValue(new_name);

    addComponent(Components::Asset_Settings_Character, "Character Settings", "Settings for this Character.", Component_Colors::Mustard_Yellow, true);
    getComponent(Components::Asset_Settings_Character)->setIcon(Component_Icons::Character);

    addPropertyToComponent(Components::Asset_Settings_Character, Properties::Asset_Character_Max_Speed, Property_Type::PointF, QPointF(1000, 1000),
                           "Max Speed", "Maximum movement speed of this Character in the x and y direction.");
    addPropertyToComponent(Components::Asset_Settings_Character, Properties::Asset_Character_Forced_Speed, Property_Type::PointF, QPointF(0, 0),
                           "Forced Speed", "Forced movement speed of this Character in the x and y direction.");
    addPropertyToComponent(Components::Asset_Settings_Character, Properties::Asset_Character_Move_Speed, Property_Type::PointF, QPointF(400, 400),
                           "Move Speed", "Button / Joystick movement speed of this Character in the x and y direction.");
    addPropertyToComponent(Components::Asset_Settings_Character, Properties::Asset_Character_Jump_Force, Property_Type::PointF, QPointF(0, 250),
                           "Jump Force", "Force of jump button in the x and y direction");

    addPropertyToComponent(Components::Asset_Settings_Character, Properties::Asset_Character_Jump_Timeout, Property_Type::Positive, 800,
                           "Jump Timeout", "Time, in milliseconds, Character should continue to gain jump force when jump button is held down.");
    addPropertyToComponent(Components::Asset_Settings_Character, Properties::Asset_Character_Jump_Counter, Property_Type::Int, 1,
                           "Jump Count", "Number of jumps Character can make before having to touch the ground or wall. For unlimited jumps "
                                         "use any negative number (like -1). A Jump Count of 0 disables jumping for this Character.");

    addPropertyToComponent(Components::Asset_Settings_Character, Properties::Asset_Character_Jump_Air, Property_Type::Bool, true,
                           "Air Jump?", "Can this Character start jumping while falling in the air? (for example, if the Character "
                                        "fell off a platform)");
    addPropertyToComponent(Components::Asset_Settings_Character, Properties::Asset_Character_Jump_Wall, Property_Type::Bool, false,
                           "Wall Jump?", "Can this Character jump off of walls?");

    addPropertyToComponent(Components::Asset_Settings_Character, Properties::Asset_Character_Air_Drag, Property_Type::PositiveDouble, 1.0,
                           "Air Drag", "Affects acceleration and decceleration in air. Usually ranging from 0.0 to 1.0 or higher.");
    addPropertyToComponent(Components::Asset_Settings_Character, Properties::Asset_Character_Ground_Drag, Property_Type::PositiveDouble, 1.0,
                           "Ground Drag", "Affects acceleration and decceleration on the ground. Usually ranging from 0.0 to 1.0 or higher.");
    addPropertyToComponent(Components::Asset_Settings_Character, Properties::Asset_Character_Rotation_Drag, Property_Type::PositiveDouble, 0.25,
                           "Rotate Drag", "Affects rotation acceleration and decceleration. Usually ranging from 0.0 to 1.0 or higher.");

    addPropertyToComponent(Components::Asset_Settings_Character, Properties::Asset_Character_Can_Rotate, Property_Type::Bool, false,
                           "Can Rotate?", "Can this character rotate (on z axis)? If not, rotation will be fixed.");
    addPropertyToComponent(Components::Asset_Settings_Character, Properties::Asset_Character_Feels_Gravity, Property_Type::Bool, true,
                           "Feels Gravity?", "Should this character be affected by gravity?");

    addPropertyToComponent(Components::Asset_Settings_Character, Properties::Asset_Character_Flip_Image_X, Property_Type::Bool, false,
                           "Flip Image X", "Should this characters image flip left and right depending on movement?");
    addPropertyToComponent(Components::Asset_Settings_Character, Properties::Asset_Character_Flip_Image_Y, Property_Type::Bool, false,
                           "Flip Image Y", "Should this characters image flip up and down depending on movement?");

    addComponent(Components::Asset_Animation, "Animation", "Images to show for this Asset.", Component_Colors::Green_SeaGrass, true);
    getComponent(Components::Asset_Animation)->setIcon(Component_Icons::Animation);
    addPropertyToComponent(Components::Asset_Animation, Properties::Asset_Animation_Default, Property_Type::Image, QVariant(pixmap),
                           "Default Animation", "Image shown for this Asset.");
}


//####################################################################################
//##    Object Asset Components
//####################################################################################
void DrAsset::initializeAssetSettingsObject(QString new_name, QPixmap pixmap) {
    DrProperty *property_name = getComponentProperty(Components::Entity_Settings, Properties::Entity_Name);
    property_name->setDisplayName("Object Name");
    property_name->setDescription("Name of the current Object Asset.");
    property_name->setValue(new_name);

//    addComponent(Components::Asset_Settings_Object, "Object Settings", "Settings for this Object.", Component_Colors::White_Snow, true);
//    getComponent(Components::Asset_Settings_Object)->setIcon(Component_Icons::Object);

//    addPropertyToComponent(Components::Asset_Settings_Object, Properties::Asset_Object_One_Way_Type, Property_Type::List, 0,
//                           "One Way Type", "Type of One Way collision for this object. <b></b>");


    addComponent(Components::Asset_Animation, "Animation", "Images to show for this Asset.", Component_Colors::Green_SeaGrass, true);
    getComponent(Components::Asset_Animation)->setIcon(Component_Icons::Animation);
    addPropertyToComponent(Components::Asset_Animation, Properties::Asset_Animation_Default, Property_Type::Image, QVariant(pixmap),
                            "Default Animation", "Image shown for this Asset.");
}


//####################################################################################
//##    Effect Asset Components
//####################################################################################
void DrAsset::initializeAssetSettingsEffect(QString new_name) {
    DrProperty *property_name = getComponentProperty(Components::Entity_Settings, Properties::Entity_Name);
    property_name->setDisplayName("Effect Name");
    property_name->setDescription("Name of the current Effect Asset.");
    property_name->setValue(new_name);
}


//####################################################################################
//##    Font Asset Components
//####################################################################################
void DrAsset::initializeAssetSettingsFont(DrFont *font) {
    DrProperty *property_name = getComponentProperty(Components::Entity_Settings, Properties::Entity_Name);
    property_name->setDisplayName("Font Name");
    property_name->setDescription("Name of the current Font Asset.");
    property_name->setValue(font->getName());

    addComponent(Components::Asset_Settings_Font, "Font Settings", "Font settings for this Text Asset.", Component_Colors::Orange_Medium, true);
    getComponent(Components::Asset_Settings_Font)->setIcon(Component_Icons::Font);
    addPropertyToComponent(Components::Asset_Settings_Font, Properties::Asset_Font_Family, Property_Type::String, font->getPropertyFontFamily(),
                           "Font Family", "Font used for this text asset.", false, false);
    addPropertyToComponent(Components::Asset_Settings_Font, Properties::Asset_Font_Size, Property_Type::Int, font->getPropertyFontSize(),
                           "Font Size", "Font size of this text asset.", false, false);
}


//####################################################################################
//##    Shared - Collision Components
//####################################################################################
void DrAsset::initializeAssetSettingsCollision(DrAssetType asset_type, DrShapeList &shape) {
    QString    type = "Thing";
    int start_shape = 0;
    if (asset_type == DrAssetType::Character) {
        type = "Character";
        start_shape = 1;
    } else if (asset_type == DrAssetType::Object) {
        type = "Object";
    }

    addComponent(Components::Asset_Collision, "Collision Settings", "Collision settings for current " + type + ".", Component_Colors::White_Snow, true);
    getComponent(Components::Asset_Collision)->setIcon(Component_Icons::Collide);
    addPropertyToComponent(Components::Asset_Collision, Properties::Asset_Collision_Shape, Property_Type::List, start_shape,
                           "Collision Shape", "Shape of the " + type + " as it interacts with other Assets in the world. Can be calculated "
                                              "automatically from the <b>Image Shape</b>. Characters are best as <b>Circle</b> and ground and platforms "
                                              "work nicely as <b>Square</b>.");
    addPropertyToComponent(Components::Asset_Collision, Properties::Asset_Collision_Image_Shape,
                           Property_Type::Collision, QVariant::fromValue<DrShapeList>(shape),
                           "Image Shape", "Stores auto generated Image Shape.", true, false);

    if (asset_type == DrAssetType::Object) {
        addPropertyToComponent(Components::Asset_Collision, Properties::Asset_Collision_One_Way_Type, Property_Type::List, 0,
                                "One Way Type", "Type of One Way collision for this Object. <br>"
                                                "<b>Pass_Through</b> - Objects / Characters can pass through in one direction. <br>"
                                                "<b>Weak_Spot</b> - Only takes damage from one direction.");
        addPropertyToComponent(Components::Asset_Collision, Properties::Asset_Collision_One_Way_Direction, Property_Type::Angle, 0,
                                "One Way Angle", "Direction that affects <b>One Way Type</b>. 0° is Up, 90° is Left, 180° is Down, 270° is Right.");
        addPropertyToComponent(Components::Asset_Collision, Properties::Asset_Collision_Gravity_Multiplier, Property_Type::Double, 1.0,
                                "Gravity Multiplier", "Use to cancel Gravity (0.0) on Things that collide (climbable ladders), or to reduce "
                                                      "Gravity (0.5 for wall sliding), or to change it completely (-1.0 for handlebars).");
    }
}


//####################################################################################
//##    Shared - Health Components
//####################################################################################
void DrAsset::initializeAssetSettingsHealth(DrAssetType asset_type, int hit_points) {
    QString type = "Thing";
    if (asset_type == DrAssetType::Character)   type = "Character";
    if (asset_type == DrAssetType::Object)      type = "Object";

    addComponent(Components::Asset_Health, "Health / Damage", "Health and Damage settings for this " + type + ".", Component_Colors::Red_Faded, true);
    getComponent(Components::Asset_Health)->setIcon(Component_Icons::Health);

    addPropertyToComponent(Components::Asset_Health, Properties::Asset_Health_Max_Health,       Property_Type::Double,  10.0,
                           "Max Health",    "Maximum full Health (a.k.a. hit points) of this " + type + ". Any value below zero represents no limit "
                                            "on how high health can become.");
    addPropertyToComponent(Components::Asset_Health, Properties::Asset_Health_Health,           Property_Type::Double, hit_points,
                           "Health",        "Health (a.k.a. hit points) of this " + type + ". A value below zero represents unlimited Health.");
    addPropertyToComponent(Components::Asset_Health, Properties::Asset_Health_Damage,           Property_Type::Double, 1.0,
                           "Damage",        "Damage dealt to other Things upon collision.");
    addPropertyToComponent(Components::Asset_Health, Properties::Asset_Health_Damage_Delay,     Property_Type::Int, 500,
                           "Damage Delay",  "Minimum time (in milliseconds) that must pass before this " + type + " can receive damage again after "
                                            "receiving damage the previous time.");
    addPropertyToComponent(Components::Asset_Health, Properties::Asset_Health_Auto_Damage,      Property_Type::Double, 0.0,
                           "Auto Damage",   "Take (x) damage per second (can be negative, i.e. adds Health). Useful for having an object dissapear "
                                            "from the World after a few seconds.");
    addPropertyToComponent(Components::Asset_Health, Properties::Asset_Health_Death_Delay,      Property_Type::Int, 100,
                           "Death Delay",   "Time it takes (in milliseconds) for this " + type + " to die after Health becomes zero. " + type + " no "
                                            "longer deals damage while dying, but does continue to provide physics collisions.");
    addPropertyToComponent(Components::Asset_Health, Properties::Asset_Health_Fade_On_Death,    Property_Type::Bool, true,
                           "Fade On Death?","If true, " + type + " is slowly faded over Death Delay time period.");
    addPropertyToComponent(Components::Asset_Health, Properties::Asset_Health_Fade_Delay,       Property_Type::Int, 750,
                           "Fade Delay",    "Time it takes (in milliseconds) for " + type + " to completely fade away after death (0 = remove immediately).");
    addPropertyToComponent(Components::Asset_Health, Properties::Asset_Health_Damage_Recoil,    Property_Type::Double, 200.0,
                           "Damage Recoil", "How much opposite force to bounce back when receiving damage from another Thing.");
    addPropertyToComponent(Components::Asset_Health, Properties::Asset_Health_Invincible,       Property_Type::Bool, false,
                           "Invincible?",   "When true, this object takes no damage nor Damage Recoil force and cannot be killed.");
    addPropertyToComponent(Components::Asset_Health, Properties::Asset_Health_Death_Touch,      Property_Type::Bool, false,
                           "Death Touch?",  "When true, kills everything on contact, even Things with unlimited Health...but not invincible Things.");

}















