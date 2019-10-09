//
//      Created by Stephens Nunnally on 10/8/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QDebug>
#include <QKeyEvent>
#include <QScrollBar>

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
#include "widgets/widgets_layout.h"


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
    DrQLayoutFlow *flow =    nullptr;
    int     layout_index =   c_no_key;
    for (auto frame : frames) {
        if (frame->property(User_Property::Key).toLongLong() == getSelectedKey()) {
            selected_frame = frame;
            parent_frame = dynamic_cast<QFrame*>(frame->parentWidget());
            flow = dynamic_cast<DrQLayoutFlow*>(parent_frame->layout());
            if (!flow) continue;
            layout_index = flow->indexOf(frame);
            break;
        }
    }
    if (layout_index == c_no_key) {     QTreeWidget::keyPressEvent(event);  return; }


    Dr::SetLabelText(Label_Names::Label_1, "Flow Width: " + QString::number(flow->rowWidth()) + ", " + Dr::CurrentTimeAsString());


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


    // ***** Delete Asset
    if (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace) {
        DrAsset *asset = m_project->findAssetFromKey(getSelectedKey());
        if (asset == nullptr) return;
        if (asset->getAssetType() == DrAssetType::Effect) return;

        removeAsset(getSelectedKey());
        setSelectedKey(c_no_key);

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


    // ***** If we pressed arrow key, update selection
    if (event->key() == Qt::Key_Right || event->key() == Qt::Key_Left || event->key() == Qt::Key_Up || event->key() == Qt::Key_Down) {
        QFrame *new_frame = dynamic_cast<QFrame*>(flow->itemAt(layout_index)->widget());
        long new_key = new_frame->property(User_Property::Key).toLongLong();
        if (new_key != getSelectedKey()) {
            setSelectedKey(new_key);

            int widget_top = new_frame->geometry().y() + parent_frame->geometry().y();
            int widget_bot = widget_top + new_frame->geometry().height();

            if (widget_bot > this->geometry().height()) {
                verticalScrollBar()->setValue( verticalScrollBar()->value() + (widget_bot - this->geometry().height() + 10));
            } else if (widget_top < 0) {
                verticalScrollBar()->setValue( verticalScrollBar()->value() + (widget_top - 10));
            }

            m_editor_relay->buildInspector( { new_key } );
        }
    }


    this->update();
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















