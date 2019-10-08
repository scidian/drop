//
//      Created by Stephens Nunnally on 10/8/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QKeyEvent>

#include "colors/colors.h"
#include "constants.h"
#include "editor_tree_assets.h"
#include "editor_view/editor_scene.h"
#include "editor_view/editor_view.h"
#include "enums.h"
#include "globals.h"
#include "interface_editor_relay.h"
#include "helper.h"
#include "helper_qt.h"
#include "project/project.h"
#include "project/project_asset.h"
#include "project/project_effect.h"
#include "project/project_font.h"
#include "project/project_image.h"
#include "project/project_world.h"
#include "project/project_world_stage.h"
#include "project/project_world_stage_thing.h"
#include "settings/settings.h"
#include "settings/settings_component_property.h"


//####################################################################################
//##    Handles Keyboard Input in Asset Tree
//####################################################################################
void TreeAssets::keyPressEvent(QKeyEvent *event) {

    if (event->key() == Qt::Key_Up) {


    // ***** Delete Asset
    } else if (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace) {
        if (getSelectedKey() == 0) return;
        DrAsset *asset = m_project->findAssetFromKey(getSelectedKey());
        if (asset == nullptr) return;
        if (asset->getAssetType() == DrAssetType::Effect) return;

        removeAsset(getSelectedKey());
        setSelectedKey(0);

        //######################
        //######################
        //######################
        // !!!!! #FIX: Don't want to rebuild them, only update them
        m_editor_relay->buildAssetTree();
        m_editor_relay->buildProjectTree();
        m_editor_relay->buildScene( c_same_key );
        m_editor_relay->buildInspector( { } );
        //######################
        //######################
        //######################

        m_editor_relay->updateItemSelection(Editor_Widgets::Asset_Tree);
    }

    this->update();

    QTreeWidget::keyPressEvent(event);
}


//####################################################################################
//##    Removes Asset from the Project
//####################################################################################
void TreeAssets::removeAsset(long asset_key) {
    DrAsset *asset = m_project->findAssetFromKey(getSelectedKey());
    if (asset == nullptr) return;

    // Delete all instances in project
    for (auto world_pair : m_project->getWorldMap()) {
        DrWorld *world = world_pair.second;
        for (auto stage_pair : world->getStageMap()) {
            DrStage *stage = stage_pair.second;

            // Find any instances of this asset in Stage
            QList<DrThing*> things;
            for (auto thing_pair : stage->getThingMap()) {
                DrThing *thing = thing_pair.second;
                if (thing->getAssetKey() == asset_key) things.append(thing);
            }

            // Delete them
            for (auto thing : things) {
                stage->deleteThing(thing);
            }
        }
    }

    // Delete underlying source to Asset
    switch (asset->getAssetType()) {
        case DrAssetType::Character:
        case DrAssetType::Object: {
            DrImage *image = m_project->getImageMap()[asset->getSourceKey()];
            m_project->getImageMap().erase( asset->getSourceKey() );
            delete image;
            break;
        }
        case DrAssetType::Effect: {
            DrEffect *effect = m_project->getEffectMap()[asset->getSourceKey()];
            m_project->getEffectMap().erase( asset->getSourceKey() );
            delete effect;
            break;
        }
        case DrAssetType::Text: {
            DrFont *font = m_project->getFontMap()[asset->getSourceKey()];
            m_project->getFontMap().erase( asset->getSourceKey() );
            delete font;
            break;
        }
    }

    m_project->getAssetMap().erase( getSelectedKey() );
    delete asset;
}
















