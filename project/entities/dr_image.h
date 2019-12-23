//
//      Created by Stephens Nunnally on 2/11/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      A container class for one image to be loaded and used in project
//
//
#ifndef DRIMAGE_H
#define DRIMAGE_H

#include "project/settings/settings.h"

// Forward declarations
class DrProject;


//####################################################################################
//##    DrImage
//##        Class to hold an Image for use in a Project, can be pulled for asset or things
//############################
class DrImage : public DrSettings
{
private:
    // Local Variables
    Asset_Category      m_category = Asset_Category::Image;        // Category for Image in Asset Tree
    std::string         m_simple_name;                             // Simple name, i.e. "pretty tree 1"

    DrBitmap            m_bitmap;                                  // Stored image as DrBitmap


public:
    // Constructors
    DrImage(DrProject *parent_project, long key, std::string image_name, DrBitmap &bitmap, Asset_Category category = Asset_Category::Image);

    // DrSettings Overrides
    virtual DrType      getType() override  { return DrType::Image; }
    virtual std::string getName() override  { return m_simple_name; }

    // Getters / Setters
    Asset_Category  getAssetCategory()      { return m_category; }
    std::string     getSimplifiedName()     { return m_simple_name; }

    const DrBitmap& getBitmap()             { return m_bitmap; }

};

#endif // DRIMAGE_H











