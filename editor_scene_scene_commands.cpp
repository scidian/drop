//
//      Created by Stephens Nunnally on 1/23/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//

#include "editor_scene_scene.h"

//undoStack->push(new MoveCommand(movedItem, oldPosition));


// Signal
void SceneGraphicsScene::itemMoved(DrItem *moved_item, const QPointF &old_position)
{
    m_undo->push(new MoveCommand(moved_item, old_position));
}
