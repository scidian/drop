//
//      Created by Stephens Nunnally on 12/7/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Loads, styles and handles events for FormMain
//
//

#include "colors.h"
#include "library.h"

#include "project.h"
#include "project_asset.h"
#include "project_world.h"
#include "project_world_scene.h"
#include "project_world_scene_object.h"

#include "settings.h"
#include "settings_component.h"
#include "settings_component_property.h"

#include "editor_scene_scene.h"
#include "editor_tree_advisor.h"
#include "editor_tree_assets.h"
#include "editor_tree_inspector.h"
#include "editor_tree_scene.h"
#include "editor_scene_view.h"

#include "form_main.h"

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
    project = new DrProject(0);
    current_world = 0;


    // !!!!! #TEMP: Add assets
    long asset_1 = project->addAsset("Dr Square",    DrAsset_Type::Object, QPixmap(":/assets/test_square.png"));
    long asset_2 = project->addAsset("Ground Fill",  DrAsset_Type::Object, QPixmap(":/assets/ground_fill.png"));
    long asset_3 = project->addAsset("Ground Top",   DrAsset_Type::Object, QPixmap(":/assets/ground_top.png"));
    long asset_4 = project->addAsset("Moon Plant 6", DrAsset_Type::Object, QPixmap(":/assets/moon_plant_6.png"));
    long asset_5 = project->addAsset("Rover Body",   DrAsset_Type::Object, QPixmap(":/assets/rover_body.png"));
    // !!!!! END



    // !!!!! #TEMP: New Project
    // Create a new project and add some stuff to it
    project->addWorld();
    project->addWorld();
    project->getWorldWithName("World 2")->addScene();
    project->getWorldWithName("World 2")->addScene("asdfasdfasdfasdfasfdasdfasdfasdfasdfasdfasdfasdfasd");
    project->getWorldWithName("World 2")->addScene();
    project->getWorldWithName("World 2")->addScene();
    project->getWorldWithName("World 2")->addScene();

    project->getWorldWithName("World 2")->getSceneWithName("4")->addObject(DrType::Object, asset_1, 0, 0);
    project->getWorldWithName("World 2")->getSceneWithName("4")->addObject(DrType::Object, asset_1, -200, -200);
    project->getWorldWithName("World 2")->getSceneWithName("4")->addObject(DrType::Object, asset_2, 250, -200);
    project->getWorldWithName("World 2")->getSceneWithName("4")->addObject(DrType::Object, asset_3, -200, 200);
    project->getWorldWithName("World 2")->getSceneWithName("4")->addObject(DrType::Object, asset_4, 100, 100);
    project->getWorldWithName("World 2")->getSceneWithName("4")->addObject(DrType::Object, asset_5, -150, 0);

    project->getWorldWithName("World 2")->getSceneWithName("2")->addObject(DrType::Object, asset_5, 100, 100);
    // !!!!! END



    // ########## Initialize form and customize colors and styles
    scene = new SceneGraphicsScene(this, project, this);
    buildMenu();
    buildWindow(Form_Main_Mode::Edit_Scene);
    Dr::ApplyColoring(this);



    // ########## Populates SceneGraphicsScene from current DrScene
    populateScene( project->getWorldWithName("World 2")->getSceneWithName("4")->getKey() );



    done_loading = true;
}


// Sets the new palette to the style sheets
void FormMain::changePalette(Color_Scheme new_color_scheme) {
    Dr::SetColorScheme(new_color_scheme);
    Dr::ApplyColoring(this);
    update();
}


//####################################################################################
//##        Interface Relay Handlers
//####################################################################################
void FormMain::buildAssetList() {
    treeAsset->buildAssetList();
}

// Sends new list to Object Inspector
void FormMain::buildObjectInspector(QList<long> key_list) {
    treeInspector->buildInspectorFromKeys(key_list);
}

void FormMain::buildTreeSceneList() {
    treeScene->populateTreeSceneList();
}


// Call to put in a signal to change the Advisor to the que
void FormMain::setAdvisorInfo(HeaderBodyList header_body_list)
{
    setAdvisorInfo(header_body_list[0], header_body_list[1]);
}

// Call to put in a signal to change the Advisor to the que
void FormMain::setAdvisorInfo(QString header, QString body)
{
    if (current_mode != Form_Main_Mode::Edit_Scene) return;
    if (advisor == nullptr) return;
    if (advisor->isHidden()) return;                                        // If Advisor dock was closed, cancel
    if (treeAdvisor == nullptr) return;
    if (treeAdvisor->getAdvisorHeader() == header) return;                  // If Advisor header is already set to proper info, cancel
    emit sendAdvisorInfo(header, body);                                     // Emits signal connected to changeAdvisor
}

// Sets the text of a label on FormMain
void FormMain::setLabelText(Label_Names label_name, QString new_text)
{
    if (!done_loading) return;

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

void FormMain::populateScene(long from_scene_key)
{
    // Emits an Undo stack command to change scenes
    if (scene->getCurrentSceneShown() != from_scene_key)
        emit newSceneSelected(project, scene, scene->getCurrentSceneShown(), from_scene_key);
}


void FormMain::updateSceneTreeSelectionBasedOnSelectionGroup()
{
    QList<QGraphicsItem*>   scene_list = scene->getSelectionGroupItems();
    QList<QTreeWidgetItem*> tree_list = treeScene->getListOfAllTreeWidgetItems();
    treeScene->clearSelection();

    long items_selected = 0;
    for (auto item : scene_list) {
        long item_key = dynamic_cast<DrItem*>(item)->getObjectKey();

        for (auto row : tree_list) {
            long row_key = row->data(0, User_Roles::Key).toLongLong();

            if (item_key == row_key) {
                row->setSelected(true);
                if (items_selected == 0)
                    treeScene->setSelectedKey(row_key);
                ++items_selected;
            }
        }
    }

    treeScene->update();
    Dr::SetLabelText(Label_Names::Label_1, "Scene: " + QString::number(scene_list.count()) + ", Tree: " + QString::number(tree_list.count()));
    Dr::SetLabelText(Label_Names::Label_2, "Matched: " + QString::number(items_selected));

}
















