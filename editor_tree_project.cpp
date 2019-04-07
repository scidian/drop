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

    for (auto world_pair: m_project->getWorlds())
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
                    case DrObjectType::Object:    object_item->setIcon(0, QIcon(":/tree_icons/tree_object.png")); break;
                    case DrObjectType::Camera:    object_item->setIcon(0, QIcon(":/tree_icons/tree_camera.png")); break;
                    case DrObjectType::Character: object_item->setIcon(0, QIcon(":/tree_icons/tree_character.png")); break;
                    default: break;
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
//##        Updates selection
//##            Checks to make sure if more than one item is selected all new items
//##            not matching original type are not selected
//####################################################################################
void TreeProject::selectionChanged (const QItemSelection &selected, const QItemSelection &deselected)
{
    // If we're updating selection from outside source this function has been turned off from that source (i.e. updateSelection in IEditorRelay)
    if (!m_allow_selection_event) {
        QTreeWidget::selectionChanged(selected, deselected);
        return;
    }

    // ***** If size of list is zero, clear selected_key and exit function
    QList<QTreeWidgetItem*> item_list = this->selectedItems();
    if (item_list.size() == 0) {
        this->setSelectedKey(0);
        m_editor_relay->buildObjectInspector(QList<long> { });
        m_editor_relay->updateItemSelection(Editor_Widgets::Project_Tree);
        return;
    }

    // ***** If size is one, reset first selected item
    if (item_list.size() == 1) {
        long selected_key = item_list.first()->data(0, User_Roles::Key).toLongLong();       // grab stored key from list view user data
        this->setSelectedKey(selected_key);

        //******************************************************

        // Call to undo command to change scene to newly selected Stage or newly selected object's parent Stage
        DrSettings *selected_item = m_project->findSettingsFromKey(selected_key);
        if (selected_item != nullptr) {
            DrType selected_type = selected_item->getType();
            long change_to_key = c_no_key;
            if (selected_type == DrType::Stage || selected_type == DrType::StartStage) {
                change_to_key = selected_key;
            } else if (selected_type == DrType::Object) {
                DrObject *as_object = dynamic_cast<DrObject*>(selected_item);
                change_to_key = as_object->getParentStage()->getKey();
            }
            if (change_to_key != c_no_key) {
                m_editor_relay->updateItemSelection(Editor_Widgets::Project_Tree);          // selects none in scene before rebuilding scene
                m_editor_relay->buildScene( change_to_key );
            }
        }

        // Callsto outside update functions to rebuild object inspector
        m_editor_relay->buildObjectInspector(QList<long> { selected_key });

        //******************************************************

    // ***** Size is more than 1, prevent items of different types being selected at same time
    } else {
        DrType selected_type = m_project->findChildTypeFromKey( this->getSelectedKey() );

        // Check if newly selected items are same type, if not, do not allow select
        for (auto check_item: item_list) {
            // Get key from each item so we can compare it to first selected item
            long    check_key = check_item->data(0, User_Roles::Key).toLongLong();
            DrType  check_type = m_project->findChildTypeFromKey(check_key);

            // If we are over item that was first selected, skip to next
            if (check_key == this->getSelectedKey()) { continue; }

            if (check_type != selected_type)
                check_item->setSelected(false);
        }
    }

    // Call to outside update functions to update selection in scene view
    m_editor_relay->updateItemSelection(Editor_Widgets::Project_Tree);

    QTreeWidget::selectionChanged(selected, deselected);                    // pass event to parent
}


//####################################################################################
//##        Selects rows based on items selected in view
//####################################################################################
void TreeProject::updateSelectionFromView(QList<QGraphicsItem*> item_list)
{
    setAllowSelectionEvent(false);
    clearSelection();

    long items_selected = 0;
    for (auto item : item_list) {
        long item_key = item->data(User_Roles::Key).toLongLong();

        for (auto row : getListOfAllTreeWidgetItems()) {
            long row_key = row->data(0, User_Roles::Key).toLongLong();

            if (item_key == row_key) {
                row->setSelected(true);

                if (items_selected == 0) {
                    setSelectedKey(row_key);
                    ++items_selected;
                }
            }
        }
    }

    update();
    setAllowSelectionEvent(true);
}


//####################################################################################
//##        Updates item names if they have been changed
//####################################################################################
void TreeProject::updateItemNames(QList<DrSettings*> changed_items, QList<long> property_keys)
{
    setAllowSelectionEvent(false);

    for (auto item : changed_items) {
        long item_key = item->getKey();
        for (auto row : getListOfAllTreeWidgetItems()) {
            long row_key = row->data(0, User_Roles::Key).toLongLong();

            if (item_key == row_key) {
                for (auto property : property_keys) {
                    Properties check_property = static_cast<Properties>(property);

                    switch (check_property) {
                    case Properties::World_Name:
                        row->setText(0, "World: " + item->getComponentPropertyValue(Components::World_Settings, Properties::World_Name).toString() );
                        break;
                    case Properties::Stage_Name:
                        row->setText(0, "Stage: " + item->getComponentPropertyValue(Components::Stage_Settings, Properties::Stage_Name).toString() );
                        break;
                    case Properties::Object_Name:
                        row->setText(0, item->getComponentPropertyValue(Components::Object_Settings, Properties::Object_Name).toString() );
                        break;
                    default: ;
                    }
                }
            }
        }
    }
    update();
    setAllowSelectionEvent(true);
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









