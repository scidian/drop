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
class DrAsset;
class DrEffect;
class DrFont;
class DrImage;
class DrWorld;
class DrStage;
class DrThing;

// Type definitions
typedef std::map<long, DrWorld*>  WorldMap;
typedef std::map<long, DrAsset*>  AssetMap;
typedef std::map<long, DrEffect*> EffectMap;
typedef std::map<long, DrFont*>   FontMap;
typedef std::map<long, DrImage*>  ImageMap;
typedef std::map<Project_Options, QVariant>  OptionMap;

// Constants
constexpr long c_key_starting_number = 1;

//####################################################################################
//##    DrProject
//##        Class to hold data for one Project
//############################
class DrProject
{
private:
    // Project Variables
    long        m_key_generator;                        // Variable to hand out unique id key's to all children items
    OptionMap   m_options;                              // Map holding DrProject Wide options

    // **********
    //      Every Item In These Project Maps is called an Entity and:
    //          - Inherit DrSettings for DrComponent / DrProperty usage
    //          - Is assigned an unique key upon creation from DrProject::getNextKey()
    //
    WorldMap    m_worlds;                               // Holds worlds for the project
    AssetMap    m_assets;                               // Holds assets for the project
    EffectMap   m_effects;                              // Holds effects for the project
    FontMap     m_fonts;                                // Holds custom fonts for the project
    ImageMap    m_images;                               // Holds images for the project




public:
    // Constructor & Destructor
    DrProject(long key_generator_starting_number = c_key_starting_number);
    ~DrProject();

    // Getters and Setters
    long        getNextKey()            { return m_key_generator++; }
    void        setKeyGeneratorStartNumber(long initial_key) { m_key_generator = initial_key; }

    long        getFirstWorldKey()      { return m_worlds.begin()->first; }
    long        getNumberOfWorlds()     { return static_cast<long>(m_worlds.size()); }
    DrWorld*    getWorld(long key)      { return m_worlds[key]; }
    WorldMap&   getWorldMap()           { return m_worlds; }
    DrWorld*    findWorldWithName(QString world_name);

    DrAsset*    getAsset(long key)      { return m_assets[key]; }
    AssetMap&   getAssetMap()           { return m_assets; }
    long        getNumberOfAssets()     { return static_cast<long>(m_assets.size()); }

    DrEffect*   getEffect(long key)     { return m_effects[key]; }
    EffectMap&  getEffectMap()          { return m_effects; }
    DrFont*     getFont(long key)       { return m_fonts[key]; }
    FontMap&    getFontMap()            { return m_fonts; }
    DrImage*    getImage(long key)      { return m_images[key]; }
    ImageMap&   getImageMap()           { return m_images; }

    // Options Calls
    QVariant    getOption(Project_Options option_to_get)   { return m_options[option_to_get]; }
    void        setOption(Project_Options option_to_set, QVariant new_value) { m_options[option_to_set] = new_value; }

    Orientation getOptionOrientation() { return static_cast<Orientation>( m_options[Project_Options::Orientation].toInt()); }

    // Building
    void            clearProject();
    void            initializeNewProject(QString project_name, Orientation orientation, int width, int height, bool test = false);

    // Function Calls
    DrSettings*     findSettingsFromKey(long check_key, bool show_warning = true);
    DrType          findChildTypeFromKey(long check_key);

    DrAsset*        findAssetFromKey(long check_key);
    DrStage*        findStageFromKey(long check_key);
    DrThing*        findThingFromKey(long check_key);
    DrWorld*        findWorldFromKey(long check_key);

    QString         testSpeedFindSettings(int test_size);

    // Children Creation Calls
    long            addWorld();
    long            addAsset(DrAssetType new_asset_type, long image_key);
    long            addEffect(QString effect_name, DrEffectType effect_type);
    long            addFont( QString font_name, QPixmap font_pixmap, QString font_family, int font_size, bool use_test_rects = false);
    long            addImage(QString image_path);

};

#endif // DRPROJECT_H








