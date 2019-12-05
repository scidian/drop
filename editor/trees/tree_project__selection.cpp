//
//      Created by Stephens Nunnally on 4/9/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "editor/interface_editor_relay.h"
#include "editor/trees/tree_project.h"
#include "project/project.h"
#include "project/project_world.h"
#include "project/project_stage.h"
#include "project/project_thing.h"


//####################################################################################
//##    Updates selection
//##        Checks to make sure if more than one item is selected all new items
//##        not matching original type are not selected
//####################################################################################
void TreeProject::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected) {

    // If we're updating selection from outside source this function has been turned off from that source (i.e. updateSelection in IEditorRelay)
    if (!m_allow_selection_event) {
        QTreeWidget::selectionChanged(selected, deselected);
        return;
    }

    // ***** If size of list is zero, clear selected_key and exit function
    QList<QTreeWidgetItem*> item_list = this->selectedItems();
    if (item_list.size() == 0) {
        this->setSelectedKey(c_no_key);
        m_editor_relay->buildInspector(QList<long> { });
        m_editor_relay->updateItemSelection(Editor_Widgets::Project_Tree);
        return;
    }

    // ***** If size is one, reset first selected item
    if (item_list.size() == 1) {
        long selected_key = item_list.first()->data(COLUMN_TITLE, User_Roles::Key).toLongLong();       // grab stored key from QTreeWidgetItem user data
        this->setSelectedKey(selected_key);

        //******************************************************

        // Call to undo command to change scene to newly selected Stage or newly selected thing's parent Stage
        DrSettings *selected_entity = getParentProject()->findSettingsFromKey(selected_key, true, "Randomitastic");
        if (selected_entity != nullptr) {
            DrType selected_type = selected_entity->getType();
            long change_to_key = c_no_key;
            if (selected_type == DrType::World) {
                DrWorld *world = dynamic_cast<DrWorld*>(selected_entity);
                change_to_key = world->getLastStageShownKey();
                DrSettings *check_for_stage = getParentProject()->findSettingsFromKey(change_to_key);
                if (check_for_stage == nullptr) change_to_key = world->getStartStageKey();

            } else if (selected_type == DrType::Stage) {
                change_to_key = selected_key;
            } else if (selected_type == DrType::Thing) {
                DrThing *as_thing = dynamic_cast<DrThing*>(selected_entity);
                change_to_key = as_thing->getParentStage()->getKey();
            }
            if (change_to_key != c_no_key) {
                m_editor_relay->updateItemSelection(Editor_Widgets::Project_Tree);                      // selects none in scene before rebuilding scene
                m_editor_relay->buildScene( change_to_key );
            }
        }

        // Calls to outside update functions to rebuild Inspector
        m_editor_relay->buildInspector(QList<long> { selected_key });

        //******************************************************

    // ***** Size is more than 1, prevent items of different types being selected at same time
    } else {
        DrType selected_type = getParentProject()->findChildTypeFromKey( this->getSelectedKey() );

        // Check if newly selected items are same type, if not, do not allow select
        for (auto check_item: item_list) {
            // Get key from each item so we can compare it to first selected item
            long    check_key = check_item->data(COLUMN_TITLE, User_Roles::Key).toLongLong();
            DrType  check_type = getParentProject()->findChildTypeFromKey(check_key);

            // If we are over item that was first selected, skip to next
            if (check_key == this->getSelectedKey()) { continue; }

            if (check_type != selected_type)
                check_item->setSelected(false);
        }
    }

    // Call to outside update functions to update selection in scene view
    m_editor_relay->updateItemSelection(Editor_Widgets::Project_Tree);

    QTreeWidget::selectionChanged(selected, deselected);                                                // pass event to parent
}


//####################################################################################
//##    Selects rows based on items selected in Scene View
//####################################################################################
void TreeProject::updateSelectionFromView(QList<QGraphicsItem*> item_list) {
    QList <long> keys;
    for (auto item : item_list) {
        long item_key = item->data(User_Roles::Key).toLongLong();
        keys.append(item_key);
    }
    updateSelectionFromKeyList(keys);
}

//####################################################################################
//##    Selects rows based on custom Key list
//####################################################################################
void TreeProject::updateSelectionFromKeyList(QList<long> key_list) {
    setAllowSelectionEvent(false);
    clearSelection();

    QList<QTreeWidgetItem*> all_items = getListOfAllTreeWidgetItems();

    long items_selected = 0;
    for (auto key : key_list) {
        for (auto row : all_items) {
            long row_key = row->data(0, User_Roles::Key).toLongLong();

            if (key == row_key) {
                row->setSelected(true);

                if (items_selected == 0) {
                    setSelectedKey(row_key);
                    ++items_selected;
                }
            }
        }
    }

    ensureSelectionVisible();

    update();
    setAllowSelectionEvent(true);
}

// Ensure selection visible
void TreeProject::ensureSelectionVisible() {
    if (selectedItems().count() > 0) {
        this->scrollToItem(selectedItems().last());
    }
}








