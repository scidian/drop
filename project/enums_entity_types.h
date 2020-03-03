//
//      Created by Stephens Nunnally on 12/7/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      List of Type Enumerations used by this project
//
//
#ifndef ENUMS_TYPES_H
#define ENUMS_TYPES_H

#include <list>
#include <string>

// Forward declarations
class       DrSettings;
enum class  Editor_Widgets;


// Global Enum Constants
constexpr int   c_no_key =        -1;               // Value that represents no item selected
constexpr int   c_same_key =    -100;               // Value signifying to use the value already obtained


//####################################################################################
//##
//##    Main Types of Entities possible in Project
//##        - All Entities inherit DrSettings to use DrComponents which contain DrProperties compatible with the Inspector
//##
//############################
enum class DrType {
    Animation,
        Frame,
    Asset,
    Device,
    Effect,
    Font,
    Image,
    Item,
    World,
        Stage,
            Thing,

    //        Background,
    //        Foreground,

    //Logic,
    //Variable,

    //UI,
    //    Label,
    //    Button,
    //    Joystick,

    NotFound,
};

// ################## Sub Types ####################
enum class DrAssetType {
    Character,
    Object,
    // Action
};

enum class DrDeviceType {
    Camera,
};

enum class DrEffectType {
    Light,
    Water,
    Fire,
    Mirror,
    Fisheye,
    Swirl,
    // Flag,
    // Rain,
    // Snow,
    // Clouds,
    // Fog,
};

enum class DrItemType {
    Foliage,
};

enum class DrThingType {
    None,

    // Pure Things
    Character,
    Object,
    Text,

    // Device Things
    Camera,

    // Effect Things
    Fire,
    Fisheye,
    Light,
    Mirror,
    Swirl,
    Water,

    // Item Things
    Foliage,

    // Action,
    // Logic,
    // Particle,
};


//####################################################################################
//##    Categories for the Asset Tree
//############################
enum class Asset_Category {
    Character,
    Object,
    Device,
    Effect,
    Item,
    Text,
    Image,

    Basic,
    Outlines,
    Gradient,
    Decoration,
    Ground,
    Polygons,
    Shapes,
    Isometric,
};


//####################################################################################
//##    Misc Model Enums (Numerically equivalent to Qt::SortOrder)
//############################
enum class Sort_Order {
    AscendingOrder   = 0,
    DescendingOrder  = 1,
};


//####################################################################################
//##    Some public forward function declarations for some enum functions
//############################
namespace Dr {
    std::string         StringFromType(DrType type);
    std::string         StringFromAssetType(DrAssetType type);
    std::string         StringFromDeviceType(DrDeviceType type);
    std::string         StringFromEffectType(DrEffectType type);
    std::string         StringFromItemType(DrItemType type);
    std::string         StringFromThingType(DrThingType type);
}





#endif // ENUMS_TYPES_H
















