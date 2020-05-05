//
//      Created by Stephens Nunnally on 12/7/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrProject - Class to hold data for one project
//
//
#ifndef DRPROJECT_H
#define DRPROJECT_H

#include "project/constants_entity_keys.h"
#include "project/settings/settings.h"


// Project Constants
const int   c_project_width =       800;            // Default Width  for Game
const int   c_project_height =     1600;            // Default Height for Game


enum class Project_Options {
    Name                = 10,       // string,  Name of Current Project
    File_Name_Path      = 11,       // string,  Full Path and File Name of Project, will save to this unless choose Save As

    Current_World       = 20,       // long,    World currently displayed in editor
    Current_Stage       = 21,       // long,    Scene currently displayed in editor

    Orientation         = 30,       // int,     This Projects target device orientation (enum Orientation)
    Width               = 31,       // long,    This Projects target device window width,  usually 800
    Height              = 32,       // long,    This Projects target device window height, usually 1600
};

enum class Orientation {
    Portrait            = 0,
    Landscape           = 1,
};

// Forward Declarations
class DrAnimation;
class DrAsset;
class DrBlock;
class DrDevice;
class DrEffect;
class DrFont;
class DrImage;
class DrItem;
class DrNode;
class DrPrefab;
class DrStage;
class DrThing;
class DrWorld;
class IProgressBar;

// Type Definitions
typedef std::map<Project_Options,   DrVariant>      OptionMap;

// Entity Type Definitions
typedef std::map<long,              DrAnimation*>   AnimationMap;
typedef std::map<long,              DrAsset*>       AssetMap;
typedef std::map<long,              DrBlock*>       BlockMap;
typedef std::map<long,              DrDevice*>      DeviceMap;
typedef std::map<long,              DrEffect*>      EffectMap;
typedef std::map<long,              DrFont*>        FontMap;
typedef std::map<long,              DrImage*>       ImageMap;
typedef std::map<long,              DrItem*>        ItemMap;
typedef std::map<long,              DrNode*>        NodeMap;
typedef std::map<long,              DrPrefab*>      PrefabMap;
typedef std::map<long,              DrWorld*>       WorldMap;


//####################################################################################
//##    DrProject
//##        Class to hold data for one Project
//############################
class DrProject
{
public:
    // Constructor / Destructor
    DrProject();
    ~DrProject();

    // #################### VARIABLES ####################
private:
    // Usage Variables
    bool            m_has_saved = false;                            // If project is edited at all, turns to false until user saves
    bool            m_test_only = false;                            // For debugging purposes, stops Drop from asking to save when exiting

    // Project Variables
    long            m_key_generator = c_key_starting_number;        // Variable to hand out unique id key's to all children entities
    OptionMap       m_options;                                      // Map holding DrProject Wide options

    // **********
    //      Every Item In These Project Maps is called an Entity and:
    //          - Inherit DrSettings for DrComponent / DrProperty usage
    //          - Is assigned an unique key upon creation from DrProject::getNextKey()
    //
    // Mapped Items
    NodeMap         m_nodes;                                        // Holds DrNodes        (World Map items that are not Worlds)
    WorldMap        m_worlds;                                       // Holds DrWorlds       (which in turn hold DrStages, which hold DrThings)

    // Shared Items
    AnimationMap    m_animations;                                   // Holds DrAnimations   (which in turn hold DrFrames)
    FontMap         m_fonts;                                        // Holds DrFonts        (currently custom bitmap fonts)
    ImageMap        m_images;                                       // Holds DrImages       (for use in DrFrames, loaded into DrEngineTextures)

    // World Map Items
    BlockMap        m_blocks;                                       // Holds DrBlocks       (used as building blocks for DrNodes)

    // Physics World 2D Items
    AssetMap        m_assets;                                       // Holds DrAssets       (used as building blocks for DrThings)
    DeviceMap       m_devices;                                      // Holds DrDevices      (used as building blocks for DrThings)
    EffectMap       m_effects;                                      // Holds DrEffects      (used as building blocks for DrThings)
    ItemMap         m_items;                                        // Holds DrItems        (used as building blocks for DrThings)
    PrefabMap       m_prefabs;                                      // Holds DrPrefabs      (used as blue prints for multiple types of pre-designed DrThings)


    // #################### FUNCTIONS TO BE EXPOSED TO API ####################
public:



    // #################### INTERNAL FUNCTIONS ####################
public:
    // Local Variable Functions
    long        checkCurrentKey()                               { return m_key_generator; }
    long        getNextKey()                                    { return m_key_generator++; }
    void        setKeyGeneratorStartNumber(long initial_key)    { m_key_generator = initial_key; }

    bool        hasSaved()                  { return m_has_saved; }
    bool        isTestOnly()                { return m_test_only; }
    void        setHasSaved(bool saved)     { m_has_saved = saved; }
    void        setTestOnly(bool test)      { m_test_only = test; }


    // Options Calls
    DrVariant       getOption(Project_Options option_to_get)                        { return m_options[option_to_get]; }
    void            setOption(Project_Options option_to_set, DrVariant new_value)   { m_options[option_to_set] = new_value; }

    Orientation     getOptionOrientation() { return static_cast<Orientation>( m_options[Project_Options::Orientation].toInt()); }


    // Finding Project Entities
    AnimationMap&   getAnimationMap()       { return m_animations; }
    AssetMap&       getAssetMap()           { return m_assets; }
    DeviceMap&      getDeviceMap()          { return m_devices; }
    EffectMap&      getEffectMap()          { return m_effects; }
    FontMap&        getFontMap()            { return m_fonts; }
    ImageMap&       getImageMap()           { return m_images; }
    ItemMap&        getItemMap()            { return m_items; }
    PrefabMap&      getPrefabMap()          { return m_prefabs; }
    WorldMap&       getWorldMap()           { return m_worlds; }

    long            getNumberOfAnimations() { return static_cast<long>(m_animations.size()); }
    long            getNumberOfAssets()     { return static_cast<long>(m_assets.size()); }
    long            getNumberOfDevices()    { return static_cast<long>(m_devices.size()); }
    long            getNumberOfEffects()    { return static_cast<long>(m_effects.size()); }
    long            getNumberOfFonts()      { return static_cast<long>(m_fonts.size()); }
    long            getNumberOfImages()     { return static_cast<long>(m_images.size()); }
    long            getNumberOfItems()      { return static_cast<long>(m_items.size()); }
    long            getNumberOfPrefabs()    { return static_cast<long>(m_prefabs.size()); }

    DrSettings*     findSettingsFromKey(long check_key, bool show_warning = true, std::string custom_error = "");
    DrType          findChildTypeFromKey(long check_key);

    DrAnimation*    findAnimationFromKey(long check_key);
    DrAsset*        findAssetFromKey(long check_key);
    DrDevice*       findDeviceFromKey(long check_key);
    DrEffect*       findEffectFromKey(long check_key);
    DrFont*         findFontFromKey(long check_key);
    DrImage*        findImageFromKey(long check_key);
    DrItem*         findItemFromKey(long check_key);
    DrPrefab*       findPrefabFromKey(long check_key);
    DrStage*        findStageFromKey(long check_key);
    DrThing*        findThingFromKey(long check_key);
    DrWorld*        findWorldFromKey(long check_key);

    DrDevice*       findDeviceFromType(DrDeviceType type);
    DrEffect*       findEffectFromType(DrEffectType type);
    DrItem*         findItemFromType(DrItemType type);
    DrPrefab*       findPrefabFromType(DrPrefabType type);

    // Worlds
    long            getFirstWorldKey()      { return m_worlds.begin()->first; }
    long            getNumberOfWorlds()     { return static_cast<long>(m_worlds.size()); }
    DrWorld*        findWorldWithName(std::string world_name);

    // Children Creation Calls
    DrAnimation*    addAnimation(std::list<long> source_image_keys, long key = c_no_key);
    DrAsset*        addAsset(DrAssetType new_asset_type, long source_image_key, long key = c_no_key);
    long            addDevice(DrDeviceType device_type, long key = c_no_key);
    long            addEffect(DrEffectType effect_type, long key = c_no_key);
    long            addFont(std::string font_name, DrBitmap font_bitmap, std::string font_family, int font_size, bool use_test_rects = false, long key = c_no_key);
    DrImage*        addImage(std::string image_name, DrBitmap &bitmap, Asset_Category category = Asset_Category::Image,
                             long key = c_no_key, IProgressBar *progress = nullptr);
    long            addItem(DrItemType item_type, long key = c_no_key);
    long            addPrefab(DrPrefabType prefab_type, long key = c_no_key);
    DrWorld*        addWorld(DrWorldType world_type);
    DrWorld*        addWorld(DrWorldType world_type, long key, long start_stage_key, long last_stage_in_editor_key);
    DrWorld*        addWorldCopyFromWorld(DrWorld* from_world, std::string new_name);


    // Project Building
    void            addDefaultAssets();
    void            clearProject(bool add_built_in_items = true);
    void            deleteAnimation(long animation_key, long ignore_asset_key);
    void            deleteAsset(long asset_key);
    void            deleteEntity(long entity_key);
    void            deleteFont(long font_key);
    void            deleteImage(long image_key);
    void            deleteWorld(long world_key);

};

#endif // DRPROJECT_H








