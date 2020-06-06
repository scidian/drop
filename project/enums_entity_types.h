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
class DrComponent;
class DrProject;
class DrProperty;
class DrSettings;
class DrSlot;
class IEditorRelay;
enum class Editor_Widgets;

// Global Enum Constants
constexpr int       c_no_key =                  -1;                 // Value that represents no item selected
constexpr int       c_same_key =                -100;               // Value signifying to use the value already obtained

// DrProject Constants
constexpr double    c_node_default_width =      192;                // Starting width of GraphicsItem
constexpr double    c_node_row_height =         32;                 // Height of each row (title, input / output)
constexpr double    c_node_spacing =            32;                 // Default seperation between new Nodes


//####################################################################################
//##
//##    Main Types of Entities possible in Project
//##        - All Entities inherit DrSettings to use DrComponents which contain DrProperties compatible with the Inspector
//##
//############################
enum class DrType {    
    // Misc Types
    NotFound = 0,                   // For passing a value in functions that represents No Type Selected / Found, !!!!! #NOTE: Keep as zero

    // Shared Types
    Animation,                      // Entities contained within DrProject::m_animations
        Frame,                      // Entities contained within DrProject::DrAnimation::m_frames
    Font,                           // Entities contained within DrProject::m_fonts
    Image,                          // Entities contained within DrProject::m_images

    // Mapped Types
    World,                          // Entities contained within DrProject::m_worlds / DrProject::m_uis
        Stage,                      // Entities contained within DrProject::DrWorld::m_stages
            Thing,                  // Entities contained within DrProject::DrWorld::DrStage::m_things
///         Background,
///         Foreground,

    // Assets for View Node: World Graph
    Block,                          // Entities contained within DrProject::m_blocks

    // Assets for View Editor: World Creator (2D Physics)
    Asset,                          // Entities contained within DrProject::m_assets
    Device,                         // Entities contained within DrProject::m_devices
    Effect,                         // Entities contained within DrProject::m_effects
    Item,                           // Entities contained within DrProject::m_items
    Prefab,                         // Entities contained within DrProject::m_prefabs
/// Logic

    // Assets for View Editor: UI Creator
/// Buttons,                        // button types
/// Controls,                       // joystick, etc?

    // Assets for View Mixer: Sound Creator
    Mix,                            // Entities contained within DrProject::m_mixes
        Track,                      // Entities contained within DrProject::m_mixes::m_tracks
    Sound,                          // Entities contained within DrProject::m_sounds
};


// ################## Misc Types ####################
// Slots
enum class DrSlotType {
    Signal,
    Output,
};

// DrSound Types
enum class DrSoundType {
    Audio_File,
    Instrument,
    Mix,
    Noise,
    Sound_Effect,
    Speech,
};


// ################## Project Mapped Types ####################
// Worlds,    Entities contained within DrProject::m_worlds / DrProject::m_uis
enum class DrWorldType {
    // Editor Worlds
    Physics_2D,
    UI,

    // Worlds Used As Nodes in World Graph
    Random_Out,
    Timed_Pause,
};


// Things,    Entities contained within DrProject::DrWorld::DrStage::m_things
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



// ################## Assets for World Graph Sub Types ####################
// Blocks,    Entities contained within DrProject::m_blocks
enum class DrBlockType {
    World,                  // Physics 2D, UI, future 3D, Cards, Tower, etc
    Logic,                  // Random, Timer, etc
};


// ################## Assets for World Physics 2D Sub Types ####################
// Assets,    Entities contained within DrProject::m_assets
enum class DrAssetType {
    Character,
    Object,
    // Action
};

// Devices,   Entities contained within DrProject::m_devices
enum class DrDeviceType {
    Camera,
};

// Effects,   Entities contained within DrProject::m_effects
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

// Items,     Entities contained within DrProject::m_items
enum class DrItemType {
    Tile,
};

// Prefabs,   Entities contained within DrProject::m_prefabs
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
namespace Asset_Category {
    // View Node: World Graph
    const std::string   Worlds          = "Worlds";
    const std::string   Logic           = "Logic";

    // View Editor: World Creator (2D Physics)
    const std::string   Characters      = "Characters";
    const std::string   Objects         = "Objects";
    const std::string   Devices         = "Devices";
    const std::string   Effects         = "Effects";
    const std::string   Items           = "Items";
    const std::string   Prefabs         = "Prefabs";
    const std::string   Text            = "Text";
    const std::string   Images          = "Images";

    // View Editor: UI Creator
    const std::string   Buttons         = "Buttons";
    const std::string   Controls        = "Controls";

    // View Mixer: Sound Creator
    const std::string   Audio_Files     = "Audio";
    const std::string   Instruments     = "Instruments";
    const std::string   Mixes           = "Mixes";
    const std::string   Noise           = "Noise";
    const std::string   Sound_Effects   = "Sound Effects";
    const std::string   Speech          = "Speech";
};


//####################################################################################
//##    Misc Model Enums
//############################
// Numerically equivalent to Qt::SortOrder
enum class Sort_Order {
    AscendingOrder   = 0,
    DescendingOrder  = 1,
};

enum class Z_Insert {
    Back,
    Front,
    At_Position,
};

enum class Direction {
    Up, Down, Left, Right
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
    std::string         StringFromThingType(DrThingType type);
    std::string         StringFromWorldType(DrWorldType type);
}





#endif // ENUMS_TYPES_H
















