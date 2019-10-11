//
//      Created by Stephens Nunnally on 3/23/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QButtonGroup>
#include <QDebug>

#include "editor/editor_tree_assets.h"
#include "editor_view/editor_item.h"
#include "editor_view/editor_scene.h"
#include "editor_view/editor_view.h"
#include "engine/form_engine.h"
#include "form_main.h"
#include "form_popup.h"
#include "globals.h"
#include "helper.h"
#include "helper_qt.h"
#include "project/project.h"
#include "project/project_world_stage_thing.h"

//####################################################################################
//##    buttonGroupMode SLOT and functions
//####################################################################################
void FormMain::buttonGroupModeClicked(int id) {
    buttonGroupModeSetChecked(id);

    Form_Main_Mode new_id = static_cast<Form_Main_Mode>(id);
    if (m_current_mode == new_id) return;

    if (new_id == Form_Main_Mode::World_Map) {
        this->setFormMainMode(Form_Main_Mode::Clear);
    } else if (new_id == Form_Main_Mode::World_Editor) {
        this->setFormMainMode(Form_Main_Mode::World_Editor);
    } else if (new_id == Form_Main_Mode::UI_Editor) {
        this->setFormMainMode(Form_Main_Mode::Clear);
    }
}

void FormMain::buttonGroupModeSetChecked(int id) {
    QList<QAbstractButton*> buttons = buttonsGroupMode->buttons();
    for (auto button : buttons) {
        bool is_button = (buttonsGroupMode->button(id) == button);
        button->setChecked(is_button);
        button->setDown(!is_button);
    }
}


//####################################################################################
//##    buttonGroupAddClicked SLOT and functions
//####################################################################################
void FormMain::buttonGroupAddClicked(int id) {
    Buttons_Add clicked = static_cast<Buttons_Add>(id);

    if (clicked == Buttons_Add::Add) {
        FormPopup *popupAdd = new FormPopup(buttonsGroupAdd->button(id), m_project, widgetGroupAdd);
        popupAdd->buildPopupAddEntity();
        popupAdd->show();
    }
}


//####################################################################################
//##    buttonGroupLayering SLOT and functions
//####################################################################################
void FormMain::buttonGroupLayeringClicked(int id) {
    Buttons_Layering clicked = static_cast<Buttons_Layering>(id);

    if (clicked == Buttons_Layering::Send_To_Back) {
        // !!!!! #TEMP: Map speed testing
        ///Dr::ShowMessageBox( m_project->testSpeedFindSettings(1000) );

    }
}


//####################################################################################
//##    buttonGroupEdit SLOT and functions
//####################################################################################
void FormMain::buttonGroupEditClicked(int id) {
    Buttons_Edit clicked = static_cast<Buttons_Edit>(id);

    if (clicked == Buttons_Edit::Delete) {
        QKeyEvent *event = new QKeyEvent(QKeyEvent::KeyPress, Qt::Key_Delete, { Qt::KeyboardModifier::NoModifier });

        if (getActiveWidget() == Editor_Widgets::Project_Tree || getActiveWidget() == Editor_Widgets::Scene_View) {
            sceneEditor->keyPressEvent(event);
            delete event;

        } else if (getActiveWidget() == Editor_Widgets::Asset_Tree) {
            treeAssetEditor->keyPressEvent(event);
            delete event;
        }
    }
}


//####################################################################################
//##    buttonGroupTransform SLOT and functions
//####################################################################################
void FormMain::buttonGroupTransformClicked(int id) {
    Buttons_Transform clicked = static_cast<Buttons_Transform>(id);

    if (clicked == Buttons_Transform::Reset_Object) {
        QList<DrSettings*> settings;
        QList<Properties>  properties { Properties::Thing_Scale, Properties::Thing_Rotation };

        for (auto item : sceneEditor->getSelectionItems()) {
            DrItem   *dritem = dynamic_cast<DrItem*>(item);
            DrThing  *thing = dritem->getThing();

            settings.append(thing);
            thing->setComponentPropertyValue(Components::Thing_Transform, Properties::Thing_Scale, QPointF(1, 1));
            thing->setComponentPropertyValue(Components::Thing_Transform, Properties::Thing_Rotation, 0);
        }
        sceneEditor->resetSelectionGroup();
        updateEditorWidgetsAfterItemChange(Editor_Widgets::ToolBar, settings, properties );

    } else if (clicked == Buttons_Transform::Flip_H || clicked == Buttons_Transform::Flip_V) {
        QPointF scale = (clicked == Buttons_Transform::Flip_H) ? QPointF(-1, 1) : QPointF(1, -1);

        if (sceneEditor->scene_mutex.tryLock() == true) {
            viewEditor->startResize(QPoint(0, 0), false);
            viewEditor->resizeSelection(QPointF(0, 0), true, scale);
            sceneEditor->scene_mutex.unlock();
        }

    } else if (clicked == Buttons_Transform::Rotate_L || clicked == Buttons_Transform::Rotate_R) {
        double angle = (clicked == Buttons_Transform::Rotate_L) ? -90 : 90;

        if (sceneEditor->scene_mutex.tryLock() == true) {
            viewEditor->startRotate(QPoint(0, 0), false);
            viewEditor->rotateSelection(QPoint(0, 0), true, angle);
            sceneEditor->scene_mutex.unlock();
        }
    }
}

//####################################################################################
//##    buttonGroupGrid SLOT and functions
//####################################################################################
void FormMain::buttonGroupGridClicked(int id) {
    Buttons_Grid clicked = static_cast<Buttons_Grid>(id);

    if (clicked == Buttons_Grid::Snap_Options) {
        FormPopup *popupGrid = new FormPopup(buttonsGroupGrid->button(id), m_project, widgetGroupGrid);
        popupGrid->buildPopupGridSnap();
        popupGrid->show();

    } else {
        switch (clicked) {
            case Buttons_Grid::Snap_To_Grid:    Dr::SetPreference(Preferences::World_Editor_Snap_To_Grid,   buttonsGroupGrid->button(id)->isChecked());  break;
            case Buttons_Grid::Resize_To_Grid:  Dr::SetPreference(Preferences::World_Editor_Resize_To_Grid, buttonsGroupGrid->button(id)->isChecked());  break;
            case Buttons_Grid::Grid_On_Top:     Dr::SetPreference(Preferences::World_Editor_Grid_On_Top,    buttonsGroupGrid->button(id)->isChecked());  break;
            case Buttons_Grid::Snap_Options:    ;
        }
        viewEditor->updateGrid();
        viewEditor->updateSelectionBoundingBox(8);
    }
}



//####################################################################################
//##    buttonGroupPlay SLOT and functions
//####################################################################################
void FormMain::buttonGroupPlayClicked(int id) {
    Buttons_Play clicked = static_cast<Buttons_Play>(id);

    if (clicked == Buttons_Play::Play) {

        FormEngine *engine = new FormEngine(m_project);             // !!!!! #NOTE: Does not like to be a child?
        engine->show();

    }
}










