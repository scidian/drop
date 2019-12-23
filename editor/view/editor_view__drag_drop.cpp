//
//      Created by Stephens Nunnally on 4/6/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QDebug>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QMimeData>

#include "editor/globals_editor.h"
#include "editor/interface_editor_relay.h"
#include "editor/project/project.h"
#include "editor/trees/tree_assets.h"
#include "editor/view/editor_scene.h"
#include "editor/view/editor_view.h"
#include "project/dr_project.h"
#include "project/entities/dr_asset.h"
#include "project/entities/dr_device.h"
#include "project/entities/dr_effect.h"
#include "project/entities/dr_image.h"
#include "project/entities/dr_stage.h"
#include "project/entities/dr_thing.h"
#include "project/entities/dr_world.h"


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
    } else {
        event->ignore();
    }
}


//####################################################################################
//##    Drop Handling - Accept an asset fropped from the Asset Tree
//####################################################################################
void DrView::dropEvent(QDropEvent *event) {
    // ***** Stop drawing crosshairs under item drop
    m_drop_might_happen = false;

    // ***** Check for Stage
    DrStage *stage = my_scene->getCurrentStageShown();
    DrThing *thing = nullptr;
    if (stage == nullptr) return;

    // ***** Find Z Position to insert
    double z_order = 0.0;
    int  sub_order = 0;
    int  thing_count = 0;
    for (auto test_z : my_scene->getSelectionItemsAsThings()) {
        double test_z_order = test_z->getComponentPropertyValue(Components::Thing_Layering, Properties::Thing_Z_Order).toDouble();
        int  test_sub_order = test_z->getComponentPropertyValue(Components::Thing_Layering, Properties::Thing_Sub_Z_Order).toInt();
        if (((test_z_order >= z_order) && (test_sub_order >= sub_order)) || thing_count == 0) {
            z_order =   test_z_order;
            sub_order = test_sub_order;
        }
        thing_count++;
    }
    sub_order += 1;

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

        // Create new Thing from drop data,                                                                 !!!!! #NOTE: Sets initial Z value of Thing
        long        entity_key = variant_key.toInt();
        DrSettings *entity =     m_editor_relay->currentProject()->findSettingsFromKey(entity_key);

        if (entity->getType() == DrType::Asset) {
            DrAsset *asset = dynamic_cast<DrAsset*>(entity);
            switch (asset->getAssetType()) {
                case DrAssetType::Character: thing = stage->addThing(DrThingType::Character, entity_key, position.x(), -position.y(),   5); break;
                case DrAssetType::Object:    thing = stage->addThing(DrThingType::Object,    entity_key, position.x(), -position.y(),   0); break;
            }

        } else if (entity->getType() == DrType::Font) {
            thing = stage->addThing(DrThingType::Text, entity_key, position.x(), -position.y(),   0);

        } else if (entity->getType() == DrType::Effect) {
            DrEffect *effect = m_project->findEffectFromKey( entity_key );
            switch (effect->getEffectType()) {
                case DrEffectType::Fire:    thing = stage->addThing(DrThingType::Fire,      entity_key, position.x(), -position.y(), -10);  break;
                case DrEffectType::Fisheye: thing = stage->addThing(DrThingType::Fisheye,   entity_key, position.x(), -position.y(),  10);  break;
                case DrEffectType::Light:   thing = stage->addThing(DrThingType::Light,     entity_key, position.x(), -position.y(), -10);  break;
                case DrEffectType::Mirror:  thing = stage->addThing(DrThingType::Mirror,    entity_key, position.x(), -position.y(),  10);  break;
                case DrEffectType::Swirl:   thing = stage->addThing(DrThingType::Swirl,     entity_key, position.x(), -position.y(),  10);  break;
                case DrEffectType::Water:   thing = stage->addThing(DrThingType::Water,     entity_key, position.x(), -position.y(),  10);  break;
                case DrEffectType::Flag:    break;
                case DrEffectType::Rain:    break;
                case DrEffectType::Snow:    break;
                case DrEffectType::Clouds:  break;
                case DrEffectType::Fog:     break;
            }

        } else if (entity->getType() == DrType::Device) {
            DrDevice *device = m_project->findDeviceFromKey( entity_key );
            switch (device->getDeviceType()) {
                case DrDeviceType::Camera:  thing = stage->addThing(DrThingType::Camera,    entity_key, position.x(), -position.y(),  10);  break;
            }

        } else {
            return;
        }

        my_scene->addItemToSceneFromThing( thing );
        m_editor_relay->updateEditorWidgetsAfterItemChange(Editor_Widgets::Asset_Tree, { thing }, { Properties::Thing_Size } );
        if (thing_count > 0) {
            thing->setZOrderWithSub(z_order, Z_Insert::At_Position, sub_order);
            my_scene->updateItemZValues();
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
        QPixmap pixmap = QPixmap::fromImage( image.convertToFormat( QImage::Format_ARGB32 ));

        // If it was an image, add the Image and Asset to the project and add the Thing to the scene
        DrImage *add_image = Dr::AddImage(m_editor_relay->currentProject(), file_path);
        DrAsset *asset = m_editor_relay->currentProject()->addAsset(DrAssetType::Object, add_image->getKey() );
        m_editor_relay->buildAssetTree();
        m_editor_relay->getAssetTree()->setSelectedKey(asset->getKey());

        // Add Object Thing at mouse position
        QPointF  position =  mapToScene(event->pos());
        thing = stage->addThing(DrThingType::Object, asset->getKey(), position.x(), -position.y(), 0);
        my_scene->addItemToSceneFromThing( thing );
        if (thing_count > 0) {
            thing->setZOrderWithSub(z_order, Z_Insert::At_Position, sub_order);
            my_scene->updateItemZValues();
        }

        // Mark event as accepted
        event->acceptProposedAction();

    // ********** Unknown drag n drop type, exit
    } else {
        event->ignore();
        return;
    }

    // ***** Update Editor Widgets
    m_editor_relay->buildProjectTree();

    // ***** Selects the newly dropped Thing, loads to Object Inspector
    if (thing != nullptr) {
        this->setFocus();
        m_editor_relay->buildInspector( { thing->getKey() } );
        m_editor_relay->updateItemSelection(Editor_Widgets::Stage_View, { thing->getKey() } );
        m_editor_relay->updateItemSelection(Editor_Widgets::Project_Tree );
    }

}









