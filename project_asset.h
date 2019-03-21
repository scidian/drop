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

    DrAssetType      m_asset_type;                      // holds asset type (object, character, etc)
    long             m_source_key;                      // holds key to source image / font

    long             m_width;
    long             m_height;

    long             m_list_order;                      // keeps track of what order to be in on the asset list tree
    long             m_group_number;                    // keeps track of what group to be in on the asset list tree

public:
    // Constructor & destructor
    explicit DrAsset(DrProject *parent_project, long new_asset_key, DrAssetType new_asset_type, long source_key);
    virtual ~DrAsset() override;


    // Getters and setters
    virtual DrType  getType() override  { return DrType::Asset; }
    DrAssetType     getAssetType()      { return m_asset_type; }

    DrProject*      getParentProject()  { return m_parent_project; }
    long            getWidth()          { return m_width; }
    long            getHeight()         { return m_height; }
    long            getSourceKey()      { return m_source_key; }

    // External calls
    void        initializeAssetSettingsObject(QString new_name, QPixmap pixmap);
    void        initializeAssetSettingsFont(QString new_name);

};





#endif // DRASSET_H
