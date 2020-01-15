//
//      Created by Stephens Nunnally on 1/15/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "core/colors/colors.h"
#include "core/dr_math.h"
#include "editor/debug.h"
#include "editor/helper_library.h"
#include "editor/interface_editor_relay.h"
#include "editor/preferences.h"
#include "editor/view/editor_item.h"
#include "editor/view/editor_scene.h"
#include "editor/view/editor_view.h"
#include "project/dr_project.h"
#include "project/entities/dr_world.h"
#include "project/entities/dr_stage.h"
#include "project/entities/dr_thing.h"
#include "project/settings/settings.h"
#include "project/settings/settings_component.h"
#include "project/settings/settings_component_property.h"

// Local Variables
static QPoint  mouse_start;
static double  angle_x_start = 0;
static double  angle_y_start = 0;


//####################################################################################
//##    Starts Rotating Camera Mode
//####################################################################################
void DrView::startRotateCamera(QPoint mouse_in_view) {
    // Set selected camera to whichever camera mouse is currently over
    m_cam_selected = m_cam_mouse_over;
    m_cam_mouse_over = nullptr;

    // Store starting mouse position
    mouse_start = mouse_in_view;

    // Get initial rotation of current camera
    DrPointF rotation;
    if (m_cam_selected->getThingType() == DrThingType::Character) {
        rotation = m_cam_selected->getComponentPropertyValue(Components::Thing_Settings_Character, Properties::Thing_Character_Camera_Rotation).toPointF();
    } else if (m_cam_selected->getThingType() == DrThingType::Camera) {
        rotation = m_cam_selected->getComponentPropertyValue(Components::Thing_Settings_Camera, Properties::Thing_Camera_Rotation).toPointF();
    }
    angle_x_start = rotation.x;
    angle_y_start = rotation.y;
}


//####################################################################################
//##    Main Rotation Function
//####################################################################################
void DrView::rotateCamera(QPointF mouse_in_view) {

    double x_diff = ((mouse_in_view.x() - mouse_start.x()) * 0.25) / currentZoomLevel();
    double y_diff = ((mouse_in_view.y() - mouse_start.y()) * 0.25) / currentZoomLevel();

    DrPointF rotation;
    rotation.x = angle_x_start + y_diff;
    rotation.y = angle_y_start + x_diff;

    if (m_cam_selected->getThingType() == DrThingType::Character) {
        m_cam_selected->setComponentPropertyValue(Components::Thing_Settings_Character, Properties::Thing_Character_Camera_Rotation, rotation);
    } else if (m_cam_selected->getThingType() == DrThingType::Camera) {
        m_cam_selected->setComponentPropertyValue(Components::Thing_Settings_Camera, Properties::Thing_Camera_Rotation, rotation);
    }

}


















