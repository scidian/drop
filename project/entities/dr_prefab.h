//
//      Created by Stephens Nunnally on 3/4/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef DR_PREFAB_H
#define DR_PREFAB_H

#include "project/settings/settings.h"

// Forward Declarations
class DrProject;


//####################################################################################
//##    DrPrefab
//##        Class to hold a Prefab for use in a Project
//############################
class DrPrefab : public DrSettings
{
private:
    // Local Variables
    DrPrefabType        m_prefab_type;                  // Prefab Type as DrPrefabType

public:
    // Constructor
    DrPrefab(DrProject *parent_project, long key, DrPrefabType prefab_type);

    // DrSettings Overrides
    virtual DrType      getType() override      { return DrType::Prefab; }

    // Getters / Setters
    DrPrefabType        getPrefabType()         { return m_prefab_type; }

};

#endif // DR_PREFAB_H
