//
//      Created by Stephens Nunnally on 1/23/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//

#include "library.h"

#include "project_world_scene.h"

#include "editor_scene_item.h"
#include "editor_scene_scene.h"
#include "editor_scene_scene_undo.h"

/*
 *  To Implement:
 *
 *      - Change scene
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
void SceneGraphicsScene::undoAction() {
    m_undo->undo();
}
void SceneGraphicsScene::redoAction() {
    m_undo->redo(); 
}

void SceneGraphicsScene::newSceneSelected(DrProject *project, SceneGraphicsScene *scene, long old_scene, long new_scene)
{
    m_undo->push(new ChangeSceneCommand(m_undo, project, scene, old_scene, new_scene));
}

void SceneGraphicsScene::selectionGroupMoved(SelectionGroup *moved_group, const QPointF &old_position)
{   m_undo->push(new MoveCommand(moved_group, old_position));   }

void SceneGraphicsScene::selectionGroupNewGroup(SelectionGroup *moved_group, QList<QGraphicsItem*> old_list, QList<QGraphicsItem*> new_list,
                                                QGraphicsItem *old_first, QGraphicsItem *new_first)
{   m_undo->push(new SelectionNewGroupCommand(moved_group, old_list, new_list, old_first, new_first));    }



//####################################################################################
//##        Move Command on the QUndoStack
//####################################################################################
ChangeSceneCommand::ChangeSceneCommand(QUndoStack *undo_stack, DrProject *project, SceneGraphicsScene *scene,
                                       long old_scene, long new_scene, QUndoCommand *parent) :
                                       QUndoCommand(parent), m_undo_stack(undo_stack), m_project(project), m_scene(scene)
{
    m_new_scene = new_scene;
    m_old_scene = old_scene;
}

void ChangeSceneCommand::undo() {
    QString new_scene_name = changeScene(m_old_scene, true);
    setText( new_scene_name );
}

void ChangeSceneCommand::redo() {
    QString new_scene_name = changeScene(m_new_scene, false);
    setText( new_scene_name );
}

QString ChangeSceneCommand::changeScene(long new_scene, bool is_undo)
{
    DrScene *from_scene = m_project->findSceneFromKey(new_scene);
    if (from_scene == nullptr) {
        ///m_undo_stack->setIndex(m_undo_stack->index());
        return "Could not change scenes!";
    }

    m_scene->clear();
    m_scene->createSelectionGroup();
    m_scene->setCurrentSceneShown(m_new_scene);

    int z_order = 0;
    for (auto object : from_scene->getObjectMap()) {
        DrItem *item = new DrItem(m_project, from_scene, object.first);
        m_scene->setPositionByOrigin(item, item->getOrigin(), item->startX(), item->startY());
        m_scene->addItem(item);
        z_order++;
    }

    m_scene->update();
    m_scene->updateView();
    if (is_undo)
        return "Redo Select Scene " + from_scene->getSceneName();
    else
        return "Undo Select Scene " + from_scene->getSceneName();
}



//####################################################################################
//##        Move Command on the QUndoStack
//####################################################################################
MoveCommand::MoveCommand(SelectionGroup *group, const QPointF &old_pos, QUndoCommand *parent) : QUndoCommand(parent) {
    m_group = group;
    m_new_pos = group->sceneTransform().map(group->boundingRect().center());
    m_old_pos = old_pos;
}

void MoveCommand::undo() {
    m_group->getParentScene()->setPositionByOrigin(m_group, Position_Flags::Center, m_old_pos.x(), m_old_pos.y());
    m_group->getParentScene()->updateView();
    QString item_text = "Items";
    if (m_group->childItems().count() == 1)
        item_text = "Item " + m_group->childItems().first()->data(User_Roles::Name).toString();
    setText(QObject::tr("Undo Move %1 to (%2, %3)")
            .arg(item_text)
            .arg(Dr::RemoveTrailingDecimals(m_new_pos.x(), 1))
            .arg(Dr::RemoveTrailingDecimals(m_new_pos.y(), 1)));
}

void MoveCommand::redo() {
    m_group->getParentScene()->setPositionByOrigin(m_group, Position_Flags::Center, m_new_pos.x(), m_new_pos.y());
    m_group->getParentScene()->updateView();
    QString item_text = "Items";
    if (m_group->childItems().count() == 1)
        item_text = "Item " + m_group->childItems().first()->data(User_Roles::Name).toString();
    setText(QObject::tr("Undo Move %1 to (%2, %3)")
            .arg(item_text)
            .arg(Dr::RemoveTrailingDecimals(m_new_pos.x(), 1))
            .arg(Dr::RemoveTrailingDecimals(m_new_pos.y(), 1)));

}



//####################################################################################
//##        Deselects old items, Selects one new item
//####################################################################################
SelectionNewGroupCommand::SelectionNewGroupCommand(SelectionGroup *group,
                                                   QList<QGraphicsItem*> old_list, QList<QGraphicsItem*> new_list,
                                                   QGraphicsItem *old_first, QGraphicsItem *new_first,
                                                   QUndoCommand *parent) : QUndoCommand(parent) {
    m_group = group;
    m_old_list = old_list;
    m_new_list = new_list;
    m_old_first_selected = old_first;
    m_new_first_selected = new_first;
}

void SelectionNewGroupCommand::undo() {
    m_group->getParentScene()->emptySelectionGroup();
    m_group->getParentScene()->setFirstSelectedItem(m_old_first_selected);
    for (auto item : m_old_list) m_group->getParentScene()->addItemToSelectionGroup(item);
    m_group->getParentScene()->updateSceneTreeSelection();
    m_group->getParentScene()->updateView();
    if (m_new_list.count() > 1)
        setText("Redo Change Selection");
    else if (m_new_list.count() == 1)
        setText("Redo New Item Selected: " + m_new_list.first()->data(User_Roles::Name).toString() );
    else
        setText("Redo Select None");
}

void SelectionNewGroupCommand::redo() {
    m_group->getParentScene()->emptySelectionGroup();
    m_group->getParentScene()->setFirstSelectedItem(m_new_first_selected);
    for (auto item : m_new_list)
        m_group->getParentScene()->addItemToSelectionGroup(item);
    m_group->getParentScene()->updateSceneTreeSelection();
    m_group->getParentScene()->updateView();
    if (m_new_list.count() > 1)
        setText("Undo Change Selection");
    else if (m_new_list.count() == 1)
        setText("Undo New Item Selected: " + m_new_list.first()->data(User_Roles::Name).toString() );
    else
        setText("Undo Select None");
}










