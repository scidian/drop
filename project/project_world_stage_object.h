//
//      Created by Stephens Nunnally on 12/7/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrObject - Class to hold one object within a stage, can holds more objects?
//
//

#ifndef DROBJECT_H
#define DROBJECT_H

#include "settings/settings.h"

// Forward declarations
class DrItem;
class DrProject;
class DrAsset;
class DrWorld;
class DrStage;


//####################################################################################
//##    DrObject
//##        Class to hold one object within a stage
//############################
class DrObject : public DrSettings
{
private:
    // Local variables
    DrObjectType    m_object_type;                      // Holds type of current object

    DrProject      *m_parent_project;                   // Holds reference to parent Project class that handles key generation for project
    DrWorld        *m_parent_world;                     // Holds reference to parent World class
    DrStage        *m_parent_stage;                     // Holds reference to parent Stage class

    long            m_asset_key;                        // Holds the associated asset key, this way we know what image to grab for GraphicsView

    DrItem         *m_item_in_scene = nullptr;          // Holds a pointer to a QGraphicsItem if this object is currently represented in the editor

public:
    // Constructor & destructor
    explicit DrObject(DrProject *parent_project, DrWorld *parent_world, DrStage *parent_stage, long new_object_key,
                      QString new_object_name, DrObjectType new_object_type, long from_asset_key, double x, double y, long z, bool should_collide = true);
    virtual ~DrObject() override;

    // DrSettings Base Class Overrides
    virtual DrType  getType() override          { return DrType::Object; }

    // Getters and setters
    DrObjectType    getObjectType()             { return m_object_type; }
    DrProject*      getParentProject()          { return m_parent_project; }
    DrWorld*        getParentWorld()            { return m_parent_world; }
    DrStage*        getParentStage()            { return m_parent_stage; }
    long            getAssetKey()               { return m_asset_key; }

    DrItem*         getDrItem()                 { return m_item_in_scene; }
    void            setDrItem(DrItem *item)     { m_item_in_scene = item; }

    // Function calls
    void addComponentSettingsObject(QString new_name, bool should_collide = true);
    void addComponentSettingsCamera(QString new_name);
    void addComponentSettingsCharacter(QString new_name);
    void addComponentSettingsText(QString new_name);

    void addComponentAppearance();
    void addComponentMovement();
    void addComponentTransform(double width, double height, double x, double y, long z);

};


#endif // DROBJECT_H







