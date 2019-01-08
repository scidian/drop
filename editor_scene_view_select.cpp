//
//      Created by Stephens Nunnally on 1/8/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Graphics View Handling - View_Mode::Selecting
//
//

#include "01_project.h"
#include "02_world.h"
#include "03_scene.h"
#include "04_object.h"
#include "05_item.h"

#include "30_settings.h"
#include "31_component.h"
#include "32_property.h"

#include "editor_scene_view.h"
#include "interface_relay.h"


// Starts selecting mode
void SceneGraphicsView::startSelect(QMouseEvent *event)
{
    m_view_mode = View_Mode::Selecting;                                     // Flag that we're in selection mode
    if (event->modifiers() & Qt::KeyboardModifier::ControlModifier) {
        m_items_start = scene()->selectedItems();                           // Store list of originally selected items
    } else {
        m_items_start.clear();
    }

    m_rubber_band->setGeometry(QRect(m_origin, QSize()));                   // Start selection box with no size
    m_rubber_band->show();                                                  // Make selection box visible
    update();
}


// Handles resizing Rubber Band box and updating Selection Area during View_Mode::Selecting
void SceneGraphicsView::processSelection(QPoint mouse_in_view)
{
    m_rubber_band->setGeometry(QRect(m_origin, mouse_in_view).normalized());                             // Resize selection box

    QPainterPath selectionArea;
    selectionArea.addPolygon(mapToScene(m_rubber_band->geometry()));                                    // Convert box to scene coords
    selectionArea.closeSubpath();                                                                       // Closes an open polygon

    scene()->setSelectionArea(selectionArea, Qt::ItemSelectionOperation::AddToSelection,
                              Qt::ItemSelectionMode::IntersectsItemBoundingRect, viewportTransform());  // Pass box to scene for selection

    // Go through selected items, unselect if rubber band box covered them and has since shrunk
    for (auto item : scene()->selectedItems()) {
        QRectF item_location = item->sceneBoundingRect();                                       // Get items shape
        if (scene()->selectionArea().intersects( item_location ) == false) {
            // Check if item was in the original selection list, if so mark to keep selected
            bool has_it = false;
            for (auto check_item : m_items_start) {
               if (check_item == item) has_it = true;                                                  // Item was in original selection list
            }
            // If it selected during this routine, but the Rubber Band has moved away, deselect it
            if (!has_it) { item->setSelected(false); }
        }
    }
}







