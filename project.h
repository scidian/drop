//
//      Created by Stephens Nunnally on 12/7/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrProject - Class to hold data for one project
//
//

#ifndef DRPROJECT_H
#define DRPROJECT_H

#include "settings.h"

class DrAsset;
class DrWorld;
class DrScene;
class DrObject;
typedef std::map<long, DrWorld*> WorldMap;
typedef std::map<long, DrAsset*> AssetMap;

//####################################################################################
//##    DrProject
//##        Class to hold data for one project
//############################
class DrProject
{
private:
    // Local variables
    WorldMap    m_worlds;                               // Holds worlds for the project
    AssetMap    m_assets;                               // Holds assets for the project

    long        m_key_generator;                        // variable to hand out unique id key's to all children objects

public:
    // Constructor & destructor
    DrProject(long key_generator_starting_number = 0);
    ~DrProject();

    // Getters and Setters
    long        getNextKey()   { return m_key_generator++; }
    void        setKeyGeneratorStartNumber(long initial_key) { m_key_generator = initial_key; }

    DrAsset*    getAsset(long key)  { return m_assets[key]; }
    AssetMap    getAssets()         { return m_assets; }
    long        getNumberOfAssets() { return static_cast<long>(m_assets.size()); }

    long        getFirstWorldKey()  { return m_worlds.begin()->first; }
    long        getNumberOfWorlds() { return static_cast<long>(m_worlds.size()); }
    DrWorld*    getWorld(long key)  { return m_worlds[key]; }
    WorldMap    getWorlds()         { return m_worlds; }
    DrWorld*    getWorldWithName(QString world_name);


    // External calls
    DrScene*        findSceneFromKey(long key);

    DrSettings*     findChildSettingsFromKey(long check_key);
    DrType          findChildTypeFromKey(long check_key);

    DrSettings*     findAssetSettingsFromKey(long check_key);
    DrAsset_Type    findAssetTypeFromKey(long check_key);

    // Children creation calls
    void        addWorld();
    long        addAsset(QString new_asset_name, DrAsset_Type new_asset_type, QPixmap pixmap);

};

#endif // DRPROJECT_H










