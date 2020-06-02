//
//      Created by Stephens Nunnally on 12/7/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrWorld - Class to hold all stages, variables and things for one world
//
//
#ifndef DRWORLD_H
#define DRWORLD_H

#include "project/settings/settings.h"

// Forward Declarations
class DrProject;
class DrStage;

// Type Definitions
typedef std::map<long, DrStage*> StageMap;


//####################################################################################
//##    DrWorld
//##        Class to hold DrStages, for one DrWorld
//############################
class DrWorld : public DrSettings
{
private:
    // Local variables
    DrWorldType     m_world_type = DrWorldType::Physics_2D;         // Type of World (Physics, UI, Puzzle, Tower Defense, etc)
    StageMap        m_stages;                                       // Map of pointers to DrStage classes (holds the stages for current world)

    long            m_start_stage_key;                              // Stores key of StartStage for this World
    long            m_last_stage_shown_in_editor;                   // Last Stage shown in editor when this World was selected, set to StartStage on creation
    bool            m_expanded = true;                              // Wether or not this is expanded in the Project Tree

public:
    // Constructor / Destructor
    DrWorld(DrProject *parent_project, long key, DrWorldType world_type, std::string new_world_name, bool add_start_stage);
    virtual ~DrWorld() override;

    // DrSettings Overrides
    virtual DrType  getType() override                  { return DrType::World; }

    // Getters / Setters
    DrWorldType     getWorldType()                      { return m_world_type; }
    StageMap&       getStageMap()                       { return m_stages; }

    long            getStartStageKey()                  { return m_start_stage_key; }
    void            setStartStageKey(long stage)        { m_start_stage_key = stage; }

    long            getLastStageShownKey()              { return m_last_stage_shown_in_editor; }
    void            setLastStageShownKey(long stage)    { m_last_stage_shown_in_editor = stage; }

    bool            getExpanded()                       { return m_expanded; }
    void            setExpanded(bool expanded)          { m_expanded = expanded; }

    // World Construction
    DrStage*    addStage(std::string new_stage_name = std::string(""));
    DrStage*    addStage(long stage_key, bool is_start_stage, DrPointF center_point, double zoom_scale);
    DrStage*    addStageCopyFromStage(DrStage *from_stage, std::string new_name, bool copy_into_start_stage = false);
    void        deleteStage(DrStage *stage);

    void        initializeSettings(DrWorldType world_type, std::string new_name);
    void        initializeWorldPhysics2D(std::string new_name);
    void        initializeWorldUI(std::string new_name);

    void        setNodePositionFromOtherWorld(DrWorld *from_world, Direction direction);

    // World Info
    long        getFirstStageKey();
    DrStage*    getStageFromKey(long from_stage_key);
    DrStage*    getStageWithName(std::string stage_name);


};


#endif // DRWORLD_H














