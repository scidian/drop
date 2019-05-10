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

#include "colors/colors.h"
#include "debug.h"
#include "enums.h"
#include "editor/editor_item.h"
#include "editor/editor_scene.h"
#include "editor/editor_view.h"
#include "editor/editor_tree_advisor.h"
#include "editor/editor_tree_assets.h"
#include "editor/editor_tree_inspector.h"
#include "editor/editor_tree_project.h"
#include "form_main.h"
#include "globals.h"
#include "helper.h"
#include "project/project.h"
#include "project/project_asset.h"
#include "project/project_world.h"
#include "project/project_world_stage.h"
#include "project/project_world_stage_object.h"
#include "settings/settings.h"
#include "settings/settings_component.h"
#include "settings/settings_component_property.h"
#include "style/style.h"


//####################################################################################
//##        Constructor / Destructor for Main Window
//####################################################################################
FormMain::~FormMain() {
    // Wait until scene is not being changed, then delete view and scene
    qApp->blockSignals(true);
    while (sceneEditor->scene_mutex.tryLock() == false)
        qApp->processEvents();
    sceneEditor->clear();
    sceneEditor->scene_mutex.unlock();
    sceneEditor->deleteLater();

    // Delete widgets not currently attached to main form
    if (current_mode != Form_Main_Mode::World_Editor)   widgetCentralEditor->deleteLater();
    if (current_mode != Form_Main_Mode::Clear)          widgetCentral->deleteLater();
    delete project;
}

FormMain::FormMain(QWidget *parent) : QMainWindow(parent) {

    // ########## Initialize new project, load DrProject options
    project = new DrProject(1);

    project->setOption(Project_Options::Name, "Rocky Rover");
    project->setOption(Project_Options::Orientation, static_cast<int>(Orientation::Portrait));



    // !!!!! #TEMP: Add stored project files
    long image_1  = project->addImage(":/assets/test_images/test_square.png");
    long image_2  = project->addImage(":/assets/test_images/ground_fill.png");
    long image_3  = project->addImage(":/assets/test_images/ground_top.png");
    long image_4  = project->addImage(":/assets/test_images/moon_plant_6.png");
    long image_5  = project->addImage(":/assets/test_images/rover_body.png");
    long image_6  = project->addImage(":/assets/test_images/ball_1.png");
    long image_7  = project->addImage(":/assets/test_images/water_1.png");
    long image_8  = project->addImage(":/assets/test_images/rover_wheel.png");
    long image_9  = project->addImage(":/assets/test_images/cake_block.png");
    long image_10 = project->addImage(":/assets/test_images/cake_chocolate.png");
    long image_11 = project->addImage(":/assets/test_images/cake_ice_cube.png");
    long image_12 = project->addImage(":/assets/test_images/planetwithareallylongname.png");
    long image_13 = project->addImage(":/assets/test_images/metal_block.png");

    long font_1 =   project->addFont("Distance Font", QPixmap(":/assets/test_images/test_font.png"),   "Arial",          20, true);
    long font_2 =   project->addFont("Coin Count",    QPixmap(":/assets/test_images/test_font_2.png"), "Britannic Bold", 15, true);
    long font_3 =   project->addFont("I Love Julie",  QPixmap(":/assets/test_images/test_font_3.png"), "Bauhaus 93",     36, true);



    // !!!!! #TEMP: Add assets
    long a_ball   =  project->addAsset(DrAssetType::Character, image_6 );       // "Ball 1"
    long a_square =  project->addAsset(DrAssetType::Object, image_1 );          // "Dr Square"
    long a_ground  = project->addAsset(DrAssetType::Object, image_2 );          // "Ground Fill"
    long a_top  =    project->addAsset(DrAssetType::Object, image_3 );          // "Ground Top"
    long a_plant  =  project->addAsset(DrAssetType::Object, image_4 );          // "Moon Plant 6"
    long a_block  =  project->addAsset(DrAssetType::Object, image_13 );         // "Metal Block"
    long a_planet =  project->addAsset(DrAssetType::Object, image_12 );         // "PlanetWithAReallyLongName"
    long a_rover  =  project->addAsset(DrAssetType::Object, image_5 );          // "Rover Body"
    long a_water  =  project->addAsset(DrAssetType::Object, image_7 );          // "Water 1"
    long a_wheel  =  project->addAsset(DrAssetType::Object, image_8 );          // "Rover Wheel"
    long a_cake1  =  project->addAsset(DrAssetType::Object, image_9 );          // "Cake Block"
    long a_cake2 =   project->addAsset(DrAssetType::Object, image_10 );         // "Cake Chocolate"
    long a_cake3 =   project->addAsset(DrAssetType::Object, image_11 );         // "Cake Ice Cube"

    long a_font =    project->addAsset(DrAssetType::Text, font_1);          // "Test Font"
    project->addAsset(DrAssetType::Text, font_2);                           // "Test Font 2"
    project->addAsset(DrAssetType::Text, font_3);                           // "Test Font 3"
    // !!!!! END



    // !!!!! #TEMP: New Project
    // Create a new project and add some stuff to it
    project->addWorld();
    project->addWorld();
    project->findWorldWithName("World 2")->addStage();
    project->findWorldWithName("World 2")->addStage("asdfasdfasdfasdfasfdasdfasdfasdfasdfasdfasdfasdfasd");
    project->findWorldWithName("World 2")->addStage();
    project->findWorldWithName("World 2")->addStage();
    project->findWorldWithName("World 2")->addStage();



    // Stage 4 Rover Test
    project->findWorldWithName("World 2")->getStageWithName("4")->addObject(DrObjectType::Object, a_planet,  -800,   400,  1);
    project->findWorldWithName("World 2")->getStageWithName("4")->addObject(DrObjectType::Object, a_planet,   200,   600,  1);
    project->findWorldWithName("World 2")->getStageWithName("4")->addObject(DrObjectType::Object, a_planet,  1200,   475,  1);

    project->findWorldWithName("World 2")->getStageWithName("4")->addObject(DrObjectType::Object, a_plant,   -900,    55,  1, false);
    project->findWorldWithName("World 2")->getStageWithName("4")->addObject(DrObjectType::Object, a_plant,    900,    55,  1, false);
    project->findWorldWithName("World 2")->getStageWithName("4")->addObject(DrObjectType::Object, a_plant,   1400,    55,  1, false);

    project->findWorldWithName("World 2")->getStageWithName("4")->addObject(DrObjectType::Object, a_ground, -1233,  -200,  3);
    project->findWorldWithName("World 2")->getStageWithName("4")->addObject(DrObjectType::Object, a_ground,  -822,  -200,  3);
    project->findWorldWithName("World 2")->getStageWithName("4")->addObject(DrObjectType::Object, a_ground,  -411,  -200,  3);
    project->findWorldWithName("World 2")->getStageWithName("4")->addObject(DrObjectType::Object, a_ground,     0,  -200,  3);
    project->findWorldWithName("World 2")->getStageWithName("4")->addObject(DrObjectType::Object, a_ground,   411,  -200,  3);
    project->findWorldWithName("World 2")->getStageWithName("4")->addObject(DrObjectType::Object, a_ground,   822,  -200,  3);
    project->findWorldWithName("World 2")->getStageWithName("4")->addObject(DrObjectType::Object, a_ground,  1233,  -200,  3);

    project->findWorldWithName("World 2")->getStageWithName("4")->addObject(DrObjectType::Object, a_top,    -1233,   -50,  4);
    project->findWorldWithName("World 2")->getStageWithName("4")->addObject(DrObjectType::Object, a_top,     -822,   -50,  4);
    project->findWorldWithName("World 2")->getStageWithName("4")->addObject(DrObjectType::Object, a_top,     -411,   -50,  4);
    project->findWorldWithName("World 2")->getStageWithName("4")->addObject(DrObjectType::Object, a_top,        0,   -50,  4);
    project->findWorldWithName("World 2")->getStageWithName("4")->addObject(DrObjectType::Object, a_top,      411,   -50,  4);
    project->findWorldWithName("World 2")->getStageWithName("4")->addObject(DrObjectType::Object, a_top,      822,   -50,  4);
    project->findWorldWithName("World 2")->getStageWithName("4")->addObject(DrObjectType::Object, a_top,     1233,   -50,  4);

    project->findWorldWithName("World 2")->getStageWithName("4")->addObject(DrObjectType::Object, a_block,    400,   150,  3);
    project->findWorldWithName("World 2")->getStageWithName("4")->addObject(DrObjectType::Object, a_block,    525,   200,  3);
    project->findWorldWithName("World 2")->getStageWithName("4")->addObject(DrObjectType::Object, a_block,    750,   350,  3);

    // Stage 2 Misc Test
    project->findWorldWithName("World 2")->getStageWithName("2")->addObject(DrObjectType::Character, a_ball,  200,   600,  4);
    project->findWorldWithName("World 2")->getStageWithName("2")->addObject(DrObjectType::Object, a_square,   100,   800,  1);
    project->findWorldWithName("World 2")->getStageWithName("2")->addObject(DrObjectType::Object, a_square,   600,   700,  2);
    project->findWorldWithName("World 2")->getStageWithName("2")->addObject(DrObjectType::Object, a_rover,    300,   700, -2);
    project->findWorldWithName("World 2")->getStageWithName("2")->addObject(DrObjectType::Object, a_water,    200,   300,  1);
    project->findWorldWithName("World 2")->getStageWithName("2")->addObject(DrObjectType::Object, a_wheel,    400,   500,  2);
    project->findWorldWithName("World 2")->getStageWithName("2")->addObject(DrObjectType::Text,   a_font,     500,   900,  6);
    project->findWorldWithName("World 2")->getStageWithName("2")->addObject(DrObjectType::Object, a_ground,   711,   300,  3);

    // Stage 5 Iso Cake Test
    project->findWorldWithName("World 2")->getStageWithName("5")->addObject(DrObjectType::Object, a_cake1,    200,   500,  2);
    project->findWorldWithName("World 2")->getStageWithName("5")->addObject(DrObjectType::Object, a_cake2,    400,   800,  4);
    project->findWorldWithName("World 2")->getStageWithName("5")->addObject(DrObjectType::Object, a_cake3,    600,  1100, -2);

    project->setOption(Project_Options::Current_World, 0);
    project->setOption(Project_Options::Current_Stage, QVariant::fromValue(project->findWorldWithName("World 2")->getStageWithName("4")->getKey()) );
    // !!!!! END



    // ########## Initialize form and customize colors and styles
    Dr::ApplyCustomStyleSheetFormatting(this);

    initializeFormMainSettings();
    buildMenu();
    buildToolBar();
    buildWidgetsShared();
    buildWidgetsEditor();

    setFormMainMode( Form_Main_Mode::World_Editor );

}



//####################################################################################
//##        Populates DrScene with passed in DrStage when Program is done loading
//####################################################################################
void FormMain::buildSceneAfterLoading(long stage_key) {
    if (Dr::CheckDoneLoading() == false) {
        QTimer::singleShot( 100, this, [this, stage_key] { this->buildSceneAfterLoading(stage_key); } );
        return;
    }
    buildScene( stage_key );
}


//####################################################################################
//##        Sets the new palette to the style sheets
//####################################################################################
void FormMain::changePalette(Color_Scheme new_color_scheme) {
    Dr::SetColorScheme(new_color_scheme);
    Dr::ApplyCustomStyleSheetFormatting(this);

    if (current_mode == Form_Main_Mode::World_Editor)
        viewEditor->updateGrid();
}



//####################################################################################
//##        FormMain Wide Event Filter, catches all events
//####################################################################################
bool FormMain::eventFilter(QObject *obj, QEvent *event) {
    // ***** Some event numbers we know, we can use this to find more
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
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key::Key_Space && current_mode == Form_Main_Mode::World_Editor)
            if (viewEditor->hasFocus() == false)
                viewEditor->spaceBarDown();
    }
    if (event->type() == QEvent::KeyRelease)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key::Key_Space && current_mode == Form_Main_Mode::World_Editor)
            if (viewEditor->hasFocus() == false)
                viewEditor->spaceBarUp();
    }


    return QObject::eventFilter(obj, event);
}

// Overrides resize event to keep toolbar proper width
void FormMain::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
    widgetToolbar->setFixedWidth( this->width() );
}


//####################################################################################
//##        Sets the text of a label on FormMain, can be called globally from Dr::SetLabelText
//####################################################################################
void FormMain::setLabelText(Label_Names label_name, QString new_text) {
    if (Dr::CheckDoneLoading() == false) return;

    switch (label_name) {
        case Label_Names::Label_1:          label1->setText(new_text);         break;
        case Label_Names::Label_2:          label2->setText(new_text);         break;
        case Label_Names::Label_3:          label3->setText(new_text);         break;
        case Label_Names::Label_Mouse_1:    labelMouse1->setText(new_text);   break;
        case Label_Names::Label_Mouse_2:    labelMouse2->setText(new_text);   break;

        case Label_Names::Label_Object_1:   labelObject1->setText(new_text);  break;
        case Label_Names::Label_Object_2:   labelObject2->setText(new_text);  break;
        case Label_Names::Label_Object_3:   labelObject3->setText(new_text);  break;
        case Label_Names::Label_Object_4:   labelObject4->setText(new_text);  break;
        case Label_Names::Label_Object_5:   labelObject5->setText(new_text);  break;

        case Label_Names::Label_Position:   labelPosition->setText(new_text);  break;
        case Label_Names::Label_Center:     labelCenter->setText(new_text);    break;
        case Label_Names::Label_Scale:      labelScale->setText(new_text);     break;
        case Label_Names::Label_Rotate:     labelRotate->setText(new_text);    break;
        case Label_Names::Label_Z_Order:    labelZOrder->setText(new_text);   break;
        case Label_Names::Label_Pos_Flag:   labelPosFlag->setText(new_text);  break;

        case Label_Names::Label_Bottom:     labelBottom->setText(new_text);    break;
    }
}












