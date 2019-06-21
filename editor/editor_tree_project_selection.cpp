//
//      Created by Stephens Nunnally on 4/9/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "editor_tree_project.h"
#include "interface_editor_relay.h"
#include "project/project.h"
#include "project/project_world.h"
#include "project/project_world_stage.h"
#include "project/project_world_stage_thing.h"


//####################################################################################
//##        Updates selection
//##            Checks to make sure if more than one item is selected all new items
//##            not matching original type are not selected
//####################################################################################
void TreeProject::selectionChanged (const QItemSelection &selected, const QItemSelection &deselected) {

    // If we're updating selection from outside source this function has been turned off from that source (i.e. updateSelection in IEditorRelay)
    if (!m_allow_selection_event) {
        QTreeWidget::selectionChanged(selected, deselected);
        return;
    }

    // ***** If size of list is zero, clear selected_key and exit function
    QList<QTreeWidgetItem*> item_list = this->selectedItems();
    if (item_list.size() == 0) {
        this->setSelectedKey(0);
        m_editor_relay->buildInspector(QList<long> { });
        m_editor_relay->updateItemSelection(Editor_Widgets::Project_Tree);
        return;
    }

    // ***** If size is one, reset first selected item
    if (item_list.size() == 1) {
        long selected_key = item_list.first()->data(0, User_Roles::Key).toLongLong();       // grab stored key from list view user data
        this->setSelectedKey(selected_key);

        //******************************************************

        // Call to undo command to change scene to newly selected Stage or newly selected thing's parent Stage
        DrSettings *selected_item = m_project->findSettingsFromKey(selected_key);
        if (selected_item != nullptr) {
            DrType selected_type = selected_item->getType();
            long change_to_key = c_no_key;
            if (selected_type == DrType::Stage || selected_type == DrType::StartStage) {
                change_to_key = selected_key;
            } else if (selected_type == DrType::Thing) {
                DrThing *as_thing = dynamic_cast<DrThing*>(selected_item);
                change_to_key = as_thing->getParentStage()->getKey();
            }
            if (change_to_key != c_no_key) {
                m_editor_relay->updateItemSelection(Editor_Widgets::Project_Tree);          // selects none in scene before rebuilding scene
                m_editor_relay->buildScene( change_to_key );
            }
        }

        // Calls to outside update functions to rebuild Inspector
        m_editor_relay->buildInspector(QList<long> { selected_key });

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
void TreeProject::updateSelectionFromView(QList<QGraphicsItem*> item_list) {
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










