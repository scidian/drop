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
class DrAsset;
class DrWorld;
class DrScene;


//####################################################################################
//##    DrObject
//##        Class to hold one object within a scene
//############################
class DrObject : public DrSettings
{
private:
    // Local variables
    DrType      m_object_type;                              // holds type of current object

    DrProject  *m_parent_project;                           // holds reference to parent Project class that handles key generation for project
    DrWorld    *m_parent_world;                             // holds reference to parent World class
    DrScene    *m_parent_scene;                             // holds reference to parent World class

    long        m_asset_key;                                // holds the associated asset key, this way we know what image to grab for GraphicsView

    long        m_z_order;                                  // keeps track of the order of this item in its container

public:
    // Constructor & destructor
    explicit DrObject(DrProject *parent_project, DrWorld *parent_world, DrScene *parent_scene, long new_object_key,
                      QString new_object_name, DrType new_object_type, long from_asset_key, double x, double y, long z_order);
    virtual ~DrObject() override;


    // Getters and setters
    virtual DrType  getType() override  { return m_object_type; }
    DrProject*      getParentProject()  { return m_parent_project; }
    DrWorld*        getParentWorld()    { return m_parent_world; }
    DrScene*        getParentScene()    { return m_parent_scene; }
    long            getAssetKey()       { return m_asset_key; }
    long            getZOrder()         { return m_z_order; }

    // External calls
    void initializeObjectSettings(QString new_name, double x, double y);
    void initializeCameraSettings();
    void initializeCharacterSettings();

};


#endif // DROBJECT_H







