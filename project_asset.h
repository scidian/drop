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

// Forward Declarations
class DrFont;
class DrProject;

//####################################################################################
//##    DrAsset
//##        Class to hold an asset for use in a project
//############################
class DrAsset : public DrSettings
{
private:
    DrProject       *m_parent_project;                  // Holds reference to parent Project class that handles key generation for project

    DrAssetType      m_asset_type;                      // Holds asset type (object, character, etc)
    long             m_source_key;                      // Holds key to source image / font

    long             m_width;
    long             m_height;

    long             m_list_order;                      // Keeps track of what order to be in on the asset list tree
    long             m_group_number;                    // Keeps track of what group to be in on the asset list tree

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
    void        initializeAssetSettingsFont(DrFont *font);

};





#endif // DRASSET_H
