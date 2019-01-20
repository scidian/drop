//
//      Created by Stephens Nunnally on 12/7/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Loads, styles and handles events for FormMain
//
//

#include "project.h"
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
FormMain::FormMain(QWidget *parent, Globals *the_globals) :
    QMainWindow(parent),
    globals(the_globals)
{

    // ########## Load saved preferences
    globals->debug_flags.set(static_cast<size_t>(Debug_Flags::Secret_Menu));
    //globals->debug_flags.set(static_cast<size_t>(Debug_Flags::FPS));
    //globals->debug_flags.set(static_cast<size_t>(Debug_Flags::Mouse_Coordinates));

    globals->debug_flags.set(static_cast<size_t>(Debug_Flags::Object_Inspector_Build));
    globals->debug_flags.set(static_cast<size_t>(Debug_Flags::Scene_Tree_Drag));
    globals->debug_flags.set(static_cast<size_t>(Debug_Flags::Scene_Tree_Selection));

    //globals->debug_flags.set(static_cast<size_t>(Debug_Flags::Rotation_Data));
    globals->debug_flags.set(static_cast<size_t>(Debug_Flags::Selection_Box_Group_Data));
    globals->debug_flags.set(static_cast<size_t>(Debug_Flags::Selected_Item_Data));
    globals->debug_flags.set(static_cast<size_t>(Debug_Flags::Shear_Matrix));



    globals->current_color_scheme = Color_Scheme::Dark;



    // !!!!! #TEMP: call to populate Graphics Scene (currently does chips)
    populateScene();
    // !!!!! END


    // ########## Initialize new project, initialize local variables
    project = new DrProject();
    current_world = 0;


    // !!!!! #TEMP: New Project
    // Create a new project and add some stuff to it
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
    // !!!!! END



    // ########## Initialize form and customize colors and styles
    buildMenu();
    buildWindow(Form_Main_Mode::Edit_Scene);
    applyColoring();




    done_loading = true;
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

// Returns if we should display debug code
bool FormMain::debugFlag(Debug_Flags option_to_check)
{
    return globals->debug_flags.test(static_cast<size_t>(option_to_check));
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

    scene->addSquare(-200, -200, 100, 100, 1, "Bob");

    scene->addSquare(0, 0, 100, 50, 2, "Joe");
    scene->addSquare(200, 0, 100, 50, 3, "Dan");

    scene->addSquare(100, 100, 1, 1, 6, "Jeff");

    scene->addSquare(0, 200, 100, 50, 4, "Ryan");
    scene->addSquare(200, 200, 100, 50, 5, "Kirk");

}


// Sets the new palette to the style sheets
void FormMain::changePalette(Color_Scheme new_color_scheme)
{
    globals->current_color_scheme = new_color_scheme;
    applyColoring();
    update();
}















