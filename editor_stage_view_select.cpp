//
//      Created by Stephens Nunnally on 1/8/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Graphics View Handling - View_Mode::Selecting
//
//

#include "project.h"
#include "project_world.h"
#include "project_world_stage.h"
#include "project_world_stage_object.h"
#include "editor_stage_item.h"

#include "settings.h"
#include "settings_component.h"
#include "settings_component_property.h"

#include "editor_stage_scene.h"
#include "editor_stage_view.h"
#include "interface_relay.h"


//####################################################################################
//##        Starts selecting mode
//####################################################################################
void StageGraphicsView::startSelect(QMouseEvent *event)
{
    StageGraphicsScene *my_scene = dynamic_cast<StageGraphicsScene *>(scene());
    m_items_start = my_scene->getSelectionGroupItems();
    m_first_start = my_scene->getFirstSelectedItem();

    // If control key isnt down, we're starting a new selection process, so remove all items
    if (event->modifiers() & Qt::KeyboardModifier::ControlModifier)
        m_items_keep = m_items_start;
    else
        m_items_keep.clear();

    m_rubber_band->setGeometry(QRect(m_origin, QSize()));                   // Start selection box with no size
    m_rubber_band->show();                                                  // Make selection box visible
    update();
}

//####################################################################################
//##        Handles resizing Rubber Band box and updating Selection Area during View_Mode::Selecting
//####################################################################################
void StageGraphicsView::processSelection(QPoint mouse_in_view)
{
    StageGraphicsScene    *my_scene = dynamic_cast<StageGraphicsScene*>(scene());

    QRect band_box = QRect(m_origin, mouse_in_view).normalized();
    if (band_box.width() < 1)  band_box.setWidth(1);
    if (band_box.height() < 1) band_box.setHeight(1);

    m_rubber_band->setGeometry(band_box);                                                       // Resize selection box

    QPainterPath selection_area;
    selection_area.addPolygon(mapToScene(m_rubber_band->geometry()));                           // Convert box to scene coords
    selection_area.closeSubpath();                                                              // Closes an open polygon

    QList<QGraphicsItem*> selection_area_items = scene()->items(selection_area, Qt::ItemSelectionMode::IntersectsItemShape,
                                                                Qt::SortOrder::DescendingOrder, viewportTransform());

    // Go through selected items, unselect if rubber band box covered them and has since shrunk
    for (auto item : my_scene->getSelectionGroupItems()) {
        if (selection_area_items.contains(item) == false && m_items_keep.contains(item) == false)
                my_scene->getSelectionGroup()->removeFromGroup(item);
    }

    if (my_scene->getSelectionGroupCount() == 0)
        my_scene->resetSelectionGroup();

    if (m_items_keep.count() == 0 && selection_area_items.count() == 1) {
        if (selection_area_items.first() != my_scene->getSelectionGroupAsGraphicsItem()) {
            DrObject *new_first = dynamic_cast<DrItem*>(selection_area_items.first())->getObject();
            my_scene->setFirstSelectedItem(new_first);
        }
    }

    // Add items in selection area to group
    for (auto item : selection_area_items) my_scene->addItemToSelectionGroup(item);


}








