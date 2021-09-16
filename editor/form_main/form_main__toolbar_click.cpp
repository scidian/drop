//
//      Created by Stephens Nunnally on 3/23/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QButtonGroup>
#include <QDebug>

#include "editor/form_main/form_main.h"
#include "editor/forms/form_popup.h"
#include "editor/helper_library.h"
#include "editor/preferences.h"
#include "editor/trees/tree_assets.h"
#include "editor/trees/tree_inspector.h"
#include "editor/trees/tree_project.h"
#include "editor/view_editor/editor_item.h"
#include "editor/view_editor/editor_scene.h"
#include "editor/view_editor/editor_view.h"
#include "editor/view_node/node_scene.h"
#include "editor/view_node/node_view.h"
#include "engine/form_engine.h"
#include "project/dr_project.h"
#include "project/entities/dr_stage.h"
#include "project/entities/dr_thing.h"


//####################################################################################
//##    buttonGroupMode SLOT and functions
//####################################################################################
void FormMain::buttonGroupModeClicked(int id) {

    Editor_Mode new_id = static_cast<Editor_Mode>(id);
    if (getEditorMode() == new_id) return;
    setEditorMode(new_id);
}

void FormMain::buttonGroupModeSetChecked(int id) {
    QList<QAbstractButton*> buttons = m_buttons_group_mode->buttons();
    for (auto button : buttons) {
        bool is_button = (m_buttons_group_mode->button(id) == button);
        button->setChecked(is_button);
        button->setDown(!is_button);
    }
}


//####################################################################################
//##    buttonGroupLayering SLOT and functions
//####################################################################################
void FormMain::buttonGroupLayeringClicked(int id) {
    Buttons_Layering clicked = static_cast<Buttons_Layering>(id);
    QKeyEvent *event = nullptr;

    if (clicked == Buttons_Layering::Send_To_Front) { event = new QKeyEvent(QKeyEvent::KeyPress, Qt::Key_Greater, { Qt::KeyboardModifier::NoModifier }); }
    if (clicked == Buttons_Layering::Send_To_Back)  { event = new QKeyEvent(QKeyEvent::KeyPress, Qt::Key_Less,    { Qt::KeyboardModifier::NoModifier }); }
    if (clicked == Buttons_Layering::Send_Forward)  { event = new QKeyEvent(QKeyEvent::KeyPress, Qt::Key_Period,  { Qt::KeyboardModifier::NoModifier }); }
    if (clicked == Buttons_Layering::Send_Backward) { event = new QKeyEvent(QKeyEvent::KeyPress, Qt::Key_Comma,   { Qt::KeyboardModifier::NoModifier }); }

    if (event) {
        m_scene_editor->keyPressEvent(event);
        delete event;
    }
}


//####################################################################################
//##    buttonGroupEdit SLOT and functions
//####################################################################################
void FormMain::buttonGroupEditClicked(int id) {
    Buttons_Edit clicked = static_cast<Buttons_Edit>(id);

    if (clicked == Buttons_Edit::Add) {
        if (getEditorMode() == Editor_Mode::World_Creator) {
            FormPopup *popupAdd = new FormPopup(m_buttons_group_edit->button(id), m_project, m_widget_group_edit);
            popupAdd->buildPopupAddWorldEntity();
            popupAdd->show();
        } else if (getEditorMode() == Editor_Mode::Sound_Creator) {
            FormPopup *popupAdd = new FormPopup(m_buttons_group_edit->button(id), m_project, m_widget_group_edit);
            popupAdd->buildPopupAddSoundEntity();
            popupAdd->show();
        }

    } else if (clicked == Buttons_Edit::Delete || clicked == Buttons_Edit::Duplicate) {
        QKeyEvent *event = nullptr;
        if (clicked == Buttons_Edit::Delete)    event = new QKeyEvent(QKeyEvent::KeyPress, Qt::Key_Delete, { Qt::KeyboardModifier::NoModifier });
        if (clicked == Buttons_Edit::Duplicate) event = new QKeyEvent(QKeyEvent::KeyPress, Qt::Key_D,      { Qt::KeyboardModifier::NoModifier });

        if (getEditorMode() == Editor_Mode::World_Creator) {
            if      (getActiveWidget() == Editor_Widgets::Asset_Tree)       { m_tree_assets->keyPressEvent(event); }
            else if (getActiveWidget() == Editor_Widgets::Inspector_Tree)   { m_tree_assets->keyPressEvent(event); }
            else if (getActiveWidget() == Editor_Widgets::Project_Tree)     { m_tree_project->keyPressEvent(event); }
            else if (getActiveWidget() == Editor_Widgets::View) {
                if (getEditorMode() == Editor_Mode::World_Graph)            { m_scene_world_graph->keyPressEvent(event); }
                if (getEditorMode() == Editor_Mode::World_Creator)          { m_scene_editor->keyPressEvent(event); }
            }

        } else if (getEditorMode() == Editor_Mode::Sound_Creator) {
            if      (getActiveWidget() == Editor_Widgets::Asset_Tree)       { m_tree_assets->keyPressEvent(event); }
            else if (getActiveWidget() == Editor_Widgets::Inspector_Tree)   { m_tree_assets->keyPressEvent(event); }
        }

        if (event != nullptr) { delete event; event = nullptr; }
    }
}


//####################################################################################
//##    buttonGroupTransform SLOT and functions
//####################################################################################
void FormMain::buttonGroupTransformClicked(int id) {
    Buttons_Transform clicked = static_cast<Buttons_Transform>(id);

    if (clicked == Buttons_Transform::Reset_Object) {
        std::list<DrSettings*> settings;
        std::list<ComponentProperty> properties_to_update { std::make_pair(Comps::Thing_Transform, Props::Thing_Scale),
                                                            std::make_pair(Comps::Thing_Transform, Props::Thing_Rotation) };

        for (auto item : m_scene_editor->getSelectionItems()) {
            EditorItem *graphics_item = dynamic_cast<EditorItem*>(item);
            if (graphics_item == nullptr) continue;
            DrThing *thing = graphics_item->getThing();
            if (thing == nullptr) continue;

            settings.push_back(thing);
            thing->setComponentPropertyValue(Comps::Thing_Transform, Props::Thing_Scale, DrPointF(1, 1));
            thing->setComponentPropertyValue(Comps::Thing_Transform, Props::Thing_Rotation, 0);
        }
        m_scene_editor->resetSelectionGroup();
        updateEditorWidgetsAfterItemChange(Editor_Widgets::ToolBar, settings, properties_to_update );

    } else if (clicked == Buttons_Transform::Flip_H || clicked == Buttons_Transform::Flip_V) {
        QPointF scale = (clicked == Buttons_Transform::Flip_H) ? QPointF(-1, 1) : QPointF(1, -1);

        if (m_scene_editor->scene_mutex.tryLock() == true) {
            m_view_editor->startResizeSelection(QPoint(0, 0), false);
            m_view_editor->resizeSelection(QPointF(0, 0), true, scale);
            m_scene_editor->scene_mutex.unlock();
        }

    } else if (clicked == Buttons_Transform::Rotate_L || clicked == Buttons_Transform::Rotate_R) {
        double angle = (clicked == Buttons_Transform::Rotate_L) ? -90 : 90;

        if (m_scene_editor->scene_mutex.tryLock() == true) {
            m_view_editor->startRotateSelection(QPoint(0, 0), false);
            m_view_editor->rotateSelection(QPoint(0, 0), true, angle);
            m_scene_editor->scene_mutex.unlock();
        }
    }
}

//####################################################################################
//##    buttonGroupGridFull SLOT and functions
//####################################################################################
void FormMain::buttonGroupGridFullClicked(int id) {
    Buttons_Grid clicked = static_cast<Buttons_Grid>(id);

    if (clicked == Buttons_Grid::Snap_Options) {
        FormPopup *popupGrid = new FormPopup(m_buttons_group_grid_full->button(id), m_project, m_widget_group_grid_full);
        popupGrid->buildPopupGridSnap();
        popupGrid->show();

    } else {
        switch (clicked) {
            case Buttons_Grid::Snap_To_Grid:    Dr::SetPreference(Preferences::Editor_Snap_To_Grid,   m_buttons_group_grid_full->button(id)->isChecked());  break;
            case Buttons_Grid::Resize_To_Grid:  Dr::SetPreference(Preferences::Editor_Resize_To_Grid, m_buttons_group_grid_full->button(id)->isChecked());  break;
            case Buttons_Grid::Grid_On_Top:     Dr::SetPreference(Preferences::Editor_Grid_On_Top,    m_buttons_group_grid_full->button(id)->isChecked());  break;
            case Buttons_Grid::Snap_Options:    ;
        }
        m_view_editor->updateGrid();
        m_view_editor->updateSelectionBoundingBox(8);
    }
}

//####################################################################################
//##    buttonGroupGridSimple SLOT and functions
//####################################################################################
void FormMain::buttonGroupGridSimpleClicked(int id) {
    Buttons_Grid clicked = static_cast<Buttons_Grid>(id);

    switch (clicked) {
        case Buttons_Grid::Snap_To_Grid:    Dr::SetPreference(Preferences::Editor_Snap_To_Grid,   m_buttons_group_grid_simple->button(id)->isChecked());  break;
        case Buttons_Grid::Resize_To_Grid:  Dr::SetPreference(Preferences::Editor_Resize_To_Grid, m_buttons_group_grid_simple->button(id)->isChecked());  break;
        case Buttons_Grid::Grid_On_Top:     Dr::SetPreference(Preferences::Editor_Grid_On_Top,    m_buttons_group_grid_simple->button(id)->isChecked());  break;
        case Buttons_Grid::Snap_Options:    ;
    }
    m_view_world_graph->updateGrid();
}

//####################################################################################
//##    buttonGroupPlay SLOT and functions
//####################################################################################
void FormMain::buttonGroupPlayClicked(int id) {
    Buttons_Play clicked = static_cast<Buttons_Play>(id);

    if (clicked == Buttons_Play::Play) {
        FormEngine *engine = new FormEngine(m_project);             // !!!!! #NOTE: Does not like to be a child?
        engine->show();

    } else if (clicked == Buttons_Play::Play_Stage) {
        long only_stage_key = this->getViewEditor()->getEditorScene()->getCurrentStageKeyShown();

        DrStage *stage = m_project->findStageFromKey(only_stage_key);
        if (stage != nullptr) {
            if (stage->isStartStage()) only_stage_key = c_no_key;
        } else {
            only_stage_key = c_no_key;
        }

        FormEngine *engine = new FormEngine(m_project, only_stage_key);
        engine->show();
    }
}











