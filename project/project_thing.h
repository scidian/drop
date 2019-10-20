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
class DrThing;

struct OrderInfo {
    long     key;
    DrThing *thing;
    int      sub_order;
};

enum class Z_Insert {
    Back,
    Front,
    At_Position,
};

//####################################################################################
//##    DrThing
//##        Class to hold one Thing within a Stage
//############################
class DrThing : public DrSettings
{
private:
    // External Borrowed Pointers
    DrWorld        *m_parent_world;                     // Holds reference to parent World class
    DrStage        *m_parent_stage;                     // Holds reference to parent Stage class

    // Local Variables
    DrThingType     m_thing_type;                       // Holds type of current Thing
    long            m_asset_key;                        // Holds the associated asset key, this way we know what image to grab for GraphicsView
    DrItem         *m_item_in_scene = nullptr;          // Holds a pointer to a QGraphicsItem if this Thing is currently represented in the editor

public:
    // Constructor & destructor
    explicit DrThing(DrProject *parent_project, DrWorld *parent_world, DrStage *parent_stage, long new_thing_key,
                     QString new_thing_name, DrThingType new_thing_type, long from_asset_key, double x, double y, double z, bool should_collide = true);
    virtual ~DrThing() override;

    // DrSettings Base Class Overrides
    virtual DrType  getType() override          { return DrType::Thing; }

    // Getters and setters
    DrWorld*        getParentWorld()            { return m_parent_world; }
    DrStage*        getParentStage()            { return m_parent_stage; }

    long            getAssetKey()               { return m_asset_key; }
    DrItem*         getDrItem()                 { return m_item_in_scene; }
    DrThingType     getThingType()              { return m_thing_type; }

    void            setAssetKey(long key)       { m_asset_key = key; }
    void            setDrItem(DrItem *item)     { m_item_in_scene = item; }
    void            setThingType(DrThingType type) { m_thing_type = type; }

    // Z Ordering
    void            moveBackward();
    void            moveForward();
    double          getZOrderWithSub();
    void            setZOrderWithSub(double z_order, Z_Insert insert, int position = 0);

    // Thing Components
    void    addComponent3D();
    void    addComponentAppearance(bool bitrate_and_pixel_only = false);
    void    addComponentLayering(double z, double opacity = 100.0);
    void    addComponentLighting();
    void    addComponentMovement();
    void    addComponentSpecialEffects();
    void    addComponentTransform(double width, double height, double x, double y, DrThingType type);

    // Thing Specific Components
    void    addComponentSettingsCamera(QString new_name);
    void    addComponentSettingsCharacter(QString new_name);
    void    addComponentSettingsObject(QString new_name, bool should_collide = true);
    void    addComponentSettingsText(QString new_name);

    // Effect Specific Components
    void    addComponentSettingsFire();
    void    addComponentSettingsFisheye();
    void    addComponentSettingsLight(QColor color);
    void    addComponentSettingsMirror();
    void    addComponentSettingsSwirl();
    void    addComponentSettingsWater();

};


#endif // DRTHING_H





