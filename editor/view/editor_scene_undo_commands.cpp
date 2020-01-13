//
//      Created by Stephens Nunnally on 1/23/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QDebug>

#include "editor/helper_library.h"
#include "editor/preferences.h"
#include "editor/view/editor_item.h"
#include "editor/view/editor_scene.h"
#include "editor/view/editor_scene_undo_commands.h"
#include "editor/view/editor_view.h"
#include "project/dr_project.h"
#include "project/entities/dr_world.h"
#include "project/entities/dr_stage.h"
#include "project/entities/dr_thing.h"


/*
 *  To Implement:
 *
 *      - Change stage
 *
 *      - Moving Item: Key press
 *
 *      - Geometry Changes: Rotation, Scaling       !! USE MERGE??
 *
 *      - Delete item
 *
 *      - Add item
 */

//####################################################################################
//##    #NOTE: Push function executes redo() function of command when called
//##
//##    SLOT, Needs to be connected to
//##        - Mouse release on View
//##        - Arrow key press on scene
//####################################################################################
void DrScene::undoAction() {
    m_undo->undo();
}
void DrScene::redoAction() {
    m_undo->redo(); 
}

void DrScene::newStageSelected(DrProject *project, DrScene *scene, long old_stage, long new_stage) {
    m_undo->push(new UndoCommandChangeStage(project, scene, old_stage, new_stage));
}

void DrScene::selectionGroupMoved(DrScene *scene, const QPointF &old_position) {
    m_undo->push(new UndoCommandMove(scene, old_position));
}

void DrScene::selectionGroupNewGroup(DrScene *scene, QList<DrThing*> old_list, QList<DrThing*> new_list) {
    m_undo->push(new UndoCommandNewSelection(scene, old_list, new_list));
}



//####################################################################################
//##        Move Command on the QUndoStack
//####################################################################################
UndoCommandChangeStage::UndoCommandChangeStage(DrProject *project, DrScene *scene,
                                       long old_stage_key, long new_stage_key, QUndoCommand *parent) :
                                       QUndoCommand(parent), m_project(project), m_scene(scene) {
    m_new_stage_key = new_stage_key;
    m_old_stage_key = old_stage_key;
}

void UndoCommandChangeStage::undo() {
    QString new_stage_name = changeStage(m_new_stage_key, m_old_stage_key, true);
    setText( new_stage_name );
}

void UndoCommandChangeStage::redo() {
    QString new_stage_name = changeStage(m_old_stage_key, m_new_stage_key, false);
    setText( new_stage_name );
}

QString UndoCommandChangeStage::changeStage(long old_stage_key, long new_stage_key, bool is_undo) {
    ///qDebug() << "Old Stage: " << old_stage_key << ", New Stage: " << new_stage_key;

    // ***** Remove any references within the current project stage things to any GraphicsScene items
    DrStage *displayed = m_project->findStageFromKey(old_stage_key);
    QString redo_name = "ERROR";
    if (displayed) {
        displayed->removeGraphicsItemReferences();
        redo_name = QString::fromStdString(displayed->getName());
    }

    // ***** Load stage we're changing to
    DrStage *new_stage = m_project->findStageFromKey(new_stage_key);
    if (new_stage == nullptr) {
        return "Redo Select Stage " + redo_name;
    }
    new_stage->getParentWorld()->setLastStageShownKey(new_stage_key);


    // ***** Calculate new scene and view rects, clear scene and set sizes
    double half_height;
    QRectF new_scene_rect = DrView::stageBoundingRect(m_project, new_stage, half_height).adjusted(-500, -500, 500, 500);
    QRectF new_view_rect = new_scene_rect.adjusted(-5000, -5000, 5000, 5000);

    m_scene->clear();
    m_scene->setSceneRect(new_scene_rect);

    m_scene->setCurrentStageShown(new_stage);
    m_scene->setCurrentStageKeyShown(m_new_stage_key);
    m_project->setOption(Project_Options::Current_World, new_stage->getParentWorld()->getKey());
    m_project->setOption(Project_Options::Current_Stage, new_stage->getKey());

    emit m_scene->setViewRect(new_view_rect);
    emit m_scene->updateGrid();

    // Load all our Things from data model into QGraphicsItems
    for (auto thing_pair : new_stage->getThingMap()) {
        m_scene->addItemToSceneFromThing(thing_pair.second);
    }


    // ***** Center the view on the new stage
    double  new_zoom_scale = new_stage->getViewZoomLevel();
    DrPointF new_center = new_stage->getViewCenterPoint();
    if (new_center == DrPointF(0, 0)) {
        double half_height;
        new_center = Dr::FromQPointF(DrView::stageBoundingRect(m_project, new_stage, half_height).center());
        new_stage->setViewCenterPoint( new_center );
    }
    m_scene->getEditorRelay()->viewCenterOnPoint( Dr::ToQPointF(new_center) );
    m_scene->getEditorRelay()->viewZoomToScale( new_zoom_scale );
    m_scene->getEditorRelay()->updateItemSelection(Editor_Widgets::Project_Tree);

    // ***** Set Undo / Redo text
    if (is_undo)    return "Redo Select Stage " + redo_name;
    else            return "Undo Select Stage " + QString::fromStdString(new_stage->getName());
}

DrItem* DrScene::addItemToSceneFromThing(DrThing *thing) {
    // Create new item representing this Thing
    DrItem *item = new DrItem(m_project, this->getEditorRelay(), thing);

    // Temporarily disable geometry signal itemChange updates
    bool flags_enabled_before = item->itemChangeFlagsEnabled();
    item->disableItemChangeFlags();

    // Add item to scene, set starting position
    this->addItem(item);
    this->setPositionByOrigin(item, Position_Flags::Center, item->startX(), item->startY());

    // Create a temporary group and destroy it, this causes some unknown but importatnt changes to the items
    // sceneTransform that we really seem to need before we try to move a scaled item with View_Mode::Translating
    QGraphicsItemGroup *group = this->createItemGroup({ item });
    this->destroyItemGroup(group);

    // Re enable geometry signal itemChange() updates
    if (flags_enabled_before) item->enableItemChangeFlags();

    // Assign this QGraphicsItem we just made to the Thing in the project so it can reference it later
    thing->setDrItem(item);

    return item;
}



//####################################################################################
//##    Move Command on the QUndoStack
//####################################################################################
UndoCommandMove::UndoCommandMove(DrScene *scene, const QPointF &old_pos, QUndoCommand *parent) : QUndoCommand(parent) {
    m_scene = scene;
//    m_new_pos = m_scene->getSelectionGroup()->sceneTransform().map(m_scene->getSelectionGroup()->boundingRect().center());
    m_old_pos = old_pos;
}

void UndoCommandMove::undo() {
//    m_scene->setPositionByOrigin(m_scene->getSelectionGroup(), Position_Flags::Center, m_old_pos.x(), m_old_pos.y());
//    //DELETED: changes now happen through DrItem::itemChange() --- m_scene->updateSelectedItemsPositionData();
//    emit m_scene->updateViews();
//    QString item_text = "Items";
//    if (m_scene->getSelectionGroup()->childItems().count() == 1)
//        item_text = "Item " + m_scene->getSelectionGroup()->childItems().first()->data(User_Roles::Name).toString();
//    setText(QObject::tr("Undo Move %1 to (%2, %3)")
//            .arg(item_text)
//            .arg(Dr::RemoveTrailingDecimals(m_new_pos.x(), 1))
//            .arg(Dr::RemoveTrailingDecimals(m_new_pos.y(), 1)));
}

void UndoCommandMove::redo() {
//    m_scene->setPositionByOrigin(m_scene->getSelectionGroup(), Position_Flags::Center, m_new_pos.x(), m_new_pos.y());
//    //DELETED: changes now happen through DrItem::itemChange() --- m_scene->updateSelectedItemsPositionData();
//    emit m_scene->updateViews();
//    QString item_text = "Items";
//    if (m_scene->getSelectionGroup()->childItems().count() == 1)
//        item_text = "Item " + m_scene->getSelectionGroup()->childItems().first()->data(User_Roles::Name).toString();
//    setText(QObject::tr("Undo Move %1 to (%2, %3)")
//            .arg(item_text)
//            .arg(Dr::RemoveTrailingDecimals(m_new_pos.x(), 1))
//            .arg(Dr::RemoveTrailingDecimals(m_new_pos.y(), 1)));

}


//####################################################################################
//##    Deselects old items, Selects one new item
//####################################################################################
UndoCommandNewSelection::UndoCommandNewSelection(DrScene *scene,
                                                   QList<DrThing*> old_list,
                                                   QList<DrThing*> new_list,
                                                   QUndoCommand *parent) : QUndoCommand(parent) {
    m_scene = scene;
    m_old_list = old_list;
    m_new_list = new_list;
}

void UndoCommandNewSelection::undo() {
//    m_scene->emptySelectionGroup();

//    for (auto thing : m_old_list) m_scene->addItemToSelectionGroup(thing->getDrItem());

//    m_scene->updateStageTreeSelection();
//    emit m_scene->updateViews();
//    if (m_new_list.count() > 1)
//        setText("Redo Change Selection");
//    else if (m_new_list.count() == 1)
//        setText("Redo New Item Selected: " + m_new_list.first()->getDrItem()->data(User_Roles::Name).toString() );
//    else
//        setText("Redo Select None");
}

void UndoCommandNewSelection::redo() {
//    m_scene->emptySelectionGroup();

//    for (auto thing : m_new_list) m_scene->addItemToSelectionGroup(thing->getDrItem());

//    m_scene->updateStageTreeSelection();
//    emit m_scene->updateViews();
//    if (m_new_list.count() > 1)
//        setText("Undo Change Selection");
//    else if (m_new_list.count() == 1)
//        setText("Undo New Item Selected: " + m_new_list.first()->getDrItem()->data(User_Roles::Name).toString() );
//    else
//        setText("Undo Select None");
}










