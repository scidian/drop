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

class DrWorld;
typedef std::map<long, DrWorld*> WorldMap;


class DrProject
{
private:
    // Local variables
    WorldMap    m_worlds;                               // map of pointers to DrWorld classes

    long        m_key_generator;                        // variable to hand out unique id key's to all children objects


public:
    // Constructor & destructor
    DrProject(long key_generator_starting_number = 0);
    ~DrProject();


    // Getters and setters
    WorldMap    getWorldMap()       { return m_worlds; }
    long        getNumberOfWorlds() { return static_cast<long>(m_worlds.size()); }

    long        getNextKey()        { return m_key_generator++; }
    void        setKeyGeneratorStartNumber(long initial_key) { m_key_generator = initial_key; }


    // External calls
    DrSettings* findSettingsFromKey(long check_key);
    DrTypes     findTypeFromKey(long check_key);

    void        addWorld();
    long        getFirstWorldKey();
    DrWorld*    getWorld(long from_world_key);
    DrWorld*    getWorldWithName(std::string world_name);



};

#endif // DRPROJECT_H










