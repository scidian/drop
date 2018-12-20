//
//
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

#include "form_main.h"

// Destructor for Main Window
FormMain::~FormMain()
{
    delete project;
}

//####################################################################################
//##        Constructor for Main Window, called upon initialization
//####################################################################################
FormMain::FormMain(QWidget *parent, Globals *the_globals) : QMainWindow(parent), globals(the_globals)
{
    // Initialize form and customize colors and styles
    applyPalette(globals->current_color_scheme);
    buildWindow();
    applyColoring(globals->current_color_scheme);


    // Initialize new project, initialize local variables
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


    // Builds out scene list from current project data
    populateTreeSceneList();



    // TEMP:
    //      Test loading data out from a pair, i.e. "POINT2D", stored as QList<QVariant>
    QList<QVariant> myPoint = project->getWorld(current_world)->getComponentProperty(World_Components::physics, World_Properties::gravity)->getValue().toList();
    label_1->setText("Gravity X:" + myPoint.first().toString());
    label_2->setText("Gravity Y:" + myPoint.last().toString());


}










//####################################################################################
//
//  On object inspector click show info about object and property
//
//void FormMain::on_tableWidget_itemClicked(QTableWidgetItem *item)
//{
//    // If no item is selected in tree view, exit function
//    if (treeScene->getSelectedKey() == 0) { return; }

//    // First, retrieve property key of item clicked in tableWidget list
//    long        property_key = item->data(Qt::UserRole).toLongLong();

//    // Grab a pointer to the component list of the first selected item from treeScene (stored in selected_list)
//    DrSettings  *selected_item_settings = project->findSettingsFromKey( treeScene->getSelectedKey() );
//    DrComponent *clicked_component = selected_item_settings->findComponentFromPropertyKey(property_key);
//    DrProperty  *clicked_property = clicked_component->getProperty(property_key);

//    std::string property_name = clicked_property->getDisplayName();
//    std::string component_name = clicked_component->getDisplayName();
//    long        component_key = clicked_component->getComponentKey();

//    // Grab type of main selected item in selected tree list
//    std::string type_string2 = StringFromType(project->findTypeFromKey( treeScene->getSelectedKey() ));
//    std::string type_string = StringFromType(selected_item_settings->getType());

//    label_object->setText("KEY: " + QString::number( treeScene->getSelectedKey() ) + ", TYPE: " + QString::fromStdString(type_string));
//    label_object_2->setText("COMPONENT: " + QString::number(component_key) +   ", NAME: " + QString::fromStdString(component_name));
//    label_object_3->setText("PROPERTY: " + QString::number(property_key) +   ", NAME: " + QString::fromStdString(property_name));

//}








