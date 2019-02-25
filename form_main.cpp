//
//      Created by Stephens Nunnally on 12/7/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Loads, styles and handles events for FormMain
//
//
#include <QDockWidget>
#include <QTimer>

#include "colors.h"
#include "debug.h"
#include "enums.h"

#include "editor_item.h"
#include "editor_scene.h"
#include "editor_view.h"
#include "editor_tree_advisor.h"
#include "editor_tree_assets.h"
#include "editor_tree_inspector.h"
#include "editor_tree_project.h"

#include "form_main.h"

#include "library.h"

#include "project.h"
#include "project_asset.h"
#include "project_world.h"
#include "project_world_stage.h"
#include "project_world_stage_object.h"
#include "settings.h"
#include "settings_component.h"
#include "settings_component_property.h"


// Destructor for Main Window
FormMain::~FormMain()
{
    delete project;
}

//####################################################################################
//##        Constructor for Main Window, called upon initialization
//####################################################################################
FormMain::FormMain(QWidget *parent) : QMainWindow(parent)
{

    // ########## Load saved preferences
    Dr::SetColorScheme(Color_Scheme::Dark);


    // ########## Initialize new project, initialize local variables
    project = new DrProject(1);
    current_world = 0;


    // !!!!! #TEMP: Add images / assets
    long image_1 = project->addImage(":/assets/test_square.png");
    long image_2 = project->addImage(":/assets/ground_fill.png");
    long image_3 = project->addImage(":/assets/ground_top.png");
    long image_4 = project->addImage(":/assets/moon_plant_6.png");
    long image_5 = project->addImage(":/assets/rover_body.png");
    long image_6 = project->addImage(":/assets/ball_1.png");
    long image_7 = project->addImage(":/assets/water_1.png");
    long image_8 = project->addImage(":/assets/rover_wheel.png");

    long asset_1 = project->addAsset(DrAssetType::Object, image_1 );           // "Dr Square"
    long asset_2 = project->addAsset(DrAssetType::Object, image_2 );           // "Ground Fill"
    long asset_3 = project->addAsset(DrAssetType::Object, image_3 );           // "Ground Top"
    long asset_4 = project->addAsset(DrAssetType::Object, image_4 );           // "Moon Plant 6"
    long asset_5 = project->addAsset(DrAssetType::Object, image_5 );           // "Rover Body"
    long asset_6 = project->addAsset(DrAssetType::Object, image_6 );           // "Ball 1"
    long asset_7 = project->addAsset(DrAssetType::Object, image_7 );           // "Water 1"
    long asset_8 = project->addAsset(DrAssetType::Object, image_8 );           // "Rover Wheel"
    // !!!!! END



    // !!!!! #TEMP: New Project
    // Create a new project and add some stuff to it
    project->addWorld();
    project->addWorld();
    project->getWorldWithName("World 2")->addStage();
    project->getWorldWithName("World 2")->addStage("asdfasdfasdfasdfasfdasdfasdfasdfasdfasdfasdfasdfasd");
    project->getWorldWithName("World 2")->addStage();
    project->getWorldWithName("World 2")->addStage();
    project->getWorldWithName("World 2")->addStage();

    project->getWorldWithName("World 2")->getStageWithName("4")->addObject(DrObjectType::Object, asset_1, 0, 0, 1);
    project->getWorldWithName("World 2")->getStageWithName("4")->addObject(DrObjectType::Object, asset_1, -200, -200, 2);
    project->getWorldWithName("World 2")->getStageWithName("4")->addObject(DrObjectType::Object, asset_2, 250, -200, 3);
    project->getWorldWithName("World 2")->getStageWithName("4")->addObject(DrObjectType::Object, asset_3, -200, 200, 10);
    project->getWorldWithName("World 2")->getStageWithName("4")->addObject(DrObjectType::Object, asset_4, 100, 100, 11);
    project->getWorldWithName("World 2")->getStageWithName("4")->addObject(DrObjectType::Object, asset_5, -150, 0, 30);

    project->getWorldWithName("World 2")->getStageWithName("2")->addObject(DrObjectType::Object, asset_5, 100, 100, -2);
    project->getWorldWithName("World 2")->getStageWithName("2")->addObject(DrObjectType::Object, asset_6, 200, 100,  4);
    project->getWorldWithName("World 2")->getStageWithName("2")->addObject(DrObjectType::Object, asset_7, -200, -200,  1);
    project->getWorldWithName("World 2")->getStageWithName("2")->addObject(DrObjectType::Object, asset_8, 0, 0,  2);
    // !!!!! END



    // ########## Initialize form and customize colors and styles
    scene = new DrScene(this, project, this);
    buildMenu();
    buildWindow(Form_Main_Mode::World_Editor);
    Dr::ApplyColoring(this);



    // ########## Populates DrScene from current DrStage
    buildScene( project->getWorldWithName("World 2")->getStageWithName("4")->getKey() );



    Dr::SetDoneLoading(true);
}


// Sets the new palette to the style sheets
void FormMain::changePalette(Color_Scheme new_color_scheme) {
    Dr::SetColorScheme(new_color_scheme);
    Dr::ApplyColoring(this);
    update();
}



//####################################################################################
//##        Editor Interface Relay Handlers
//####################################################################################
///enum class Editor_Widgets {
///    Asset_Tree,
///    Object_Inspector,
///    Project_Tree,
///    Scene_View,

void FormMain::buildAssetTree() {
    treeAsset->buildAssetTree();
}

// Sends new list to Object Inspector
void FormMain::buildObjectInspector(QList<long> key_list) {
    // If we're currently in the middle of selecting with rubber band box, don't update yet
    if (viewMain->currentViewMode() == View_Mode::Selecting) return;

    // If key_list.count() = 0, then an empty list was passed in. This will clear the object inspector.
    // If we're doing anything at all in viewMain (i.e. View_Mode != None), let's wait to clear the inspector.
    if (viewMain->currentViewMode() != View_Mode::None && key_list.count() == 0) return;

    treeInspector->buildInspectorFromKeys(key_list);
}

void FormMain::buildProjectTree() {
    treeProject->buildProjectTree();
}

// Emits an Undo stack command to change Stages within Scene
void FormMain::buildScene(long from_stage_key)
{
    if (scene->scene_mutex.tryLock(10) == false) return;

    if (scene->getCurrentStageKeyShown() != from_stage_key)
        emit newStageSelected(project, scene, scene->getCurrentStageKeyShown(), from_stage_key);

    scene->scene_mutex.unlock();
}




void FormMain::updateEditorWidgetsAfterItemChange(Editor_Widgets changed_from, QList<DrSettings*> changed_items, QList<Properties> property_keys) {
    updateEditorWidgetsAfterItemChange(changed_from, changed_items, Dr::ConvertPropertyListToLongs(property_keys));    }
void FormMain::updateEditorWidgetsAfterItemChange(Editor_Widgets changed_from, QList<DrSettings*> changed_items, QList<long> property_keys)
{
    if (viewMain->currentViewMode() == View_Mode::Translating) return;

    if (changed_from != Editor_Widgets::Object_Inspector)   treeInspector->updateInspectorPropertyBoxes(changed_items, property_keys);
    if (changed_from != Editor_Widgets::Scene_View)         scene->updateChangesInScene(changed_items, property_keys);
    if (changed_from != Editor_Widgets::Project_Tree)       treeProject->updateItemNames(changed_items, property_keys);
    if (changed_from != Editor_Widgets::Asset_Tree)         treeAsset->updateAssetList(changed_items, property_keys);

    // !!!!! TEMP: Testing to make sure not running non stop
    setLabelText(Label_Names::Label_Bottom, "Update Editor Widgets: " + QTime::currentTime().toString());
}

void FormMain::updateItemSelection(Editor_Widgets selected_from)
{
    // Selects items in scene based on new selection in tree view
    if (selected_from != Editor_Widgets::Scene_View)    scene->updateSelectionFromProjectTree( treeProject->selectedItems() );
    if (selected_from != Editor_Widgets::Project_Tree)  treeProject->updateSelectionFromView( scene->getSelectionItems() );

    // !!!!! TEMP: Testing to make sure not running non stop
    setLabelText(Label_Names::Label_Bottom, "Update Selection: " + QTime::currentTime().toString());
}





// Emits a single shot timer to update view coordinates after event calls are done
void FormMain::centerViewOnPoint(QPointF center_point) {
    if (viewMain->hasLoadedFirstScene())
        viewMain->centerOn(center_point);
    else
        QTimer::singleShot(0, this, [this, center_point] { this->centerViewTimer(center_point); } );
}
void FormMain::centerViewTimer(QPointF center_point) {  viewMain->centerOn(center_point); viewMain->loadedFirstScene(); }



//####################################################################################
//##        Misc Interface Relay Handlers
//####################################################################################
// Call to put in a signal to change the Advisor to the que
void FormMain::setAdvisorInfo(HeaderBodyList header_body_list) {
    setAdvisorInfo(header_body_list[0], header_body_list[1]);
}

// Call to put in a signal to change the Advisor to the que
void FormMain::setAdvisorInfo(QString header, QString body)
{
    if (current_mode != Form_Main_Mode::World_Editor) return;
    if (advisor == nullptr) return;
    if (advisor->isHidden()) return;                                        // If Advisor dock was closed, cancel
    if (treeAdvisor == nullptr) return;
    if (treeAdvisor->getAdvisorHeader() == header) return;                  // If Advisor header is already set to proper info, cancel
    emit sendAdvisorInfo(header, body);                                     // Emits signal connected to changeAdvisor
}

// Sets the text of a label on FormMain
void FormMain::setLabelText(Label_Names label_name, QString new_text)
{
    if (Dr::CheckDoneLoading() == false) return;

    switch (label_name)
    {
    case Label_Names::Label_1:          label_1->setText(new_text);         break;
    case Label_Names::Label_2:          label_2->setText(new_text);         break;
    case Label_Names::Label_3:          label_3->setText(new_text);         break;
    case Label_Names::Label_Mouse_1:    label_mouse_1->setText(new_text);   break;
    case Label_Names::Label_Mouse_2:    label_mouse_2->setText(new_text);   break;

    case Label_Names::Label_Object_1:   label_object_1->setText(new_text);  break;
    case Label_Names::Label_Object_2:   label_object_2->setText(new_text);  break;
    case Label_Names::Label_Object_3:   label_object_3->setText(new_text);  break;
    case Label_Names::Label_Object_4:   label_object_4->setText(new_text);  break;
    case Label_Names::Label_Object_5:   label_object_5->setText(new_text);  break;

    case Label_Names::Label_Position:   label_position->setText(new_text);  break;
    case Label_Names::Label_Center:     label_center->setText(new_text);    break;
    case Label_Names::Label_Scale:      label_scale->setText(new_text);     break;
    case Label_Names::Label_Rotate:     label_rotate->setText(new_text);    break;
    case Label_Names::Label_Z_Order:    label_z_order->setText(new_text);   break;
    case Label_Names::Label_Pos_Flag:   label_pos_flag->setText(new_text);  break;

    case Label_Names::Label_Bottom:     label_bottom->setText(new_text);    break;
    }
}















