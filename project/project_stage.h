//
//      Created by Stephens Nunnally on 12/7/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrStage - Class to hold one stage within a world
//
//
#ifndef DRSTAGE_H
#define DRSTAGE_H

#include "settings/settings.h"

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
    DrWorld    *m_parent_world;                             // Holds reference to parent World class

    // Local variables
    ThingMap    m_things;                                   // Map of pointers to DrThing classes (holds the things for current stage)

    bool        m_is_start_stage;                           // True if this is the first stage in a world

    bool        m_expanded = true;                          // Wether or not this is expanded in the Project Tree
    QPointF     m_center_view_point {0, 0};                 // Holds center point the View was on last time this Stage was shown
    double      m_zoom_level = 0.5;                         // Holds zoom level   the View was on last time this Stage was shown


public:
    // Constructor / Destructor
    DrStage(DrProject *parent_project, DrWorld *parent_world, long new_stage_key, QString new_stage_name, bool is_start_stage = false);
    virtual ~DrStage() override;


    // Getters / Setters
    virtual DrType  getType() override      { return DrType::Stage; }
    DrWorld*        getParentWorld()        { return m_parent_world; }
    ThingMap&       getThingMap()           { return m_things; }
    DrThing*        getThing(long key)      { return m_things[key]; }

    bool            isStartStage()          { return m_is_start_stage; }
    void            setIsStartStage(bool is_start_stage)    { m_is_start_stage = is_start_stage; }

    bool            getExpanded()           { return m_expanded; }
    QPointF         getViewCenterPoint()    { return m_center_view_point; }
    double          getViewZoomLevel()      { return m_zoom_level; }
    void            setExpanded(bool expanded)              { m_expanded = expanded; }
    void            setViewCenterPoint(QPointF new_point)   { m_center_view_point = new_point; }
    void            setViewZoomLevel(double zoom)           { m_zoom_level = zoom; }


    // Misc Stage Functions
    DrThing*        addThing(DrThingType new_type, long from_asset_key, double x, double y, double z, bool should_collide = true, long key = c_no_key);
    void            deleteThing(DrThing *&thing) ;

    QList<long>     thingKeysSortedByZOrder(Qt::SortOrder sort_order);
    QList<DrThing*> thingsSortedByZOrder(Qt::SortOrder sort_order, bool all_things = true, QList<DrThing*> just_these_things = { });
    void            initializeStageSettings(QString new_name);
    void            removeGraphicsItemReferences();

};





#endif // DRSTAGE_H














