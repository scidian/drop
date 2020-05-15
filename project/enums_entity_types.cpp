//
//      Created by Stephens Nunnally on 12/7/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Functions associated with enums
//
//
#include "project/enums_entity_types.h"
#include "project/settings/settings.h"

namespace Dr {


//####################################################################################
//##    Returns a std::string respresentation of the passed DrTypes type
//####################################################################################
std::string StringFromType(DrType type) {
    switch (type) {
        case DrType::Animation:             return "Animation";
        case DrType::Asset:                 return "Asset";
        case DrType::Block:                 return "Block";
        case DrType::Device:                return "Device";
        case DrType::Effect:                return "Effect";
        case DrType::Font:                  return "Font";
        case DrType::Frame:                 return "Frame";
        case DrType::Image:                 return "Image";
        case DrType::Item:                  return "Item";
        case DrType::Node:                  return "Node";
        case DrType::Prefab:                return "Prefab";
        case DrType::Stage:                 return "Stage";
        case DrType::Thing:                 return "Thing";
        case DrType::World:                 return "World";

        case DrType::NotFound:              return "Not Found";
    }
    return "Unknown";
}

std::string StringFromAssetType(DrAssetType type) {
    switch (type) {
        case DrAssetType::Object:           return "Object";
        case DrAssetType::Character:        return "Character";

        ///case DrAssetType::Action:        return "Action Asset";
    }
    return "Unknown";
}

std::string StringFromDeviceType(DrDeviceType type) {
    switch (type) {
        case DrDeviceType::Camera:          return "Camera";
    }
}

std::string StringFromEffectType(DrEffectType type) {
    switch (type) {
        case DrEffectType::Light:           return "Light";
        case DrEffectType::Water:           return "Water";
        case DrEffectType::Fire:            return "Fire";
        case DrEffectType::Mirror:          return "Mirror";
        case DrEffectType::Fisheye:         return "Fisheye";
        case DrEffectType::Swirl:           return "Swirl";
        ///case DrEffectType::Flag:         return "Flag";
        ///case DrEffectType::Rain:         return "Rain";
        ///case DrEffectType::Snow:         return "Snow";
        ///case DrEffectType::Clouds:       return "Clouds";
        ///case DrEffectType::Fog:          return "Fog";
    }
}

std::string StringFromItemType(DrItemType type) {
    switch (type) {
        case DrItemType::Tile:              return "Tile";
    }
}

std::string StringFromPrefabType(DrPrefabType type) {
    switch (type) {
        case DrPrefabType::Blob:            return "Blob";
        case DrPrefabType::Character:       return "Character";
        case DrPrefabType::Foliage:         return "Foliage";
        case DrPrefabType::Ground:          return "Ground";
        case DrPrefabType::Ladder:          return "Ladder";
        case DrPrefabType::Object:          return "Object";
        case DrPrefabType::Spike:           return "Spike";
        case DrPrefabType::Spring:          return "Spring";
    }
}

std::string StringFromThingType(DrThingType type) {
    switch (type) {
        case DrThingType::None:             return "None";

        // ***** 2D Physics World Sub Types
        // Asset Things
        case DrThingType::Character:        return "Character";
        case DrThingType::Object:           return "Object";
        case DrThingType::Text:             return "Text";

        // Device Things
        case DrThingType::Camera:           return "Camera";

        // Effect Things
        case DrThingType::Fire:             return "Fire";
        case DrThingType::Fisheye:          return "Fisheye";
        case DrThingType::Light:            return "Light";
        case DrThingType::Mirror:           return "Mirror";
        case DrThingType::Swirl:            return "Swirl";
        case DrThingType::Water:            return "Water";

        // Item Things
        case DrThingType::Tile:             return "Tile";

        // ***** UI World Sub Types
        case DrThingType::Button:           return "Button";
        case DrThingType::Joystick:         return "Joystick";
        case DrThingType::Label:            return "Label";
        case DrThingType::Navigation:       return "Navigation";
    }
}

std::string StringFromWorldType(DrWorldType type) {
    switch (type) {
        case DrWorldType::Physics_2D:       return "2D Physics";
        case DrWorldType::UI:               return "UI";
        case DrWorldType::Random_Out:       return "Random Out";
        case DrWorldType::Timed_Pause:      return "Timed Pause";
    }
}


}   // namespace Dr





















