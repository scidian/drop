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

#include "editor/editor_tree_assets.h"
#include "enums.h"
#include "helper.h"
#include "imaging/imaging.h"
#include "interface_editor_relay.h"
#include "project/project.h"
#include "project/project_asset.h"
#include "project/project_effect.h"
#include "project/project_font.h"
#include "settings/settings.h"
#include "settings/settings_component_property.h"


//####################################################################################
//##    Drag and Drop start for Asset
//####################################################################################
void DrFilterAssetMouseHandler::startDragAndDrop(QLabel *label_pixmap, long asset_key) {
    // Get asset from project
    DrAsset *asset = m_editor_relay->currentProject()->getAsset(asset_key);

    // Pull pixmap of asset and scale based on current view zoom level
    QPixmap pre_pixmap = *label_pixmap->pixmap();
    QPixmap pixmap;
    QString text;
    switch (asset->getAssetType()) {
        case DrAssetType::Object:
        case DrAssetType::Character:
            pixmap = asset->getComponentProperty(Components::Asset_Animation, Properties::Asset_Animation_Default)->getValue().value<QPixmap>();
            break;
        case DrAssetType::Effect:
            pixmap = m_editor_relay->currentProject()->getEffect( asset->getSourceKey() )->getPixmap();
            break;
        case DrAssetType::Text:
            //text =   asset->getComponentPropertyValue(Components::Thing_Settings_Text, Properties::Thing_Text_User_Text).toString();
            pixmap = m_editor_relay->currentProject()->getFont( asset->getSourceKey() )->createText( "Text" );
            break;
    }
    pixmap = DrImaging::applySinglePixelFilter( Image_Filter_Type::Opacity, pixmap, -64);
    int scaled_x = static_cast<int>( pixmap.width() *  m_editor_relay->currentViewZoom() );
    int scaled_y = static_cast<int>( pixmap.height() * m_editor_relay->currentViewZoom() );
    scaled_x = Dr::Clamp(scaled_x, 5, QGuiApplication::screenAt( QCursor::pos() )->geometry().width()  * 1);
    scaled_y = Dr::Clamp(scaled_y, 5, QGuiApplication::screenAt( QCursor::pos() )->geometry().height() * 1);
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












