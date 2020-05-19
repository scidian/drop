//
//      Created by Stephens Nunnally on 3/3/2020, (c) 2020 Scidian Software, All Rights Reserved
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
//##    Camera Components
//####################################################################################
void DrThing::addComponentSettingsCamera(std::string new_name) {
    DrProperty *property_name = getComponentProperty(Comps::Entity_Settings, Props::Entity_Name);
    property_name->setDisplayName("Device Name");
    property_name->setDescription("Name of this Device.");
    property_name->setValue(new_name);

    addComponent(Comps::Thing_Settings_Camera, "Camera Settings", "Settings for this Camera.", Component_Colors::RGB_08_SeaFoam, false);
    getComponent(Comps::Thing_Settings_Camera)->setIcon(Component_Icons::Camera);

    addPropertyToComponent(Comps::Thing_Settings_Camera, Props::Thing_Camera_Set_As_Active, Property_Type::Bool, true,
                           "Set As Active?", "If set to true and Camera is in a Start Stage, this camera will attempt to be the Active Game Camera when this "
                                             "world starts. Only one Camera should be set as Active in each Start Stage.");
    addPropertyToComponent(Comps::Thing_Settings_Camera, Props::Thing_Camera_Speed, Property_Type::PointF, DrPointF(0, 0),
                           "Camera Speed", "Fixed movement speed of camera.");
    addPropertyToComponent(Comps::Thing_Settings_Camera, Props::Thing_Camera_Rotation, Property_Type::PointF, DrPointF(0, 0),
                           "Camera Rotation", "This setting is the camera rotation of that Camera. The "
                                              "<b>X</b> value changes <b>Up / Down</b> rotation, <b>Y</b> changes <b>Left / Right</b> rotation.");
    addPropertyToComponent(Comps::Thing_Settings_Camera, Props::Thing_Camera_Up_Vector, Property_Type::List, static_cast<int>(Up_Vector::Y),
                           "Camera Up Vector", "Default for most games / styles of gameplay is for a Y Axis Up Vector. "
                                               "If making a First Person style game, rotate camera X or Y to between 75 and 90 degrees and "
                                               "set Up Vector to Z Axis.");
    // !!!!! Hidden Property
    addPropertyToComponent(Comps::Thing_Settings_Camera, Props::Thing_Camera_Distance, Property_Type::Double, c_default_camera_z,
                           "Camera Distance", "Distance camera is away from z axis 0.0. Default is 801.", true, false);
    addPropertyToComponent(Comps::Thing_Settings_Camera, Props::Thing_Camera_Zoom, Property_Type::PositiveDouble, 1.0,
                           "Camera Zoom", "Magnification zoom level of this camera. A value of 1.0 is no zoom, 0.5 is twice as far away, 2.0 is twice as close. "
                                          "Generally less than 10.");
}





















