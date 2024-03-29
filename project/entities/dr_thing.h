//
//      Created by Stephens Nunnally on 12/7/18, (c) 2019 Scidian Studios, All Rights Reserved
//
//  File:
//      DrThing - Class to hold one Thing within a Stage, can holds more Things?
//
//
#ifndef DRTHING_H
#define DRTHING_H

#include "project/settings/settings.h"

// Forward Declarations
class DrAsset;
class DrProject;
class DrSettings;
class DrStage;
class DrThing;
class DrWorld;
class EditorItem;

// Local Structs / Enums
struct Order_Info_Thing {
    long        key;
    DrThing    *thing;
    int         sub_order;
};


//####################################################################################
//##    DrThing
//##        Class to hold one DrThing within a DrStage
//############################
class DrThing : public DrSettings
{

private:
    // External Borrowed Pointers
    DrWorld            *m_parent_world;                     // Holds reference to parent DrWorld class
    DrStage            *m_parent_stage;                     // Holds reference to parent DrStage class

    // Local Variables
    DrThingType         m_thing_type;                       // Holds type of current Thing
    long                m_asset_key;                        // Holds the associated asset (Asset, Effect, etc) key, this way we know what image to grab for GraphicsView
    EditorItem         *m_item_in_scene = nullptr;          // Holds a pointer to a QGraphicsItem if this Thing is currently represented in the editor

public:
    // Constructor / Destructor
    explicit DrThing(DrProject *parent_project, DrWorld *parent_world, DrStage *parent_stage, long new_thing_key,
                     std::string new_thing_name, DrThingType new_thing_type, DrSettings *from_entity, double x, double y, double z, bool should_collide = true);
    virtual ~DrThing() override;

    // DrSettings Base Class Overrides
    virtual DrType  getType() override          { return DrType::Thing; }

    // Getters / Setters
    DrWorld*        getParentWorld()            { return m_parent_world; }
    DrStage*        getParentStage()            { return m_parent_stage; }

    long            getAssetKey()               { return m_asset_key; }
    EditorItem*     getEditorItem()             { return m_item_in_scene; }
    DrThingType     getThingType()              { return m_thing_type; }

    void            setAssetKey(long key)               { m_asset_key = key; }
    void            setEditorItem(EditorItem *item)     { m_item_in_scene = item; }
    void            setThingType(DrThingType type)      { m_thing_type = type; }

    // Size Limits
    bool            keepSquare()        { return getComponentPropertyValue(Comps::Size_Settings, Props::Size_Keep_Square).toBool(); }
    DrPointF        maxSize()           { return getComponentPropertyValue(Comps::Size_Settings, Props::Size_Max_Size).toPointF();  }
    DrPointF        minSize()           { return getComponentPropertyValue(Comps::Size_Settings, Props::Size_Min_Size).toPointF();  }

    // Z-Ordering
    void            moveBackward();
    void            moveForward();
    double          getZOrderWithSub();
    void            setZOrderWithSub(double z_order, Z_Insert insert, int position = 1);

    // Thing Components
    void    addComponent3D();
    void    addComponentAppearance(bool bitrate_and_pixel_only = false, bool show_component = true);
    void    addComponentLayering(double z, double opacity = 100.0, bool show_component = true);
    void    addComponentLighting();
    void    addComponentMovement();
    void    addComponentSpawn();
    void    addComponentSpecialEffects();
    void    addComponentTransform(double width, double height, double x, double y, DrThingType type);

    // Thing Specific Components
    void    addComponentSettingsCamera(std::string new_name);
    void    addComponentSettingsCharacter(std::string new_name);
    void    addComponentSettingsObject(std::string new_name, bool should_collide = true);
    void    addComponentSettingsText(std::string new_name);
    void    addComponentSettingsTile(std::string new_name);

    // Effect Specific Components
    void    addComponentSettingsFire();
    void    addComponentSettingsFisheye();
    void    addComponentSettingsLight(DrColor color);
    void    addComponentSettingsMirror();
    void    addComponentSettingsSwirl();
    void    addComponentSettingsWater();

};

#endif // DRTHING_H





