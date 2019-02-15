//
//      Created by Stephens Nunnally on 12/7/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrObject - Class to hold one object within a stage, can holds more objects?
//
//

#ifndef DROBJECT_H
#define DROBJECT_H

#include "settings.h"

class DrProject;
class DrAsset;
class DrWorld;
class DrStage;

class DrItem;

//####################################################################################
//##    DrObject
//##        Class to hold one object within a stage
//############################
class DrObject : public DrSettings
{
private:
    // Local variables
    DrObjectType    m_object_type;                          // holds type of current object

    DrProject      *m_parent_project;                       // holds reference to parent Project class that handles key generation for project
    DrWorld        *m_parent_world;                         // holds reference to parent World class
    DrStage        *m_parent_stage;                         // holds reference to parent Stage class

    long            m_asset_key;                            // holds the associated asset key, this way we know what image to grab for GraphicsView

    DrItem         *m_item_in_scene = nullptr;              // holds a pointer to a QGraphicsItem if this object is currently represented in the editor

public:
    // Constructor & destructor
    explicit DrObject(DrProject *parent_project, DrWorld *parent_world, DrStage *parent_stage, long new_object_key,
                      QString new_object_name, DrObjectType new_object_type, long from_asset_key, double x, double y, long z);
    virtual ~DrObject() override;

    // Virtual overrides
    virtual DrType  getType() override          { return DrType::Object; }
    DrObjectType    getObjectType()             { return m_object_type; }

    // Getters and setters
    DrProject*      getParentProject()          { return m_parent_project; }
    DrWorld*        getParentWorld()            { return m_parent_world; }
    DrStage*        getParentStage()            { return m_parent_stage; }
    long            getAssetKey()               { return m_asset_key; }

    DrItem*         getDrItem()                 { return m_item_in_scene; }
    void            setDrItem(DrItem *item)     { m_item_in_scene = item; }

    // External calls
    void initializeObjectSettings(QString new_name, double width, double height, double x, double y, long z);
    void initializeCameraSettings();
    void initializeCharacterSettings();

};


#endif // DROBJECT_H







