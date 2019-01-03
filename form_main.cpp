//
//      Created by Stephens Nunnally on 12/7/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Loads, styles and handles events for FormMain
//
//

#include "01_project.h"
#include "02_world.h"
#include "03_scene.h"
#include "04_object.h"

#include "30_settings.h"
#include "31_component.h"
#include "32_property.h"

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
FormMain::FormMain(QWidget *parent, Globals *the_globals) :
    QMainWindow(parent),
    globals(the_globals)
{

    // ########## Load saved preferences
    globals->show_debug = true;
    globals->current_color_scheme = Color_Scheme::Dark;



    // TEMP: temp call to populate Graphics Scene (currently does chips)
    populateScene();


    // ########## Initialize new project, initialize local variables
    project = new DrProject();
    current_world = 0;

    // TEMP NEW PROJECT:
    //      Create a new project and add some stuff to it
    project->addWorld();
    project->getWorldWithName("World 2")->addScene();
    project->getWorldWithName("World 2")->addScene("asdfasdfasdfasdfasfdasdfasdfasdfasdfasdfasdfasdfasd");
    project->getWorldWithName("World 2")->addScene();
    project->getWorldWithName("World 2")->addScene();
    project->getWorldWithName("World 2")->addScene();
    project->getWorldWithName("World 2")->getSceneWithName("4")->addObject(DrTypes::Object);
    project->getWorldWithName("World 2")->getSceneWithName("4")->addObject(DrTypes::Object);
    project->addWorld();
    project->addWorld();




    // ########## Initialize form and customize colors and styles
    buildMenu();
    buildWindow(Form_Main_Mode::Edit_Scene);
    applyColoring();








    // TEMP:
    //      Test loading data out from a pair, i.e. "POINT2D", stored as QList<QVariant>
    if (current_mode == Form_Main_Mode::Edit_Scene) {
        QList<QVariant> myPoint = project->getWorld(current_world)->getComponentProperty(World_Components::physics, World_Properties::gravity)->getValue().toList();
        setLabelText(Label_Names::Label1, "Gravity X:" + myPoint.first().toString());
        setLabelText(Label_Names::Label2, "Gravity Y:" + myPoint.last().toString());
    }

}



// Sends new list to Object Inspector
void FormMain::buildObjectInspector(QList<long> key_list)
{
    treeInspector->buildInspectorFromKeys(key_list);
}


void FormMain::buildTreeSceneList()
{
    treeScene->populateTreeSceneList();
}

QColor FormMain::getColor(Window_Colors color_role)
{
    return globals->getColor(color_role);
}

// Call to put in a signal to change the Advisor to the que
void FormMain::setAdvisorInfo(HeaderBodyList header_body_list)
{
    if (current_mode != Form_Main_Mode::Edit_Scene) return;
    if (advisor == nullptr) return;
    if (advisor->isHidden()) return;                                        // If Advisor dock was closed, cancel
    if (treeAdvisor == nullptr) return;
    if (treeAdvisor->getAdvisorHeader() == header_body_list[0]) return;     // If Advisor header is already set to proper info, cancel
    emit sendAdvisorInfo(header_body_list);                                 // Emits signal connected to changeAdvisor
}

// Sets the text of a label on FormMain
void FormMain::setLabelText(Label_Names label_name, QString new_text)
{
    switch (label_name)
    {
    case Label_Names::Label1:       label_1->setText(new_text);         break;
    case Label_Names::Label2:       label_2->setText(new_text);         break;
    case Label_Names::Label3:       label_3->setText(new_text);         break;
    case Label_Names::LabelObject1: label_object_1->setText(new_text);  break;
    case Label_Names::LabelObject2: label_object_2->setText(new_text);  break;
    case Label_Names::LabelObject3: label_object_3->setText(new_text);  break;
    }
}

// Pops up message box from Globals
void FormMain::showMessageBox(QString message)
{
    globals->showMessageBox(message);
}


void FormMain::populateScene()
{
    scene = new SceneGraphicsScene(this, project, this);

    // Populate scene
    scene->addSquare(0, 0);
    scene->addSquare(200, 0);
    scene->addSquare(0, 200);
    scene->addSquare(200, 200);
}



// Sets the new palette to the style sheets
void FormMain::changePalette(Color_Scheme new_color_scheme)
{
    globals->current_color_scheme = new_color_scheme;
    applyColoring();
    refreshMainView();
}
// After many tried update calls, force view to redraw by a quick zoom in / out
void FormMain::refreshMainView()
{
    if (viewMain != nullptr) {
        viewMain->zoomInOut(1);
        viewMain->zoomInOut(-1);
    }
}















