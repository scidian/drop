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

    populateScene();




    // Initialize form and customize colors and styles
    buildWindow();
    applyColoring();


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
    setLabelText(Label_Names::Label1, "Gravity X:" + myPoint.first().toString());
    setLabelText(Label_Names::Label2, "Gravity Y:" + myPoint.last().toString());


}

// Sets the Advisor Dock text
void FormMain::setAdvisorInfo(HeaderBodyList header_body_list)
{
    setAdvisorInfo(QString::fromStdString(header_body_list[0]), QString::fromStdString(header_body_list[1]));
}
void FormMain::setAdvisorInfo(QString header_text, QString body_text)
{
    // Clear advisor tree
    treeAdvisor->clear();

    // Insert top level item to act as header
    QTreeWidgetItem *topLevelItem = new QTreeWidgetItem(treeAdvisor);
    topLevelItem->setText(0, header_text);
    treeAdvisor->addTopLevelItem(topLevelItem);

    // Create child tree item for body
    QTreeWidgetItem *sub_item = new QTreeWidgetItem(topLevelItem);
    topLevelItem->addChild(sub_item);

    // Create a label to display body text and format
    QLabel *body_label = new QLabel(body_text);
    QFont font_label;
    font_label.setPointSize(11);
    body_label->setFont(font_label);
    body_label->setWordWrap(true);
    body_label->setAlignment(Qt::AlignTop);
    body_label->setStyleSheet("QLabel { color : " + globals->getColor(Window_Colors::Text).name() + "; } ");

    // Apply label to tree, expand all
    treeAdvisor->setItemWidget(sub_item, 0, body_label);
    treeAdvisor->expandAll();
}

// Sets the text of a label on FormMain
void FormMain::setLabelText(Label_Names label_name, QString new_text)
{
    switch (label_name)
    {
    case Label_Names::Label1:       label_1->setText(new_text);         break;
    case Label_Names::Label2:       label_2->setText(new_text);         break;
    case Label_Names::Label3:       label_3->setText(new_text);         break;
    case Label_Names::LabelObject1: label_object_1->setText(new_text);    break;
    case Label_Names::LabelObject2: label_object_2->setText(new_text);  break;
    case Label_Names::LabelObject3: label_object_3->setText(new_text);  break;
    }
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

//    setLabelText(Label_Names::LabelObject1, "KEY: " + QString::number( treeScene->getSelectedKey() ) + ", TYPE: " + QString::fromStdString(type_string));
//    setLabelText(Label_Names::LabelObject2, "COMPONENT: " + QString::number(component_key) +   ", NAME: " + QString::fromStdString(component_name));
//    setLabelText(Label_Names::LabelObject3, "PROPERTY: " + QString::number(property_key) +   ", NAME: " + QString::fromStdString(property_name));

//}








