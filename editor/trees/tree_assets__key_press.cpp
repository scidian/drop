//
//      Created by Stephens Nunnally on 10/8/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QApplication>
#include <QDebug>
#include <QKeyEvent>
#include <QScrollBar>

#include "colors/colors.h"
#include "editor/interface_editor_relay.h"
#include "editor/trees/tree_assets.h"
#include "editor/view/editor_scene.h"
#include "editor/view/editor_view.h"
#include "project/project.h"
#include "project/project_asset.h"
#include "project/project_effect.h"
#include "project/project_font.h"
#include "project/project_image.h"
#include "project/project_world.h"
#include "project/project_stage.h"
#include "project/project_thing.h"
#include "settings/settings.h"
#include "settings/settings_component_property.h"

#include "constants.h"
#include "enums.h"
#include "globals.h"
#include "helper.h"
#include "helper_qt.h"


//####################################################################################
//##    Handles Keyboard Input in Asset Tree
//####################################################################################
void TreeAssets::keyPressEvent(QKeyEvent *event) {

    // If nothing selected, pass on key press and exit
    if (getSelectedKey() == c_no_key) { QTreeWidget::keyPressEvent(event);  return; }

    // ***** Find frame of selected item
    QList<QFrame*> frames =  m_asset_frames; /// this->findChildren<QFrame *>("assetFrame");
    QFrame *selected_frame = nullptr;
    QFrame *parent_frame =   nullptr;
    AssetFlowLayout *flow = nullptr;
    int     layout_index =   c_no_key;
    for (auto frame : frames) {
        if (frame->property(User_Property::Key).toLongLong() == getSelectedKey()) {
            selected_frame = frame;
            parent_frame = dynamic_cast<QFrame*>(frame->parentWidget());
            flow = dynamic_cast<AssetFlowLayout*>(parent_frame->layout());
            if (flow == nullptr) continue;
            layout_index = flow->indexOf(frame);
            break;
        }
    }
    if (layout_index == c_no_key) {     QTreeWidget::keyPressEvent(event);  return; }


    // ***** Process arrow keys
    int adjusted_index = layout_index + 1;
    if (event->key() == Qt::Key_Right) {
        // Normal right movement
        if (adjusted_index < flow->count() && adjusted_index % flow->rowWidth() > 0) {
            layout_index++;
        // Wrap around back to the left movement
        } else if (flow->rowWidth() > 1) {
            layout_index = (layout_index / flow->rowWidth()) * flow->rowWidth();
        }

    } else if (event->key() == Qt::Key_Left) {
        // Normal left movement
        if (adjusted_index > 1 && adjusted_index % flow->rowWidth() != 1) {
            layout_index--;
        // Wrap around back to the right movement
        } else if (flow->rowWidth() > 1) {
            layout_index += (flow->rowWidth() - 1);
        }

    } else if (event->key() == Qt::Key_Up) {
        if (layout_index >= flow->rowWidth()) {
            layout_index -= flow->rowWidth();
        }

    } else if (event->key() == Qt::Key_Down) {
        if (layout_index < (flow->count() - flow->rowWidth())) {
            layout_index += flow->rowWidth();
        }
    }
    // Safety check for out of bounds
    if (layout_index >= flow->count())  layout_index = flow->count() - 1;
    if (layout_index < 0)               layout_index = 0;


    // ***** Duplicate Asset
    if (event->key() == Qt::Key_D) {
        DrSettings *entity = getParentProject()->findSettingsFromKey(getSelectedKey());
        if (entity == nullptr) return;
        if (entity->getType() != DrType::Asset) return;
        DrAsset *asset = dynamic_cast<DrAsset*>(entity);
        if (asset == nullptr) return;

        // Create new Asset, copy Settings / Components / Properties
        DrAsset *copy_asset = getParentProject()->addAsset( asset->getAssetType(), asset->getBaseKey() );
        copy_asset->copyEntitySettings(asset);

        // Find a new name for Asset
        QString new_name;
        bool    has_name;
        int     i = 1;
        do {
            has_name = false;
            new_name = (i == 1) ? copy_asset->getName() + " copy" :  copy_asset->getName() + " copy (" + QString::number(i) + ")";
            for (auto &asset_pair : getParentProject()->getAssetMap()) {
                if (asset_pair.second->getName() == new_name) has_name = true;
            }
            i++;
        } while (has_name != false);
        copy_asset->setName( new_name );

        // Update EditorRelay widgets
        m_editor_relay->buildAssetTree();
        m_editor_relay->buildInspector( { copy_asset->getKey() } );
        m_editor_relay->updateItemSelection(Editor_Widgets::Asset_Tree);
        setSelectedKey(copy_asset->getKey());
        setFocus(Qt::FocusReason::PopupFocusReason);
    }


    // ***** Delete Asset
    if (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace) {

        DrSettings *entity = getParentProject()->findSettingsFromKey(getSelectedKey());
        if (entity == nullptr) return;
        if (entity->getType() != DrType::Asset && entity->getType() != DrType::Font) return;

        removeAsset(getSelectedKey());

        // Select next availale item if there is one
        long new_key = c_no_key;
        if (flow->count() >= 2) {
            if (layout_index + 1 == flow->count())
                layout_index--;
            else
                layout_index++;
            QFrame *new_frame = dynamic_cast<QFrame*>(flow->itemAt(layout_index)->widget());
            new_key = new_frame->property(User_Property::Key).toLongLong();
        }

        // ***** Update Editor Widgets
        m_editor_relay->buildAssetTree();
        m_editor_relay->buildProjectTree();
        m_editor_relay->buildScene( c_same_key );
        m_editor_relay->buildInspector( { new_key } );

        // ***** Update Selected Key
        setSelectedKey(new_key);
        m_editor_relay->updateItemSelection(Editor_Widgets::Asset_Tree);
    }


    // ***** If we pressed arrow key, update selection
    if (event->key() == Qt::Key_Right || event->key() == Qt::Key_Left || event->key() == Qt::Key_Up || event->key() == Qt::Key_Down) {
        QFrame *new_frame = dynamic_cast<QFrame*>(flow->itemAt(layout_index)->widget());
        long    new_key = new_frame->property(User_Property::Key).toLongLong();
        if (new_key != getSelectedKey()) {
            setSelectedKey(new_key);
            m_editor_relay->buildInspector( { new_key } );
        }
    }


    this->update();
}


//####################################################################################
//##    Removes Entity from the Project
//####################################################################################
void TreeAssets::removeAsset(long entity_key) {

    DrSettings *entity = getParentProject()->findSettingsFromKey(entity_key);
    if (entity == nullptr) return;

    // ***** Delete all instances in project
    for (auto world_pair : getParentProject()->getWorldMap()) {
        DrWorld *world = world_pair.second;
        for (auto stage_pair : world->getStageMap()) {
            DrStage *stage = stage_pair.second;

            // Find any instances of this asset in Stage
            QList<DrThing*> things;
            for (auto thing_pair : stage->getThingMap()) {
                DrThing *thing = thing_pair.second;
                if (thing->getAssetKey() == entity_key) things.append(thing);
            }

            // Delete them
            for (auto thing : things) {
                stage->deleteThing(thing);
            }
        }
    }

    // ***** Delete Entity
    getParentProject()->deleteEntity(entity_key);
}
















