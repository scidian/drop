//
//      Created by Stephens Nunnally on 1/8/2019, (c) 2019 Scidian Studios, All Rights Reserved
//
//  File:
//      Graphics View Handling - View_Mode::Selecting
//
//
#include <QMouseEvent>

#include "core/colors/colors.h"
#include "editor/interface_editor_relay.h"
#include "editor/view_editor/editor_item.h"
#include "editor/view_editor/editor_scene.h"
#include "editor/view_editor/editor_view.h"
#include "editor/widgets/widgets_view.h"
#include "engine/debug_flags.h"
#include "project/dr_project.h"
#include "project/entities/dr_world.h"
#include "project/entities/dr_stage.h"
#include "project/entities/dr_thing.h"
#include "project/settings/settings.h"
#include "project/settings/settings_component.h"
#include "project/settings/settings_component_property.h"


//####################################################################################
//##    Starts selecting mode
//####################################################################################
void EditorView::startSelect(QMouseEvent *event) {
    m_items_start = my_scene->getSelectionItems();

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
//##    Handles resizing Rubber Band box and updating Selection Area during View_Mode::Selecting
//####################################################################################
void EditorView::processSelection(QPoint mouse_in_view) {
    QRect band_box = QRect(m_origin, mouse_in_view).normalized();
    if (band_box.width() < 1)  band_box.setWidth(1);
    if (band_box.height() < 1) band_box.setHeight(1);

    m_rubber_band->setGeometry(band_box);                                                           // Resize selection box

    QPainterPath selection_area;
                 selection_area.addPolygon(mapToScene(m_rubber_band->geometry()));                  // Convert box to scene coords
                 selection_area.closeSubpath();                                                     // Closes an open polygon

    QList<QGraphicsItem*> before_band = my_scene->getSelectionItems();

    // Blocks signals while we try to figure out which items should be selected
    my_scene->blockSignals(true);

    // Figure out new selection items
    my_scene->setSelectionArea(selection_area, Qt::ItemSelectionOperation::ReplaceSelection,
                               Qt::ItemSelectionMode::IntersectsItemShape, viewportTransform());     // Pass box to scene for selection
    for (auto item : m_items_keep) {
        if (item->isSelected() == false) {
            item->setSelected(true);
        }
    }

    // Re-connect signal and call selectionChanged if necessary
    my_scene->blockSignals(false);
    if (before_band != scene()->selectedItems()) my_scene->selectionChanged();
}



























