//
//      Created by Stephens Nunnally on 12/20/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QPainter>

#include "editor/helper_library.h"
#include "editor/project/project.h"
#include "project/entities/dr_asset.h"
#include "project/entities/dr_device.h"
#include "project/entities/dr_effect.h"
#include "project/entities/dr_font.h"

namespace Dr {


//####################################################################################
//##    Returns Asset Tree Device Pixmap
//####################################################################################
QPixmap GetAssetPixmapDevice(DrDeviceType device_type) {
    switch (device_type) {
        case DrDeviceType::Camera:      return QPixmap(":/assets/asset_types/camera.png");
    }
}


//####################################################################################
//##    Returns Asset Tree Effect Pixmap
//####################################################################################
QPixmap GetAssetPixmapEffect(DrEffectType effect_type) {
    switch (effect_type) {
        case DrEffectType::Light:       return QPixmap(":/assets/asset_types/light.png");
        case DrEffectType::Water:       return QPixmap(":/assets/asset_types/water.png");
        case DrEffectType::Fire:        return QPixmap(":/assets/asset_types/fire.png");
        case DrEffectType::Mirror:      return QPixmap(":/assets/asset_types/mirror.png");
        case DrEffectType::Fisheye:     return QPixmap(":/assets/asset_types/fisheye.png");
        case DrEffectType::Swirl:       return QPixmap(":/assets/asset_types/swirl.png");
        //case DrEffectType::Flag:        return QPixmap(":/assets/asset_types/flag.png");
        //case DrEffectType::Rain:        return QPixmap(":/assets/asset_types/rain.png");
        //case DrEffectType::Snow:        return QPixmap(":/assets/asset_types/snow.png");
        //case DrEffectType::Clouds:      return QPixmap(":/assets/asset_types/clouds.png");
        //case DrEffectType::Fog:         return QPixmap(":/assets/asset_types/fog.png");
    }
}


//####################################################################################
//##    Returns Asset Tree Item Pixmap
//####################################################################################
QPixmap GetAssetPixmapItem(DrItemType item_type) {
    switch (item_type) {
        case DrItemType::Tile:          return QPixmap(":/assets/asset_types/tile.png");
    }
}


//####################################################################################
//##    Returns Asset Tree Prefab Pixmap
//####################################################################################
QPixmap GetAssetPixmapPrefab(DrPrefabType prefab_type) {
    switch (prefab_type) {
        case DrPrefabType::Blob:        return QPixmap(":/assets/asset_types/blob.png");
        case DrPrefabType::Foliage:     return QPixmap(":/assets/asset_types/foliage.png");
    }
}


//####################################################################################
//##    Temp Create Text
//##        !!!!! #TEMP: Need to implement bitmap fonts or actual font rendering in Project
//####################################################################################
// !!! Need to incorporate letter spacing
QPixmap CreateText(DrFont *font, std::string text) {
    int width = 0;
    int height = font->getCharRect('A').height;
    for (int i = 0; i < static_cast<int>(text.length()); i++) {
        width += font->getCharRect( text.at(i) ).width;
    }
    if (width ==  0) width =  1;
    if (height == 0) height = 1;
    QPixmap text_image(width, height);
    text_image.fill(Qt::transparent);

    QPainter painter(&text_image);
    painter.setPen(Qt::NoPen);

    QPixmap text_atlas = Dr::ToQPixmap(font->getBitmap());

    int x = 0;
    for (int i = 0; i < static_cast<int>(text.length()); i++) {
        char letter = text.at(i);
        int  letter_width = font->getCharRect( letter ).width;

        QRect character(font->getCharRect(letter).left(), font->getCharRect(letter).top(), font->getCharRect(letter).width, font->getCharRect(letter).height);
        painter.drawPixmap( QRect(x, 0, letter_width, height), text_atlas, character );
        x += letter_width;
    }

    return text_image;
}


}   // End namespace Dr











