//
//      Created by Stephens Nunnally on 12/7/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrThing - Class to hold one Thing within a Stage, can holds more Things?
//
//

#ifndef DRTHING_H
#define DRTHING_H

#include "settings/settings.h"

// Forward declarations
class DrItem;
class DrProject;
class DrAsset;
class DrWorld;
class DrStage;


//####################################################################################
//##    DrThing
//##        Class to hold one Thing within a Stage
//############################
class DrThing : public DrSettings
{
private:
    // External Borrowed Pointers
    DrProject      *m_parent_project;                   // Holds reference to parent Project class that handles key generation for project
    DrWorld        *m_parent_world;                     // Holds reference to parent World class
    DrStage        *m_parent_stage;                     // Holds reference to parent Stage class

    // Local Variables
    DrThingType     m_thing_type;                       // Holds type of current Thing
    long            m_asset_key;                        // Holds the associated asset key, this way we know what image to grab for GraphicsView
    DrItem         *m_item_in_scene = nullptr;          // Holds a pointer to a QGraphicsItem if this Thing is currently represented in the editor

public:
    // Constructor & destructor
    explicit DrThing(DrProject *parent_project, DrWorld *parent_world, DrStage *parent_stage, long new_thing_key,
                     QString new_thing_name, DrThingType new_thing_type, long from_asset_key, double x, double y, long z, bool should_collide = true);
    virtual ~DrThing() override;

    // DrSettings Base Class Overrides
    virtual DrType  getType() override          { return DrType::Thing; }

    // Getters and setters
    DrThingType     getThingType()              { return m_thing_type; }
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


#endif // DRTHING_H







