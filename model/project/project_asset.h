//
//      Created by Stephens Nunnally on 12/13/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrAsset - Class to hold an asset for use in a project
//
//
#ifndef DRASSET_H
#define DRASSET_H

#include "model/settings/settings.h"

// Forward Declarations
class DrFont;
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
    DrAssetType     m_asset_type;                       // Holds asset type (object, character, etc)

    long            m_base_key = c_no_key;              // Holds key to source (asset pulls from an DrAnimation, DrImage)
                                                        // For more info, look at DrAsset Constructor

    long            m_width;
    long            m_height;


public:
    // Constructor / Destructor
    explicit DrAsset(DrProject *parent_project, long key, DrAssetType new_asset_type, long base_key);
    virtual ~DrAsset() override;

    // DrSettings Overrides
    virtual DrType  getType() override      { return DrType::Asset; }

    // Getters / Setters
    long            getWidth()              { return m_width; }
    long            getHeight()             { return m_height; }

    DrAssetType     getAssetType()          { return m_asset_type; }
    void            setAssetType(DrAssetType type)  { m_asset_type = type; }

    long            getBaseKey()            { return m_base_key; }
    void            setBaseKey(long key)            { m_base_key = key; }

    // Function Calls
    std::list<long> animationsUsedByAsset();
    void            deleteAnimations();
    long            getIdleAnimationKey();
    long            getIdleAnimationFirstFrameImageKey();
    void            updateAnimationProperty(std::list<long> image_keys, Properties animation_property);

    // Initialize Calls
    void        initializeAssetSettingsCharacter(QString new_name);
    void        initializeAssetSettingsObject(QString new_name);

    void        initializeAssetSettingsAnimation(DrAssetType asset_type, long idle_animation_key);
    void        initializeAssetSettingsCollision(DrAssetType asset_type, DrPropertyCollision &shape);
    void        initializeAssetSettingsControls(DrAssetType asset_type);
    void        initializeAssetSettingsHealth(DrAssetType asset_type, int hit_points = 1);
    void        initializeAssetSettingsPhysics(DrAssetType asset_type);
};


#endif // DRASSET_H









