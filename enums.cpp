//
//
//      Functions associated with enums
//
//

#include "enums.h"


//####################################################################################
//##        Returns true if same class type (or if both are DrObjects)
//####################################################################################
bool CheckTypesAreSame(DrTypes type1, DrTypes type2)
{
    if (IsDrObjectClass(type1) == true && IsDrObjectClass(type2) == true)
    {
        return true;
    }
    else {
        return (type1 == type2);
    }
}

//####################################################################################
//##        Returns true if type is an DrObject sub-type
//####################################################################################
bool IsDrObjectClass(DrTypes type_to_check)
{
    switch (type_to_check)
    {
    case DrTypes::Object:
    case DrTypes::Camera:
    case DrTypes::Character:
        return true;
    default:
        return false;
    }
}

//####################################################################################
//##    Returns a std::string respresentaiton of the passed DrTypes type
//####################################################################################
std::string StringFromType(DrTypes drtype)
{
    switch (drtype)
    {
    case DrTypes::Project:      return "Project";
    case DrTypes::Folder:       return "Folder";
    case DrTypes::World:        return "World";
    case DrTypes::Scene:        return "Scene";
    case DrTypes::Background:   return "Background";
    case DrTypes::Foreground:   return "Foreground";
    case DrTypes::StartScene:   return "Foreground";
    case DrTypes::Variable:     return "Variable";
    case DrTypes::Object:       return "Object";
    case DrTypes::Character:    return "Character";
    case DrTypes::Action:       return "Action";
    case DrTypes::Camera:       return "Camera";
    case DrTypes::Light:        return "Light";
    case DrTypes::Logic:        return "Logic";
    case DrTypes::Particle:     return "Particle";
    case DrTypes::UI:           return "UI";
    case DrTypes::Label:        return "Label";
    case DrTypes::Button:       return "Button";
    case DrTypes::Joystick:     return "Joystick";
    case DrTypes::NotFound:     return "Not Found";
    case DrTypes::BaseClass:    return "Base Class";
    }
    return "Unknown";
}









