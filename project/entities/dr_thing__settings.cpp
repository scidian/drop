//
//      Created by Stephens Nunnally on 10/20/2019, (c) 2019 Scidian Studios, All Rights Reserved
//
//  File:
//
//
//
#include "engine/constants_engine.h"
#include "engine/enums_engine.h"
#include "project/constants_component_info.h"
#include "project/dr_project.h"
#include "project/entities/dr_stage.h"
#include "project/entities/dr_thing.h"
#include "project/entities/dr_world.h"
#include "project/entities_physics_2d/dr_asset.h"
#include "project/settings/settings.h"
#include "project/settings/settings_component.h"
#include "project/settings/settings_component_property.h"


//####################################################################################
//##    Character Components
//####################################################################################
void DrThing::addComponentSettingsCharacter(std::string new_name) {
    DrProperty *property_name = getComponentProperty(Comps::Entity_Settings, Props::Entity_Name);
    property_name->setDisplayName("Character Name");
    property_name->setDescription("Name of this Character.");
    property_name->setValue(new_name);

    addComponent(Comps::Thing_Settings_Character, "Character Settings", "Settings for this Character.", Component_Colors::RGB_20_Tan, false);
    getComponent(Comps::Thing_Settings_Character)->setIcon(Component_Icons::Character);

    addPropertyToComponent(Comps::Thing_Settings_Character, Props::Thing_Character_Camera_Position, Property_Type::PointF, DrPointF(0, 0),
                           "Camera Offset", "Every Character has a Camera attached which always looks directly at the Character. "
                                            "The Camera Offset allows for the camera to look at a point offset from the Character in 2D space.");
    addPropertyToComponent(Comps::Thing_Settings_Character, Props::Thing_Character_Camera_Rotation, Property_Type::PointF, DrPointF(0, 0),
                           "Camera Rotation", "Every Character has a Camera attached which always looks directly at the Character. "
                                              "This setting is the camera rotation of that Camera. The "
                                              "<b>X</b> value changes <b>Up / Down</b> rotation, <b>Y</b> changes <b>Left / Right</b> rotation.");
    addPropertyToComponent(Comps::Thing_Settings_Character, Props::Thing_Character_Camera_Up_Vector, Property_Type::List, static_cast<int>(Up_Vector::Y),
                           "Camera Up Vector", "Default for most games / styles of gameplay is for a Y Axis Up Vector. "
                                               "If making a First Person style game, rotate camera X or Y to between 75 and 90 degrees and "
                                               "set Up Vector to Z Axis.");
    // !!!!! Hidden Property
    addPropertyToComponent(Comps::Thing_Settings_Character, Props::Thing_Character_Camera_Distance, Property_Type::Double, c_default_camera_z,
                           "Camera Distance", "Every Character has a Camera attached which always looks directly at the Character. "
                                              "This is the distance away that camera is from the Character. Defaults to 801.", true, false);
    addPropertyToComponent(Comps::Thing_Settings_Character, Props::Thing_Character_Camera_Zoom, Property_Type::PositiveDouble, 1.0,
                           "Camera Zoom", "Every Character has a Camera attached which always looks directly at the Character. "
                                          "This is the magnification zoom level of that camera. Generally less than 10.");
    addPropertyToComponent(Comps::Thing_Settings_Character, Props::Thing_Character_Camera_Lag, Property_Type::PositiveSizeF, DrPointF(200, 200),
                           "Camera Lag", "Every Character has a Camera attached which always looks directly at the Character. "
                                         "This setting allows the character to move around a box of this many World units (i.e. pixels) before the "
                                         "camera starts to move.");
    addPropertyToComponent(Comps::Thing_Settings_Character, Props::Thing_Character_Camera_Tilt, Property_Type::Double, 0.0,
                           "Camera Tilt", "Every Character has a Camera attached which always looks directly at the Character. "
                                          "This is the tilt angle (Z Axis rotation) of that camera. ");
    addPropertyToComponent(Comps::Thing_Settings_Character, Props::Thing_Character_Camera_Match_Angle, Property_Type::Bool, false,
                           "Rotation Follow", "Should this camera rotate to so that it matches the Character's Z Rotation? "
                                              "When Up Vector is set to Y the whole world will spin. When it is set to Z, it allows you to have "
                                              "first person directional looking. ");
    // !!!!! Hidden Property
    addPropertyToComponent(Comps::Thing_Settings_Character, Props::Thing_Character_Camera_Sides, Property_Type::Array, std::vector<DrVariant>({0, 0, 0, 0}),
                           "Frame Edges", "This is a list of the 4 edges of the camera frame, in order: top, right, bottom, left. "
                                          "0 is Normal, 1 is Blocked, 2 is Death", true, false);
}

//####################################################################################
//##    Object Components
//####################################################################################
void DrThing::addComponentSettingsObject(std::string new_name, bool should_collide) {
    DrProperty *property_name = getComponentProperty(Comps::Entity_Settings, Props::Entity_Name);
    property_name->setDisplayName("Object Name");
    property_name->setDescription("Name of this Object.");
    property_name->setValue(new_name);

    addComponent(Comps::Thing_Settings_Object, "Object Settings", "Settings for this Object.", Component_Colors::RGB_18_Gray, false);
    getComponent(Comps::Thing_Settings_Object)->setIcon(Component_Icons::Object);

    addPropertyToComponent(Comps::Thing_Settings_Object, Props::Thing_Object_Physics_Type, Property_Type::List, static_cast<int>(Body_Type::Static),
                           "Object Type", "<b>Static</b> - Can not move. <br> "
                                          "<b>Kinematic</b> - Moves at fixed speed. <br> "
                                          "<b>Dynamic</b> - Physics object.");
    addPropertyToComponent(Comps::Thing_Settings_Object, Props::Thing_Object_Collide, Property_Type::Bool, should_collide,
                           "Physics Collide?", "Should this Object collide with Dynamic Objects? Objects not marked to collide "
                                               "still provide damage and sound reponses when coming into contact with other Objects.");
    addPropertyToComponent(Comps::Thing_Settings_Object, Props::Thing_Object_Collision_Group, Property_Type::List, static_cast<int>(Collision_Groups::All),
                           "Collide With", "What types of other Things should this Object collide with. Objects types not selected will not provide "
                                           "physics or damage collision responses.");
    addPropertyToComponent(Comps::Thing_Settings_Object, Props::Thing_Object_Damage, Property_Type::List, static_cast<int>(Collision_Type::Damage_None),
                           "Damage Type", "Choose the type of Object this will damage when coming into contact. By choosing \"Damage Player\" this "
                                          "Object will be treated as an enemy and vice versa.");
}

//####################################################################################
//##    Text Components
//####################################################################################
void DrThing::addComponentSettingsText(std::string new_name) {
    DrProperty *property_name = getComponentProperty(Comps::Entity_Settings, Props::Entity_Name);
    property_name->setDisplayName("Text Name");
    property_name->setDescription("Name of this Text Box.");
    property_name->setValue(new_name);

    addComponent(Comps::Thing_Settings_Text, "Text Settings", "Settings for this Text Box.", Component_Colors::RGB_17_Brown, false);
    getComponent(Comps::Thing_Settings_Text)->setIcon(Component_Icons::Font);

    addPropertyToComponent(Comps::Thing_Settings_Text, Props::Thing_Text_User_Text, Property_Type::String, "Text",
                           "User Text", "Custom text value to be shown in this Text Box.");
}

















