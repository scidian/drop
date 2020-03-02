//
//      Created by Stephens Nunnally on 11/11/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "engine/enums_engine.h"
#include "project/constants_component_info.h"
#include "project/dr_project.h"
#include "project/entities/dr_asset.h"
#include "project/entities/dr_effect.h"
#include "project/entities/dr_font.h"
#include "project/entities/dr_image.h"
#include "project/entities/dr_world.h"
#include "project/entities/dr_stage.h"
#include "project/entities/dr_thing.h"
#include "project/properties/property_collision.h"
#include "project/settings/settings.h"
#include "project/settings/settings_component.h"
#include "project/settings/settings_component_property.h"


//####################################################################################
//##    Collision Components
//####################################################################################
void DrAsset::initializeAssetSettingsCollision(DrAssetType asset_type, DrPropertyCollision &shape) {
    std::string type = "Thing";
    Collision_Shape start_shape_type = Collision_Shape::Image;
    if (asset_type == DrAssetType::Character) {
        type = "Character";
        start_shape_type = Collision_Shape::Circle;
    } else if (asset_type == DrAssetType::Object) {
        type = "Object";
    }

    addComponent(Comps::Asset_Collision, "Collision", "Collision settings for this " + type + ".", Component_Colors::RGB_19_Silver, true);
    getComponent(Comps::Asset_Collision)->setIcon(Component_Icons::Collide);

    addPropertyToComponent(Comps::Asset_Collision, Props::Asset_Collision_Shape, Property_Type::List, static_cast<int>(start_shape_type),
                           "Collision Shape", "Shape of the " + type + " as it interacts with other Assets in the world. Can be calculated "
                                              "automatically from the <b>Image Shape</b>. Characters are best as <b>Circle</b> and ground and platforms "
                                              "work nicely as <b>Square</b>.");
    // !!!!! Hidden Property
    addPropertyToComponent(Comps::Asset_Collision, Props::Asset_Collision_Image_Shape, Property_Type::Collision, shape,
                           "Image Shape", "Stores auto generated Image Shape.", true, false);

    if (asset_type == DrAssetType::Object) {
        addPropertyToComponent(Comps::Asset_Collision, Props::Asset_Collision_One_Way_Type, Property_Type::List, static_cast<int>(One_Way::None),
                               "One Way Type", "Type of One Way collision for this Object. <br>"
                                               "<b>Pass_Through</b> - Objects / Characters can pass through in one direction. <br>"
                                               "<b>Weak_Spot</b> - Only takes damage from one direction.");
        addPropertyToComponent(Comps::Asset_Collision, Props::Asset_Collision_One_Way_Direction, Property_Type::Angle, 0,
                               "One Way Angle", "Direction that affects <b>One Way Type</b>. 0° is Up, 90° is Left, 180° is Down, 270° is Right.");
        addPropertyToComponent(Comps::Asset_Collision, Props::Asset_Collision_Surface_Velocity, Property_Type::PointF, DrPointF(0.0, 0.0),
                               "Surface Velocity", "Speed given to other Things when touching. Useful for objects like conveyor belts. Works better with "
                                                   "higher friction.");
        addPropertyToComponent(Comps::Asset_Collision, Props::Asset_Collision_Gravity_Multiplier, Property_Type::Double, 1.0,
                               "Gravity Multiplier", "Use to cancel Gravity (0.0) on Things that collide (climbable ladders), or to reduce "
                                                     "Gravity (0.75 for wall sliding), or to flip it completely (-1.0 for monkey bars).");
    }
}

//####################################################################################
//##    Animation Components
//####################################################################################
void DrAsset::initializeAssetSettingsAnimation(DrAssetType asset_type, long idle_animation_key) {
    std::string type = Dr::StringFromAssetType(asset_type);

    addComponent(Comps::Asset_Animation, "Animation", "Images to show for this " + type + ".", Component_Colors::RGB_12_Olive, true);
    getComponent(Comps::Asset_Animation)->setIcon(Component_Icons::Animation);

    addPropertyToComponent(Comps::Asset_Animation, Props::Asset_Animation_Idle, Property_Type::Image, idle_animation_key,
                           "Idle Animation", "Idle Animation for this " + type + ".");

    if (asset_type == DrAssetType::Character) {
        addPropertyToComponent(Comps::Asset_Animation, Props::Asset_Animation_Jumping, Property_Type::Image, c_no_key,
                               "Jumping Animation", "Jumping Animation for this " + type + ".");
    }
}


//####################################################################################
//##    Physics Components
//####################################################################################
void DrAsset::initializeAssetSettingsPhysics(DrAssetType asset_type) {
    std::string type = Dr::StringFromAssetType(asset_type);

    addComponent(Comps::Asset_Physics, "Physics", "Physics settings for this " + type + ".", Component_Colors::RGB_15_Sunset, true);
    getComponent(Comps::Asset_Physics)->setIcon(Component_Icons::Physics);

    addPropertyToComponent(Comps::Asset_Physics, Props::Asset_Physics_Body_Style, Property_Type::List, static_cast<int>(Body_Style::Rigid_Body),
                           "Body Style", "Body style for Object when used as a Dynamic object. Using Blob or 2D Cloth physics will override collision "
                                         "shape of Object. Blob and 2D Cloth physics bodies are currently only able to be rendered in 2D.");
    addPropertyToComponent(Comps::Asset_Physics, Props::Asset_Physics_Body_Rigidness, Property_Type::Percent, 100.0,
                           "Body Rigidness", "When using soft bodies, how rigid will this body be?");
    addPropertyToComponent(Comps::Asset_Physics, Props::Asset_Physics_Gravity_Scale, Property_Type::PointF, DrPointF(1.0, 1.0),
                           "Gravity Scale", "Changes how gravity affects this " + type + ". Set to (0, 0) to "
                                            "ignore gravity comepletely. Also great for making balloons.");
    // BoolDouble std::vector<DrVariant> of 6 values: bool, double value, min, max, double step size, string spinText
    addPropertyToComponent(Comps::Asset_Physics, Props::Asset_Physics_Custom_Friction,
                           Property_Type::BoolDouble, std::vector<DrVariant>({false, 1.0, 0.0, 10000, 0.1, " "}),
                           "Custom Friction?", "All Things default to World Friction unless specified otherwise here. Friction usually ranges from "
                                                "0.0 to 2.0 or higher.");
    addPropertyToComponent(Comps::Asset_Physics, Props::Asset_Physics_Custom_Bounce,
                           Property_Type::BoolDouble, std::vector<DrVariant>({false, 1.0, 0.0, 10000, 0.1, " "}),
                           "Custom Bounce?", "All Things default to World Bounce unless specified otherwise here. Bounce usually ranges from "
                                              "0.0 to 2.0 or higher.");
    bool default_can_rotate = (asset_type == DrAssetType::Character) ? false : true;
    addPropertyToComponent(Comps::Asset_Physics, Props::Asset_Physics_Can_Rotate, Property_Type::Bool, default_can_rotate,
                           "Can Rotate?", "Can this character rotate (on z axis)? If not, rotation will be fixed. Rotation can still be altered by Motor Buttons.");
}



//####################################################################################
//##    Health Components
//####################################################################################
void DrAsset::initializeAssetSettingsHealth(DrAssetType asset_type, int hit_points) {
    std::string type = "Thing";
    if (asset_type == DrAssetType::Character)   type = "Character";
    if (asset_type == DrAssetType::Object)      type = "Object";

    addComponent(Comps::Asset_Health, "Health / Damage", "Health and Damage settings for this " + type + ".", Component_Colors::RGB_02_Pink, true);
    getComponent(Comps::Asset_Health)->setIcon(Component_Icons::Health);

    addPropertyToComponent(Comps::Asset_Health, Props::Asset_Health_Max_Health,       Property_Type::Double,  10.0,
                           "Max Health",    "Maximum full Health (a.k.a. hit points) of this " + type + ". Any value below zero represents no limit "
                                            "on how high health can become.");
    addPropertyToComponent(Comps::Asset_Health, Props::Asset_Health_Health,           Property_Type::Double, hit_points,
                           "Health",        "Health (a.k.a. hit points) of this " + type + ". A value below zero represents unlimited Health.");
    addPropertyToComponent(Comps::Asset_Health, Props::Asset_Health_Damage,           Property_Type::Double, 1.0,
                           "Damage",        "Damage dealt to other Things upon collision.");
    addPropertyToComponent(Comps::Asset_Health, Props::Asset_Health_Damage_Delay,     Property_Type::Int, 500,
                           "Damage Delay",  "Minimum time (in milliseconds) that must pass before this " + type + " can receive damage again after "
                                            "receiving damage the previous time.");
    addPropertyToComponent(Comps::Asset_Health, Props::Asset_Health_Auto_Damage,      Property_Type::Double, 0.0,
                           "Auto Damage",   "Take (x) damage per second (can be negative, i.e. adds Health). Useful for having an object dissapear "
                                            "from the World after a few seconds.");
    addPropertyToComponent(Comps::Asset_Health, Props::Asset_Health_Death_Delay,      Property_Type::Int, 100,
                           "Death Delay",   "Time it takes (in milliseconds) for this " + type + " to die after Health becomes zero. " + type + " no "
                                            "longer deals damage while dying, but does continue to provide physics collisions.");
    addPropertyToComponent(Comps::Asset_Health, Props::Asset_Health_Death_Animation,  Property_Type::List, static_cast<int>(Death_Animation::Fade),
                           "Death Animation", "Animation for " + type + " as it dissappears from the game.");
    addPropertyToComponent(Comps::Asset_Health, Props::Asset_Health_Death_Duration,   Property_Type::Int, 750,
                           "Death Duration", "Duration (in milliseconds) of Death Animation, like to fade away, or shrink away, etc. "
                                             "(0 = remove immediately).");
    addPropertyToComponent(Comps::Asset_Health, Props::Asset_Health_Damage_Recoil,    Property_Type::Double, 200.0,
                           "Damage Recoil", "How much opposite force to bounce back when receiving damage from another Thing.");
    addPropertyToComponent(Comps::Asset_Health, Props::Asset_Health_Invincible,       Property_Type::Bool, false,
                           "Invincible?",   "When true, this object takes no damage nor Damage Recoil force and cannot be killed.");
    addPropertyToComponent(Comps::Asset_Health, Props::Asset_Health_Death_Touch,      Property_Type::Bool, false,
                           "Death Touch?",  "When true, kills everything on contact, even Things with unlimited Health...but not invincible Things.");

}


//####################################################################################
//##    Controls Components
//####################################################################################
void DrAsset::initializeAssetSettingsControls(DrAssetType asset_type) {
    std::string type = Dr::StringFromAssetType(asset_type);

    addComponent(Comps::Asset_Controls, "Controls", "Control settings for this " + type + ".", Component_Colors::RGB_04_Purple, true);
    getComponent(Comps::Asset_Controls)->setIcon(Component_Icons::Controls);

    addPropertyToComponent(Comps::Asset_Controls, Props::Asset_Controls_Rotate_Speed, Property_Type::Double, 0.0,
                           "Motor Speed", "Speed at which this " + type + " rotates when Motor Buttons are pressed.");

    // BoolDouble std::vector<DrVariant> of 6 values: bool, double value, min, max, double step size, string spinText
    addPropertyToComponent(Comps::Asset_Controls, Props::Asset_Controls_Touch_Damage,
                           Property_Type::BoolDouble, std::vector<DrVariant>({false, 1.0, -1000000000, 1000000000, 1, "Damage: "}),
                           "Touch Damage", "Should this " + type + " take damage when touched / tapped / clicked with the mouse? "
                                           "If so, how much damage? Can be negative (i.e. healing).");
    addPropertyToComponent(Comps::Asset_Controls, Props::Asset_Controls_Touch_Drag,
                           Property_Type::BoolDouble, std::vector<DrVariant>({false, 1000.0, -1000000000, 1000000000, 10, "Force: "}),
                           "Touch Drag", "Should this " + type + " be able to be dragged by mouse / touch? If so, how much force "
                                         "to apply? <br><br> <b>NOTE:</b> Object Type must be <b>Kinematic</b> or <b>Dynamic</b> to use this setting!");

    std::vector<DrVariant> sling_properties  {  Props::Asset_Slingshot_Arrow_Image,
                                                Props::Asset_Slingshot_Arrow_Offset,
                                                Props::Asset_Slingshot_Arrow_Scale,
                                                Props::Asset_Slingshot_Tail_Image,
                                                Props::Asset_Slingshot_Tail_Offset,
                                                Props::Asset_Slingshot_Tail_Scale,
                                                Props::Asset_Slingshot_Max_Distance,
                                                Props::Asset_Slingshot_Force_Multiplier };
    addPropertyToComponent(Comps::Asset_Controls, Props::Asset_Slingshot_Enabled, Property_Type::BoolEnabled,
                           std::vector<DrVariant>({false, sling_properties }),
                           "Touch Slingshot", "Should this " + type + " be able to be shot by slingshot control? "
                                              "<br><br> <b>NOTE:</b> Object Type must be <b>Dynamic</b> to use this setting!");
    addPropertyToComponent(Comps::Asset_Controls, Props::Asset_Slingshot_Force_Multiplier, Property_Type::Double, 1.0,
                           "Force Multiplier", "Force multiplier can be used to increase or decrease power of slingshot.");
    addPropertyToComponent(Comps::Asset_Controls, Props::Asset_Slingshot_Max_Distance, Property_Type::PositiveDouble, 500.0,
                           "Pull Back Distance", "Maximum distance slingshot can be pulled away from item.");
    addPropertyToComponent(Comps::Asset_Controls, Props::Asset_Slingshot_Arrow_Image, Property_Type::Image, c_no_key,
                           "Slingshot Arrow", "Image used to show direction of slingshot.");
    addPropertyToComponent(Comps::Asset_Controls, Props::Asset_Slingshot_Arrow_Offset, Property_Type::PositiveDouble, 100.0,
                           "Arrow Offset", "Distance to render Slingshot Arrow away from this " + type );
    addPropertyToComponent(Comps::Asset_Controls, Props::Asset_Slingshot_Arrow_Scale, Property_Type::PositiveScaleF, DrPointF(1.0, 1.0),
                           "Arrow Scale", "Scale used to render Slingshot Arrow.");
    addPropertyToComponent(Comps::Asset_Controls, Props::Asset_Slingshot_Tail_Image, Property_Type::Image, c_no_key,
                           "Slingshot Tail", "Image used to show pull back of slingshot.");
    addPropertyToComponent(Comps::Asset_Controls, Props::Asset_Slingshot_Tail_Offset, Property_Type::PositiveDouble, 100.0,
                           "Tail Offset", "Distance to start rendering Slingshot Tail away from this " + type );
    addPropertyToComponent(Comps::Asset_Controls, Props::Asset_Slingshot_Tail_Scale, Property_Type::PositiveScaleF, DrPointF(1.0, 1.0),
                           "Tail Scale", "Scale used to render Slingshot Tail.");

}









