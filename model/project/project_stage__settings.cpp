//
//      Created by Stephens Nunnally on 10/20/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "editor/colors/colors.h"
#include "editor/constants_editor.h"
#include "editor/helper_editor.h"
#include "model/project/project.h"
#include "model/project/project_asset.h"
#include "model/project/project_world.h"
#include "model/project/project_stage.h"
#include "model/project/project_thing.h"
#include "model/settings/settings.h"
#include "model/settings/settings_component.h"
#include "model/settings/settings_component_property.h"


//####################################################################################
//##    Property loading - initializeStageSettings
//####################################################################################
void DrStage::initializeStageSettings(std::string new_name) {
    DrProperty *property_name = getComponentProperty(Components::Entity_Settings, Properties::Entity_Name);
    property_name->setDisplayName("Stage Name");
    property_name->setDescription("Name of this Stage.");
    property_name->setValue(new_name);

    addComponent(Components::Stage_Settings, "Stage Settings", "Settings for this Stage.", Component_Colors::White_Snow, true);
    getComponent(Components::Stage_Settings)->setIcon(Component_Icons::Settings);
    addPropertyToComponent(Components::Stage_Settings, Properties::Stage_Enabled, Property_Type::Bool, true,
                           "Enabled?", "Should this Stage be able to be shown?");
    addPropertyToComponent(Components::Stage_Settings, Properties::Stage_Start, Property_Type::Positive, 0,
                           "Start", "Start randomly showing this Stage at this distance.");
    addPropertyToComponent(Components::Stage_Settings, Properties::Stage_End, Property_Type::Int, -1,
                           "End", "Stop randomly showing this Stage at this distance. Use a value of <= 0 (i.e. -1) for an infinite end distance. Does not apply to the "
                                  "Start Stage (Start Stage is always only shown exactly once).");
    addPropertyToComponent(Components::Stage_Settings, Properties::Stage_Size, Property_Type::Positive, 1200,
                           "Size", "Length of stage.");
    addPropertyToComponent(Components::Stage_Settings, Properties::Stage_Cooldown, Property_Type::Positive, 0,
                           "Cooldown", "Distance to wait after stage plays before it is possible to start showing again.");

    addComponent(Components::Stage_Grid, "Grid", "Settings for the alignment grid within the editor. For an Isometric Grid: set \"Grid Rotation\" "
                                                 "to 45 degrees, and set \"Grid Scale\" X value twice the size \"Grid Scale\" Y value (i.e. X: 2, Y: 1).",
                                                 Component_Colors::Pink_Pearl, true);
    getComponent(Components::Stage_Grid)->setIcon(Component_Icons::Transform);
    addPropertyToComponent(Components::Stage_Grid, Properties::Stage_Grid_Style, Property_Type::List, 0,
                           "Grid Style", "Visual style of alignment grid.");
    addPropertyToComponent(Components::Stage_Grid, Properties::Stage_Grid_Origin_Point, Property_Type::PointF, DrPointF(0, 0),
                           "Grid Origin Point", "Origin point in stage the grid begins at. Allows for small adjustments of enitre grid.");
    addPropertyToComponent(Components::Stage_Grid, Properties::Stage_Grid_Size, Property_Type::GridF, DrPointF(50, 50),
                           "Grid Cell Size", "Width and height of the cells in the grid.");
    addPropertyToComponent(Components::Stage_Grid, Properties::Stage_Grid_Scale, Property_Type::GridScaleF, DrPointF(1, 1),
                           "Grid Scale", "X and Y stretch factor after grid has been rotated. For Isometric Grids, set X value twice that of Y value.");
    addPropertyToComponent(Components::Stage_Grid, Properties::Stage_Grid_Rotation, Property_Type::Angle, 0,
                           "Grid Rotation", "Rotation of the grid lines. For Isometric Grids, set \"Grid Rotation\" to 45 degrees.");
    addPropertyToComponent(Components::Stage_Grid, Properties::Stage_Grid_Color, Property_Type::Color, Dr::GetColor(Window_Colors::Background_Light).rgba(),
                           "Grid Color", "Color of grid lines.", true);
}

















