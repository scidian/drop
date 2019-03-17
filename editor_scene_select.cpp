//
//      Created by Stephens Nunnally on 1/15/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Handles item selection in the GraphicsScene
//
//
#include "editor_item.h"
#include "editor_scene.h"
#include "globals.h"
#include "library.h"
#include "project.h"
#include "project_world.h"
#include "project_world_stage.h"
#include "project_world_stage_object.h"
#include "settings.h"
#include "settings_component.h"
#include "settings_component_property.h"

//####################################################################################
//##        SLOT: selectionChanged,
//##              connected from built in Qt SIGNAL DrScene::selectionChanged()
//####################################################################################
//  QList<QGraphicsItem*>   m_selection_items;          // List of selected items
//  double                  m_selection_angle;          // Angle current selection has been rotated to
//  QPointF                 m_selection_scale;          // Scaling applied to current selection
//  QRectF                  m_selection_box;            // Starting outline of selected items
void DrScene::selectionChanged()
{
    if (selectedItems() == m_selection_items) return;

    resetSelectionGroup();

    if (m_editor_relay) {
        QList<long> item_keys { };
        for (auto item : selectedItems())
            item_keys.append(item->data(User_Roles::Key).toLongLong());

        m_editor_relay->buildObjectInspector(item_keys);
        m_editor_relay->updateItemSelection(Editor_Widgets::Scene_View);
    }
}

// Called from selectionChanged(), resets properties of current selection. Checks if items are at resize to grid angle
void DrScene::resetSelectionGroup()
{
    m_selection_items = selectedItems();
    m_selection_scale = QPointF(1, 1);
    m_selection_angle = m_editor_relay->currentViewGridAngle();
    if (shouldEnableResizeToGrid() == false) m_selection_angle = 0;

    updateSelectionBox();
    ///m_selection_box = totalSelectionSceneRect();
}

// Checks if all selected items are at the current grid angle (to enable snapping)
bool DrScene::shouldEnableResizeToGrid()
{
    bool   match_angle = true;
    double current_view_grid_angle = m_editor_relay->currentViewGridAngle();

    // Check that all selected items match grid angle
    for (auto item : m_selection_items) {
        if (Dr::IsSimilarAngle(item->data(User_Roles::Rotation).toDouble(), current_view_grid_angle) == false) match_angle = false;
    }

    // Check that selectiopn angle matches grid angle
    if (Dr::IsSimilarAngle(m_selection_angle, current_view_grid_angle) == false) match_angle = false;

    // Check that grid is squared
    QPointF current_view_grid_scale = m_editor_relay->currentViewGridScale();
    if (qFuzzyCompare(current_view_grid_scale.x(), current_view_grid_scale.y()) == false) match_angle = false;

    return match_angle;
}

// Check that all selected items have similar (parrallel or perpendicular) angles
bool DrScene::checkAllSelectedItemsHaveSameAngle()
{
    if (m_selection_items.count() < 1) return false;
    bool have_the_same = true;
    double first_angle = m_selection_items.first()->data(User_Roles::Rotation).toDouble();

    for (auto item : m_selection_items) {
        if (Dr::IsSimilarAngle(item->data(User_Roles::Rotation).toDouble(), first_angle) == false) have_the_same = false;
    }
    return have_the_same;
}


//####################################################################################
//##        Returns a scene rect containing all the selected items
//####################################################################################
QRectF DrScene::totalSelectionSceneRect()
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

//####################################################################################
//##        Returns a transform representing the total changes that have been
//##        applied since selection group creation
//####################################################################################
QTransform DrScene::getSelectionTransform()
{
    QPointF center = m_selection_box.center();
    QTransform t =   QTransform()
            .translate(center.x(), center.y())
            .rotate(m_selection_angle)
            .scale(m_selection_scale.x(), m_selection_scale.y())
            .translate(-center.x(), -center.y());
    return t;
}

// Creates a temporary item group to extract a new bounding box with new location and new shearing removed
void DrScene::updateSelectionBox()
{
    // Recreate selection bounding box based on new item locations
    double  angle = getSelectionAngle();
    QPointF scale = getSelectionScale();

    QGraphicsItemGroup *group = createEmptyItemGroup(angle);
    for (auto item : this->getSelectionItems()) {
        group->addToGroup(item);
    }

    QPointF top_left =   group->sceneTransform().map( group->boundingRect().topLeft() );
    QPointF bot_right =  group->sceneTransform().map( group->boundingRect().bottomRight() );
    QPointF map_center = group->sceneTransform().map( group->boundingRect().center() );

    QTransform t = QTransform()
            .translate(map_center.x(), map_center.y())
            .scale(1 / scale.x(), 1 / scale.y())
            .rotate(-angle)
            .translate(-map_center.x(), -map_center.y());

    top_left =  t.map(top_left);
    bot_right = t.map(bot_right);
    QRectF new_box = QRectF(top_left, bot_right);
    this->setSelectionBox( new_box );

    this->destroyItemGroup(group);
}


// Creates an empty QGraphicsItemGroup at angle starting angle
QGraphicsItemGroup* DrScene::createEmptyItemGroup(double angle, QPointF scale)
{
    QGraphicsItemGroup *group = new QGraphicsItemGroup();
    addItem(group);
    QPointF    center = group->boundingRect().center();
    double transform_scale_x = Dr::CheckScaleNotZero(scale.x());
    double transform_scale_y = Dr::CheckScaleNotZero(scale.y());
    QTransform transform = QTransform()
            .translate(center.x(), center.y())
            .rotate(angle)
            .scale(transform_scale_x, transform_scale_y)
            .translate(-center.x(), -center.y());
    group->setTransform(transform);
    return group;
}


//####################################################################################
//##        Selects items based on rows selected in Editor_Project_Tree
//####################################################################################
void DrScene::updateSelectionFromProjectTree(QList<QTreeWidgetItem*> tree_list)
{
    // Turn off signals to stop recurssive calling of interface_relay->updateItemSelection()
    blockSignals(true);

    // Clear current selection
    for (auto item : selectedItems()) item->setSelected(false);

    for (auto row : tree_list) {
        long row_key = row->data(0, User_Roles::Key).toLongLong();

        for (auto item : items()) {
            long item_key = item->data(User_Roles::Key).toLongLong();

            if (item_key == row_key) item->setSelected(true);
        }
    }
    resetSelectionGroup();
    updateSelectionBox();

    blockSignals(false);
}














