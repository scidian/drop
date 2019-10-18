//
//      Created by Stephens Nunnally on 4/9/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "editor/tree_project.h"
#include "interface_editor_relay.h"
#include "project/project.h"
#include "project/project_world.h"
#include "project/project_world_stage.h"
#include "project/project_world_stage_thing.h"
#include "settings/settings_component_property.h"


//####################################################################################
//##    Updates item names if they have been changed
//####################################################################################
void TreeProject::updateItems(QList<DrSettings*> changed_items, QList<long> property_keys) {
    if (changed_items.isEmpty()) return;

    setAllowSelectionEvent(false);

    // Process each Entity that has been changed, and update Project_Tree as necessary
    for (auto entity : changed_items) {
        long item_key = entity->getKey();

        QTreeWidgetItem *item_in_tree = findItemWithProjectKey(item_key);
        if (item_in_tree == nullptr) continue;

        for (auto property : property_keys) {
            Properties check_property = static_cast<Properties>(property);

            if (check_property == Properties::Entity_Name) {
                QString new_name = entity->getName();
                if (entity->getType() == DrType::World) new_name = "World: " + new_name;
                if (entity->getType() == DrType::Stage) new_name = "Stage: " + new_name;
                item_in_tree->setText(COLUMN_TITLE, new_name);
            }

            if (check_property == Properties::Hidden_Item_Locked ||
                check_property == Properties::Hidden_Hide_From_Trees) {
                if (entity->getType() == DrType::Thing) installLockBox(entity, item_in_tree);
            }

            if (check_property == Properties::Thing_Z_Order) {
                double z_order = entity->findPropertyFromPropertyKey(property)->getValue().toDouble();
                item_in_tree->setData(COLUMN_Z_ORDER, Qt::DisplayRole, z_order);
                item_in_tree->parent()->sortChildren(COLUMN_Z_ORDER, Qt::SortOrder::DescendingOrder);
            }
        }
    }

    update();
    setAllowSelectionEvent(true);
}















