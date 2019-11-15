//
//      Created by Stephens Nunnally on 10/20/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
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
//##    Camera Components
//####################################################################################
void DrThing::addComponentSettingsCamera(QString new_name) {
    DrProperty *property_name = getComponentProperty(Components::Entity_Settings, Properties::Entity_Name);
    property_name->setDisplayName("Camera Name");
    property_name->setDescription("Name of this Camera.");
    property_name->setValue(new_name);

    addComponent(Components::Thing_Settings_Camera, "Camera Settings", "Settings for this Camera.", Component_Colors::Beige_Apricot, true);
    getComponent(Components::Thing_Settings_Camera)->setIcon(Component_Icons::Camera);

//    Thing_Camera_Set_As_Active          = 178,      // bool
//    Thing_Camera_Speed                  = 179,      // pointf
//    Thing_Camera_Rotation               = 180,      // pointf

    addPropertyToComponent(Components::Thing_Settings_Camera, Properties::Thing_Camera_Set_As_Active, Property_Type::Bool, true,
                           "Set As Active?", "If set to true and Camera is in a Start Stage, this camera will attempt to be the Active Game Camera when this "
                                             "world starts. Only one Camera should be set as Active in each Start Stage.");
    addPropertyToComponent(Components::Thing_Settings_Camera, Properties::Thing_Camera_Speed, Property_Type::PointF, QPointF(0, 0),
                           "Camera Speed", "Fixed movement speed of camera.");
    addPropertyToComponent(Components::Thing_Settings_Camera, Properties::Thing_Camera_Rotation, Property_Type::PointF, QPointF(0, 0),
                           "Camera Rotation", "This setting is the camera rotation of that Camera. The "
                                              "<b>X</b> value changes <b>Up / Down</b> rotation, <b>Y</b> changes <b>Left / Right</b> rotation.");
    addPropertyToComponent(Components::Thing_Settings_Camera, Properties::Thing_Camera_Up_Vector, Property_Type::List, 0,
                           "Camera Up Vector", "Camera Up Vectors for this World. Default for most games / styles of gameplay is for a Y Axis Up "
                                        "Vector. If making a First Person style game, rotate camera X or Y to 90 degrees and set Up Vector to Z Axis.");
    addPropertyToComponent(Components::Thing_Settings_Camera, Properties::Thing_Camera_Zoom, Property_Type::Double, 1.0,
                           "Camera Zoom", "Magnification zoom level of this camera. A value of 1.0 is no zoom, 0.5 is twice as far away, 2.0 is twice as close. ");
}

//####################################################################################
//##    Character Components
//####################################################################################
void DrThing::addComponentSettingsCharacter(QString new_name) {
    DrProperty *property_name = getComponentProperty(Components::Entity_Settings, Properties::Entity_Name);
    property_name->setDisplayName("Character Name");
    property_name->setDescription("Name of this Character.");
    property_name->setValue(new_name);

    addComponent(Components::Thing_Settings_Character, "Character Settings", "Settings for this Character.", Component_Colors::White_Snow, true);
    getComponent(Components::Thing_Settings_Character)->setIcon(Component_Icons::Character);

    addPropertyToComponent(Components::Thing_Settings_Character, Properties::Thing_Character_Camera_Lag, Property_Type::SizeF, QPointF(50, 50),
                           "Camera Lag", "Every Character has a Camera attached which always looks directly at the Character. "
                                         "This setting allows the character to move around a box of this many World units (i.e. pixels) before the "
                                         "camera starts to move.");
    addPropertyToComponent(Components::Thing_Settings_Character, Properties::Thing_Character_Camera_Position, Property_Type::PointF, QPointF(0, 100),
                           "Camera Offset", "Every Character has a Camera attached which always looks directly at the Character. "
                                            "The Camera Offset allows for the camera to look at a point offset from the Character in 2D space.");
    addPropertyToComponent(Components::Thing_Settings_Character, Properties::Thing_Character_Camera_Rotation, Property_Type::PointF, QPointF(0, 0),
                           "Camera Rotation", "Every Character has a Camera attached which always looks directly at the Character. "
                                              "This setting is the camera rotation of that Camera. The "
                                              "<b>X</b> value changes <b>Up / Down</b> rotation, <b>Y</b> changes <b>Left / Right</b> rotation.");
    addPropertyToComponent(Components::Thing_Settings_Character, Properties::Thing_Character_Camera_Up_Vector, Property_Type::List, 0,
                           "Camera Up Vector", "Camera Up Vectors for this World. Default for most games / styles of gameplay is for a Y Axis Up "
                                        "Vector. If making a First Person style game, rotate camera X or Y to 90 degrees and set Up Vector to Z Axis.");
    addPropertyToComponent(Components::Thing_Settings_Character, Properties::Thing_Character_Camera_Zoom, Property_Type::Double, 1.0,
                           "Camera Zoom", "Every Character has a Camera attached which always looks directly at the Character. "
                                          "This is the magnification zoom level of that camera. ");
    addPropertyToComponent(Components::Thing_Settings_Character, Properties::Thing_Character_Camera_Tilt, Property_Type::Double, 0.0,
                           "Camera Tilt", "Every Character has a Camera attached which always looks directly at the Character. "
                                          "This is the tilt angle (Z Axis rotation) of that camera. ");


}

//####################################################################################
//##    Object Components
//####################################################################################
void DrThing::addComponentSettingsObject(QString new_name, bool should_collide) {
    DrProperty *property_name = getComponentProperty(Components::Entity_Settings, Properties::Entity_Name);
    property_name->setDisplayName("Object Name");
    property_name->setDescription("Name of this Object.");
    property_name->setValue(new_name);

    addComponent(Components::Thing_Settings_Object, "Object Settings", "Settings for this Object.", Component_Colors::White_Snow, true);
    getComponent(Components::Thing_Settings_Object)->setIcon(Component_Icons::Object);

    addPropertyToComponent(Components::Thing_Settings_Object, Properties::Thing_Object_Physics_Type, Property_Type::List, 0,
                           "Object Type", "<b>Static</b> - Can not move. <br> "
                                          "<b>Kinematic</b> - Moves at fixed speed. <br> "
                                          "<b>Dynamic</b> - Physics object.");
    addPropertyToComponent(Components::Thing_Settings_Object, Properties::Thing_Object_Collide, Property_Type::Bool, should_collide,
                           "Physics Collide?", "Should this Object collide with Dynamic Objects? Objects not marked to collide "
                                               "still provide damage and sound reponses when coming into contact with other Objects.");
    addPropertyToComponent(Components::Thing_Settings_Object, Properties::Thing_Object_Collision_Group, Property_Type::List, 1,
                           "Collide With", "What types of other items should this Object collide with. Objects types not selected will not provide "
                                           "physics or damage collision responses.");
    addPropertyToComponent(Components::Thing_Settings_Object, Properties::Thing_Object_Damage, Property_Type::List, 0,
                           "Damage Type", "Choose the type of Object this will damage when coming into contact. By choosing \"Damage Player\" this "
                                          "Object will be treated as an enemy and vice versa.");
}

//####################################################################################
//##    Text Components
//####################################################################################
void DrThing::addComponentSettingsText(QString new_name) {
    DrProperty *property_name = getComponentProperty(Components::Entity_Settings, Properties::Entity_Name);
    property_name->setDisplayName("Text Name");
    property_name->setDescription("Name of this Text Box.");
    property_name->setValue(new_name);

    addComponent(Components::Thing_Settings_Text, "Text Settings", "Settings for this Text Box.", Component_Colors::Orange_Medium, true);
    getComponent(Components::Thing_Settings_Text)->setIcon(Component_Icons::Font);

    addPropertyToComponent(Components::Thing_Settings_Text, Properties::Thing_Text_User_Text, Property_Type::String, "Text",
                           "User Text", "Custom text value to be shown in this Text Box.");
}

















