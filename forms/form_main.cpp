//
//      Created by Stephens Nunnally on 12/7/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Loads, styles and handles events for FormMain
//
//
#include <QApplication>
#include <QDockWidget>
#include <QKeyEvent>
#include <QTimer>

#include "colors/colors.h"
#include "debug.h"
#include "enums.h"
#include "editor/editor_tree_advisor.h"
#include "editor/editor_tree_assets.h"
#include "editor/editor_tree_inspector.h"
#include "editor/editor_tree_project.h"
#include "editor_view/editor_item.h"
#include "editor_view/editor_scene.h"
#include "editor_view/editor_view.h"
#include "form_main.h"
#include "globals.h"
#include "helper.h"
#include "project/project.h"
#include "project/project_asset.h"
#include "project/project_world.h"
#include "project/project_world_stage.h"
#include "project/project_world_stage_thing.h"
#include "settings/settings.h"
#include "settings/settings_component.h"
#include "settings/settings_component_property.h"
#include "style/style.h"


//####################################################################################
//##    Constructor / Destructor for Main Window
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
    if (m_current_mode != Form_Main_Mode::World_Editor)   widgetCentralEditor->deleteLater();
    if (m_current_mode != Form_Main_Mode::Clear)          widgetCentral->deleteLater();
    delete m_project;
}

FormMain::FormMain(QWidget *parent) : QMainWindow(parent) {

    // ########## Initialize new project, load DrProject options
    m_project = new DrProject(1);

    m_project->setOption(Project_Options::Name, "Rocky Rover");
    m_project->setOption(Project_Options::Orientation, static_cast<int>(Orientation::Portrait));
    m_project->setOption(Project_Options::Width,   800);
    m_project->setOption(Project_Options::Height, 1600);



    // !!!!! #TEMP: Add stored project files
    long effect_1 =  m_project->addEffect("Light",          DrEffectType::Light);
    long effect_2 =  m_project->addEffect("Water",          DrEffectType::Water);
    long effect_3 =  m_project->addEffect("Fire",           DrEffectType::Fire);
    long effect_4 =  m_project->addEffect("Mirror",         DrEffectType::Mirror);
    long effect_5 =  m_project->addEffect("Fisheye Lens",   DrEffectType::Fisheye);
    long effect_6 =  m_project->addEffect("Swirl",          DrEffectType::Swirl);
//    long effect_7 =  m_project->addEffect("Flag",           DrEffectType::Flag);
//    long effect_8 =  m_project->addEffect("Rain",           DrEffectType::Rain);
//    long effect_9 =  m_project->addEffect("Snow",           DrEffectType::Snow);
//    long effect_10 = m_project->addEffect("Clouds",         DrEffectType::Clouds);
//    long effect_11 = m_project->addEffect("Fog",            DrEffectType::Fog);


    long image_1  = m_project->addImage(":/assets/test_images/test_square.png");
    long image_2  = m_project->addImage(":/assets/test_images/ground_fill.png");
    long image_3  = m_project->addImage(":/assets/test_images/ground_top.png");
    long image_4  = m_project->addImage(":/assets/test_images/moon_plant_6.png");
    long image_5  = m_project->addImage(":/assets/test_images/rover_body.png");
    long image_6  = m_project->addImage(":/assets/test_images/ball_1.png");
    long image_7  = m_project->addImage(":/assets/test_images/water_1.png");
    long image_8  = m_project->addImage(":/assets/test_images/rover_wheel.png");
    long image_9  = m_project->addImage(":/assets/test_images/cake_block.png");
    long image_10 = m_project->addImage(":/assets/test_images/cake_chocolate.png");
    long image_11 = m_project->addImage(":/assets/test_images/cake_ice_cube.png");
    long image_12 = m_project->addImage(":/assets/test_images/planetwithareallylongname.png");
    long image_13 = m_project->addImage(":/assets/test_images/metal_block.png");
    long image_14 = m_project->addImage(":/assets/test_images/donut.png");
    long image_15 = m_project->addImage(":/assets/test_images/shapes2.png");
    long image_16 = m_project->addImage(":/assets/test_images/dragon.png");

    long font_1 =   m_project->addFont("Distance Font", QPixmap(":/assets/test_images/test_font.png"),   "Arial",          20, true);
    long font_2 =   m_project->addFont("Coin Count",    QPixmap(":/assets/test_images/test_font_2.png"), "Britannic Bold", 15, true);
    long font_3 =   m_project->addFont("I Love Julie",  QPixmap(":/assets/test_images/test_font_3.png"), "Bauhaus 93",     36, true);



    // !!!!! #TEMP: Add assets
    m_project->addAsset(DrAssetType::Effect, effect_1);                             // Light
    m_project->addAsset(DrAssetType::Effect, effect_2);                             // Water
    m_project->addAsset(DrAssetType::Effect, effect_3);                             // Fire
    m_project->addAsset(DrAssetType::Effect, effect_4);                             // Mirror
    m_project->addAsset(DrAssetType::Effect, effect_5);                             // Fisheye
    m_project->addAsset(DrAssetType::Effect, effect_6);                             // Swirl
//    m_project->addAsset(DrAssetType::Effect, effect_7);                             // Flag
//    m_project->addAsset(DrAssetType::Effect, effect_8);                             // Rain
//    m_project->addAsset(DrAssetType::Effect, effect_9);                             // Snow
//    m_project->addAsset(DrAssetType::Effect, effect_10);                            // Clouds
//    m_project->addAsset(DrAssetType::Effect, effect_11);                            // Fog


                     m_project->addAsset(DrAssetType::Character, image_6 );         // "Ball 1"
    long a_dragon =  m_project->addAsset(DrAssetType::Character, image_16 );        // "Dragon"

    long a_square =  m_project->addAsset(DrAssetType::Object, image_1 );            // "Dr Square"
    long a_ground  = m_project->addAsset(DrAssetType::Object, image_2 );            // "Ground Fill"
    long a_top  =    m_project->addAsset(DrAssetType::Object, image_3 );            // "Ground Top"
    long a_plant  =  m_project->addAsset(DrAssetType::Object, image_4 );            // "Moon Plant 6"
    long a_block  =  m_project->addAsset(DrAssetType::Object, image_13 );           // "Metal Block"
    long a_planet =  m_project->addAsset(DrAssetType::Object, image_12 );           // "PlanetWithAReallyLongName"

    long a_cake1  =  m_project->addAsset(DrAssetType::Object, image_9 );            // "Cake Block"
    long a_cake2 =   m_project->addAsset(DrAssetType::Object, image_10 );           // "Cake Chocolate"
    long a_cake3 =   m_project->addAsset(DrAssetType::Object, image_11 );           // "Cake Ice Cube"

    m_project->addAsset(DrAssetType::Object, image_7 );                             // "Water 1"
    m_project->addAsset(DrAssetType::Object, image_5 );                             // "Rover Body"
    m_project->addAsset(DrAssetType::Object, image_8 );                             // "Rover Wheel"
    m_project->addAsset(DrAssetType::Object, image_14 );                            // "Donut"
    m_project->addAsset(DrAssetType::Object, image_15 );                            // "Shapes 2"

    long a_font =    m_project->addAsset(DrAssetType::Text, font_1);                // "Test Font"
    m_project->addAsset(DrAssetType::Text, font_2);                                 // "Test Font 2"
    m_project->addAsset(DrAssetType::Text, font_3);                                 // "Test Font 3"
    // !!!!! END



    // !!!!! #TEMP: New Project
    // Create a new project and add some stuff to it
    m_project->addWorld();
    m_project->addWorld();
    m_project->findWorldWithName("World 2")->addStage();                                    // Stage 2
    m_project->findWorldWithName("World 2")->addStage("asdfasdfasdfasdfasdfasdfasd");       // Stage 3
    m_project->findWorldWithName("World 2")->addStage();                                    // Stage 4



    // Stage 4 Rover Test
    long start_stage = m_project->findWorldWithName("World 2")->getFirstStageKey();
    m_project->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Character, a_dragon,  0,    60,   0);

    m_project->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_planet,   200,   600,  -1);
    m_project->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_planet,  1200,   475,  -1);

    m_project->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_plant,    900,    55,  -4, false);
    m_project->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_plant,   1400,    55,  -4, false);

    m_project->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_ground,  -411,  -200,  -3);
    m_project->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_ground,     0,  -200,  -3);
    m_project->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_ground,   411,  -200,  -3);
    m_project->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_ground,   822,  -200,  -3);
    m_project->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_ground,  1233,  -200,  -3);

    m_project->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_top,     -411,   -50,  -2);
    m_project->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_top,        0,   -50,  -2);
    m_project->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_top,      411,   -50,  -2);
    m_project->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_top,      822,   -50,  -2);
    m_project->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_top,     1233,   -50,  -2);

    m_project->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_block,    400,   200,  -3);
    m_project->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_block,    550,   200,  -3);
    m_project->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_block,    750,   350,  -3);

    // Stage 2 Misc Test
    m_project->findWorldWithName("World 2")->getStageWithName("2")->addThing(DrThingType::Object, a_square,   600,   700,  2);
    m_project->findWorldWithName("World 2")->getStageWithName("2")->addThing(DrThingType::Text,   a_font,     500,   900,  6);

    m_project->findWorldWithName("World 2")->getStageWithName("2")->addThing(DrThingType::Object, a_ground,   200,  -200,  -3);
    m_project->findWorldWithName("World 2")->getStageWithName("2")->addThing(DrThingType::Object, a_ground,   600,  -200,  -3);
    m_project->findWorldWithName("World 2")->getStageWithName("2")->addThing(DrThingType::Object, a_ground,  1000,  -200,  -3);

    // Stage 3 Ground
    m_project->findWorldWithName("World 2")->getStageWithName("asdfasdfasdfasdfasdfasdfasd")->addThing(DrThingType::Object, a_ground,   200,  -200,  -3);
    m_project->findWorldWithName("World 2")->getStageWithName("asdfasdfasdfasdfasdfasdfasd")->addThing(DrThingType::Object, a_ground,   600,  -200,  -3);
    m_project->findWorldWithName("World 2")->getStageWithName("asdfasdfasdfasdfasdfasdfasd")->addThing(DrThingType::Object, a_ground,  1000,  -200,  -3);

    // Stage 4 Iso Cake Test
    m_project->findWorldWithName("World 2")->getStageWithName("4")->addThing(DrThingType::Object, a_cake1,    200,   500,  2);
    m_project->findWorldWithName("World 2")->getStageWithName("4")->addThing(DrThingType::Object, a_cake2,    400,   800,  4);
    m_project->findWorldWithName("World 2")->getStageWithName("4")->addThing(DrThingType::Object, a_cake3,    600,  1100, -2);

    m_project->findWorldWithName("World 2")->getStageWithName("4")->addThing(DrThingType::Object, a_ground,   200,  -200,  -3);
    m_project->findWorldWithName("World 2")->getStageWithName("4")->addThing(DrThingType::Object, a_ground,   600,  -200,  -3);
    m_project->findWorldWithName("World 2")->getStageWithName("4")->addThing(DrThingType::Object, a_ground,  1000,  -200,  -3);

    DrStage* current_stage = m_project->findWorldWithName("World 2")->getStageFromKey(start_stage);
    m_project->setOption(Project_Options::Current_Stage, QVariant::fromValue(current_stage->getKey()) );
    m_project->setOption(Project_Options::Current_World, QVariant::fromValue(current_stage->getParentWorld()->getKey()) );

    // !!!!! END



    // ########## Initialize form and customize colors and styles
    this->setStyleSheet( Dr::CustomStyleSheetFormatting() );

    initializeFormMainSettings();
    buildMenu();
    buildToolBar();
    buildWidgetsShared();
    buildWidgetsEditor();
    initializeDockWidgets();


    setFormMainMode( Form_Main_Mode::World_Editor );

}



//####################################################################################
//##    Populates DrScene with passed in DrStage when Program is done loading
//####################################################################################
void FormMain::buildSceneAfterLoading(long stage_key) {
    if (Dr::CheckDoneLoading() == false) {
        QTimer::singleShot( 100, this, [this, stage_key] { this->buildSceneAfterLoading(stage_key); } );
        return;
    }
    buildScene( stage_key );
}


//####################################################################################
//##    Sets the new palette to the style sheets
//####################################################################################
void FormMain::changePalette(Color_Scheme new_color_scheme) {
    Dr::SetColorScheme(new_color_scheme);

    // When changing from Qt 5.12 to Qt 5.13.1 style sheet propagation seemed to stop working, tried the following things:
    ///     QCoreApplication::setAttribute(Qt::AA_UseStyleSheetPropagationInWidgetStyles, true);
    ///     this->setAttribute(Qt::WA_WindowPropagation, true);
    ///     this->ensurePolished();
    // but didn't work, so now we loop through all the children QWidgets and Polish / Update them...

    this->setStyleSheet( Dr::CustomStyleSheetFormatting() );

    for (auto widget : findChildren<QWidget *>()) {
        widget->style()->unpolish(widget);
        widget->style()->polish(widget);
        widget->update();
    }

    if (m_current_mode == Form_Main_Mode::World_Editor)
        viewEditor->updateGrid();
}



//####################################################################################
//##    FormMain Wide Event Filter, catches all events
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
        if (keyEvent->key() == Qt::Key::Key_Space && m_current_mode == Form_Main_Mode::World_Editor)
            if (viewEditor->hasFocus() == false)
                viewEditor->spaceBarDown();
    }
    if (event->type() == QEvent::KeyRelease)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key::Key_Space && m_current_mode == Form_Main_Mode::World_Editor)
            if (viewEditor->hasFocus() == false)
                viewEditor->spaceBarUp();
    }


    return QObject::eventFilter(obj, event);
}

// Overrides close event to make sure all rogue windows are closed
void FormMain::closeEvent(QCloseEvent *event) {
    qApp->closeAllWindows();
    event->accept();
}

// Overrides resize event to keep toolbar proper width
void FormMain::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
    widgetToolbar->setFixedWidth( this->width() );
}


//####################################################################################
//##    Sets the text of a label on FormMain, can be called globally from Dr::SetLabelText
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












