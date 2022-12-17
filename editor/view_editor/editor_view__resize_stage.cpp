//
//      Created by Stephens Nunnally on 4/17/2020, (c) 2020 Scidian Studios, All Rights Reserved
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
#include "editor/view_editor/editor_item.h"
#include "editor/view_editor/editor_scene.h"
#include "editor/view_editor/editor_view.h"
#include "editor/widgets/widgets_view.h"
#include "engine/debug_flags.h"
#include "project/dr_project.h"
#include "project/entities/dr_world.h"
#include "project/entities/dr_stage.h"
#include "project/entities/dr_thing.h"
#include "project/settings/settings.h"
#include "project/settings/settings_component.h"
#include "project/settings/settings_component_property.h"


//####################################################################################
//##    Starts Resizing Stage Mode
//####################################################################################
void EditorView::startResizeStage(QPoint mouse_in_view) {
    // Store starting mouse position
    m_resize_stage_mouse_start = mouse_in_view;

    // Get rotation of current Stage
    DrStage *stage = my_scene->getCurrentStageShown();
    m_resize_stage_angle = stage->getParentWorld()->getComponentPropertyValue(Comps::World_Settings, Props::World_Game_Direction).toDouble();
    m_resize_stage_angle = Dr::EqualizeAngle0to360(m_resize_stage_angle);

    // Get size of current Stage
    m_resize_stage_start_size = stage->getComponentPropertyValue(Comps::Stage_Settings, Props::Stage_Size).toDouble();
}


//####################################################################################
//##    Main Stage Resize Function
//####################################################################################
void EditorView::resizeStage(QPointF mouse_in_view) {
    // Get current DrStage
    DrStage *stage = my_scene->getCurrentStageShown();

    // Calculate difference between starting mouse position and current mouse posisiton
    QTransform t = QTransform().rotate(-m_resize_stage_angle);
    double diff = t.map(m_resize_stage_mouse_start).x() - t.map(mouse_in_view).x();
           diff /= currentZoomLevel();

    // Figure out new stage size
    int new_size = static_cast<int>(m_resize_stage_start_size - diff);
    if (new_size < 0) new_size = 0;

    // Update data model and widgets
    stage->setComponentPropertyValue(Comps::Stage_Settings, Props::Stage_Size, new_size);
    m_editor_relay->updateEditorWidgetsAfterItemChange(Editor_Widgets::View, { stage }, { ComponentProperty(Comps::Stage_Settings, Props::Stage_Size) } );
}














