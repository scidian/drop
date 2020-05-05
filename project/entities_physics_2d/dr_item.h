//
//      Created by Stephens Nunnally on 3/3/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef DR_ITEM_H
#define DR_ITEM_H

#include "project/settings/settings.h"

// Forward Declarations
class DrProject;


//####################################################################################
//##    DrItem
//##        Class to hold a Specilzed Item for use in a Project
//############################
class DrItem : public DrSettings
{
private:
    // Local Variables
    DrItemType          m_item_type;                        // Item Type as DrItemType

public:
    // Constructor
    DrItem(DrProject *parent_project, long key, DrItemType item_type);

    // DrSettings Overrides
    virtual DrType      getType() override      { return DrType::Item; }

    // Getters / Setters
    DrItemType          getItemType()           { return m_item_type; }

};

#endif // DR_ITEM_H


















