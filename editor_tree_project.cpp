//
//      Created by Stephens Nunnally on 1/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Tree Project List Definitions
//
//
#include <QCheckBox>
#include <QPainter>

#include "colors.h"
#include "debug.h"
#include "editor_tree_project.h"
#include "globals.h"
#include "interface_editor_relay.h"
#include "library.h"
#include "project.h"
#include "project_world.h"
#include "project_world_stage.h"
#include "project_world_stage_object.h"
#include "settings.h"
#include "settings_component.h"
#include "settings_component_property.h"


//####################################################################################
//##        Populates Tree Project List based on project data
//####################################################################################
void TreeProject::buildProjectTree()
{
    setAllowSelectionEvent(false);
    this->clear();

    for (auto world_pair: m_project->getWorldMap())
    {
        QTreeWidgetItem *world_item = new QTreeWidgetItem(this);                                            // Create new item (top level item)

        world_item->setIcon(0, QIcon(":/tree_icons/tree_world.png"));                                       // Loads icon for world
        world_item->setText(0, "World: " + world_pair.second->getComponentPropertyValue(
                               Components::World_Settings, Properties::World_Name).toString());             // Set text for item
        world_item->setData(0, User_Roles::Key, QVariant::fromValue(world_pair.second->getKey()));
        this->addTopLevelItem(world_item);                                                                  // Add it on our tree as the top item.

        for (auto stage_pair: world_pair.second->getStageMap())
        {
            QTreeWidgetItem *stage_item = new QTreeWidgetItem(world_item);                                  // Create new item and add as child item
            stage_item->setIcon(0, QIcon(":/tree_icons/tree_stage.png"));                                   // Loads icon for stage
            stage_item->setText(0, "Stage: " + stage_pair.second->getComponentPropertyValue(
                                   Components::Stage_Settings, Properties::Stage_Name).toString());         // Set text for item
            stage_item->setData(0, User_Roles::Key, QVariant::fromValue(stage_pair.second->getKey()));


            // ***** Iterates through objects based on z-order of each object
            ObjectMap   objects = stage_pair.second->getObjectMap();
            QList<long> keys = stage_pair.second->objectKeysSortedByZOrder();
            for (auto key: keys)
            {
                DrObject *object = objects[key];

                QTreeWidgetItem *object_item = new QTreeWidgetItem(stage_item);                             // Create new item and add as child item
                switch (object->getObjectType()) {
                    case DrObjectType::Object:    object_item->setIcon(0, QIcon(":/tree_icons/tree_object.png"));       break;
                    case DrObjectType::Text:      object_item->setIcon(0, QIcon(":/tree_icons/tree_text.png"));         break;
                    case DrObjectType::Character: object_item->setIcon(0, QIcon(":/tree_icons/tree_character.png"));    break;

                    ///case DrObjectType::Camera:    object_item->setIcon(0, QIcon(":/tree_icons/tree_camera.png"));       break;
                }

                object_item->setText(0, object->getComponentPropertyValue(
                                        Components::Object_Settings, Properties::Object_Name).toString());          // Set text for item
                object_item->setData(0, User_Roles::Key, QVariant::fromValue(object->getKey()));            // Store item key in user data

                stage_item->addChild(object_item);

                // Add lock box
                QString check_images = QString(" QCheckBox::indicator { width: 12px; height: 12px; }"
                                               " QCheckBox::indicator:checked { image: url(:/tree_icons/tree_lock.png); }"
                                               " QCheckBox::indicator:unchecked { image: url(:/tree_icons/tree_bullet.png); }");
                QCheckBox *lock_item = new QCheckBox();
                lock_item->setFocusPolicy(Qt::FocusPolicy::NoFocus);
                lock_item->setStyleSheet(check_images);
                this->setItemWidget(object_item, 1, lock_item);

            }
        }
    }
    this->expandAll();                                             // Expand all items
    this->update();
    setAllowSelectionEvent(true);
}

// Handles changing the Advisor on Mouse Enter
void TreeProject::enterEvent(QEvent *event)
{
    m_editor_relay->setAdvisorInfo(Advisor_Info::Project_Tree);
    QTreeWidget::enterEvent(event);
}



//####################################################################################
//##        Returns a list of the items contained within the tree
//####################################################################################
QList <QTreeWidgetItem*> TreeProject::getListOfAllTreeWidgetItems() {
    return getListOfChildrenFromItem( this->invisibleRootItem() );
}

QList <QTreeWidgetItem*> TreeProject::getListOfChildrenFromItem( QTreeWidgetItem *item )
{
    QList <QTreeWidgetItem*> items;

    for( int i = 0; i < item->childCount(); ++i ) {
        items.append( item->child(i) );
        items.append( getListOfChildrenFromItem( item->child(i) ) );
    }

    return items;
}









