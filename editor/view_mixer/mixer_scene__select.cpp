//
//      Created by Stephens Nunnally on 6/2/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QGraphicsItem>

#include "editor/view_mixer/mixer_item.h"
#include "editor/view_mixer/mixer_scene.h"
#include "editor/view_mixer/mixer_view.h"
#include "project/entities_sound/dr_mix.h"
#include "project/entities_sound/dr_sound.h"
#include "project/entities_sound/dr_track.h"


//####################################################################################
//##    SLOT: selectionChanged,
//##          connected from built in Qt SIGNAL EditorScene::selectionChanged()
//####################################################################################
//  QList<QGraphicsItem*>   m_selection_items;          // List of selected items
void MixerScene::selectionChanged() {

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
QRectF MixerScene::totalSelectionSceneRect() {
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
//##    Selects items based on custom Key list
//####################################################################################
void MixerScene::updateSelectionFromKeyList(QList<long> key_list) {
    // Turn off signals to stop recurssive calling of interface_relay->updateItemSelection()
    blockSignals(true);

    // Clear current selection
    for (auto item : selectedItems()) item->setSelected(false);

    for (auto item : items()) {
        long item_key = item->data(User_Roles::Key).toLongLong();

        if (key_list.contains(item_key)) {
            item->setSelected(true);
        }
    }
    blockSignals(false);
}





















