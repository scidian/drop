//
//      Created by Stephens Nunnally on 12/7/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrStage - Class to hold one stage within a world
//
//
#ifndef DRSTAGE_H
#define DRSTAGE_H

#include "core/types/dr_pointf.h"
#include "project/settings/settings.h"

// Forward declarations
class DrProject;
class DrAsset;
class DrWorld;
class DrThing;

// Type definitions
typedef std::map<long, DrThing*> ThingMap;


//####################################################################################
//##    DrStage
//##        Class to hold one Stage within a World
//############################
class DrStage : public DrSettings
{
private:
    // External Borrowed Pointers
    DrWorld        *m_parent_world;                             // Holds reference to parent World class

    // Local variables
    ThingMap        m_things;                                   // Map of pointers to DrThing classes (holds the things for current stage)

    bool            m_is_start_stage;                           // True if this is the first stage in a world

    bool            m_expanded = true;                          // Wether or not this is expanded in the Project Tree
    DrPointF        m_center_view_point;                        // Holds center point the View was on last time this Stage was shown
    double          m_zoom_level = 0.5;                         // Holds zoom level   the View was on last time this Stage was shown


public:
    // Constructor / Destructor
    DrStage(DrProject *parent_project, DrWorld *parent_world, long new_stage_key, std::string new_stage_name, bool is_start_stage = false);
    virtual ~DrStage() override;

    // DrSettings Overrides
    virtual DrType  getType() override                      { return DrType::Stage; }

    // Getters / Setters
    DrWorld*        getParentWorld()                        { return m_parent_world; }
    ThingMap&       getThingMap()                           { return m_things; }
    DrThing*        getThing(long key)                      { return m_things[key]; }

    bool            isStartStage()                          { return m_is_start_stage; }
    void            setIsStartStage(bool is_start_stage)    { m_is_start_stage = is_start_stage; }

    bool            getExpanded()                           { return m_expanded; }
    DrPointF        getViewCenterPoint()                    { return m_center_view_point; }
    double          getViewZoomLevel()                      { return m_zoom_level; }
    void            setExpanded(bool expanded)              { m_expanded = expanded; }
    void            setViewCenterPoint(DrPointF new_point)  { m_center_view_point = new_point; }
    void            setViewZoomLevel(double zoom)           { m_zoom_level = zoom; }


    // Misc Stage Functions
    DrThing*        addThing(DrThingType new_type, long from_asset_key, double x, double y, double z, bool should_collide = true, long key = c_no_key);
    void            deleteThing(DrThing *&thing) ;

    std::vector<long>       thingKeysSortedByZOrder(Sort_Order sort_order);
    std::vector<DrThing*>   thingsSortedByZOrder(Sort_Order sort_order, bool all_things = true, std::list<DrThing*> just_these_things = { });
    void                    initializeStageSettings(std::string new_name);
    void                    removeGraphicsItemReferences();

};





#endif // DRSTAGE_H














