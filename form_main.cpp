//
//      Created by Stephens Nunnally on 12/7/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Loads, styles and handles events for FormMain
//
//
#include <QApplication>
#include <QKeyEvent>
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

#include "globals.h"
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

    // ########## Initialize new project, load DrProject options
    project = new DrProject(1);

    project->setOption(Project_Options::Orientation, static_cast<int>(Orientation::Portrait));



    // !!!!! #TEMP: Add images / assets
    long image_1  = project->addImage(":/assets/test_square.png");
    long image_2  = project->addImage(":/assets/ground_fill.png");
    long image_3  = project->addImage(":/assets/ground_top.png");
    long image_4  = project->addImage(":/assets/moon_plant_6.png");
    long image_5  = project->addImage(":/assets/rover_body.png");
    long image_6  = project->addImage(":/assets/ball_1.png");
    long image_7  = project->addImage(":/assets/water_1.png");
    long image_8  = project->addImage(":/assets/rover_wheel.png");
    long image_9  = project->addImage(":/assets/cake_block.png");
    long image_10 = project->addImage(":/assets/cake_chocolate.png");
    long image_11 = project->addImage(":/assets/cake_ice_cube.png");

    long asset_1  = project->addAsset(DrAssetType::Object, image_1 );           // "Dr Square"
    long asset_2  = project->addAsset(DrAssetType::Object, image_2 );           // "Ground Fill"
    long asset_3  = project->addAsset(DrAssetType::Object, image_3 );           // "Ground Top"
    long asset_4  = project->addAsset(DrAssetType::Object, image_4 );           // "Moon Plant 6"
    long asset_5  = project->addAsset(DrAssetType::Object, image_5 );           // "Rover Body"
    long asset_6  = project->addAsset(DrAssetType::Object, image_6 );           // "Ball 1"
    long asset_7  = project->addAsset(DrAssetType::Object, image_7 );           // "Water 1"
    long asset_8  = project->addAsset(DrAssetType::Object, image_8 );           // "Rover Wheel"
    long asset_9  = project->addAsset(DrAssetType::Object, image_9 );           // "Cake Block"
    long asset_10 = project->addAsset(DrAssetType::Object, image_10 );          // "Cake Chocolate"
    long asset_11 = project->addAsset(DrAssetType::Object, image_11 );          // "Cake Ice Cube"
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

    project->getWorldWithName("World 2")->getStageWithName("4")->addObject(DrObjectType::Object, asset_1, 100, -800, 1);
    project->getWorldWithName("World 2")->getStageWithName("4")->addObject(DrObjectType::Object, asset_1, 600, -700, 2);
    project->getWorldWithName("World 2")->getStageWithName("4")->addObject(DrObjectType::Object, asset_2, 250, -300, 3);
    project->getWorldWithName("World 2")->getStageWithName("4")->addObject(DrObjectType::Object, asset_3, 300, -500, 10);
    project->getWorldWithName("World 2")->getStageWithName("4")->addObject(DrObjectType::Object, asset_4, 407, -600, 11);
    project->getWorldWithName("World 2")->getStageWithName("4")->addObject(DrObjectType::Object, asset_5, 600, -600, 30);

    project->getWorldWithName("World 2")->getStageWithName("2")->addObject(DrObjectType::Object, asset_5, 300, -700, -2);
    project->getWorldWithName("World 2")->getStageWithName("2")->addObject(DrObjectType::Object, asset_6, 200, -600,  4);
    project->getWorldWithName("World 2")->getStageWithName("2")->addObject(DrObjectType::Object, asset_7, 200, -300,  1);
    project->getWorldWithName("World 2")->getStageWithName("2")->addObject(DrObjectType::Object, asset_8, 400, -500,  2);

    project->getWorldWithName("World 2")->getStageWithName("5")->addObject(DrObjectType::Object, asset_9,  200,  -500,  2);
    project->getWorldWithName("World 2")->getStageWithName("5")->addObject(DrObjectType::Object, asset_10, 400,  -800,  4);
    project->getWorldWithName("World 2")->getStageWithName("5")->addObject(DrObjectType::Object, asset_11, 600, -1100, -2);
    // !!!!! END



    // ########## Initialize form and customize colors and styles
    scene = new DrScene(this, project, this);
    buildMenu();
    buildWindow( static_cast<Form_Main_Mode>(Dr::GetPreference(Preferences::Form_Main_Mode).toInt()) );
    Dr::ApplyColoring(this);


    // ########## Populates DrScene from current DrStage
    buildSceneAfterLoading( project->getWorldWithName("World 2")->getStageWithName("4")->getKey() );

}



//####################################################################################
//##        Populates DrScene with passed in DrStage when Program is done loading
//####################################################################################
void FormMain::buildSceneAfterLoading(long stage_key) {
    if (Dr::CheckDoneLoading() && this->isActiveWindow())
        buildScene( stage_key );
    else
        QTimer::singleShot( 200, this, [this, stage_key] { this->buildSceneAfterLoading(stage_key); } );
}


//####################################################################################
//##        Sets the new palette to the style sheets
//####################################################################################
void FormMain::changePalette(Color_Scheme new_color_scheme) {
    Dr::SetColorScheme(new_color_scheme);
    Dr::ApplyColoring(this);
    update();
}



//####################################################################################
//##        FormMain Wide Event Filter, catches all events
//####################################################################################
bool FormMain::eventFilter(QObject *obj, QEvent *event)
{
    //int t = event->type();
    //   1 = Timer
    //  11 = Leave            12 = Paint
    //  13 = Move             14 = Resize
    //  17 = Show             24 = Window Activate
    //  74 = Polish Request   78 = Update Later
    // 128 = Hover Leave     129 = Hover Move
    //
    //if (t != 1 && t != 11 && t != 12 && t != 13 && t != 14 && t != 17 && t != 24 && t != 74 && t != 78 && t != 128 && t != 129)
    //    Dr::SetLabelText(Label_Names::Label_2, QString::number(event->type()));


    // ********** Catch space bar for view to make sure we can drag even if view didnt have focus
    Form_Main_Mode mode = static_cast<Form_Main_Mode>(Dr::GetPreference(Preferences::Form_Main_Mode).toInt());

    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key::Key_Space && mode == Form_Main_Mode::World_Editor)
            if (viewMain->hasFocus() == false)
                viewMain->spaceBarDown();
    }
    if (event->type() == QEvent::KeyRelease)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key::Key_Space && mode == Form_Main_Mode::World_Editor)
            if (viewMain->hasFocus() == false)
                viewMain->spaceBarUp();
    }


    return QObject::eventFilter(obj, event);
}



//####################################################################################
//##        Sets the text of a label on FormMain, can be called globally from Dr::SetLabelText
//####################################################################################
void FormMain::setLabelText(Label_Names label_name, QString new_text)
{
    if (Dr::CheckDoneLoading() == false) return;

    switch (label_name) {
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












