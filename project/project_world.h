//
//      Created by Stephens Nunnally on 12/7/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrWorld - Class to hold all stages, variables and things for one world
//
//

#ifndef DRWORLD_H
#define DRWORLD_H

#include "settings/settings.h"

// Forward declarations
class DrProject;
class DrStage;

// Type definitions
typedef std::map<long, DrStage*> StageMap;


//####################################################################################
//##    DrWorld
//##        Class to hold all stages, variables and things for one world
//############################
class DrWorld : public DrSettings
{
private:
    // Local variables
    StageMap    m_stages;                               // Map of pointers to DrStage classes       (holds the stages for current world)

    long        m_start_stage_key;                      // Stores key of StartStage for this World
    long        m_last_stage_shown_in_editor;           // Last Stage shown in editor when this World was selected, set to StartStage on creation


public:
    // Constructor & Destructor
    DrWorld(DrProject *parent_project, long key, QString new_world_name, bool add_start_stage = true);
    virtual ~DrWorld() override;

    // Getters and Setters
    virtual DrType  getType() override                  { return DrType::World; }
    StageMap&       getStageMap()                       { return m_stages; }

    long            getStartStageKey()                  { return m_start_stage_key; }
    void            setStartStageKey(long stage)        { m_start_stage_key = stage; }

    long            getLastStageShownKey()              { return m_last_stage_shown_in_editor; }
    void            setLastStageShownKey(long stage)    { m_last_stage_shown_in_editor = stage; }


    // Function Calls
    long        addStage(QString new_stage_name = "");
    void        addStage(long stage_key, bool is_start_stage, QPointF center_point);
    void        deleteStage(DrStage *stage);
    void        initializeWorldSettings(QString new_name);

    long        getFirstStageKey();
    DrStage*    getStageFromKey(long from_stage_key);
    DrStage*    getStageWithName(QString stage_name);


};


#endif // DRWORLD_H














