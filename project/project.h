//
//      Created by Stephens Nunnally on 12/7/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrProject - Class to hold data for one project
//
//
#ifndef DRPROJECT_H
#define DRPROJECT_H

#include "constants.h"
#include "settings/settings.h"

enum class Project_Options {
    Name,                   // string,  Name of Current Project
    File_Name_Path,         // string,  Full Path and File Name of Project, will save to this unless choose Save As

    Current_World,          // long,    World currently displayed in editor
    Current_Stage,          // long,    Scene currently displayed in editor

    Orientation,            // int,     This Projects target device orientation (enum Orientation)
    Width,                  // long,    This Projects target device window width,  usually 800
    Height,                 // long,    This Projects target device window height, usually 1600
};

enum class Orientation {
    Portrait,
    Landscape
};

// Forward declarations
class DrAnimation;
class DrAsset;
class DrDevice;
class DrEffect;
class DrFont;
class DrImage;
class DrStage;
class DrThing;
class DrWorld;

// Type definitions
typedef std::map<long, DrAnimation*>    AnimationMap;
typedef std::map<long, DrAsset*>        AssetMap;
typedef std::map<long, DrDevice*>       DeviceMap;
typedef std::map<long, DrEffect*>       EffectMap;
typedef std::map<long, DrFont*>         FontMap;
typedef std::map<long, DrImage*>        ImageMap;
typedef std::map<long, DrWorld*>        WorldMap;

typedef std::map<Project_Options, QVariant>  OptionMap;


//####################################################################################
//##    Project Key Constants
//####################################################################################
// Starting Key
constexpr long c_key_starting_number =  1001;       // First 1001 keys or so are reserved for built in Entities in DrProject (like default images, etc)
                                                    // ....."1001" is an easily searchable number

// Built In Images
constexpr long c_key_asset_empty =      1;          //  32 x  32    Alpha value 0 square
constexpr long c_key_asset_character =  2;          // 256 x 256    Gray Circle
constexpr long c_key_asset_object =     3;          // 256 x 256    Gray Box


// Devices
constexpr long c_key_device_camera =    400;        // Camera Device

constexpr long c_key_asset_camera =     500;        // Camera Asset


// Effects
constexpr long c_key_effect_light =     600;        // Light Effect
constexpr long c_key_effect_water =     601;        // Water Effect
constexpr long c_key_effect_fire =      602;        // Fire Effect
constexpr long c_key_effect_mirror =    603;        // Mirror Effect
constexpr long c_key_effect_fisheye =   604;        // Fisheye Effect
constexpr long c_key_effect_swirl =     605;        // Swirl Effect

constexpr long c_key_asset_light =      700;        // Light Asset
constexpr long c_key_asset_water =      701;        // Water Asset
constexpr long c_key_asset_fire =       702;        // Fire Asset
constexpr long c_key_asset_mirror =     703;        // Mirror Asset
constexpr long c_key_asset_fisheye =    704;        // Fisheye Asset
constexpr long c_key_asset_swirl =      705;        // Swirl Asset


//####################################################################################
//##    DrProject
//##        Class to hold data for one Project
//############################
class DrProject
{
private:
    // Usage Variables
    bool        m_has_saved = false;                            // If project is edited at all, turns to false until user saves
    bool        m_test_only = false;                            // For debugging purposes, stops Drop from asking to save when exiting

    // Project Variables
    long        m_key_generator = c_key_starting_number;        // Variable to hand out unique id key's to all children items
    OptionMap   m_options;                                      // Map holding DrProject Wide options

    // **********
    //      Every Item In These Project Maps is called an Entity and:
    //          - Inherit DrSettings for DrComponent / DrProperty usage
    //          - Is assigned an unique key upon creation from DrProject::getNextKey()
    //
    AnimationMap    m_animations;                                   // Holds animations     for the project
    AssetMap        m_assets;                                       // Holds assets         for the project
    DeviceMap       m_devices;                                      // Holds devices        for the project
    EffectMap       m_effects;                                      // Holds effects        for the project
    FontMap         m_fonts;                                        // Holds custom fonts   for the project
    ImageMap        m_images;                                       // Holds images         for the project
    WorldMap        m_worlds;                                       // Holds worlds         for the project


public:
    // Constructor / Destructor
    DrProject();
    ~DrProject();

    // Getters / Setters
    long        getNextKey()            { return m_key_generator++; }
    void        setKeyGeneratorStartNumber(long initial_key) { m_key_generator = initial_key; }

    bool        hasSaved()              { return m_has_saved; }
    bool        isTestOnly()            { return m_test_only; }
    void        setHasSaved(bool saved) { m_has_saved = saved; }
    void        setTestOnly(bool test)  { m_test_only = test; }

    long            getFirstWorldKey()      { return m_worlds.begin()->first; }
    long            getNumberOfWorlds()     { return static_cast<long>(m_worlds.size()); }
    DrWorld*        getWorld(long key)      { return m_worlds[key]; }
    WorldMap&       getWorldMap()           { return m_worlds; }
    DrWorld*        findWorldWithName(QString world_name);

    DrAsset*        getAsset(long key)      { return m_assets[key]; }
    AssetMap&       getAssetMap()           { return m_assets; }
    long            getNumberOfAssets()     { return static_cast<long>(m_assets.size()); }

    DrAnimation*    getAnimation(long key)  { return m_animations[key]; }
    AnimationMap&   getAnimationMap()       { return m_animations; }
    DrDevice*       getDevice(long key)     { return m_devices[key]; }
    DeviceMap&      getDeviceMap()          { return m_devices; }
    DrEffect*       getEffect(long key)     { return m_effects[key]; }
    EffectMap&      getEffectMap()          { return m_effects; }
    DrFont*         getFont(long key)       { return m_fonts[key]; }
    FontMap&        getFontMap()            { return m_fonts; }
    DrImage*        getImage(long key)      { return m_images[key]; }
    ImageMap&       getImageMap()           { return m_images; }

    // Options Calls
    QVariant    getOption(Project_Options option_to_get)   { return m_options[option_to_get]; }
    void        setOption(Project_Options option_to_set, QVariant new_value) { m_options[option_to_set] = new_value; }

    Orientation getOptionOrientation() { return static_cast<Orientation>( m_options[Project_Options::Orientation].toInt()); }

    // Building
    void            addDefaultAssets();
    void            addSettingsToMap(DrSettings *entity, QVariantMap &map);
    void            clearProject(bool add_built_in_items = true);
    void            deleteAnimation(long animation_key);
    void            deleteWorld(DrWorld *world);
    void            initializeNewProject(QString project_name, Orientation orientation, int width, int height, bool test = false);
    void            loadSettingsFromMap(DrSettings *entity, QVariantMap &map);
    bool            openProjectFromFile(QString open_file);
    void            saveProjectToFile();

    // Function Calls
    DrSettings*     findSettingsFromKey(long check_key, bool show_warning = true, QString custom_error = "");
    DrType          findChildTypeFromKey(long check_key);

    DrAnimation*    findAnimationFromKey(long check_key);
    DrAsset*        findAssetFromKey(long check_key);
    DrDevice*       findDeviceFromType(DrDeviceType type);
    DrEffect*       findEffectFromType(DrEffectType type);
    DrStage*        findStageFromKey(long check_key);
    DrThing*        findThingFromKey(long check_key);
    DrWorld*        findWorldFromKey(long check_key);

    QString         testSpeedFindSettings(int test_size);

    // Children Creation Calls
    DrAnimation*    addAnimation(QList<long> source_image_keys, long key = c_no_key);
    DrAsset*        addAsset(DrAssetType new_asset_type, long source_image_key, long key = c_no_key);
    long            addDevice(DrDeviceType device_type, long key = c_no_key);
    long            addEffect(DrEffectType effect_type, long key = c_no_key);
    long            addFont(QString font_name, QPixmap font_pixmap, QString font_family, int font_size, bool use_test_rects = false, long key = c_no_key);
    DrImage*        addImage(QString image_path);
    long            addImage(long key, QString full_path, QString filename, QString simple_name, QImage &image);
    DrWorld*        addWorld();
    DrWorld*        addWorld(long key, long start_stage_key, long last_stage_in_editor_key);
    DrWorld*        addWorldCopyFromWorld(DrWorld* from_world);

};

#endif // DRPROJECT_H








