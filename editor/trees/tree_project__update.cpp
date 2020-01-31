//
//      Created by Stephens Nunnally on 4/9/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "editor/interface_editor_relay.h"
#include "editor/trees/tree_project.h"
#include "project/dr_project.h"
#include "project/entities/dr_world.h"
#include "project/entities/dr_stage.h"
#include "project/entities/dr_thing.h"
#include "project/settings/settings_component_property.h"


//####################################################################################
//##    Updates item names if they have been changed
//####################################################################################
void TreeProject::updateItems(std::list<DrSettings*> changed_items, std::list<ComponentProperty> component_property_pairs) {
    if (changed_items.empty()) return;

    setAllowSelectionEvent(false);

    // Process each Entity that has been changed, and update Project_Tree as necessary
    for (auto entity : changed_items) {
        long item_key = entity->getKey();

        QTreeWidgetItem *item_in_tree = findItemWithProjectKey(item_key);
        if (item_in_tree == nullptr) continue;

        for (auto component_property_pair : component_property_pairs) {
            std::string comp = component_property_pair.first;
            std::string prop = component_property_pair.second;

            if (comp == Comps::Entity_Settings && prop == Props::Entity_Name) {
                QString new_name = QString::fromStdString(entity->getName());
                if (entity->getType() == DrType::World) new_name = "World: " + new_name;
                if (entity->getType() == DrType::Stage) new_name = "Stage: " + new_name;
                item_in_tree->setText(COLUMN_TITLE, new_name);
            }

            if ((comp == Comps::Hidden_Settings && prop == Props::Hidden_Item_Locked) ||
                (comp == Comps::Hidden_Settings && prop == Props::Hidden_Hide_From_Trees)) {
                if (entity->getType() == DrType::Thing) installLockBox(entity, item_in_tree);
            }

            if (comp == Comps::Thing_Layering && prop == Props::Thing_Z_Order) {
                DrThing *thing = dynamic_cast<DrThing*>(entity);
                if (thing) {
                    item_in_tree->setData(COLUMN_Z_ORDER, Qt::DisplayRole, thing->getZOrderWithSub());
                    item_in_tree->parent()->sortChildren(COLUMN_Z_ORDER, Qt::SortOrder::DescendingOrder);
                }
            }
        }
    }

    update();
    setAllowSelectionEvent(true);
}


//####################################################################################
//##    Handles Expand / Collapse of QTreeWidgetItems
//####################################################################################
void TreeProject::handleCollapsed(QTreeWidgetItem *item) {
    long key = item->data(COLUMN_TITLE, User_Roles::Key).toLongLong();
    DrSettings *settings = getParentProject()->findSettingsFromKey(key, false);
    if (settings != nullptr) {
        if (settings->getType() == DrType::World) {
            DrWorld *world = dynamic_cast<DrWorld*>(settings);
            if (world) world->setExpanded(false);
        } else if (settings->getType() == DrType::Stage) {
            DrStage *stage = dynamic_cast<DrStage*>(settings);
            if (stage) stage->setExpanded(false);
        }
    }
}

void TreeProject::handleExpanded(QTreeWidgetItem *item) {
    long key = item->data(COLUMN_TITLE, User_Roles::Key).toLongLong();
    DrSettings *settings = getParentProject()->findSettingsFromKey(key, false);
    if (settings != nullptr) {
        if (settings->getType() == DrType::World) {
            DrWorld *world = dynamic_cast<DrWorld*>(settings);
            if (world) world->setExpanded(true);
        } else if (settings->getType() == DrType::Stage) {
            DrStage *stage = dynamic_cast<DrStage*>(settings);
            if (stage) stage->setExpanded(true);
        }
    }
}












