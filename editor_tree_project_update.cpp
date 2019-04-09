//
//      Created by Stephens Nunnally on 4/9/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "editor_tree_project.h"
#include "interface_editor_relay.h"
#include "project.h"
#include "project_world.h"
#include "project_world_stage.h"
#include "project_world_stage_object.h"


//####################################################################################
//##        Updates item names if they have been changed
//####################################################################################
void TreeProject::updateItemNames(QList<DrSettings*> changed_items, QList<long> property_keys)
{
    if (changed_items.isEmpty()) return;
    if (property_keys.isEmpty()) return;

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
