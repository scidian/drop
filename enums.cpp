//
//      Created by Stephens Nunnally on 12/7/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Functions associated with enums
//
//
#include "enums.h"
#include "enums_engine.h"
#include "editor/editor_item.h"
#include "editor/editor_scene.h"
#include "project/project_world_stage_thing.h"
#include "settings/settings.h"


namespace Dr {


//####################################################################################
//##    Returns a QString respresentaiton of the passed DrTypes type
//####################################################################################
QString StringFromType(DrType type) {
    switch (type) {
        case DrType::Asset:        return "Asset";
        case DrType::Font:         return "Font";
        case DrType::Image:        return "Image";
        case DrType::World:        return "World";
        case DrType::Stage:        return "Stage";
        case DrType::StartStage:   return "Start Stage";
        case DrType::Thing:        return "Thing";
        case DrType::NotFound:     return "Not Found";
        case DrType::BaseClass:    return "Base Class";
    }
    return "Unknown";
}

QString StringFromAssetType(DrAssetType type) {
    switch (type) {
        case DrAssetType::Object:       return "Object Asset";
        case DrAssetType::Character:    return "Character Asset";
        case DrAssetType::Effect:       return "Effect Asset";
        case DrAssetType::Text:         return "Text Asset";

        ///case DrAssetType::Action:       return "Action Asset";
        ///case DrAssetType::Logic:        return "Logic Asset";
    }
    return "Unknown";
}

QString StringFromThingType(DrThingType type) {
    switch (type) {
        case DrThingType::Character:    return "Character";
        case DrThingType::Object:       return "Object";
        case DrThingType::Light:        return "Light";
        case DrThingType::Text:         return "Text";
        case DrThingType::Water:        return "Water";

        ///case DrThingType::Action:       return "Action";
        ///case DrThingType::Camera:       return "Camera";
        ///case DrThingType::Logic:        return "Logic";
        ///case DrThingType::Particle:     return "Particle";
    }
}

QString StringFromPositionFlag(Position_Flags flag) {
    switch (flag) {
        case Position_Flags::Top:           return "Top";
        case Position_Flags::Top_Right:     return "Top Right";
        case Position_Flags::Top_Left:      return "Top Left";
        case Position_Flags::Bottom:        return "Bottom";
        case Position_Flags::Bottom_Left:   return "Bottom Left";
        case Position_Flags::Bottom_Right:  return "Bottom Right";
        case Position_Flags::Left:          return "Left";
        case Position_Flags::Right:         return "Right";
        case Position_Flags::Center:        return "Center";
        case Position_Flags::Rotate:        return "Rotate";
        case Position_Flags::Move_Item:     return "Move Item";
        case Position_Flags::No_Position:   return "No Position";
    }
}

QString StringFromBodyType(Body_Type type) {
    switch (type) {
        case Body_Type::Static:             return "Static";
        case Body_Type::Dynamic:            return "Dynamic";
        case Body_Type::Kinematic:          return "Kinematic";
    }
}

QString StringFromShapeType(Shape_Type type) {
    switch (type) {
        case Shape_Type::Segment:           return "Segment";
        case Shape_Type::Box:               return "Box";
        case Shape_Type::Circle:            return "Circle";
        case Shape_Type::Polygon:           return "Polygon";
    }
}


QList<long> ConvertPropertyListToLongs(QList<Properties> list) {
    QList<long> new_list;
    for (auto item : list)
        new_list.append(static_cast<long>(item));
    return new_list;
}

QList<DrSettings*> ConvertItemListToSettings(QList<QGraphicsItem*> list) {
    QList<DrSettings*> new_list;
    for (auto item : list) {
        DrItem* as_item = dynamic_cast<DrItem*>(item);
        DrThing* as_thing = as_item->getThing();
        new_list.append(dynamic_cast<DrSettings*>(as_thing));
    }
    return new_list;
}





}   // namespace Dr












