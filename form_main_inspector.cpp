//
//
//      Handles dealing with the Object Inspector
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


//####################################################################################
//
//  Need to finish dynamically building object inspector
//
void FormMain::buildObjectInspector()
{
    // First, retrieve unique key of item clicked in list
    long        selected_key = treeScene->getSelectedKey();
    DrTypes     selected_type = project->findTypeFromKey( selected_key );
    std::string type_string = StringFromType(selected_type);
    label_object->setText("KEY: " + QString::number( selected_key ) + ", TYPE: " + QString::fromStdString(type_string));
    label_object_2->setText("");
    label_object_3->setText("");

    ComponentMap components = project->findSettingsFromKey( selected_key )->getComponentList();


    // Inserts properties into Object Inspector Tree
    int rowCount = 0;
    treeObject->clear();
    for (auto i: components)
    {
        QTreeWidgetItem *topLevelItem = new QTreeWidgetItem(treeObject);                             // Create new item (top level item)
        //topLevelItem->setTextColor(0, i.second->getColor());

        topLevelItem->setBackgroundColor(0, i.second->getColor());

        topLevelItem->setFirstColumnSpanned(true);
        topLevelItem->setText(0, i.second->getDisplayNameQString());
        topLevelItem->setData(0, Qt::UserRole, QVariant::fromValue(i.second->getComponentKey()));    // Stores component key in list iser data
        treeObject->addTopLevelItem(topLevelItem);                                                   // Add it on our tree as the top item.


        for (auto j: i.second->getPropertyList())
        {
            QTreeWidgetItem *sub_item = new QTreeWidgetItem(topLevelItem);
            //sub_item->setIcon(0, QIcon(":/tree_icons/tree_object.png"));
            sub_item->setText(0, j.second->getDisplayNameQString());
            //sub_item->setData(0, Qt::UserRole, QVariant::fromValue(object_pair.second->getKey()));

            rowCount++;
        }


        //treeScene->setItemWidget(sub_item, 0, propTable);

    }

    treeObject->expandAll();
}











