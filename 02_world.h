//
//
//      DrWorld - Class to hold all scenes, variables and objects for one world
//
//

#ifndef DRWORLD_H
#define DRWORLD_H

#include "30_settings.h"

class DrProject;
class DrScene;
typedef std::map<long, DrScene*> SceneMap;


class DrWorld : public DrSettings
{
private:
    // Local variables
    SceneMap    m_scenes;                                   // map of pointers to DrScene classes       (holds the scenes for current world)

    DrProject  *m_parent_project;                           // holds reference to parent Project class that handles key generation for project


public:
    // Constructor & destructor
    DrWorld(DrProject *parent_project, long new_world_key, std::string new_world_name);
    virtual ~DrWorld() override;


    // Getters and setters
    virtual DrTypes getType() override  { return DrTypes::World; }
    DrProject*      getParentProject()  { return m_parent_project; }
    SceneMap        getSceneMap()       { return m_scenes; }


    // External calls
    void        addScene(std::string new_scene_name = "");
    void        initializeWorldSettings(std::string new_name);

    long        getFirstSceneKey();
    DrScene*    getScene(long from_scene_key);
    DrScene*    getSceneWithName(std::string scene_name);



};


#endif // DRWORLD_H














