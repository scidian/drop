//
//      Created by Stephens Nunnally on 10/20/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "core/colors/colors.h"
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
//##    Property loading - initializeStageSettings
//####################################################################################
void DrStage::initializeStageSettings(std::string new_name) {
    DrProperty *property_name = getComponentProperty(Comps::Entity_Settings, Props::Entity_Name);
    property_name->setDisplayName("Stage Name");
    property_name->setDescription("Name of this Stage.");
    property_name->setValue(new_name);

    addComponent(Comps::Stage_Settings, "Stage Settings", "Settings for this Stage.", Component_Colors::RGB_18_Gray, false);
    getComponent(Comps::Stage_Settings)->setIcon(Component_Icons::Settings);
    addPropertyToComponent(Comps::Stage_Settings, Props::Stage_Enabled, Property_Type::Bool, true,
                           "Enabled?", "Should this Stage be able to be shown?");
    addPropertyToComponent(Comps::Stage_Settings, Props::Stage_Start, Property_Type::Positive, 0,
                           "Start", "Start randomly showing this Stage at this distance.");
    addPropertyToComponent(Comps::Stage_Settings, Props::Stage_End, Property_Type::Int, -1,
                           "End", "Stop randomly showing this Stage at this distance. Use a value of <= 0 (i.e. -1) for an infinite end distance. Does not apply to the "
                                  "Start Stage (Start Stage is always only shown exactly once).");
    addPropertyToComponent(Comps::Stage_Settings, Props::Stage_Size, Property_Type::Positive, 1024,
                           "Size", "Length of stage.");
    addPropertyToComponent(Comps::Stage_Settings, Props::Stage_Cooldown, Property_Type::Positive, 0,
                           "Cooldown", "Distance to wait after stage plays before it is possible to start showing again.");


    addComponent(Comps::Stage_Grid, "Grid", "Settings for the alignment grid within the editor. For an Isometric Grid: set \"Grid Rotation\" "
                                                 "to 45 degrees, and set \"Grid Scale\" X value twice the size \"Grid Scale\" Y value (i.e. X: 2, Y: 1).",
                                                 Component_Colors::RGB_19_Silver, false);
    getComponent(Comps::Stage_Grid)->setIcon(Component_Icons::Transform);
    addPropertyToComponent(Comps::Stage_Grid, Props::Stage_Grid_Style, Property_Type::List, static_cast<int>(0),
                           "Grid Style", "Visual style of alignment grid.");
    addPropertyToComponent(Comps::Stage_Grid, Props::Stage_Grid_Origin_Point, Property_Type::PointF, DrPointF(0, 0),
                           "Grid Origin Point", "Origin point in stage the grid begins at. Allows for small adjustments of enitre grid.");
    addPropertyToComponent(Comps::Stage_Grid, Props::Stage_Grid_Size, Property_Type::GridF, DrPointF(32, 32),
                           "Grid Cell Size", "Width and height of the cells in the grid.");
    addPropertyToComponent(Comps::Stage_Grid, Props::Stage_Grid_Scale, Property_Type::GridScaleF, DrPointF(1, 1),
                           "Grid Scale", "X and Y stretch factor after grid has been rotated. For Isometric Grids, set X value twice that of Y value.");
    addPropertyToComponent(Comps::Stage_Grid, Props::Stage_Grid_Rotation, Property_Type::Angle, 0,
                           "Grid Rotation", "Rotation of the grid lines. For Isometric Grids, set \"Grid Rotation\" to 45 degrees.");
    addPropertyToComponent(Comps::Stage_Grid, Props::Stage_Grid_Color, Property_Type::Color, Dr::GetColor(Window_Colors::Background_Light).rgba(),
                           "Grid Color", "Color of grid lines.", true);

}

















