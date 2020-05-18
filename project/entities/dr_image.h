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

// Forward Declarations
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
    std::string                 m_simple_name;                                              // Simple name, i.e. "pretty tree 1"
    DrBitmap                    m_bitmap;                                                   // Stored image as DrBitmap

public:
    vec<vec<DrPointF>>          m_poly_list;                                                // Stores list of image outline points
    vec<vec<vec<DrPointF>>>     m_hole_list;                                                // Stores list of hole  outline points
    bool                        m_use_simple_square     { false };                          // True when Image Outline has been canceled, when true extrudes in 3D as simple square
    bool                        m_outline_processed     { false };                          // Turns true when autoOutlinePoints() has completed successfully

private:
    // Internal Variables
    std::string                 m_folder_name           { "" };                             // Used for External Images to belong to a category


public:
    // Constructors
    DrImage(DrProject *parent_project, long key, std::string image_name, DrBitmap &bitmap, bool outline, IProgressBar *progress);

    // DrSettings Overrides
    virtual DrType          getType() override  { return DrType::Image; }
    virtual std::string     getName() override  { return m_simple_name; }

    // Image Helper Functions
    void                autoOutlinePoints(IProgressBar *progress = nullptr);
    bool                outlineProcessed()                  { return m_outline_processed; }
    bool                useSimpleSquare()                   { return m_use_simple_square; }
    void                setSimpleBox();

    // Getters / Setters
    std::string         getSimplifiedName()                 { return m_simple_name; }
    const DrBitmap&     getBitmap() const                   { return m_bitmap; }


    // Internal Variable Functions
    std::string         getFolderName()                     { return m_folder_name; }
    void                setFolderName(std::string folder)   { m_folder_name = folder; }

};

#endif // DRIMAGE_H











