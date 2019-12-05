//
//      Created by Stephens Nunnally on 12/7/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Functions associated with enums
//
//
#include "engine/enums_engine.h"
#include "model/enums_types.h"
#include "model/project/project_thing.h"
#include "model/settings/settings.h"


namespace Dr {


//####################################################################################
//##    Returns a QString respresentaiton of the passed DrTypes type
//####################################################################################
QString StringFromType(DrType type) {
    switch (type) {
        case DrType::Animation:     return "Animation";
        case DrType::Asset:         return "Asset";
        case DrType::Device:        return "Device";
        case DrType::Effect:        return "Effect";
        case DrType::Font:          return "Font";
        case DrType::Frame:         return "Frame";
        case DrType::Image:         return "Image";
        case DrType::Stage:         return "Stage";
        case DrType::Thing:         return "Thing";
        case DrType::World:         return "World";

        case DrType::NotFound:      return "Not Found";
    }
    return "Unknown";
}

QString StringFromAssetType(DrAssetType type) {
    switch (type) {
        case DrAssetType::Object:       return "Object Asset";
        case DrAssetType::Character:    return "Character Asset";

        ///case DrAssetType::Action:       return "Action Asset";
    }
    return "Unknown";
}

QString StringFromThingType(DrThingType type) {
    switch (type) {
        case DrThingType::None:         return "None";
        case DrThingType::Character:    return "Character";
        case DrThingType::Object:       return "Object";
        case DrThingType::Camera:       return "Camera";
        case DrThingType::Text:         return "Text";

        case DrThingType::Fire:         return "Fire";
        case DrThingType::Fisheye:      return "Fisheye";
        case DrThingType::Light:        return "Light";
        case DrThingType::Mirror:       return "Mirror";
        case DrThingType::Swirl:        return "Swirl";
        case DrThingType::Water:        return "Water";

        ///case DrThingType::Action:       return "Action";
        ///case DrThingType::Camera:       return "Camera";
        ///case DrThingType::Logic:        return "Logic";
        ///case DrThingType::Particle:     return "Particle";
    }
}

QString StringFromDeviceType(DrDeviceType type) {
    switch (type) {
        case DrDeviceType::Camera:      return "Camera";
    }
}

QString StringFromEffectType(DrEffectType type) {
    switch (type) {
        case DrEffectType::Light:       return "Light";
        case DrEffectType::Water:       return "Water";
        case DrEffectType::Fire:        return "Fire";
        case DrEffectType::Mirror:      return "Mirror";
        case DrEffectType::Fisheye:     return "Fisheye";
        case DrEffectType::Swirl:       return "Swirl";
        case DrEffectType::Flag:        return "Flag";
        case DrEffectType::Rain:        return "Rain";
        case DrEffectType::Snow:        return "Snow";
        case DrEffectType::Clouds:      return "Clouds";
        case DrEffectType::Fog:         return "Fog";
    }
}

QList<long> ConvertPropertyListToLongs(QList<Properties> list) {
    QList<long> new_list;
    for (auto item : list) {
        new_list.append(static_cast<long>(item));
    }
    return new_list;
}






}   // namespace Dr












