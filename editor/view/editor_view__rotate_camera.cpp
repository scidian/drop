//
//      Created by Stephens Nunnally on 1/15/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "core/colors/colors.h"
#include "core/dr_math.h"
#include "editor/helper_library.h"
#include "editor/interface_editor_relay.h"
#include "editor/preferences.h"
#include "editor/view/editor_item.h"
#include "editor/view/editor_scene.h"
#include "editor/view/editor_view.h"
#include "engine/debug_flags.h"
#include "project/dr_project.h"
#include "project/entities/dr_world.h"
#include "project/entities/dr_stage.h"
#include "project/entities/dr_thing.h"
#include "project/settings/settings.h"
#include "project/settings/settings_component.h"
#include "project/settings/settings_component_property.h"


//####################################################################################
//##    Starts Rotating Camera Mode
//####################################################################################
void DrView::startRotateCamera(QPoint mouse_in_view) {
    // Set selected camera to whichever camera mouse is currently over
    m_cam_selected = m_cam_mouse_over;
    m_cam_mouse_over = nullptr;

    // Store starting mouse position
    m_cam_mouse_start = mouse_in_view;
    m_cam_at_back = (m_cam_data[m_cam_selected].z_order < m_cam_selected->getZOrderWithSub());

    // Get initial rotation of current camera
    DrPointF rotation;
    if (m_cam_selected->getThingType() == DrThingType::Character) {
        rotation = m_cam_selected->getComponentPropertyValue(Comps::Thing_Settings_Character, Props::Thing_Character_Camera_Rotation).toPointF();
    } else if (m_cam_selected->getThingType() == DrThingType::Camera) {
        rotation = m_cam_selected->getComponentPropertyValue(Comps::Thing_Settings_Camera, Props::Thing_Camera_Rotation).toPointF();
    }
    m_cam_angle_x_start = rotation.x;
    m_cam_angle_y_start = rotation.y;

    double equalized_x = Dr::EqualizeAngle0to360(rotation.x);
    m_cam_reversed = (equalized_x > 90 && equalized_x < 270);
}


//####################################################################################
//##    Main Rotation Function
//####################################################################################
void DrView::rotateCamera(QPointF mouse_in_view) {

    // Calculate difference between starting mouse position and current mouse posisiton. Use that to assign new rotation angle
    double x_diff = ((mouse_in_view.x() - m_cam_mouse_start.x()) * 0.125) / currentZoomLevel();
    double y_diff = ((mouse_in_view.y() - m_cam_mouse_start.y()) * 0.125) / currentZoomLevel();
    DrPointF rotation;
    rotation.x = m_cam_angle_x_start + ((m_cam_reversed) ? -y_diff :  y_diff);
    rotation.y = m_cam_angle_y_start + ((m_cam_at_back)  ?  x_diff : -x_diff);

    // Update data model
    if (m_cam_selected->getThingType() == DrThingType::Character) {
        m_cam_selected->setComponentPropertyValue(Comps::Thing_Settings_Character, Props::Thing_Character_Camera_Rotation, rotation);
        m_editor_relay->updateEditorWidgetsAfterItemChange( Editor_Widgets::Stage_View, { m_cam_selected },
                        { std::make_pair(Comps::Thing_Settings_Character, Props::Thing_Character_Camera_Rotation) });
    } else if (m_cam_selected->getThingType() == DrThingType::Camera) {
        m_cam_selected->setComponentPropertyValue(Comps::Thing_Settings_Camera, Props::Thing_Camera_Rotation, rotation);
        m_editor_relay->updateEditorWidgetsAfterItemChange( Editor_Widgets::Stage_View, { m_cam_selected },
                        { std::make_pair(Comps::Thing_Settings_Camera, Props::Thing_Camera_Rotation) });
    }

}


















