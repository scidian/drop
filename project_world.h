//
//      Created by Stephens Nunnally on 12/7/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrWorld - Class to hold all scenes, variables and objects for one world
//
//

#ifndef DRWORLD_H
#define DRWORLD_H

#include "settings.h"

class DrProject;
class DrScene;
typedef std::map<long, DrScene*> SceneMap;


//####################################################################################
//##    DrWorld
//##        Class to hold all scenes, variables and objects for one world
//############################
class DrWorld : public DrSettings
{
private:
    // Local variables
    SceneMap    m_scenes;                                   // map of pointers to DrScene classes       (holds the scenes for current world)

    DrProject  *m_parent_project;                           // holds reference to parent Project class that handles key generation for project


public:
    // Constructor & destructor
    DrWorld(DrProject *parent_project, long new_world_key, QString new_world_name);
    virtual ~DrWorld() override;


    // Getters and setters
    virtual DrTypes getType() override  { return DrTypes::World; }
    DrProject*      getParentProject()  { return m_parent_project; }
    SceneMap        getSceneMap()       { return m_scenes; }


    // External calls
    void        addScene(QString new_scene_name = "");
    void        initializeWorldSettings(QString new_name);

    long        getFirstSceneKey();
    DrScene*    getScene(long from_scene_key);
    DrScene*    getSceneWithName(QString scene_name);



};


#endif // DRWORLD_H














