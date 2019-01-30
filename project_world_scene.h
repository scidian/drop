//
//      Created by Stephens Nunnally on 12/7/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrScene - Class to hold one scene within a world
//
//

#ifndef DRSCENE_H
#define DRSCENE_H

#include "settings.h"

class DrProject;
class DrAsset;
class DrWorld;
class DrObject;
typedef std::map<long, DrObject*> ObjectMap;


//####################################################################################
//##    DrScene
//##        Class to hold one scene within a world
//############################
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
    DrScene(DrProject *parent_project, DrWorld *parent_world, long new_scene_key, QString new_scene_name, bool is_start_scene = false);
    virtual ~DrScene() override;


    // Getters and setters
    virtual DrType  getType() override  { return DrType::Scene; }
    DrProject*      getParentProject()  { return m_parent_project; }
    DrWorld*        getParentWorld()    { return m_parent_world; }
    ObjectMap       getObjectMap()      { return m_objects; }
    DrObject*       getObject(long key) { return m_objects[key]; }

    // External calls
    void            addObject(DrType new_type, long from_asset_key, double x, double y);
    QList<long>     objectKeysSortedByZOrder();
    void            initializeSceneSettings(QString new_name);

};



#endif // DRSCENE_H
