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

// Forward Declarations
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
    // Shared Types
    Animation,                  // Entities contained within DrProject::m_animations
        Frame,                  // Entities contained within DrProject::DrAnimation::m_frames
    Font,                       // Entities contained within DrProject::m_fonts
    Image,                      // Entities contained within DrProject::m_images

    // Mapped Types
    Node,                       // Entities contained within DrProject::m_nodes
    World,                      // Entities contained within DrProject::m_worlds / DrProject::m_uis
        Stage,                  // Entities contained within DrProject::DrWorld::m_stages
            Thing,              // Entities contained within DrProject::DrWorld::DrStage::m_things
///         Background,
///         Foreground,

    // Assets for World Map
    Block,                      // Entities contained within DrProject::m_blocks

    // Assets for Editor: World Physics 2D
    Asset,                      // Entities contained within DrProject::m_assets
    Device,                     // Entities contained within DrProject::m_devices
    Effect,                     // Entities contained within DrProject::m_effects
    Item,                       // Entities contained within DrProject::m_items
    Prefab,                     // Entities contained within DrProject::m_prefabs
/// Logic

    // Assets for Editor: World UI
/// Buttons,                    // button types
/// Controls,               // joystick, etc?

    // Misc Types
    NotFound,                   // For passing a value in functions that represents No Type Selected / Found
};


// ################## Project Mapped Types ####################
// ***** Nodes,     Entities contained within DrProject::m_nodes
enum class DrNodeType {
    // Logic
    Random_Out,
    Timed_Pause,
    World,
};
enum class DrSlotType {     Input,  Output,     };


// ***** Worlds,    Entities contained within DrProject::m_worlds / DrProject::m_uis
enum class DrWorldType {
    Physics_2D,
    UI,
};


// ***** Things,    Entities contained within DrProject::DrWorld::DrStage::m_things
enum class DrThingType {
    None,

    // ***** 2D Physics World Sub Types
    // Asset Things
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
    Tile,

    // Future Things...
/// Action,
/// Particle,

    // ***** UI World Sub Types
    Button,
    Joystick,
    Label,
    Navigation,
};



// ################## Assets for World Map Sub Types ####################
// ***** Blocks,    Entities contained within DrProject::m_blocks
enum class DrBlockType {
    World,
    Switch,
};


// ################## Assets for World Physics 2D Sub Types ####################
// ***** Assets,    Entities contained within DrProject::m_assets
enum class DrAssetType {
    Character,
    Object,
    // Action
};

// ***** Devices,   Entities contained within DrProject::m_devices
enum class DrDeviceType {
    Camera,
};

// ***** Effects,   Entities contained within DrProject::m_effects
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

// ***** Items,     Entities contained within DrProject::m_items
enum class DrItemType {
    Tile,
};

// ***** Prefabs,   Entities contained within DrProject::m_prefabs
enum class DrPrefabType {
    Blob,
    Character,
    Foliage,
    Ground,
    Ladder,
    Object,
    Spike,
    Spring,
};


//####################################################################################
//##    Categories for the Asset Tree
//############################
enum class Asset_Category {
    // World Map
    World,
    Switch,

    // World Physics 2D
    Character,
    Object,
    Device,
    Effect,
    Item,
    Prefab,
    Text,
    Image,

    // World UI
    Button,
    Control,

    // Images
    Basic,
    Outline,
    Gradient,
    Decoration,
    Ground,
    Polygon,
    Shape,
    Isometric,
};


//####################################################################################
//##    Misc Model Enums (Numerically equivalent to Qt::SortOrder)
//############################
enum class Sort_Order {
    AscendingOrder   = 0,
    DescendingOrder  = 1,
};

enum class Z_Insert {
    Back,
    Front,
    At_Position,
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
    std::string         StringFromPrefabType(DrPrefabType type);
    std::string         StringFromNodeType(DrNodeType type);
    std::string         StringFromThingType(DrThingType type);
    std::string         StringFromWorldType(DrWorldType type);
}





#endif // ENUMS_TYPES_H
















