//
//      Created by Stephens Nunnally on 1/8/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Graphics View Handling - View_Mode::Selecting
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
#include "editor_scene_view.h"
#include "interface_relay.h"


//####################################################################################
//##        Starts selecting mode
//####################################################################################
void SceneGraphicsView::startSelect(QMouseEvent *event)
{
    if (scene() == nullptr) return;

    m_view_mode = View_Mode::Selecting;                                     // Flag that we're in selection mode
    if (event->modifiers() & Qt::KeyboardModifier::ControlModifier) {
        m_items_start = dynamic_cast<SceneGraphicsScene*>(scene())->getSelectionGroupItems();
    } else {
        m_items_start.clear();
    }

    m_rubber_band->setGeometry(QRect(m_origin, QSize()));                   // Start selection box with no size
    m_rubber_band->show();                                                  // Make selection box visible
    update();
}

//####################################################################################
//##        Handles resizing Rubber Band box and updating Selection Area during View_Mode::Selecting
//####################################################################################
void SceneGraphicsView::processSelection(QPoint mouse_in_view)
{
    SceneGraphicsScene    *my_scene = dynamic_cast<SceneGraphicsScene*>(scene());

    m_rubber_band->setGeometry(QRect(m_origin, mouse_in_view).normalized());                            // Resize selection box

    QPainterPath selection_area;
    selection_area.addPolygon(mapToScene(m_rubber_band->geometry()));                                   // Convert box to scene coords
    selection_area.closeSubpath();                                                                      // Closes an open polygon

    QList<QGraphicsItem*> selection_area_items = scene()->items(selection_area, Qt::ItemSelectionMode::IntersectsItemShape,
                                                                Qt::SortOrder::DescendingOrder, viewportTransform());

    // Add items in selection area to group
    for (auto item : selection_area_items) my_scene->addItemToSelectionGroup(item);

    // Go through selected items, unselect if rubber band box covered them and has since shrunk
    for (auto item : my_scene->getSelectionGroupItems()) {
        if (selection_area_items.contains(item) == false && m_items_start.contains(item) == false)
                my_scene->getSelectionGroup()->removeFromGroup(item);
    }

    // If we don't have any items left in selection group, reset it
    if (my_scene->getSelectionGroupCount() < 1) my_scene->emptySelectionGroup();
}








