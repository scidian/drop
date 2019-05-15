//
//      Created by Stephens Nunnally on 12/7/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrProject - Class to hold data for one project
//
//
#ifndef DRPROJECT_H
#define DRPROJECT_H

#include "settings/settings.h"

enum class Project_Options {
    Name,                       // string

    Current_World,              // long
    Current_Stage,              // long

    Orientation,                // int (enum Orientation)
};

enum class Orientation {
    Portrait,
    Landscape
};

// Forward declarations
class DrAsset;
class DrFont;
class DrImage;
class DrWorld;
class DrStage;
class DrObject;

// Type definitions
typedef std::map<long, DrWorld*> WorldMap;
typedef std::map<long, DrFont*>  FontMap;
typedef std::map<long, DrAsset*> AssetMap;
typedef std::map<long, DrImage*> ImageMap;
typedef std::map<Project_Options, QVariant>  OptionMap;

// Constants
constexpr long c_key_starting_number = 1;

//####################################################################################
//##    DrProject
//##        Class to hold data for one project
//############################
class DrProject
{
private:
    // Project Variables
    long        m_key_generator;                        // Variable to hand out unique id key's to all children objects

    WorldMap    m_worlds;                               // Holds worlds for the project
    AssetMap    m_assets;                               // Holds assets for the project
    FontMap     m_fonts;
    ImageMap    m_images;                               // Holds images for the project

    OptionMap   m_options;                              // Map holding DrProject Wide options


public:
    // Constructor & destructor
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

    DrFont*     getDrFont(long key)     { return m_fonts[key]; }
    FontMap&    getDrFontMap()          { return m_fonts; }
    DrImage*    getDrImage(long key)    { return m_images[key]; }
    ImageMap&   getDrImageMap()         { return m_images; }

    // Options calls
    QVariant    getOption(Project_Options option_to_get)   { return m_options[option_to_get]; }
    void        setOption(Project_Options option_to_set, QVariant new_value) { m_options[option_to_set] = new_value; }

    Orientation getOptionOrientation() { return static_cast<Orientation>( m_options[Project_Options::Orientation].toInt()); }


    // External calls
    DrSettings*     findSettingsFromKey(long check_key);
    DrType          findChildTypeFromKey(long check_key);

    DrAsset*        findAssetFromKey(long check_key);
    DrStage*        findStageFromKey(long check_key);
    DrObject*       findObjectFromKey(long check_key);

    QString         testSpeedFindSettings(int test_size);

    // Children creation calls
    void            addWorld();
    long            addAsset(DrAssetType new_asset_type, long image_key);
    long            addFont( QString font_name, QPixmap font_pixmap, QString font_family, int font_size, bool use_test_rects = false);
    long            addImage(QString image_path);

};

#endif // DRPROJECT_H








