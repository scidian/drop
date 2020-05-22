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

#include "editor/forms/form_progress_box.h"
#include "editor/interface_editor_relay.h"
#include "editor/preferences.h"
#include "editor/project/project.h"
#include "editor/trees/tree_assets.h"
#include "editor/view_editor/editor_scene.h"
#include "editor/view_editor/editor_view.h"
#include "engine/enums_engine.h"
#include "project/dr_project.h"
#include "project/entities/dr_image.h"
#include "project/entities/dr_stage.h"
#include "project/entities/dr_thing.h"
#include "project/entities/dr_world.h"
#include "project/entities_physics_2d/dr_asset.h"
#include "project/entities_physics_2d/dr_device.h"
#include "project/entities_physics_2d/dr_effect.h"
#include "project/entities_physics_2d/dr_item.h"
#include "project/entities_physics_2d/dr_prefab.h"


//####################################################################################
//##    Drag Handling
//####################################################################################
void EditorView::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasFormat("application/x-drop-asset-data")) {                // From asset tree
        m_drop_might_happen = true;
        m_drop_location = roundToGrid( mapToScene(event->pos()) );
        event->acceptProposedAction();
    } else if (event->mimeData()->hasUrls()) {                                          // External files
        event->acceptProposedAction();
    } else  event->ignore();
}

void EditorView::dragLeaveEvent(QDragLeaveEvent *event) {
    // Make sure we stop drawing drop locaiton, calling update to repaint
    m_drop_might_happen = false;
    update();
    event->accept();
}

void EditorView::dragMoveEvent(QDragMoveEvent *event) {
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
void EditorView::dropEvent(QDropEvent *event) {
    // ***** Stop drawing crosshairs under item drop
    m_drop_might_happen = false;

    // ***** Check for Stage
    DrStage *stage = my_scene->getCurrentStageShown();
    DrThing *thing = nullptr;
    DrAsset *asset = nullptr;
    if (stage == nullptr) return;

    // ***** Find Z Position to insert
    double z_order =     0.0;
    int    sub_order =   0;
    int    thing_count = 0;
    for (auto test_z : my_scene->getSelectionItemsAsThings()) {
        double test_z_order = test_z->getComponentPropertyValue(Comps::Thing_Layering, Props::Thing_Z_Order).toDouble();
        int  test_sub_order = test_z->getComponentPropertyValue(Comps::Thing_Layering, Props::Thing_Sub_Z_Order).toInt();
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

        // Create new Thing from drop data
        long        entity_key = variant_key.toInt();
        DrSettings *entity =     m_editor_relay->currentProject()->findSettingsFromKey(entity_key);

        if (entity->getType() == DrType::Asset) {
            DrAsset *asset = dynamic_cast<DrAsset*>(entity);
            switch (asset->getAssetType()) {
                case DrAssetType::Character: thing = stage->addThing(DrThingType::Character, entity_key, position.x(), -position.y(),   5); break;
                case DrAssetType::Object:    thing = stage->addThing(DrThingType::Object,    entity_key, position.x(), -position.y(),   0); break;
            }

            // Assets with a Dynamic type body style should drop as Dynamic by default
            if (asset->getAssetType() == DrAssetType::Object) {
                int body_style = asset->getComponentPropertyValue(Comps::Asset_Physics, Props::Asset_Physics_Body_Style).toInt();
                if (static_cast<Body_Style>(body_style) != Body_Style::Rigid_Body) {
                    thing->setComponentPropertyValue(Comps::Thing_Settings_Object, Props::Thing_Object_Physics_Type, static_cast<int>(Body_Type::Dynamic));
                }
            }

        } else if (entity->getType() == DrType::Device) {
            DrDevice *device = m_project->findDeviceFromKey( entity_key );
            switch (device->getDeviceType()) {
                case DrDeviceType::Camera:  thing = stage->addThing(DrThingType::Camera,    entity_key, position.x(), -position.y(),  10);  break;
            }

        } else if (entity->getType() == DrType::Effect) {
            DrEffect *effect = m_project->findEffectFromKey( entity_key );
            switch (effect->getEffectType()) {
                case DrEffectType::Fire:    thing = stage->addThing(DrThingType::Fire,      entity_key, position.x(), -position.y(), -10);  break;
                case DrEffectType::Fisheye: thing = stage->addThing(DrThingType::Fisheye,   entity_key, position.x(), -position.y(),  10);  break;
                case DrEffectType::Light:   thing = stage->addThing(DrThingType::Light,     entity_key, position.x(), -position.y(), -10);  break;
                case DrEffectType::Mirror:  thing = stage->addThing(DrThingType::Mirror,    entity_key, position.x(), -position.y(),  10);  break;
                case DrEffectType::Swirl:   thing = stage->addThing(DrThingType::Swirl,     entity_key, position.x(), -position.y(),  10);  break;
                case DrEffectType::Water:   thing = stage->addThing(DrThingType::Water,     entity_key, position.x(), -position.y(),  10);  break;
                //case DrEffectType::Flag:    break;
                //case DrEffectType::Rain:    break;
                //case DrEffectType::Snow:    break;
                //case DrEffectType::Clouds:  break;
                //case DrEffectType::Fog:     break;
            }

        } else if (entity->getType() == DrType::Font) {
            thing = stage->addThing(DrThingType::Text, entity_key, position.x(), -position.y(),   0);

        } else if (entity->getType() == DrType::Item) {
            DrItem *dr_item = m_project->findItemFromKey( entity_key );
            switch (dr_item->getItemType()) {
                case DrItemType::Tile:      thing = stage->addThing(DrThingType::Tile,      entity_key, position.x(), -position.y(),  0);   break;
            }

        } else if (entity->getType() == DrType::Prefab) {
            DrPrefab *prefab = m_project->findPrefabFromKey( entity_key );
            asset = Dr::AddPrefab(m_project, prefab->getPrefabType());
            if (prefab->getPrefabType() != DrPrefabType::Character) {
                thing = stage->addThing(DrThingType::Object,    asset->getKey(), position.x(), -position.y(),   5);
            } else {
                thing = stage->addThing(DrThingType::Character, asset->getKey(), position.x(), -position.y(),   0);
            }

            switch (prefab->getPrefabType()) {
                case DrPrefabType::Blob:
                    thing->setComponentPropertyValue(Comps::Thing_Settings_Object, Props::Thing_Object_Physics_Type, static_cast<int>(Body_Type::Dynamic));
                    break;
                case DrPrefabType::Character:
                    break;
                case DrPrefabType::Foliage:
                    thing->setComponentPropertyValue(Comps::Thing_Settings_Object, Props::Thing_Object_Physics_Type, static_cast<int>(Body_Type::Dynamic));
                    break;
                case DrPrefabType::Ground:
                    break;
                case DrPrefabType::Ladder:
                    thing->setComponentPropertyValue(Comps::Thing_Settings_Object, Props::Thing_Object_Collide, false);
                    break;
                case DrPrefabType::Object:
                    thing->setComponentPropertyValue(Comps::Thing_Settings_Object, Props::Thing_Object_Physics_Type, static_cast<int>(Body_Type::Dynamic));
                    break;
                case DrPrefabType::Spike:
                    thing->setComponentPropertyValue(Comps::Thing_Settings_Object, Props::Thing_Object_Damage, static_cast<int>(Collision_Type::Damage_Player));
                    break;
                case DrPrefabType::Spring:
                    break;
            }

        } else {
            return;
        }


        my_scene->addItemToSceneFromThing( thing );
        m_editor_relay->updateEditorWidgetsAfterItemChange(Editor_Widgets::Asset_Tree, { thing }, { std::make_pair(Comps::Thing_Transform, Props::Thing_Size) } );
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

        // Check if images are valid, if no valid images, exit.
        QList<QString> file_paths;
        for (auto path : path_list) {
            QImage image(path);
            if (image.isNull() == false) file_paths.append(path);
        }
        if (file_paths.isEmpty()) {
            event->ignore();
            return;
        }

        // Add the Images and Asset to the project and add the Thing to the scene
        FormProgressBox *progress = new FormProgressBox("...", "Cancel", file_paths.size() * 2, this->window());
        progress->setPrefix(" Importing Images: \n");
        std::vector<DrAsset*> assets;
        for (auto file_path : file_paths) {
            DrImage *image = Dr::AddImage(m_editor_relay->currentProject(), file_path, c_no_key, true, progress);
            assets.push_back(m_editor_relay->currentProject()->addAsset(DrAssetType::Object, image->getKey()));
        }
        progress->stopProgress();

        // Rebuild Asset Tree
        m_editor_relay->buildAssetTree();
        m_editor_relay->getAssetTree()->setSelectedKey(assets.front()->getKey());

        // Add Thing Objects at mouse position
        QPointF position = mapToScene(event->pos());
        for (auto asset : assets) {
            thing = stage->addThing(DrThingType::Object, asset->getKey(), position.x(), -position.y(), 0);
            my_scene->addItemToSceneFromThing( thing );
            if (thing_count > 0) {
                thing->setZOrderWithSub(z_order, Z_Insert::At_Position, sub_order);
                my_scene->updateItemZValues();
            }
            thing_count++;
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

    // ***** Selects the newly dropped Asset if created, also loads selection to Object Inspector
    ///if (asset == nullptr && thing != nullptr) { asset = m_project->findAssetFromKey(thing->getAssetKey()); }
    /**
    if (asset != nullptr) {
        m_editor_relay->buildInspector( { asset->getKey() } );
        m_editor_relay->updateItemSelection(Editor_Widgets::Asset_Tree);
        if (thing != nullptr) {
            m_editor_relay->updateItemSelection(Editor_Widgets::Editor_View, { thing->getKey() } );
            m_editor_relay->updateItemSelection(Editor_Widgets::Project_Tree );
        }
        m_editor_relay->getAssetTree()->setSelectedKey(asset->getKey());
        m_editor_relay->getAssetTree()->setFocus(Qt::FocusReason::PopupFocusReason);
    } else **/

    // ***** Selects the newly dropped Thing if created, also loads selection to Object Inspector
    if (thing != nullptr) {
        this->setFocus();
        m_editor_relay->buildInspector( { thing->getKey() } );
        m_editor_relay->updateItemSelection(Editor_Widgets::Editor_View, { thing->getKey() } );
        m_editor_relay->updateItemSelection(Editor_Widgets::Project_Tree );
    }

}






















