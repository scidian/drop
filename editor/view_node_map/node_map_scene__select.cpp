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
#include "editor/view_node_map/node_map_item.h"
#include "editor/view_node_map/node_map_scene.h"
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
//  double                  m_selection_angle;          // Angle current selection has been rotated to
//  QPointF                 m_selection_scale;          // Scaling applied to current selection
//  QRectF                  m_selection_box;            // Starting outline of selected items
void NodeMapScene::selectionChanged() {
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
        m_editor_relay->updateItemSelection(Editor_Widgets::Node_View);
    }
}


//####################################################################################
//##    Unselects all locked items that may have been selected
//####################################################################################
void NodeMapScene::unselectLockedItems() {
    // Turn off signals to stop recurssive calling of interface_relay->updateItemSelection()
    bool was_blocked = signalsBlocked();
    blockSignals(true);

    for (auto item : selectedItems()) {
        // Find referenced entity to check if locked
        DrSettings  *entity = nullptr;
        NodeMapItem *map_item = dynamic_cast<NodeMapItem*>(item);
        if (map_item != nullptr) entity = map_item->getEntity();
        // Another way to find reference entity, guaranteed not to access dangling pointer, but much slower
        ///long item_key = item->data(User_Roles::Key).toLongLong();
        ///DrSettings *entity = m_project->findSettingsFromKey(item_key);
        // Check entity is valid, see if locked
        if (entity == nullptr) continue;
        if (entity->isLocked()) {
            item->setSelected(false);
        }
    }

    blockSignals(was_blocked);
}



















