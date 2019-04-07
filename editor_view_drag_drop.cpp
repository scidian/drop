//
//      Created by Stephens Nunnally on 4/6/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QMimeData>

#include "editor_scene.h"
#include "editor_view.h"
#include "globals.h"
#include "interface_editor_relay.h"
#include "project.h"
#include "project_asset.h"
#include "project_world.h"
#include "project_world_stage.h"
#include "project_world_stage_object.h"

//####################################################################################
//##        Drag Handling
//####################################################################################
void DrView::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasFormat("application/x-drop-asset-data"))
            event->acceptProposedAction();
    else    event->ignore();
}
void DrView::dragLeaveEvent(QDragLeaveEvent *event) { event->accept(); }
void DrView::dragMoveEvent(QDragMoveEvent *event) {
    if (event->mimeData()->hasFormat("application/x-drop-asset-data"))
            event->acceptProposedAction();
    else    event->ignore();
}


//####################################################################################
//##        Drop Handling - Accept an asset fropped from the Asset Tree
//####################################################################################
void DrView::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-drop-asset-data")) {
        QByteArray item_data = event->mimeData()->data("application/x-drop-asset-data");
        QDataStream dataStream(&item_data, QIODevice::ReadOnly);

        // Grab data from drop
        QPoint   offset;
        QVariant variant_key;
        dataStream >> offset >> variant_key;

        QPointF  position =  mapToScene(event->pos() + offset);
        long     asset_key = variant_key.toInt();
        DrAsset *asset =     m_editor_relay->currentProject()->getAsset(asset_key);


        // Create new object from data
        switch (asset->getAssetType()) {
        case DrAssetType::Character:
        case DrAssetType::Object:
            DrObject *object = my_scene->getCurrentStageShown()->addObject(DrObjectType::Object, asset_key, position.x(), position.y(), 0); // FIX: z order
            my_scene->addItemToSceneFromObject( object );
            break;
        }


        //######################
        //######################
        //######################
        // !! Don't want to rebuild it, only update it
        m_editor_relay->buildProjectTree();
        //######################
        //######################
        //######################

        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}
















