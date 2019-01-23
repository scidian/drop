//
//      Created by Stephens Nunnally on 1/23/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//

#include "editor_scene_scene.h"
#include "editor_scene_scene_commands.h"


//emit selectionGroupMoved(qgraphicsitem_cast<DiagramItem *>(movingItem), oldPos);

//####################################################################################
//##        SLOT, Needs to be connected to
//##            - Mouse release on View
//##            - Arrow key press on scene
//####################################################################################
void SceneGraphicsScene::selectionGroupMoved(SelectionGroup *moved_group, const QPointF &old_position)
{
    // !!!!! NOTE: Push executes redo() function of command
    m_undo->push(new MoveCommand(moved_group, old_position));
}


void SceneGraphicsScene::selectionGroupAddItem(SelectionGroup *moved_group, QGraphicsItem *new_item)
{
    m_undo->push(new SelectionAddItemCommand(moved_group, new_item));
}


void SceneGraphicsScene::selectionGroupEmpty(SelectionGroup *moved_group, QList<QGraphicsItem*> old_list)
{
    m_undo->push(new SelectionEmptyCommand(moved_group, old_list));
}


void SceneGraphicsScene::selectionGroupNewGroup(SelectionGroup *moved_group, QList<QGraphicsItem*> old_list, QGraphicsItem *new_item)
{
    m_undo->push(new SelectionNewGroupCommand(moved_group, old_list, new_item));
}

