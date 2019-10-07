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
#include "project/project.h"
#include "project/project_asset.h"
#include "project/project_effect.h"
#include "project/project_world.h"
#include "project/project_world_stage.h"
#include "project/project_world_stage_thing.h"

//####################################################################################
//##    Drag Handling
//####################################################################################
void DrView::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasFormat("application/x-drop-asset-data")) {                // From asset tree
        m_drop_might_happen = true;
        m_drop_location = roundToGrid( mapToScene(event->pos()) );
        event->acceptProposedAction();
    } else if (event->mimeData()->hasUrls()) {                                          // External files
        event->acceptProposedAction();
    } else  event->ignore();
}

void DrView::dragLeaveEvent(QDragLeaveEvent *event) {
    // Make sure we stop drawing drop locaiton, calling update to repaint
    m_drop_might_happen = false;
    update();
    event->accept();
}

void DrView::dragMoveEvent(QDragMoveEvent *event) {
    if (event->mimeData()->hasFormat("application/x-drop-asset-data")) {                // From asset tree
        m_drop_might_happen = true;

        // If potential drop location has changed, redraw view by calling update
        QPointF new_rounded_position = roundToGrid( mapToScene(event->pos()) );
        if (m_drop_location != new_rounded_position) {
            m_drop_location = roundToGrid( mapToScene(event->pos()) );
            update();
        }

        event->acceptProposedAction();
    } else if (event->mimeData()->hasUrls()) {                                          // External files
        event->acceptProposedAction();
    } else    event->ignore();
}


//####################################################################################
//##    Drop Handling - Accept an asset fropped from the Asset Tree
//####################################################################################
void DrView::dropEvent(QDropEvent *event) {
    // Stop drawing crosshairs under item drop
    m_drop_might_happen = false;

    DrStage *stage = my_scene->getCurrentStageShown();
    DrThing *thing = nullptr;

    // ********** From Asset Tree
    if (event->mimeData()->hasFormat("application/x-drop-asset-data")) {
        QByteArray item_data = event->mimeData()->data("application/x-drop-asset-data");
        QDataStream data_stream(&item_data, QIODevice::ReadOnly);

        // Grab data from drop
        QPoint   offset;
        QVariant variant_key;
        data_stream >> offset >> variant_key;

        // Figure out where we want to drop this thing
        QPointF  position;
        if (Dr::GetPreference(Preferences::World_Editor_Snap_To_Grid).toBool() == false)
            position = mapToScene(event->pos() + offset);
        else
            position = m_drop_location;

        // Create new Thing from drop data
        long      asset_key = variant_key.toInt();
        DrAsset  *asset =     m_editor_relay->currentProject()->getAsset(asset_key);
        DrEffect *effect;
        switch (asset->getAssetType()) {
            case DrAssetType::Character:
                thing = stage->addThing(DrThingType::Character, asset_key, position.x(), -position.y(), 0);                 // !!!!! #FIX: z order
                my_scene->addItemToSceneFromThing( thing );
                break;
            case DrAssetType::Object:
                thing = stage->addThing(DrThingType::Object,    asset_key, position.x(), -position.y(), 0);                 // !!!!! #FIX: z order
                my_scene->addItemToSceneFromThing( thing );
                break;
            case DrAssetType::Text:
                thing = stage->addThing(DrThingType::Text,      asset_key, position.x(), -position.y(), 0);                 // !!!!! #FIX: z order
                my_scene->addItemToSceneFromThing( thing );
                break;
            case DrAssetType::Effect:
                effect = m_project->getDrEffect( asset->getSourceKey() );
                switch (effect->getEffectType()) {
                    case DrEffectType::Fire:
                        thing = stage->addThing(DrThingType::Fire, asset_key, position.x(), -position.y(), -5);             // !!!!! #FIX: z order
                        my_scene->addItemToSceneFromThing( thing );
                        break;
                    case DrEffectType::Fisheye:
                        thing = stage->addThing(DrThingType::Fisheye, asset_key, position.x(), -position.y(), 5);           // !!!!! #FIX: z order
                        my_scene->addItemToSceneFromThing( thing );
                        break;
                    case DrEffectType::Light:
                        thing = stage->addThing(DrThingType::Light, asset_key, position.x(), -position.y(), -10);           // !!!!! #FIX: z order
                        my_scene->addItemToSceneFromThing( thing );
                        break;
                    case DrEffectType::Mirror:
                        thing = stage->addThing(DrThingType::Mirror, asset_key, position.x(), -position.y(), 5);            // !!!!! #FIX: z order
                        my_scene->addItemToSceneFromThing( thing );
                        break;
                    case DrEffectType::Swirl:
                        thing = stage->addThing(DrThingType::Swirl, asset_key, position.x(), -position.y(), 5);             // !!!!! #FIX: z order
                        my_scene->addItemToSceneFromThing( thing );
                        break;
                    case DrEffectType::Water:
                        thing = stage->addThing(DrThingType::Water, asset_key, position.x(), -position.y(), 10);            // !!!!! #FIX: z order
                        my_scene->addItemToSceneFromThing( thing );
                        break;
                }
                break;
        }

        event->acceptProposedAction();

    // ********** External Files
    } else if (event->mimeData()->hasUrls()) {

        // Extract the local paths of the files
        QStringList path_list;
        QList<QUrl> url_list = event->mimeData()->urls();
        for (auto url : url_list)
            path_list.append( url.toLocalFile() );

        // Try to load the first image, if it doesnt load, exit. If it does, make sure it is #AARRGGBB and convert to pixmap
        QString file_path = path_list[0];
        QImage image(file_path);
        if (image.isNull()) {
            event->ignore();
            return;
        }
        if ( image.format() != QImage::Format::Format_ARGB32 )
            image = image.convertToFormat( QImage::Format_ARGB32 );
        QPixmap pixmap = QPixmap::fromImage( image );

        // If it was an image, add it to the project and add the Thing to the scene
        long image_key = m_editor_relay->currentProject()->addImage(file_path);
        long asset_key = m_editor_relay->currentProject()->addAsset(DrAssetType::Object, image_key );

        // Add Object Thing at mouse position
        QPointF  position =  mapToScene(event->pos());
        thing = stage->addThing(DrThingType::Object, asset_key, position.x(), -position.y(), 0);    // FIX: z order
        my_scene->addItemToSceneFromThing( thing );

        //######################
        //######################
        //######################
        // !! Don't want to rebuild it, only update it
        m_editor_relay->buildAssetTree();
        //######################
        //######################
        //######################

        event->acceptProposedAction();

    // ***** Unknown drag n drop type, exit
    } else {
        event->ignore();
        return;
    }


    //######################
    //######################
    //######################
    // !! Don't want to rebuild it, only update it
    m_editor_relay->buildProjectTree();
    //######################
    //######################
    //######################


    // ***** Selects the newly dropped Thing, loads to Object Inspector
    if (thing != nullptr) {
        m_editor_relay->buildInspector( { thing->getKey() } );
        m_editor_relay->updateItemSelection(Editor_Widgets::Scene_View, { thing->getKey() } );
        m_editor_relay->updateItemSelection(Editor_Widgets::Project_Tree );
    }

}









