//
//      Created by Stephens Nunnally on 11/11/2019, (c) 2019 Scidian Software, All Rights Reserved
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
//##    Collision Components
//####################################################################################
void DrAsset::initializeAssetSettingsCollision(DrAssetType asset_type, DrShapeList &shape) {
    QString    type = "Thing";
    int start_shape_type = 0;
    if (asset_type == DrAssetType::Character) {
        type = "Character";
        start_shape_type = 1;
    } else if (asset_type == DrAssetType::Object) {
        type = "Object";
    }

    addComponent(Components::Asset_Collision, "Collision", "Collision settings for this " + type + ".", Component_Colors::Green_SeaGrass, true);
    getComponent(Components::Asset_Collision)->setIcon(Component_Icons::Collide);

    addPropertyToComponent(Components::Asset_Collision, Properties::Asset_Collision_Shape, Property_Type::List, start_shape_type,
                           "Collision Shape", "Shape of the " + type + " as it interacts with other Assets in the world. Can be calculated "
                                              "automatically from the <b>Image Shape</b>. Characters are best as <b>Circle</b> and ground and platforms "
                                              "work nicely as <b>Square</b>.");
    // !! Hidden Property
    addPropertyToComponent(Components::Asset_Collision, Properties::Asset_Collision_Image_Shape,
                           Property_Type::Collision, QVariant::fromValue<DrShapeList>(shape),
                           "Image Shape", "Stores auto generated Image Shape.", true, false);

    if (asset_type == DrAssetType::Object) {
        addPropertyToComponent(Components::Asset_Collision, Properties::Asset_Collision_Gravity_Multiplier, Property_Type::Double, 1.0,
                               "Gravity Multiplier", "Use to cancel Gravity (0.0) on Things that collide (climbable ladders), or to reduce "
                                                      "Gravity (0.75 for wall sliding), or to flip it completely (-1.0 for monkey bars).");
        addPropertyToComponent(Components::Asset_Collision, Properties::Asset_Collision_Surface_Velocity, Property_Type::PointF, 0.0,
                               "Surface Velocity", "Speed given to other Things when touching. Useful for objects like conveyor belts. Works better with "
                                                    "higher friction.");
        addPropertyToComponent(Components::Asset_Collision, Properties::Asset_Collision_One_Way_Type, Property_Type::List, 0,
                               "One Way Type", "Type of One Way collision for this Object. <br>"
                                                "<b>Pass_Through</b> - Objects / Characters can pass through in one direction. <br>"
                                                "<b>Weak_Spot</b> - Only takes damage from one direction.");
        addPropertyToComponent(Components::Asset_Collision, Properties::Asset_Collision_One_Way_Direction, Property_Type::Angle, 0,
                               "One Way Angle", "Direction that affects <b>One Way Type</b>. 0° is Up, 90° is Left, 180° is Down, 270° is Right.");
    }
}

//####################################################################################
//##    Animation Components
//####################################################################################
void DrAsset::initializeAssetSettingsAnimation(DrAssetType asset_type, QPixmap default_animation) {
    QString type = Dr::StringFromAssetType(asset_type);

    addComponent(Components::Asset_Animation, "Animation", "Images to show for this " + type + ".", Component_Colors::Blue_Yonder, true);
    getComponent(Components::Asset_Animation)->setIcon(Component_Icons::Animation);

    addPropertyToComponent(Components::Asset_Animation, Properties::Asset_Animation_Default, Property_Type::Image, QVariant(default_animation),
                           "Default Animation", "Image shown for this " + type + ".");
}


//####################################################################################
//##    Health Components
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
    addPropertyToComponent(Components::Asset_Health, Properties::Asset_Health_Death_Animation,  Property_Type::List, 1,
                           "Death Animation", "Animation for " + type + " as it dissappears from the game.");
    addPropertyToComponent(Components::Asset_Health, Properties::Asset_Health_Death_Duration,   Property_Type::Int, 750,
                           "Death Duration", "Duration (in milliseconds) of Death Animation, like to fade away, or shrink away, etc. "
                                             "(0 = remove immediately).");
    addPropertyToComponent(Components::Asset_Health, Properties::Asset_Health_Damage_Recoil,    Property_Type::Double, 200.0,
                           "Damage Recoil", "How much opposite force to bounce back when receiving damage from another Thing.");
    addPropertyToComponent(Components::Asset_Health, Properties::Asset_Health_Invincible,       Property_Type::Bool, false,
                           "Invincible?",   "When true, this object takes no damage nor Damage Recoil force and cannot be killed.");
    addPropertyToComponent(Components::Asset_Health, Properties::Asset_Health_Death_Touch,      Property_Type::Bool, false,
                           "Death Touch?",  "When true, kills everything on contact, even Things with unlimited Health...but not invincible Things.");

}


//####################################################################################
//##    Physics Components
//####################################################################################
void DrAsset::initializeAssetSettingsPhysics(DrAssetType asset_type) {
    QString type = Dr::StringFromAssetType(asset_type);

    addComponent(Components::Asset_Physics, "Physics", "Physics settings for this " + type + ".", Component_Colors::Orange_Pastel, true);
    getComponent(Components::Asset_Physics)->setIcon(Component_Icons::Physics);

    addPropertyToComponent(Components::Asset_Physics, Properties::Asset_Physics_Feels_Gravity, Property_Type::Bool, true,
                           "Feels Gravity?", "Should this character be affected by gravity?");
    // BoolDouble QList<QVariant> of 6 values: bool, double value, min, max, double step size, string spinText
    addPropertyToComponent(Components::Asset_Physics, Properties::Asset_Physics_Custom_Friction,
                           Property_Type::BoolDouble, QList<QVariant>({false, 1.0, 0.0, 10000, 0.1, " "}),
                           "Custom Friction?", "All Things default to World Friction unless specified otherwise here. Friction usually ranges from "
                                                "0.0 to 2.0 or higher.");
    addPropertyToComponent(Components::Asset_Physics, Properties::Asset_Physics_Custom_Bounce,
                           Property_Type::BoolDouble, QList<QVariant>({false, 1.0, 0.0, 10000, 0.1, " "}),
                           "Custom Bounce?", "All Things default to World Bounce unless specified otherwise here. Bounce usually ranges from "
                                              "0.0 to 2.0 or higher.");
    bool default_can_rotate = (asset_type == DrAssetType::Character) ? false : true;
    addPropertyToComponent(Components::Asset_Physics, Properties::Asset_Physics_Can_Rotate, Property_Type::Bool, default_can_rotate,
                           "Can Rotate?", "Can this character rotate (on z axis)? If not, rotation will be fixed. Rotation can still be altered by Motor Buttons.");
}


//####################################################################################
//##    Controls Components
//####################################################################################
void DrAsset::initializeAssetSettingsControls(DrAssetType asset_type) {
    QString type = Dr::StringFromAssetType(asset_type);

    addComponent(Components::Asset_Controls, "Controls", "Control settings for this " + type + ".", Component_Colors::Brown_Sugar, true);
    getComponent(Components::Asset_Controls)->setIcon(Component_Icons::Controls);

    // BoolDouble QList<QVariant> of 6 values: bool, double value, min, max, double step size, string spinText
    addPropertyToComponent(Components::Asset_Controls, Properties::Asset_Controls_Touch_Damage,
                           Property_Type::BoolDouble, QList<QVariant>({false, 1.0, -1000000000, 1000000000, 1, "Damage: "}),
                           "Touch Damage", "Should this " + Dr::StringFromAssetType(asset_type) + " take damage when touched / tapped / clicked with the mouse? "
                                           "If so, how much damage? Can be negative (i.e. healing).");
    addPropertyToComponent(Components::Asset_Controls, Properties::Asset_Controls_Touch_Drag,
                           Property_Type::BoolDouble, QList<QVariant>({false, 100.0, -1000000000, 1000000000, 10, "Force: "}),
                           "Touch Drag", "Should this " + Dr::StringFromAssetType(asset_type) + " be able to be dragged by mouse / touch? If so, how much force "
                                         "to apply? <br><br> <b>NOTE:</b> Object Type must be <b>Kinematic</b> or <b>Dynamic</b> to use this setting!");
    addPropertyToComponent(Components::Asset_Controls, Properties::Asset_Controls_Rotate_Speed, Property_Type::Double, 0.0,
                           "Motor Speed", "Speed at which this " + Dr::StringFromAssetType(asset_type) + " rotates when Motor Buttons are pressed.");

}









