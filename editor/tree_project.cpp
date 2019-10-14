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

#define COLUMN_TITLE    0
#define COLUMN_LOCK     1


//####################################################################################
//##    Populates Tree Project List based on project data
//####################################################################################
void TreeProject::buildProjectTree() {

    setAllowSelectionEvent(false);
    this->clear();

    QColor icon_color = Dr::GetColor(Window_Colors::Icon_Dark);
    QImage icon_image;

    for (auto world_pair: m_project->getWorldMap()) {

        QTreeWidgetItem *world_item = new QTreeWidgetItem(this);                                            // Create new item (top level item)

        DrWorld *world = world_pair.second;
        icon_image = QPixmap(":/assets/tree_icons/tree_world.png").toImage();
        world_item->setIcon(0, QIcon(QPixmap::fromImage(DrImaging::colorizeImage(icon_image, icon_color))));
        world_item->setText(0, "World: " + world->getName());                                               // Set text for item
        world_item->setData(0, User_Roles::Key, QVariant::fromValue(world->getKey()));
        this->addTopLevelItem(world_item);                                                                  // Add it on our tree as the top item.

        for (auto stage_pair: world->getStageMap()) {

            QTreeWidgetItem *stage_item = new QTreeWidgetItem(world_item);                                  // Create new item and add as child item

            DrStage *stage = stage_pair.second;
            icon_image = QPixmap(":/assets/tree_icons/tree_stage.png").toImage();
            stage_item->setIcon(0, QIcon(QPixmap::fromImage(DrImaging::colorizeImage(icon_image, icon_color))));
            stage_item->setText(0, "Stage: " + stage->getName());                                           // Set text for item
            stage_item->setData(0, User_Roles::Key, QVariant::fromValue(stage->getKey()));


            // ***** Iterates through Things based on z-order of each Thing
            ThingMap &things = stage->getThingMap();
            QList<long> keys = stage->thingKeysSortedByZOrder();

            for (auto key: keys) {
                DrThing *thing = things[key];
                if (!thing) continue;

                if (thing->getComponentPropertyValue(Components::Hidden_Settings, Properties::Hidden_Hide_From_Trees).toBool()) {
                    if (Dr::CheckDebugFlag(Debug_Flags::Show_Hidden_Component) == false) continue;
                }

                QTreeWidgetItem *thing_item = new QTreeWidgetItem(stage_item);                             // Create new item and add as child item
                switch (thing->getThingType()) {
                    case DrThingType::None:      Dr::ShowErrorMessage("buildProjectTree", "No Icon for DrThingType::None");     break;
                    case DrThingType::Character: icon_image = QPixmap(":/assets/tree_icons/tree_character.png").toImage();      break;
                    case DrThingType::Object:    icon_image = QPixmap(":/assets/tree_icons/tree_object.png").toImage();         break;
                    case DrThingType::Text:      icon_image = QPixmap(":/assets/tree_icons/tree_text.png").toImage();           break;
                    case DrThingType::Fire:      icon_image = QPixmap(":/assets/tree_icons/tree_fire.png").toImage();           break;
                    case DrThingType::Fisheye:   icon_image = QPixmap(":/assets/tree_icons/tree_fisheye.png").toImage();        break;
                    case DrThingType::Light:     icon_image = QPixmap(":/assets/tree_icons/tree_light.png").toImage();          break;
                    case DrThingType::Mirror:    icon_image = QPixmap(":/assets/tree_icons/tree_mirror.png").toImage();         break;
                    case DrThingType::Swirl:     icon_image = QPixmap(":/assets/tree_icons/tree_swirl.png").toImage();          break;
                    case DrThingType::Water:     icon_image = QPixmap(":/assets/tree_icons/tree_water.png").toImage();          break;
                }
                thing_item->setIcon(0, QIcon(QPixmap::fromImage(DrImaging::colorizeImage(icon_image, icon_color))));
                thing_item->setText(0, thing->getName());                                                   // Set text for item
                thing_item->setData(0, User_Roles::Key, QVariant::fromValue(thing->getKey()));              // Store item key in user data

                stage_item->addChild(thing_item);

                // Add lock box
                bool forced = thing->getComponentPropertyValue(Components::Hidden_Settings, Properties::Hidden_Item_Locked).toBool();
                QString check_images = QString(" QCheckBox::indicator { width: 12px; height: 12px; } "
                                               " QCheckBox::indicator:unchecked { image: url(:/assets/tree_icons/tree_bullet.png); } ");
                if (forced) check_images +=    " QCheckBox::indicator:checked {   image: url(:/assets/tree_icons/tree_lock_disable.png); } ";
                else        check_images +=    " QCheckBox::indicator:checked {   image: url(:/assets/tree_icons/tree_lock.png); } ";

                QCheckBox *lock_item = new QCheckBox();
                lock_item->setObjectName("projectTreeLock");
                lock_item->setProperty(User_Property::Key, QVariant::fromValue( thing->getKey() ));
                lock_item->setFocusPolicy( Qt::FocusPolicy::NoFocus );
                lock_item->setStyleSheet(  check_images );
                lock_item->setCheckState(  thing->isLocked() ? Qt::CheckState::Checked : Qt::CheckState::Unchecked );

                // Locking / unlocking function
                connect(lock_item, &QCheckBox::toggled, this, [this, lock_item]() { this->processLockClick(lock_item); });

                this->setItemWidget(thing_item, COLUMN_LOCK, lock_item);
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

// Sets this as the Active Editor widget
void TreeProject::focusInEvent(QFocusEvent *event) {
    m_editor_relay->setActiveWidget(Editor_Widgets::Project_Tree);
    QTreeWidget::focusInEvent(event);
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

    // Update Object Inspector Properties and Update SceneView selected items
    this->m_editor_relay->updateInspectorEnabledProperties();
    this->m_editor_relay->updateItemSelection(Editor_Widgets::Project_Tree);
}





















