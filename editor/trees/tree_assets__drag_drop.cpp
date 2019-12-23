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

#include "core/dr_math.h"
#include "editor/globals_editor.h"
#include "editor/helper_library.h"
#include "editor/interface_editor_relay.h"
#include "editor/pixmap/pixmap.h"
#include "editor/project/project.h"
#include "editor/trees/tree_assets.h"
#include "project/enums_entity_types.h"
#include "project/dr_project.h"
#include "project/entities/dr_animation.h"
#include "project/entities/dr_asset.h"
#include "project/entities/dr_device.h"
#include "project/entities/dr_effect.h"
#include "project/entities/dr_font.h"
#include "project/entities/dr_image.h"
#include "project/settings/settings.h"
#include "project/settings/settings_component_property.h"


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
                long animation_key = asset->getComponentPropertyValue(Components::Asset_Animation, Properties::Asset_Animation_Idle).toLong();
                DrAnimation *ani = asset->getParentProject()->findAnimationFromKey(animation_key);
                if (ani != nullptr) pixmap = Dr::ToQPixmap(ani->getFirstFrameImage()->getBitmap());
                break;
            }
        }

    } else if (entity->getType() == DrType::Device) {
        pixmap = Dr::GetAssetPixmapDevice( m_editor_relay->currentProject()->findDeviceFromKey( entity->getKey() )->getDeviceType() );

    } else if (entity->getType() == DrType::Effect) {
        pixmap = Dr::GetAssetPixmapEffect( m_editor_relay->currentProject()->findEffectFromKey( entity->getKey() )->getEffectType() );

    } else if (entity->getType() == DrType::Font) {
        //text =   asset->getComponentPropertyValue(Components::Thing_Settings_Text, Properties::Thing_Text_User_Text).toString();
        pixmap = Dr::CreateText(m_editor_relay->currentProject()->findFontFromKey( entity->getKey() ), "Text" );
    } else {
        return;
    }

    if (pixmap.isNull()) pixmap = QPixmap(1, 1);
    pixmap = Dr::ApplySinglePixelFilter(Image_Filter_Type::Opacity, pixmap, -64);
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
    QPixmap temp_pixmap = Dr::ApplySinglePixelFilter(Image_Filter_Type::Opacity, pre_pixmap, -128);
    label_pixmap->setPixmap(temp_pixmap);
    drag->exec(Qt::CopyAction, Qt::CopyAction);
    label_pixmap->setPixmap(pre_pixmap);

}













