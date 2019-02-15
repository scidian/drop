//
//      Created by Stephens Nunnally on 12/7/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Functions associated with enums
//
//

#include "enums.h"


namespace Dr {

//####################################################################################
//##        Returns true if same class type (or if both are DrObjects)
//####################################################################################
bool CheckTypesAreSame(DrType type1, DrType type2)
{
    if (IsDrObjectClass(type1) == true && IsDrObjectClass(type2) == true)
        return true;
    else
        return (type1 == type2);
}

//####################################################################################
//##        Returns true if type is an DrObject sub-type
//####################################################################################
bool IsDrObjectClass(DrType type_to_check)
{
    switch (type_to_check)
    {
    case DrType::Object:
    case DrType::Camera:
    case DrType::Character:
        return true;
    default:
        return false;
    }
}

//####################################################################################
//##    Returns a QString respresentaiton of the passed DrTypes type
//####################################################################################
QString StringFromType(DrType type)
{
    switch (type)
    {
    case DrType::Project:      return "Project";
    case DrType::Folder:       return "Folder";
    case DrType::World:        return "World";
    case DrType::Stage:        return "Stage";
    case DrType::Background:   return "Background";
    case DrType::Foreground:   return "Foreground";
    case DrType::StartStage:   return "Start Stage";
    case DrType::Variable:     return "Variable";
    case DrType::Object:       return "Object";
    case DrType::Character:    return "Character";
    case DrType::Action:       return "Action";
    case DrType::Camera:       return "Camera";
    case DrType::Light:        return "Light";
    case DrType::Logic:        return "Logic";
    case DrType::Particle:     return "Particle";
    case DrType::UI:           return "UI";
    case DrType::Label:        return "Label";
    case DrType::Button:       return "Button";
    case DrType::Joystick:     return "Joystick";
    case DrType::NotFound:     return "Not Found";
    case DrType::BaseClass:    return "Base Class";
    case DrType::Asset:        return "Asset";
    }
    return "Unknown";
}

QString StringFromAssetType(DrAsset_Type type)
{
    switch (type)
    {
    case DrAsset_Type::Object:      return "Object Asset";
    case DrAsset_Type::Character:   return "Character Asset";
    case DrAsset_Type::NotFound:    return "Not Found";
    }
    return "Unknown";
}


QString StringFromPositionFlag(Position_Flags flag)
{
    switch (flag)
    {
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






}   // END NAMESPACE DR












