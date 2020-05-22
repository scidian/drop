//
//      Created by Stephens Nunnally on 1/15/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Handles item selection in the GraphicsScene
//
//
#include <QDebug>

#include "core/dr_math.h"
#include "editor/helper_library.h"
#include "editor/preferences.h"
#include "editor/view_editor/editor_item.h"
#include "editor/view_editor/editor_scene.h"
#include "project/dr_project.h"
#include "project/entities/dr_world.h"
#include "project/entities/dr_stage.h"
#include "project/entities/dr_thing.h"
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
void EditorScene::selectionChanged() {
    // Don't allow selection if locked
    unselectLockedItems();

    // If selection hasnt changed, return
    if (selectedItems() == m_selection_items) return;

    // Reset selection group
    resetSelectionGroup();

    // Pass on selection changes
    if (m_editor_relay != nullptr) {
        QList<long> item_keys { };
        for (auto &item : selectedItems()) {
            item_keys.append(item->data(User_Roles::Key).toLongLong());
        }

        m_editor_relay->buildInspector(item_keys);
        m_editor_relay->updateItemSelection(Editor_Widgets::Stage_View);
    }

    // !!!!! #TEMP: Testing to make sure not running non stop
    static long change_count = 0;
    Dr::SetLabelText(Label_Names::Label_Bottom, "Selection Changed: " + QString::number(change_count++));
}

// Called from selectionChanged(), resets properties of current selection. Checks if items are at resize to grid angle
void EditorScene::resetSelectionGroup() {
    m_selection_items = selectedItems();
    m_selection_scale = QPointF(1, 1);
    m_selection_angle = m_editor_relay->currentViewGridAngle();
    if (shouldEnableResizeToGrid() == false) m_selection_angle = 0;

    updateSelectionBox();
    ///m_selection_box = totalSelectionSceneRect();
}

// Checks if all selected items are at the current grid angle (to enable snapping)
bool EditorScene::shouldEnableResizeToGrid() {
    if (m_editor_relay == nullptr) return false;

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
    if (Dr::FuzzyCompare(current_view_grid_scale.x(), current_view_grid_scale.y()) == false) match_angle = false;

    return match_angle;
}

// Check that all selected items have similar (parrallel or perpendicular) angles
bool EditorScene::checkAllSelectedItemsHaveSameAngle() {
    if (m_selection_items.count() < 1) return false;
    bool have_the_same = true;
    double first_angle = m_selection_items.first()->data(User_Roles::Rotation).toDouble();

    for (auto item : m_selection_items) {
        if (Dr::IsSimilarAngle(item->data(User_Roles::Rotation).toDouble(), first_angle) == false) have_the_same = false;
    }
    return have_the_same;
}


//####################################################################################
//##    Returns a scene rect containing all the selected items
//####################################################################################
QRectF EditorScene::totalSelectionSceneRect() {
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
//##    Returns a transform representing the total changes that have been
//##        applied since selection group creation
//####################################################################################
QTransform EditorScene::getSelectionTransform() {
    QPointF center = m_selection_box.center();
    QTransform t =   QTransform()
            .translate(center.x(), center.y())
            .rotate(m_selection_angle)
            .scale(m_selection_scale.x(), m_selection_scale.y())
            .translate(-center.x(), -center.y());
    return t;
}


//####################################################################################
//##    Returns current selection as a list of Things
//####################################################################################
QList<DrThing*> EditorScene::getSelectionItemsAsThings() {
    QList<DrThing*> selected_things { };
    for (auto item : getSelectionItems()) {
        EditorItem  *graphics_item =    dynamic_cast<EditorItem*>(item);    if (graphics_item == nullptr) continue;
        DrThing     *thing =            graphics_item->getThing();          if (thing == nullptr) continue;
        selected_things.append(thing);
    }
    return selected_things;
}

//####################################################################################
//##    Creates a temporary item group to extract a new bounding box with
//##        new location and new shearing removed
//####################################################################################
void EditorScene::updateSelectionBox() {
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
QGraphicsItemGroup* EditorScene::createEmptyItemGroup(double angle, QPointF scale) {
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
//##    Selects items based on rows selected in Editor_Project_Tree
//####################################################################################
void EditorScene::updateSelectionFromProjectTree(QList<QTreeWidgetItem*> tree_list) {
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
void EditorScene::updateSelectionFromKeyList(QList<long> key_list) {
    // Turn off signals to stop recurssive calling of interface_relay->updateItemSelection()
    blockSignals(true);

    // Clear current selection
    for (auto item : selectedItems()) item->setSelected(false);

    for (auto key : key_list) {
        for (auto item : items()) {
            long item_key = item->data(User_Roles::Key).toLongLong();

            if (item_key == key) {
                DrSettings *settings = m_project->findSettingsFromKey(item_key);
                if (settings == nullptr)  continue;
                if (settings->isLocked()) continue;
                item->setSelected(true);
            }
        }
    }
    resetSelectionGroup();
    updateSelectionBox();

    blockSignals(false);
}


//####################################################################################
//##    Unselects all locked items that may have been selected
//####################################################################################
void EditorScene::unselectLockedItems() {
    // Turn off signals to stop recurssive calling of interface_relay->updateItemSelection()
    bool was_blocked = signalsBlocked();
    blockSignals(true);

    for (auto item : selectedItems()) {
        // Find referenced entity to check if locked
        DrSettings *entity = nullptr;
        EditorItem *editor_item = dynamic_cast<EditorItem*>(item);
        if (editor_item != nullptr) entity = editor_item->getThing();
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









