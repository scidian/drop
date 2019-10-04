//
//      Created by Stephens Nunnally on 1/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Tree Project List Definitions
//
//
#include <QCheckBox>
#include <QPainter>

#include "colors/colors.h"
#include "debug.h"
#include "editor_tree_project.h"
#include "globals.h"
#include "interface_editor_relay.h"
#include "helper.h"
#include "helper_qt.h"
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

    setAllowSelectionEvent(false);
    this->clear();

    for (auto world_pair: m_project->getWorldMap()) {

        QTreeWidgetItem *world_item = new QTreeWidgetItem(this);                                            // Create new item (top level item)

        DrWorld *world = world_pair.second;
        world_item->setIcon(0, QIcon(":/assets/tree_icons/tree_world.png"));                                // Loads icon for world
        world_item->setText(0, "World: " + world->getName());                                               // Set text for item
        world_item->setData(0, User_Roles::Key, QVariant::fromValue(world->getKey()));
        this->addTopLevelItem(world_item);                                                                  // Add it on our tree as the top item.

        for (auto stage_pair: world->getStageMap()) {

            QTreeWidgetItem *stage_item = new QTreeWidgetItem(world_item);                                  // Create new item and add as child item

            DrStage *stage = stage_pair.second;
            stage_item->setIcon(0, QIcon(":/assets/tree_icons/tree_stage.png"));                            // Loads icon for stage
            stage_item->setText(0, "Stage: " + stage->getName());                                           // Set text for item
            stage_item->setData(0, User_Roles::Key, QVariant::fromValue(stage->getKey()));


            // ***** Iterates through Things based on z-order of each Thing
            ThingMap &things = stage->getThingMap();
            QList<long> keys = stage->thingKeysSortedByZOrder();

            for (auto key: keys) {
                DrThing *thing = things[key];

                QTreeWidgetItem *thing_item = new QTreeWidgetItem(stage_item);                             // Create new item and add as child item
                switch (thing->getThingType()) {
                    case DrThingType::None:      Dr::ShowErrorMessage("buildProjectTree", "No Icon for DrThingType::None");     break;
                    case DrThingType::Character: thing_item->setIcon(0, QIcon(":/assets/tree_icons/tree_character.png"));       break;
                    case DrThingType::Object:    thing_item->setIcon(0, QIcon(":/assets/tree_icons/tree_object.png"));          break;
                    case DrThingType::Text:      thing_item->setIcon(0, QIcon(":/assets/tree_icons/tree_text.png"));            break;
                    case DrThingType::Fire:      thing_item->setIcon(0, QIcon(":/assets/tree_icons/tree_fire.png"));            break;
                    case DrThingType::Fisheye:   thing_item->setIcon(0, QIcon(":/assets/tree_icons/tree_fisheye.png"));         break;
                    case DrThingType::Light:     thing_item->setIcon(0, QIcon(":/assets/tree_icons/tree_light.png"));           break;
                    case DrThingType::Mirror:    thing_item->setIcon(0, QIcon(":/assets/tree_icons/tree_mirror.png"));          break;
                    case DrThingType::Swirl:     thing_item->setIcon(0, QIcon(":/assets/tree_icons/tree_swirl.png"));           break;
                    case DrThingType::Water:     thing_item->setIcon(0, QIcon(":/assets/tree_icons/tree_water.png"));           break;
                }

                thing_item->setText(0, thing->getName());                                                   // Set text for item
                thing_item->setData(0, User_Roles::Key, QVariant::fromValue(thing->getKey()));              // Store item key in user data

                stage_item->addChild(thing_item);

                // Add lock box
                QString check_images = QString(" QCheckBox::indicator { width: 12px; height: 12px; }"
                                               " QCheckBox::indicator:checked {   image: url(:/assets/tree_icons/tree_lock.png); }"
                                               " QCheckBox::indicator:unchecked { image: url(:/assets/tree_icons/tree_bullet.png); }");
                QCheckBox *lock_item = new QCheckBox();
                lock_item->setFocusPolicy( Qt::FocusPolicy::NoFocus );
                lock_item->setStyleSheet(  check_images );
                lock_item->setCheckState(  thing->isLocked() ? Qt::CheckState::Checked : Qt::CheckState::Unchecked );

                // Locking / unlocking function
                connect(lock_item, &QCheckBox::toggled, this, [this, thing, lock_item] () {
                    if (thing->getComponentPropertyValue(Components::Hidden_Settings, Properties::Hidden_Item_Locked).toBool()) {
                        lock_item->setCheckState( Qt::CheckState::Checked );
                    } else {
                        bool locked = (lock_item->checkState() == Qt::CheckState::Checked) ? true : false;
                        thing->setLocked( locked );
                    }
                    this->m_editor_relay->updateInspectorEnabledProperties();
                });

                this->setItemWidget(thing_item, 1, lock_item);
            }
        }
    }

    this->expandAll();                                             // Expand all items
    this->update();
    setAllowSelectionEvent(true);
}

// Handles changing the Advisor on Mouse Enter
void TreeProject::enterEvent(QEvent *event) {
    m_editor_relay->setAdvisorInfo(Advisor_Info::Project_Tree);
    QTreeWidget::enterEvent(event);
}



//####################################################################################
//##    Returns a list of the items contained within the tree
//####################################################################################
QList <QTreeWidgetItem*> TreeProject::getListOfAllTreeWidgetItems() {
    return getListOfChildrenFromItem( this->invisibleRootItem() );
}

QList <QTreeWidgetItem*> TreeProject::getListOfChildrenFromItem( QTreeWidgetItem *item ) {
    QList <QTreeWidgetItem*> items;

    for (int i = 0; i < item->childCount(); ++i) {
        items.append( item->child(i) );
        items.append( getListOfChildrenFromItem( item->child(i) ) );
    }

    return items;
}









