//
//      Created by Stephens Nunnally on 1/23/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//

#include "library.h"

#include "project_world_scene.h"
#include "project_world_scene_object.h"

#include "editor_scene_item.h"
#include "editor_scene_scene.h"
#include "editor_scene_scene_commands.h"

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
    m_undo->push(new ChangeSceneCommand(project, scene, old_scene, new_scene));
}

void SceneGraphicsScene::selectionGroupMoved(SceneGraphicsScene *scene, const QPointF &old_position)
{   m_undo->push(new MoveCommand(scene, old_position));   }

void SceneGraphicsScene::selectionGroupNewGroup(SceneGraphicsScene *scene,
                                                QList<DrObject*> old_list,
                                                QList<DrObject*> new_list,
                                                DrObject *old_first,
                                                DrObject *new_first)
{   m_undo->push(new SelectionNewGroupCommand(scene, old_list, new_list, old_first, new_first));    }



//####################################################################################
//##        Move Command on the QUndoStack
//####################################################################################
ChangeSceneCommand::ChangeSceneCommand(DrProject *project, SceneGraphicsScene *scene,
                                       long old_scene, long new_scene, QUndoCommand *parent) :
                                       QUndoCommand(parent), m_project(project), m_scene(scene)
{
    m_new_scene = new_scene;
    m_old_scene = old_scene;
}

void ChangeSceneCommand::undo() {
    QString new_scene_name = changeScene(m_new_scene, m_old_scene, true);
    setText( new_scene_name );
}

void ChangeSceneCommand::redo() {
    QString new_scene_name = changeScene(m_old_scene, m_new_scene, false);
    setText( new_scene_name );
}

QString ChangeSceneCommand::changeScene(long old_scene, long new_scene, bool is_undo)
{
    // Remove any references within the current project scene objects to any GraphicsScene items
    DrScene *displayed = m_project->findSceneFromKey(old_scene);
    if (displayed) {
        for (auto object_pair : displayed->getObjectMap()) {
            object_pair.second->setDrItem(nullptr);
        }
    }

    // Load scene we're changing to
    DrScene *from_scene = m_project->findSceneFromKey(new_scene);
    if (from_scene == nullptr) {
        return "Redo Select Scene " + displayed->getSceneName();
    }

    m_scene->clear();
    m_scene->createSelectionGroup();
    m_scene->setCurrentSceneShown(from_scene);
    m_scene->setCurrentSceneKeyShown(m_new_scene);

    for (auto object_pair : from_scene->getObjectMap()) {
        DrItem *item = new DrItem(m_project, object_pair.second);
        m_scene->setPositionByOrigin(item, item->getOrigin(), item->startX(), item->startY());
        m_scene->addItem(item);

        object_pair.second->setDrItem(item);
    }

    m_scene->getRelay()->centerViewOn(from_scene->getViewCenterPoint());
    m_scene->update();
    m_scene->updateView();
    if (is_undo)
        return "Redo Select Scene " + displayed->getSceneName();
    else
        return "Undo Select Scene " + from_scene->getSceneName();
}



//####################################################################################
//##        Move Command on the QUndoStack
//####################################################################################
MoveCommand::MoveCommand(SceneGraphicsScene *scene, const QPointF &old_pos, QUndoCommand *parent) : QUndoCommand(parent) {
    m_scene = scene;
    m_new_pos = m_scene->getSelectionGroup()->sceneTransform().map(m_scene->getSelectionGroup()->boundingRect().center());
    m_old_pos = old_pos;
}

void MoveCommand::undo() {
    m_scene->setPositionByOrigin(m_scene->getSelectionGroup(), Position_Flags::Center, m_old_pos.x(), m_old_pos.y());
    m_scene->updateChildrenPositionData();
    m_scene->updateView();
    QString item_text = "Items";
    if (m_scene->getSelectionGroup()->childItems().count() == 1)
        item_text = "Item " + m_scene->getSelectionGroup()->childItems().first()->data(User_Roles::Name).toString();
    setText(QObject::tr("Undo Move %1 to (%2, %3)")
            .arg(item_text)
            .arg(Dr::RemoveTrailingDecimals(m_new_pos.x(), 1))
            .arg(Dr::RemoveTrailingDecimals(m_new_pos.y(), 1)));
}

void MoveCommand::redo() {
    m_scene->setPositionByOrigin(m_scene->getSelectionGroup(), Position_Flags::Center, m_new_pos.x(), m_new_pos.y());
    m_scene->updateChildrenPositionData();
    m_scene->updateView();
    QString item_text = "Items";
    if (m_scene->getSelectionGroup()->childItems().count() == 1)
        item_text = "Item " + m_scene->getSelectionGroup()->childItems().first()->data(User_Roles::Name).toString();
    setText(QObject::tr("Undo Move %1 to (%2, %3)")
            .arg(item_text)
            .arg(Dr::RemoveTrailingDecimals(m_new_pos.x(), 1))
            .arg(Dr::RemoveTrailingDecimals(m_new_pos.y(), 1)));

}


//####################################################################################
//##        Deselects old items, Selects one new item
//####################################################################################
SelectionNewGroupCommand::SelectionNewGroupCommand(SceneGraphicsScene *scene,
                                                   QList<DrObject*> old_list,
                                                   QList<DrObject*> new_list,
                                                   DrObject *old_first,
                                                   DrObject *new_first,
                                                   QUndoCommand *parent) : QUndoCommand(parent) {
    m_scene = scene;
    m_old_list = old_list;
    m_new_list = new_list;
    m_old_first_selected = old_first;
    m_new_first_selected = new_first;
}

void SelectionNewGroupCommand::undo() {
    m_scene->emptySelectionGroup();
    m_scene->setFirstSelectedItem(m_old_first_selected);

    for (auto object : m_old_list) m_scene->addItemToSelectionGroup(object->getDrItem());

    m_scene->updateSceneTreeSelection();
    m_scene->updateView();
    if (m_new_list.count() > 1)
        setText("Redo Change Selection");
    else if (m_new_list.count() == 1)
        setText("Redo New Item Selected: " + m_new_list.first()->getDrItem()->data(User_Roles::Name).toString() );
    else
        setText("Redo Select None");
}

void SelectionNewGroupCommand::redo() {
    m_scene->emptySelectionGroup();
    m_scene->setFirstSelectedItem(m_new_first_selected);

    for (auto object : m_new_list) m_scene->addItemToSelectionGroup(object->getDrItem());

    m_scene->updateSceneTreeSelection();
    m_scene->updateView();
    if (m_new_list.count() > 1)
        setText("Undo Change Selection");
    else if (m_new_list.count() == 1)
        setText("Undo New Item Selected: " + m_new_list.first()->getDrItem()->data(User_Roles::Name).toString() );
    else
        setText("Undo Select None");
}










