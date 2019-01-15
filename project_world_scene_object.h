//
//      Created by Stephens Nunnally on 12/7/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrObject - Class to hold one object within a scene, can holds more objects?
//
//

#ifndef DROBJECT_H
#define DROBJECT_H

#include "settings.h"

class DrProject;
class DrWorld;
class DrScene;

class DrObject : public DrSettings
{
private:
    // Local variables
    DrTypes     m_object_type;                              // holds type of current object

    DrProject  *m_parent_project;                           // holds reference to parent Project class that handles key generation for project
    DrWorld    *m_parent_world;                             // holds reference to parent World class
    DrScene    *m_parent_scene;                             // holds reference to parent World class

    //long m_asset_key;                            // TODO: will hold the associated asset key, this way we know what image to grab for GraphicsView
                                                   //       will need to preload some assets that don't use GraphicsItems (like camera, particle, trail, etc)

public:
    // Constructor & destructor
    DrObject(DrProject *parent_project, DrWorld *parent_world, DrScene *parent_scene,
             long new_object_key, std::string new_object_name, DrTypes new_object_type);
    virtual ~DrObject() override;


    // Getters and setters
    DrProject*      getParentProject()  { return m_parent_project; }
    DrWorld*        getParentWorld()    { return m_parent_world; }
    DrScene*        getParentScene()    { return m_parent_scene; }
    virtual DrTypes getType() override  { return m_object_type; }


    // External calls
    void initializeObjectSettings(std::string new_name);
    void initializeCameraSettings();
    void initializeCharacterSettings();

};


#endif // DROBJECT_H







