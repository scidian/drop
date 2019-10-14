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


//####################################################################################
//##    Updates item names if they have been changed
//####################################################################################
void TreeProject::updateItemNames(QList<DrSettings*> changed_items, QList<long> property_keys) {
    if (changed_items.isEmpty()) return;

    setAllowSelectionEvent(false);

    for (auto item : changed_items) {
        long item_key = item->getKey();
        for (auto row : getListOfAllTreeWidgetItems()) {
            long row_key = row->data(0, User_Roles::Key).toLongLong();

            if (item_key == row_key) {
                for (auto property : property_keys) {
                    Properties check_property = static_cast<Properties>(property);

                    if (check_property == Properties::Entity_Name) {
                        switch (item->getType()) {
                            case DrType::World:
                                row->setText(0, "World: " + item->getName());
                                break;
                            case DrType::Stage:
                                row->setText(0, "Stage: " + item->getName());
                                break;
                            case DrType::Thing:
                                row->setText(0, item->getName());
                                break;
                            default: ;
                        }
                    }
                }
            }
        }
    }
    update();
    setAllowSelectionEvent(true);
}
