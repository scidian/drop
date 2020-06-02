//
//      Created by Stephens Nunnally on 5/22/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QDebug>

#include "core/dr_math.h"
#include "editor/helper_library.h"
#include "editor/preferences.h"
#include "editor/view_node/node_item.h"
#include "editor/view_node/node_scene.h"
#include "project/dr_project.h"
#include "project/entities/dr_world.h"
#include "project/settings/settings.h"
#include "project/settings/settings_component.h"
#include "project/settings/settings_component_property.h"


//####################################################################################
//##    SLOT: selectionChanged,
//##          connected from built in Qt SIGNAL EditorScene::selectionChanged()
//####################################################################################
//  QList<QGraphicsItem*>   m_selection_items;          // List of selected items
void NodeScene::selectionChanged() {
    // Don't allow selection if locked
    unselectLockedItems();

    // If selection hasnt changed, return
    if (selectedItems() == m_selection_items) return;
    m_selection_items = selectedItems();

    // Pass on selection changes
    if (m_editor_relay != nullptr) {
        QList<long> item_keys { };
        for (auto &item : selectedItems()) {
            item_keys.append(item->data(User_Roles::Key).toLongLong());
        }

        m_editor_relay->buildInspector(item_keys);
        m_editor_relay->updateItemSelection(Editor_Widgets::View);
    }
}


//####################################################################################
//##    Returns a scene rect containing all the selected items
//####################################################################################
QRectF NodeScene::totalSelectionSceneRect() {
    // If no items selected, return empty rect
    QRectF total_rect;
    if (selectedItems().count() < 1) return total_rect;

    // Start with rect of first item, add on each additional items rect
    total_rect = selectedItems().first()->sceneBoundingRect();
    for (auto item: selectedItems()) {
        total_rect = total_rect.united(item->sceneBoundingRect());
    }

    return total_rect;
}


//####################################################################################
//##    Selects items based on rows selected in Editor_Project_Tree
//####################################################################################
void NodeScene::updateSelectionFromProjectTree(QList<QTreeWidgetItem*> tree_list) {
    QList <long> keys;
    for (auto row : tree_list) {
        long row_key = row->data(0, User_Roles::Key).toLongLong();
        keys.append(row_key);
    }
    updateSelectionFromKeyList(keys);
}


//####################################################################################
//##    Selects items based on custom Key list
//####################################################################################
void NodeScene::updateSelectionFromKeyList(QList<long> key_list) {
    // Turn off signals to stop recurssive calling of interface_relay->updateItemSelection()
    blockSignals(true);

    // Clear current selection
    for (auto item : selectedItems()) item->setSelected(false);

    for (auto item : items()) {
        long item_key = item->data(User_Roles::Key).toLongLong();

        if (key_list.contains(item_key)) {
            ///DrSettings *entity = m_project->findSettingsFromKey(item_key);
            NodeItem *map_item = dynamic_cast<NodeItem*>(item);
            if (map_item == nullptr) continue;
            DrSettings *entity = map_item->getEntity();
            if (entity == nullptr)  continue;
            if (entity->isLocked()) continue;
            item->setSelected(true);
        }
    }
    blockSignals(false);
}


//####################################################################################
//##    Unselects all locked items that may have been selected
//####################################################################################
void NodeScene::unselectLockedItems() {
    // Turn off signals to stop recurssive calling of interface_relay->updateItemSelection()
    bool was_blocked = signalsBlocked();
    blockSignals(true);

    for (auto item : selectedItems()) {
        // Find referenced entity to check if locked
        ///DrSettings  *entity = nullptr;
        ///NodeItem *map_item = dynamic_cast<NodeItem*>(item);
        ///if (map_item != nullptr) entity = map_item->getEntity();

        // Another way to find reference entity, guaranteed not to access dangling pointer, but much slower
        long item_key = item->data(User_Roles::Key).toLongLong();
        DrSettings *entity = m_project->findSettingsFromKey(item_key);

        // Check entity is valid, see if locked
        if (entity == nullptr) continue;
        if (entity->isLocked()) item->setSelected(false);
    }

    blockSignals(was_blocked);
}



















