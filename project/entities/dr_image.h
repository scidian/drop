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
class IProgressBar;

// Local Constants
#define         vec                     std::vector
const double    c_alpha_tolerance =     0.875;


//####################################################################################
//##    DrImage
//##        Class to hold an Image for use in a Project, can be pulled for asset or things
//############################
class DrImage : public DrSettings
{
private:
    // Local Variables
    Asset_Category              m_category              { Asset_Category::Image };          // Category for Image in Asset Tree
    std::string                 m_simple_name;                                              // Simple name, i.e. "pretty tree 1"
    DrBitmap                    m_bitmap;                                                   // Stored image as DrBitmap

public:
    vec<vec<DrPointF>>          m_poly_list;                                                // Stores list of image outline points
    vec<vec<vec<DrPointF>>>     m_hole_list;                                                // Stores list of hole  outline points
    int                         m_number_of_objects     { 0 };                              // Stores number of objects found in image
    bool                        m_use_simple_square     { false };                          // When true extrudes in 3D as simple square


public:
    // Constructors
    DrImage(DrProject *parent_project, long key, std::string image_name, DrBitmap &bitmap,
            Asset_Category category = Asset_Category::Image, bool force_outline = false, IProgressBar *progress = nullptr);

    // DrSettings Overrides
    virtual DrType      getType() override  { return DrType::Image; }
    virtual std::string getName() override  { return m_simple_name; }

    // Image Helper Functions
    void            autoOutlinePoints(IProgressBar *progress = nullptr);
    void            setSimpleBox();

    // Getters / Setters
    Asset_Category  getAssetCategory()      { return m_category; }
    std::string     getSimplifiedName()     { return m_simple_name; }

    const DrBitmap& getBitmap() const       { return m_bitmap; }

};

#endif // DRIMAGE_H











