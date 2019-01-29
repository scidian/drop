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

    DrAsset_Type     m_asset_type;                      // holds asset type (object, character, etc)

    long             m_width;
    long             m_height;

    long             m_list_order;                      // keeps track of what order to be in on the asset list tree
    long             m_group_number;                    // keeps track of what group to be in on the asset list tree

public:
    DrAsset();

    // Constructor & destructor
    explicit DrAsset(DrProject *parent_project, long new_asset_key, QString new_asset_name,
                     DrAsset_Type new_asset_type, QPixmap pixmap);
    virtual ~DrAsset() override;


    // Getters and setters
    virtual DrType  getType() override  { return DrType::Asset; }

    DrAsset_Type    getAssetType()      { return m_asset_type; }
    DrProject*      getParentProject()  { return m_parent_project; }
    long            width()             { return m_width; }
    long            height()            { return m_height; }


    // External calls
    void        initializeAssetSettings(QString new_name, QPixmap pixmap);

};





#endif // DRASSET_H
