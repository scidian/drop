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
class DrImage;
class DrWorld;
class DrStage;
class DrObject;
typedef std::map<long, DrWorld*> WorldMap;
typedef std::map<long, DrAsset*> AssetMap;
typedef std::map<long, DrImage*> ImageMap;

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
    ImageMap    m_images;                               // Holds images for the project

    long        m_key_generator;                        // variable to hand out unique id key's to all children objects

public:
    // Constructor & destructor
    DrProject(long key_generator_starting_number = 1);
    ~DrProject();

    // Getters and Setters
    long        getNextKey()   { return m_key_generator++; }
    void        setKeyGeneratorStartNumber(long initial_key) { m_key_generator = initial_key; }

    long        getFirstWorldKey()  { return m_worlds.begin()->first; }
    long        getNumberOfWorlds() { return static_cast<long>(m_worlds.size()); }
    DrWorld*    getWorld(long key)  { return m_worlds[key]; }
    WorldMap    getWorlds()         { return m_worlds; }
    DrWorld*    getWorldWithName(QString world_name);

    DrAsset*    getAsset(long key)  { return m_assets[key]; }
    AssetMap    getAssets()         { return m_assets; }
    long        getNumberOfAssets() { return static_cast<long>(m_assets.size()); }

    DrImage*    getImage(long key)  { return m_images[key]; }

    // External calls
    DrStage*        findStageFromKey(long check_key);
    DrSettings*     findSettingsFromKey(long check_key);
    DrType          findChildTypeFromKey(long check_key);


    // Children creation calls
    void        addWorld();
    long        addAsset(DrAssetType new_asset_type, long image_key);
    long        addImage(QString image_path);



};

#endif // DRPROJECT_H










