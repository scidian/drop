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
FormMain::FormMain(QWidget *parent, Globals *the_globals) :
    QMainWindow(parent),
    globals(the_globals)
{
    // Fires signal that is picked up by Advisor to change the help info
    connect(this, SIGNAL(sendAdvisorInfo(HeaderBodyList)), this, SLOT(changeAdvisor(HeaderBodyList)) , Qt::QueuedConnection);


    // ########## Load saved preferences
    globals->current_color_scheme = Color_Scheme::Dark;


    // TEMP: temp call to populate Graphics Scene
    populateScene();


    // ########## Initialize form and customize colors and styles
    buildWindow();
    applyColoring();


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


    // Builds out scene list from current project data
    populateTreeSceneList();









    // TEMP:
    //      Test loading data out from a pair, i.e. "POINT2D", stored as QList<QVariant>
    QList<QVariant> myPoint = project->getWorld(current_world)->getComponentProperty(World_Components::physics, World_Properties::gravity)->getValue().toList();
    setLabelText(Label_Names::Label1, "Gravity X:" + myPoint.first().toString());
    setLabelText(Label_Names::Label2, "Gravity Y:" + myPoint.last().toString());


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
    //scene->invalidate(scene->sceneRect(), QGraphicsScene::BackgroundLayer);
    //scene->update();
    //this->style()->unpolish(this);
    //this->style()->polish(this);
    //viewMain->update();
    //viewMain->repaint();
    //qApp->processEvents();
    viewMain->zoomInOut(1);
    viewMain->zoomInOut(-1);
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
//    long        property_key = item->data(User_Roles::Key).toLongLong();

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

//    setLabelText(Label_Names::LabelObject1, "KEY: " + QString::number( treeScene->getSelectedKey() ) + ", TYPE: " + QString::fromStdString(type_string));
//    setLabelText(Label_Names::LabelObject2, "COMPONENT: " + QString::number(component_key) +   ", NAME: " + QString::fromStdString(component_name));
//    setLabelText(Label_Names::LabelObject3, "PROPERTY: " + QString::number(property_key) +   ", NAME: " + QString::fromStdString(property_name));

//}








