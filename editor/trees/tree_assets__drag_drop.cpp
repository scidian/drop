//
//      Created by Stephens Nunnally on 10/8/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QDrag>
#include <QGuiApplication>
#include <QMimeData>
#include <QScreen>

#include "editor/globals_editor.h"
#include "editor/imaging/imaging.h"
#include "editor/interface_editor_relay.h"
#include "editor/trees/tree_assets.h"
#include "helper.h"
#include "model/enums_model_types.h"
#include "model/project/project.h"
#include "model/project/project_animation.h"
#include "model/project/project_asset.h"
#include "model/project/project_device.h"
#include "model/project/project_effect.h"
#include "model/project/project_font.h"
#include "model/project/project_image.h"
#include "model/settings/settings.h"
#include "model/settings/settings_component_property.h"


//####################################################################################
//##    Drag and Drop start for Asset
//####################################################################################
void DrFilterAssetMouseHandler::startDragAndDrop(QLabel *label_pixmap, long asset_key) {
    // Get asset from project
    DrSettings *entity = m_editor_relay->currentProject()->findSettingsFromKey(asset_key);
    if (entity == nullptr) return;

    // Pull pixmap of asset and scale based on current view zoom level
    QPixmap pre_pixmap = *label_pixmap->pixmap();
    QPixmap pixmap;
    QString text;

    if (entity->getType() == DrType::Asset) {
        DrAsset *asset = dynamic_cast<DrAsset*>(entity);

        switch (asset->getAssetType()) {
            case DrAssetType::Object:
            case DrAssetType::Character: {
                long animation_key = asset->getComponentPropertyValue(Components::Asset_Animation, Properties::Asset_Animation_Idle).toLongLong();
                DrAnimation *ani = asset->getParentProject()->findAnimationFromKey(animation_key);
                if (ani != nullptr) pixmap = ani->getPixmapFromFirstFrame();
                break;
            }
        }

    } else if (entity->getType() == DrType::Device) {
        pixmap = m_editor_relay->currentProject()->findDeviceFromKey( entity->getKey() )->getPixmap();

    } else if (entity->getType() == DrType::Effect) {
        pixmap = m_editor_relay->currentProject()->findEffectFromKey( entity->getKey() )->getPixmap();

    } else if (entity->getType() == DrType::Font) {
        //text =   asset->getComponentPropertyValue(Components::Thing_Settings_Text, Properties::Thing_Text_User_Text).toString();
        pixmap = m_editor_relay->currentProject()->findFontFromKey( entity->getKey() )->createText( "Text" );
    } else {
        return;
    }

    if (pixmap.isNull()) pixmap = QPixmap(1, 1);
    pixmap = DrImaging::applySinglePixelFilter( Image_Filter_Type::Opacity, pixmap, -64);
    int scaled_x = static_cast<int>( pixmap.width() *  m_editor_relay->currentViewZoom() );
    int scaled_y = static_cast<int>( pixmap.height() * m_editor_relay->currentViewZoom() );
    int max_width =  2048;
    int max_height = 2048;
    QScreen *screen = QGuiApplication::screenAt(QCursor::pos());
    if (screen != nullptr) {
        max_width =  screen->geometry().width();
        max_height = screen->geometry().height();
    }
    scaled_x = Dr::Clamp(scaled_x, 5, max_width);
    scaled_y = Dr::Clamp(scaled_y, 5, max_height);
    pixmap = pixmap.scaled(scaled_x, scaled_y, Qt::AspectRatioMode::KeepAspectRatio, Qt::TransformationMode::SmoothTransformation);

    // Use center of pixmap as the hot spot
    QPoint hot_spot = pixmap.rect().center();

    // Create item data for QDrag event
    QByteArray item_data;
    QDataStream data_stream(&item_data, QIODevice::WriteOnly);
    QPoint relative_center = pixmap.rect().center() - hot_spot + QPoint(0, 1);          // Helps place asset into scene at mouse position
    data_stream << relative_center << QVariant::fromValue(asset_key);
    QMimeData *mime_data = new QMimeData;
    mime_data->setData("application/x-drop-asset-data", item_data);

    // Create drag object
    QDrag *drag = new QDrag(label_pixmap);
    drag->setMimeData(mime_data);
    drag->setPixmap(pixmap);
    drag->setHotSpot(hot_spot);

    // Execute drag command
    QPixmap temp_pixmap = DrImaging::applySinglePixelFilter( Image_Filter_Type::Opacity, pre_pixmap, -128);
    label_pixmap->setPixmap(temp_pixmap);
    drag->exec(Qt::CopyAction, Qt::CopyAction);
    label_pixmap->setPixmap(pre_pixmap);

}












