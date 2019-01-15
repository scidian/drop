//
//      Created by Stephens Nunnally on 1/15/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//


#include "project.h"
#include "project_world.h"
#include "project_world_scene.h"
#include "project_world_scene_object.h"
#include "editor_scene_item.h"

#include "settings.h"
#include "settings_component.h"
#include "settings_component_property.h"

#include "editor_scene_scene.h"
#include "interface_relay.h"


// SLOT: Called when item selection is changed in scene
void SceneGraphicsScene::selectionChanged()
{
    m_selection_rect = totalSelectedItemsSceneRect();
    update();

    // !!!!! TEMP:
    m_relay->setLabelText(Label_Names::Label_1, "Group Pos  X: " + QString::number(m_selection_rect.x()) +
                                                         ", Y: " + QString::number(m_selection_rect.y()) );
    m_relay->setLabelText(Label_Names::Label_2, "Group Size X: " + QString::number(m_selection_rect.width()) +
                                                         ", Y: " + QString::number(m_selection_rect.height()) );
    // !!!!! END

}


// Returns a scene rect containing all the selected items
QRectF SceneGraphicsScene::totalSelectedItemsSceneRect()
{
    // If no items selected, return empty rect
    QRectF total_rect;
    if (selectedItems().count() < 1) return total_rect;

    // Start with rect of first item, add on each additional items rect
    total_rect = selectedItems().first()->sceneBoundingRect();
    for (auto item: selectedItems())
        total_rect = total_rect.united(item->sceneBoundingRect());

    return total_rect;
}










