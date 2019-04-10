//
//      Created by Stephens Nunnally on 12/7/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Functions associated with enums
//
//
#include "enums.h"
#include "editor_item.h"
#include "editor_scene.h"
#include "project_world_stage_object.h"
#include "settings.h"


namespace Dr {


//####################################################################################
//##    Returns a QString respresentaiton of the passed DrTypes type
//####################################################################################
QString StringFromType(DrType type)
{
    switch (type) {
    case DrType::Asset:        return "Asset";
    case DrType::Font:         return "Font";
    case DrType::Image:        return "Image";
    case DrType::World:        return "World";
    case DrType::Stage:        return "Stage";
    case DrType::Background:   return "Background";
    case DrType::Foreground:   return "Foreground";
    case DrType::StartStage:   return "Start Stage";
    case DrType::Variable:     return "Variable";
    case DrType::Object:       return "Object";
    case DrType::UI:           return "UI";
    case DrType::Label:        return "Label";
    case DrType::Button:       return "Button";
    case DrType::Joystick:     return "Joystick";
    case DrType::NotFound:     return "Not Found";
    case DrType::BaseClass:    return "Base Class";
    }
    return "Unknown";
}

QString StringFromAssetType(DrAssetType type)
{
    switch (type) {
    case DrAssetType::Object:      return "Object Asset";
    case DrAssetType::Text:        return "Text Asset";
    case DrAssetType::Character:     return "Character Asset";

    ///case DrAssetType::Action:        return "Action Asset";
    }
    return "Unknown";
}

QString StringFromObjectType(DrObjectType type)
{
    switch (type) {
    case DrObjectType::Object:       return "Object";
    case DrObjectType::Text:         return "Text";
    case DrObjectType::Character:    return "Character";

    ///case DrObjectType::Action:       return "Action";
    ///case DrObjectType::Camera:       return "Camera";
    ///case DrObjectType::Light:        return "Light";
    ///case DrObjectType::Logic:        return "Logic";
    ///case DrObjectType::Particle:     return "Particle";
    }
}

QString StringFromPositionFlag(Position_Flags flag)
{
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
    case Position_Flags::Total:         return "Total";
    case Position_Flags::No_Position:   return "No Position";
    }
}


QList<long> ConvertPropertyListToLongs(QList<Properties> list) {
    QList<long> new_list;
    for (auto item : list) new_list.append(static_cast<long>(item));
    return new_list;
}

QList<DrSettings*> ConvertItemListToSettings(QList<QGraphicsItem*> list) {
    QList<DrSettings*> new_list;
    for (auto item : list) {
        DrItem* as_item = dynamic_cast<DrItem*>(item);
        DrObject* as_object = as_item->getObject();
        new_list.append(dynamic_cast<DrSettings*>(as_object));
    }
    return new_list;
}





}   // namespace Dr












