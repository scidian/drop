//
//      Created by Stephens Nunnally on 1/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Tree Project List Definitions
//
//
#include <QCheckBox>
#include <QDebug>
#include <QKeyEvent>
#include <QPainter>
#include <QScrollBar>

#include "colors/colors.h"
#include "debug.h"
#include "editor/tree_project.h"
#include "editor_view/editor_view.h"
#include "globals.h"
#include "helper.h"
#include "helper_qt.h"
#include "imaging/imaging.h"
#include "interface_editor_relay.h"
#include "project/project.h"
#include "project/project_world.h"
#include "project/project_world_stage.h"
#include "project/project_world_stage_thing.h"
#include "settings/settings.h"
#include "settings/settings_component.h"
#include "settings/settings_component_property.h"


//####################################################################################
//##    Populates Tree Project List based on project data
//####################################################################################
void TreeProject::buildProjectTree() {

    // ********** Turn off selection event during build
    setAllowSelectionEvent(false);

    // ********** Store some data about the tree so we can restore after its rebuilt
    QMap<long, QTreeWidgetItem*> item_map;
    QList<QTreeWidgetItem*>      item_list = getListOfAllTreeWidgetItems();
    QTreeWidgetItem             *last_added = nullptr;

    // ***** Gets list of items in tree, if keys have been removed from project, removes items
    for (auto item : getListOfAllTreeWidgetItems() ) {
        // Check if item source key is still in project, if so add to map
        long key = item->data(0, User_Roles::Key).toLongLong();
        DrSettings *settings = m_project->findSettingsFromKey(key, false);
        if (settings != nullptr) {
            item_map[key] = item;
            continue;
        }

        // If we made it here, remove item from tree
        int index = this->indexOfTopLevelItem(item);
        if (index > -1) {
            this->takeTopLevelItem(index);
        } else {
            QTreeWidgetItem *parent = item->parent();
            if (parent) parent->takeChild(parent->indexOfChild(item));
        }
    }


    // ********** Go through Project and add items as necessary
    QColor icon_color = Dr::GetColor(Window_Colors::Icon_Dark);
    QImage icon_image;

    for (auto world_pair: m_project->getWorldMap()) {

        // ***** Create new Item for World if necessary
        DrWorld         *world =        world_pair.second;
        long             world_key =    world_pair.first;
        QTreeWidgetItem *world_item =   item_map[world_key];
        if (world_item == nullptr) {
            world_item = new QTreeWidgetItem(this);
            last_added = world_item;
            icon_image = QPixmap(":/assets/tree_icons/tree_world.png").toImage();
            world_item->setIcon(0, QIcon(QPixmap::fromImage(DrImaging::colorizeImage(icon_image, icon_color))));
            world_item->setText(0, "World: " + world->getName());
            world_item->setData(0, User_Roles::Key, QVariant::fromValue(world->getKey()));
            world_item->setExpanded(true);
            this->addTopLevelItem(world_item);
        }

        for (auto stage_pair: world->getStageMap()) {

            // ***** Create new Item for Stage if necessary
            DrStage         *stage =        stage_pair.second;
            long             stage_key =    stage_pair.first;
            QTreeWidgetItem *stage_item =   item_map[stage_key];
            if (stage_item == nullptr) {
                stage_item = new QTreeWidgetItem(world_item);
                last_added = stage_item;
                icon_image = QPixmap(":/assets/tree_icons/tree_stage.png").toImage();
                stage_item->setIcon(0, QIcon(QPixmap::fromImage(DrImaging::colorizeImage(icon_image, icon_color))));
                stage_item->setText(0, "Stage: " + stage->getName());
                stage_item->setData(0, User_Roles::Key, QVariant::fromValue(stage->getKey()));
                stage_item->setExpanded(true);
            }

            // ***** Iterates through Things based on z-order of each Thing
            ThingMap &things = stage->getThingMap();
            QList<long> keys = stage->thingKeysSortedByZOrder();

            for (auto key: keys) {
                DrThing         *thing =        things[key];
                            if (!thing) continue;
                long             thing_key =    thing->getKey();
                QTreeWidgetItem *thing_item =   item_map[thing_key];

                // ***** Create new Item for Thing if necessary
                if (thing_item == nullptr) {
                    thing_item = new QTreeWidgetItem(stage_item);
                    last_added = thing_item;
                    switch (thing->getThingType()) {
                        case DrThingType::None:      icon_image = QImage();                                                             break;
                        case DrThingType::Character: icon_image = QPixmap(":/assets/tree_icons/tree_character.png").toImage();          break;
                        case DrThingType::Object:    icon_image = QPixmap(":/assets/tree_icons/tree_object.png").toImage();             break;
                        case DrThingType::Text:      icon_image = QPixmap(":/assets/tree_icons/tree_text.png").toImage();               break;
                        case DrThingType::Fire:      icon_image = QPixmap(":/assets/tree_icons/tree_fire.png").toImage();               break;
                        case DrThingType::Fisheye:   icon_image = QPixmap(":/assets/tree_icons/tree_fisheye.png").toImage();            break;
                        case DrThingType::Light:     icon_image = QPixmap(":/assets/tree_icons/tree_light.png").toImage();              break;
                        case DrThingType::Mirror:    icon_image = QPixmap(":/assets/tree_icons/tree_mirror.png").toImage();             break;
                        case DrThingType::Swirl:     icon_image = QPixmap(":/assets/tree_icons/tree_swirl.png").toImage();              break;
                        case DrThingType::Water:     icon_image = QPixmap(":/assets/tree_icons/tree_water.png").toImage();              break;
                    }
                    thing_item->setIcon(0, QIcon(QPixmap::fromImage(DrImaging::colorizeImage(icon_image, icon_color))));
                    thing_item->setText(0, thing->getName());
                    thing_item->setData(0, User_Roles::Key, QVariant::fromValue(thing->getKey()));

                }

                // ***** Install / update lock box
                installLockBox(thing, thing_item);

                // ***** Hide / Unhide as necessary
                bool should_hide = false;
                if (thing->getComponentPropertyValue(Components::Hidden_Settings, Properties::Hidden_Hide_From_Trees).toBool()) {
                    if (Dr::CheckDebugFlag(Debug_Flags::Show_Hidden_Component) == false) should_hide = true;
                }
                if (should_hide) thing_item->setHidden(should_hide);
            }
        }
    }

    // ***** If we added new item(s), make sure visible
    if (last_added != nullptr) {
        this->scrollToItem(last_added);
        ///this->verticalScrollBar()->setValue(scroll_position);
    }

    // ***** Update and allow selection again
    this->update();
    setAllowSelectionEvent(true);
}

// Handles changing the Advisor on Mouse Enter
void TreeProject::enterEvent(QEvent *event) {
    m_editor_relay->setAdvisorInfo(Advisor_Info::Project_Tree);
    QTreeWidget::enterEvent(event);
}

// Sets this as the Active Editor widget
void TreeProject::focusInEvent(QFocusEvent *event) {
    m_editor_relay->setActiveWidget(Editor_Widgets::Project_Tree);
    QTreeWidget::focusInEvent(event);
}



//####################################################################################
//##    Returns a list of the items contained within the tree
//####################################################################################
QList<QTreeWidgetItem*> TreeProject::getListOfAllTreeWidgetItems() {
    return getListOfChildrenFromItem( this->invisibleRootItem() );
}

QList<QTreeWidgetItem*> TreeProject::getListOfChildrenFromItem( QTreeWidgetItem *item ) {
    QList <QTreeWidgetItem*> items;

    for (int i = 0; i < item->childCount(); ++i) {
        items.append( item->child(i) );
        items.append( getListOfChildrenFromItem( item->child(i) ) );
    }

    return items;
}


//####################################################################################
//##    Handles click on little Lock Button
//####################################################################################
void TreeProject::processLockClick(QCheckBox *from_lock) {
    // Check if currently locked
    bool locked = (from_lock->checkState() == Qt::CheckState::Checked) ? true : false;

    // Find entity key attached to lock box
    long lock_key = from_lock->property(User_Property::Key).toLongLong();

    // See if item is selected
    QList<QTreeWidgetItem*> item_list = this->selectedItems();
    bool selected = false;
    for (auto item : item_list) {
        if (lock_key == item->data(0, User_Roles::Key).toLongLong()) {                  // grab stored key from QTreeWidgetItem user data
            selected = true;
            break;
        }
    }

    // Make a list of locks to process, either just the one, or all items that are selected
    QList<QCheckBox*> locks;
    if (selected == false) {
        locks.append(from_lock);
    } else {
        for (auto item : item_list) {
            QCheckBox *check_box = dynamic_cast<QCheckBox*>( this->itemWidget(item, COLUMN_LOCK) );
            if (check_box) locks.append(check_box);
        }
    }

    // Process locks
    for (auto lock : locks) {
        long key = lock->property(User_Property::Key).toLongLong();
        DrSettings *entity = m_project->findSettingsFromKey( key );                     // grab stored key from QCheckBox property
        if (!entity) continue;

        if (entity->getComponentPropertyValue(Components::Hidden_Settings, Properties::Hidden_Item_Locked).toBool()) {
            lock->setCheckState( Qt::CheckState::Checked );
        } else {
            entity->setLocked( locked );
        }
        lock->setCheckState( entity->isLocked() ? Qt::CheckState::Checked : Qt::CheckState::Unchecked );
    }

    // Update Object Inspector Properties and Update StageView selected items
    this->m_editor_relay->updateInspectorEnabledProperties();
    this->m_editor_relay->updateItemSelection(Editor_Widgets::Project_Tree);
}





















