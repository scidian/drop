//
//      Created by Stephens Nunnally on 12/13/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrAsset - Class to hold an asset for use in a project
//
//

#ifndef DRASSET_H
#define DRASSET_H

#include "settings.h"

class DrProject;

//####################################################################################
//##    DrAsset
//##        Class to hold an asset for use in a project
//############################
class DrAsset : public DrSettings
{
private:
    DrProject       *m_parent_project;                  // holds reference to parent Project class that handles key generation for project

    DrAsset_Types    m_asset_type;                      // holds asset type (object, character, etc)

public:
    DrAsset();

    // Constructor & destructor
    DrAsset(DrProject *parent_project, long new_asset_key, QString new_asset_name, DrAsset_Types new_asset_type);
    virtual ~DrAsset() override;


    // Getters and setters
    virtual DrTypes getType() override  { return DrTypes::Asset; }

    DrAsset_Types   getAssetType()      { return m_asset_type; }
    DrProject*      getParentProject()  { return m_parent_project; }


    // External calls
    void        initializeAssetSettings(QString new_name);

};





#endif // DRASSET_H
