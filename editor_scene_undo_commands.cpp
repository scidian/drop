//
//      Created by Stephens Nunnally on 1/23/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "editor_item.h"
#include "editor_scene.h"
#include "editor_scene_undo_commands.h"

#include "globals.h"
#include "library.h"

#include "project.h"
#include "project_world_stage.h"
#include "project_world_stage_object.h"


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
//##        NOTE: Push function executes redo() function of command when called
//##
//##        SLOT, Needs to be connected to
//##            - Mouse release on View
//##            - Arrow key press on scene
//####################################################################################
void DrScene::undoAction() {
    m_undo->undo();
}
void DrScene::redoAction() {
    m_undo->redo(); 
}

void DrScene::newStageSelected(DrProject *project, DrScene *scene, long old_stage, long new_stage)
{
    m_undo->push(new ChangeStageCommand(project, scene, old_stage, new_stage));
}

void DrScene::selectionGroupMoved(DrScene *scene, const QPointF &old_position)
{   m_undo->push(new MoveCommand(scene, old_position));   }

void DrScene::selectionGroupNewGroup(DrScene *scene,
                                                QList<DrObject*> old_list,
                                                QList<DrObject*> new_list)
{   m_undo->push(new SelectionNewGroupCommand(scene, old_list, new_list));    }



//####################################################################################
//##        Move Command on the QUndoStack
//####################################################################################
ChangeStageCommand::ChangeStageCommand(DrProject *project, DrScene *scene,
                                       long old_stage, long new_stage, QUndoCommand *parent) :
                                       QUndoCommand(parent), m_project(project), m_scene(scene)
{
    m_new_stage = new_stage;
    m_old_stage = old_stage;
}

void ChangeStageCommand::undo() {
    QString new_stage_name = changeStage(m_new_stage, m_old_stage, true);
    setText( new_stage_name );
}

void ChangeStageCommand::redo() {
    QString new_stage_name = changeStage(m_old_stage, m_new_stage, false);
    setText( new_stage_name );
}

QString ChangeStageCommand::changeStage(long old_stage, long new_stage, bool is_undo)
{
    // Remove any references within the current project stage objects to any GraphicsScene items
    DrStage *displayed = m_project->findStageFromKey(old_stage);
    if (displayed) displayed->removeGraphicsItemReferences();

    // Load stage we're changing to
    DrStage *from_stage = m_project->findStageFromKey(new_stage);
    if (from_stage == nullptr) {
        return "Redo Select Stage " + displayed->getStageName();
    }

    // Calculate new scene and view rects, clear scene and set sizes
    QRectF new_scene_rect(-1000, -1000, 2000, 2000);
    QRectF new_view_rect( -4000, -4000, 8000, 8000);
    QPointF adjust;

    switch (m_project->getOptionOrientation()) {
    case Orientation::Portrait:     adjust = QPointF(400, -800);    break;
    case Orientation::Landscape:    adjust = QPointF(800, -400);    break;
    }
    new_scene_rect.adjust(adjust.x(), adjust.y(), adjust.x(), adjust.y());
    new_view_rect.adjust( adjust.x(), adjust.y(), adjust.x(), adjust.y());

    m_scene->clear();
    m_scene->setSceneRect(new_scene_rect);
    m_scene->clearViewSceneRect(new_view_rect);
    m_scene->setCurrentStageShown(from_stage);
    m_scene->setCurrentStageKeyShown(m_new_stage);

    // Load all our objects from data model into QGraphicsItems
    for (auto object_pair : from_stage->getObjectMap()) {
        m_scene->addItemToSceneFromObject(object_pair.second);
    }

    m_scene->update();
    m_scene->updateAlignmentGrid();
    m_scene->updateView();

    // Center the view on the new stage
    QPointF new_center = from_stage->getViewCenterPoint();
    if (new_center == QPointF(0, 0)) new_center = new_view_rect.center();
    m_scene->getRelay()->centerViewOnPoint( new_center );

    // Set Undo / Redo text
    if (is_undo)    return "Redo Select Stage " + displayed->getStageName();
    else            return "Undo Select Stage " + from_stage->getStageName();
}

DrItem* DrScene::addItemToSceneFromObject(DrObject *object)
{
    // Create new item representing this object
    DrItem *item = new DrItem(m_project, this->getRelay(), object);

    // Temporarily disable geometry signal itemChange updates
    item->disableItemChangeFlags();

    // Add item to scene, set starting position
    this->addItem(item);
    this->setPositionByOrigin(item, Position_Flags::Center, item->startX(), item->startY());

    // Create a temporary group and destroy it, this causes some unknown but importatnt changes to the items
    // sceneTransform that we really seem to need before we try to move a scaled item with View_Mode::Translating
    QGraphicsItemGroup *group = this->createItemGroup({ item });
    this->destroyItemGroup(group);

    // Re enable geometry signal itemChange() updates
    item->enableItemChangeFlags();

    // Assign this QGraphicsItem we just made to the object in the project so it can reference it later
    object->setDrItem(item);

    return item;
}



//####################################################################################
//##        Move Command on the QUndoStack
//####################################################################################
MoveCommand::MoveCommand(DrScene *scene, const QPointF &old_pos, QUndoCommand *parent) : QUndoCommand(parent) {
    m_scene = scene;
//    m_new_pos = m_scene->getSelectionGroup()->sceneTransform().map(m_scene->getSelectionGroup()->boundingRect().center());
    m_old_pos = old_pos;
}

void MoveCommand::undo() {
//    m_scene->setPositionByOrigin(m_scene->getSelectionGroup(), Position_Flags::Center, m_old_pos.x(), m_old_pos.y());
//    //DELETED: changes now happen through DrItem::itemChange() --- m_scene->updateSelectedItemsPositionData();
//    m_scene->updateView();
//    QString item_text = "Items";
//    if (m_scene->getSelectionGroup()->childItems().count() == 1)
//        item_text = "Item " + m_scene->getSelectionGroup()->childItems().first()->data(User_Roles::Name).toString();
//    setText(QObject::tr("Undo Move %1 to (%2, %3)")
//            .arg(item_text)
//            .arg(Dr::RemoveTrailingDecimals(m_new_pos.x(), 1))
//            .arg(Dr::RemoveTrailingDecimals(m_new_pos.y(), 1)));
}

void MoveCommand::redo() {
//    m_scene->setPositionByOrigin(m_scene->getSelectionGroup(), Position_Flags::Center, m_new_pos.x(), m_new_pos.y());
//    //DELETED: changes now happen through DrItem::itemChange() --- m_scene->updateSelectedItemsPositionData();
//    m_scene->updateView();
//    QString item_text = "Items";
//    if (m_scene->getSelectionGroup()->childItems().count() == 1)
//        item_text = "Item " + m_scene->getSelectionGroup()->childItems().first()->data(User_Roles::Name).toString();
//    setText(QObject::tr("Undo Move %1 to (%2, %3)")
//            .arg(item_text)
//            .arg(Dr::RemoveTrailingDecimals(m_new_pos.x(), 1))
//            .arg(Dr::RemoveTrailingDecimals(m_new_pos.y(), 1)));

}


//####################################################################################
//##        Deselects old items, Selects one new item
//####################################################################################
SelectionNewGroupCommand::SelectionNewGroupCommand(DrScene *scene,
                                                   QList<DrObject*> old_list,
                                                   QList<DrObject*> new_list,
                                                   QUndoCommand *parent) : QUndoCommand(parent) {
    m_scene = scene;
    m_old_list = old_list;
    m_new_list = new_list;
}

void SelectionNewGroupCommand::undo() {
//    m_scene->emptySelectionGroup();

//    for (auto object : m_old_list) m_scene->addItemToSelectionGroup(object->getDrItem());

//    m_scene->updateStageTreeSelection();
//    m_scene->updateView();
//    if (m_new_list.count() > 1)
//        setText("Redo Change Selection");
//    else if (m_new_list.count() == 1)
//        setText("Redo New Item Selected: " + m_new_list.first()->getDrItem()->data(User_Roles::Name).toString() );
//    else
//        setText("Redo Select None");
}

void SelectionNewGroupCommand::redo() {
//    m_scene->emptySelectionGroup();

//    for (auto object : m_new_list) m_scene->addItemToSelectionGroup(object->getDrItem());

//    m_scene->updateStageTreeSelection();
//    m_scene->updateView();
//    if (m_new_list.count() > 1)
//        setText("Undo Change Selection");
//    else if (m_new_list.count() == 1)
//        setText("Undo New Item Selected: " + m_new_list.first()->getDrItem()->data(User_Roles::Name).toString() );
//    else
//        setText("Undo Select None");
}










