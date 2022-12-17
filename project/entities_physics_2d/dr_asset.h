//
//      Created by Stephens Nunnally on 12/13/18, (c) 2019 Scidian Studios, All Rights Reserved
//
//  File:
//      DrAsset - Class to hold an asset for use in a project
//
//
#ifndef DRASSET_H
#define DRASSET_H

#include "project/settings/settings.h"

// Forward Declarations
class DrImage;
class DrProject;
class DrPropertyCollision;


//####################################################################################
//##    DrAsset
//##        Class to hold an Asset for use in a Project
//############################
class DrAsset : public DrSettings
{
private:
    // Local Variables
    DrAssetType     m_asset_type;                               // Holds asset type (object, character, etc)

    long            m_base_key          { c_no_key };           // Holds key to source (asset pulls from an DrAnimation, DrImage)
                                                                // For more info, look at DrAsset Constructor
    long            m_width             { 0 };
    long            m_height            { 0 };


public:
    // Constructor / Destructor
    explicit DrAsset(DrProject *parent_project, long key, DrAssetType new_asset_type, long base_key);
    virtual ~DrAsset() override;

    // DrSettings Overrides
    virtual DrType  getType() override      { return DrType::Asset; }

    // Getters / Setters
    long            getWidth()              { return m_width; }
    long            getHeight()             { return m_height; }
    void            setWidth(long width)    { m_width = width; }
    void            setHeight(long height)  { m_height = height; }

    DrAssetType     getAssetType()          { return m_asset_type; }
    void            setAssetType(DrAssetType type)  { m_asset_type = type; }

    long            getBaseKey()            { return m_base_key; }
    void            setBaseKey(long key)    { m_base_key = key; }

    // Function Calls
    long                        getIdleAnimationKey();
    long                        getIdleAnimationFirstFrameImageKey();

    // Shape Functions
    static DrPropertyCollision  autoCollisionShape(const DrImage *image);

    // Initialize Calls
    void        initializeAssetSettingsCharacter(std::string new_name);
    void        initializeAssetSettingsObject(std::string new_name);

    void        initializeAssetSettingsAnimation(DrAssetType asset_type, long idle_animation_key);
    void        initializeAssetSettingsCollision(DrAssetType asset_type, DrPropertyCollision &shape);
    void        initializeAssetSettingsControls(DrAssetType asset_type);
    void        initializeAssetSettingsHealth(DrAssetType asset_type, int hit_points = 1);
    void        initializeAssetSettingsPhysics(DrAssetType asset_type);
};


#endif // DRASSET_H









