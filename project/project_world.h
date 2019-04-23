//
//      Created by Stephens Nunnally on 12/7/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrWorld - Class to hold all stages, variables and objects for one world
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
//##        Class to hold all stages, variables and objects for one world
//############################
class DrWorld : public DrSettings
{
private:
    // Local variables
    StageMap    m_stages;                                   // Map of pointers to DrStage classes       (holds the stages for current world)

    DrProject  *m_parent_project;                           // Holds reference to parent Project class that handles key generation for project

public:
    // Constructor & destructor
    DrWorld(DrProject *parent_project, long new_world_key, QString new_world_name);
    virtual ~DrWorld() override;


    // Getters and setters
    virtual DrType  getType() override  { return DrType::World; }
    DrProject*      getParentProject()  { return m_parent_project; }
    StageMap        getStageMap()       { return m_stages; }


    // External calls
    void        addStage(QString new_stage_name = "");
    void        initializeWorldSettings(QString new_name);

    long        getFirstStageKey();
    DrStage*    getStage(long from_stage_key);
    DrStage*    getStageWithName(QString stage_name);



};


#endif // DRWORLD_H














