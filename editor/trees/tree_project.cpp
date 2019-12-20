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

#include "editor/debug.h"
#include "editor/globals_editor.h"
#include "editor/helper_library.h"
#include "editor/imaging/imaging.h"
#include "editor/interface_editor_relay.h"
#include "editor/trees/tree_project.h"
#include "editor/view/editor_view.h"
#include "library/colors/colors.h"
#include "model/project/project.h"
#include "model/project/project_stage.h"
#include "model/project/project_thing.h"
#include "model/project/project_world.h"
#include "model/settings/settings.h"
#include "model/settings/settings_component.h"
#include "model/settings/settings_component_property.h"


//####################################################################################
//##    Constructor
//####################################################################################
TreeProject::TreeProject(QWidget *parent, DrProject *project, IEditorRelay *editor_relay) : QTreeWidget (parent) {
    m_editor_relay = editor_relay;
    m_project = project;

    connect(this, SIGNAL(itemCollapsed(QTreeWidgetItem *)), this, SLOT(handleCollapsed(QTreeWidgetItem *)));
    connect(this, SIGNAL(itemExpanded(QTreeWidgetItem *)),  this, SLOT(handleExpanded(QTreeWidgetItem *)));
}

// Handles changing the Advisor on Mouse Enter
void TreeProject::enterEvent(QEvent *event) {
    m_editor_relay->setAdvisorInfo(Advisor_Info::Project_Tree);
    QTreeWidget::enterEvent(event);
}

// Sets this as the Active Editor widget
void TreeProject::focusInEvent(QFocusEvent *event) {
    m_editor_relay->setActiveWidget(Editor_Widgets::Project_Tree);
    if (selectedItems().count() > 0) {
        long selected_key = selectedItems().first()->data(COLUMN_TITLE, User_Roles::Key).toLongLong();
        if (selected_key > 0) m_editor_relay->buildInspector(QList<long> { selected_key });
    }
    QTreeWidget::focusInEvent(event);
}


//####################################################################################
//##    Populates Tree Project List based on Project data
//####################################################################################
void TreeProject::buildProjectTree(bool total_rebuild) {

    // Store current scroll bar position
    int scroll_position = this->verticalScrollBar()->value();
    if (total_rebuild) this->clear();

    // ********** Turn off selection event during build
    bool started_empty = this->invisibleRootItem()->childCount() < 1;
    setAllowSelectionEvent(false);

    // ********** Store some data about the tree so we can restore after its rebuilt
    QMap<long, QTreeWidgetItem*> item_map;
    QList<QTreeWidgetItem*>      item_list = getListOfAllTreeWidgetItems();
    QTreeWidgetItem             *last_added = nullptr;

    // ***** Gets list of items in tree, if keys have been removed from project, removes items
    for (auto item : item_list) {
        // Check if item source key is still in project, if so add to map
        long key = item->data(COLUMN_TITLE, User_Roles::Key).toLongLong();
        if (key <= 0)               { item_map[key] = item; continue; }
        DrSettings *settings = getParentProject()->findSettingsFromKey(key, false);
        if (settings != nullptr)    { item_map[key] = item; continue; }

        // If we made it here, remove item from tree
        int index = this->indexOfTopLevelItem(item);
        if (index > -1) {
            this->takeTopLevelItem(index);
        } else {
            QTreeWidgetItem *parent = dynamic_cast<QTreeWidgetItem*>(item->parent());
            if (parent != nullptr) parent->takeChild(parent->indexOfChild(item));
        }
    }


    // ********** Go through Project and add items as necessary
    QColor icon_color = Dr::ToQColor(Dr::GetColor(Window_Colors::Icon_Dark));
    QImage icon_image;

    for (auto world_pair: getParentProject()->getWorldMap()) {

        // ***** Create new Item for World if necessary
        DrWorld         *world =        world_pair.second;
        long             world_key =    world_pair.first;
        QTreeWidgetItem *world_item =   item_map[world_key];
        if (world_item == nullptr) {
            world_item = new QTreeWidgetItem(this);
            last_added = world_item;
            icon_image = QPixmap(":/assets/tree_icons/tree_world.png").toImage();
            world_item->setIcon(COLUMN_TITLE, QIcon(QPixmap::fromImage(Dr::ColorizeImage(icon_image, icon_color))));
            world_item->setText(COLUMN_TITLE, "World: " + QString::fromStdString(world->getName()));
            world_item->setData(COLUMN_TITLE, User_Roles::Key, QVariant::fromValue(world->getKey()));
            world_item->setExpanded( world->getExpanded() );
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
                stage_item->setIcon(COLUMN_TITLE, QIcon(QPixmap::fromImage(Dr::ColorizeImage(icon_image, icon_color))));
                stage_item->setText(COLUMN_TITLE, "Stage: " + QString::fromStdString(stage->getName()));
                stage_item->setData(COLUMN_TITLE, User_Roles::Key, QVariant::fromValue(stage->getKey()));                
                stage_item->setExpanded( stage->getExpanded() );
            }

            // ***** Iterates through Things based on z-order of each Thing
            ThingMap &things = stage->getThingMap();
            std::vector<long> keys = stage->thingKeysSortedByZOrder(Sort_Order::DescendingOrder);

            for (auto key: keys) {
                DrThing         *thing =        things[key];
                             if (thing == nullptr) continue;
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
                        case DrThingType::Camera:    icon_image = QPixmap(":/assets/tree_icons/tree_camera.png").toImage();             break;
                        case DrThingType::Text:      icon_image = QPixmap(":/assets/tree_icons/tree_text.png").toImage();               break;
                        case DrThingType::Fire:      icon_image = QPixmap(":/assets/tree_icons/tree_fire.png").toImage();               break;
                        case DrThingType::Fisheye:   icon_image = QPixmap(":/assets/tree_icons/tree_fisheye.png").toImage();            break;
                        case DrThingType::Light:     icon_image = QPixmap(":/assets/tree_icons/tree_light.png").toImage();              break;
                        case DrThingType::Mirror:    icon_image = QPixmap(":/assets/tree_icons/tree_mirror.png").toImage();             break;
                        case DrThingType::Swirl:     icon_image = QPixmap(":/assets/tree_icons/tree_swirl.png").toImage();              break;
                        case DrThingType::Water:     icon_image = QPixmap(":/assets/tree_icons/tree_water.png").toImage();              break;
                    }
                    thing_item->setIcon(COLUMN_TITLE, QIcon(QPixmap::fromImage(Dr::ColorizeImage(icon_image, icon_color))));
                    thing_item->setText(COLUMN_TITLE, QString::fromStdString(thing->getName()));
                    thing_item->setData(COLUMN_TITLE, User_Roles::Key, QVariant::fromValue(thing->getKey()));
                }

                // ***** Update Z-Order
                thing_item->setData(COLUMN_Z_ORDER, Qt::DisplayRole, thing->getZOrderWithSub());

                // ***** Install / update lock box
                installLockBox(thing, thing_item);

                // ***** Hide / Unhide as necessary
                bool should_hide = false;
                if (thing->getComponentPropertyValue(Components::Hidden_Settings, Properties::Hidden_Hide_From_Trees).toBool()) {
                    if (Dr::CheckDebugFlag(Debug_Flags::Show_Hidden_Component) == false) should_hide = true;
                }
                if (should_hide) thing_item->setHidden(should_hide);
            }

            // ***** Sort Stage Items by Z-Order
            stage_item->sortChildren(COLUMN_Z_ORDER, Qt::SortOrder::DescendingOrder);
        }
    }

    // ***** If we added new item(s), make sure visible
    if (last_added != nullptr && !started_empty) {
        this->scrollToItem(last_added);
        ///this->verticalScrollBar()->setValue(scroll_position);
    }

    // ***** Restore scroll bar position
    if (total_rebuild) this->verticalScrollBar()->setValue(scroll_position);

    // ***** Update and allow selection again
    this->update();
    setAllowSelectionEvent(true);
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

// Returns item from getListOfAllTreeWidgetItems with the desired Project Key, or nullptr
QTreeWidgetItem* TreeProject::findItemWithProjectKey(long key) {
    if (key <= 0) return nullptr;
    for (auto item : getListOfAllTreeWidgetItems()) {
        long check_key = item->data(COLUMN_TITLE, User_Roles::Key).toLongLong();
        if (check_key == key) return item;
    }
    return nullptr;
}



















