//
//      Created by Stephens Nunnally on 1/23/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//

#include "editor_scene_scene.h"



//emit itemMoved(qgraphicsitem_cast<DiagramItem *>(movingItem), oldPos);

//####################################################################################
//##        SLOT, Needs to be connected to
//##            - Mouse release on View
//##            - Arrow key press on scene
//####################################################################################
void SceneGraphicsScene::itemMoved(SelectionGroup *moved_group, const QPointF &old_position)
{
    m_undo->push(new MoveCommand(moved_group, old_position));
}
