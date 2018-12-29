//
//
//      DrScene - Class to hold one scene within a world
//
//

#ifndef DRSCENE_H
#define DRSCENE_H

#include "30_settings.h"

class DrProject;
class DrWorld;
class DrObject;
typedef std::map<long, DrObject*> ObjectMap;

class DrScene : public DrSettings
{
private:
    // Local variables
    ObjectMap   m_objects;                                  // map of pointers to DrScene classes       (holds the scenes for current world)

    DrProject  *m_parent_project;                           // holds reference to parent Project class that handles key generation for project
    DrWorld    *m_parent_world;                             // holds reference to parent World class

    bool m_is_start_scene;

public:
    // Constructor & destructor
    DrScene(DrProject *parent_project, DrWorld *parent_world, long new_scene_key, std::string new_scene_name, bool is_start_scene = false);
    virtual ~DrScene() override;


    // Getters and setters
    virtual DrTypes getType() override  { return DrTypes::Scene; }
    DrProject*      getParentProject()  { return m_parent_project; }
    DrWorld*        getParentWorld()    { return m_parent_world; }
    ObjectMap       getObjectMap()      { return m_objects; }


    // External calls
    void addObject(DrTypes new_type);
    void initializeSceneSettings(std::string new_name);


};



#endif // DRSCENE_H
