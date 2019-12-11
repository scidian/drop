//
//      Created by Stephens Nunnally on 2/11/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      A container class for one image to be loaded and used in project
//
//
#ifndef DRIMAGE_H
#define DRIMAGE_H

#include <QImage>
#include <QPixmap>

#include "model/settings/settings.h"

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
    std::string         m_full_path;                               // Full path,  i.e.  ":/images/more/pretty_tree_1.png"
    std::string         m_filename;                                // Image name, i.e.  "pretty_tree_1.png"
    std::string         m_simple_name;                             // Simple name, i.e. "pretty tree 1

    QImage              m_image;                                   // Stored image

    Asset_Category      m_category = Asset_Category::Image;        // Category for Image in Asset Tree

public:
    // Constructors
    DrImage(DrProject *parent_project, long key, std::string image_path,
            Asset_Category category = Asset_Category::Image);
    DrImage(DrProject *parent_project, long key, std::string full_path, std::string filename, std::string simple_name, QImage &image,
            Asset_Category category = Asset_Category::Image);

    // DrSettings Overrides
    virtual DrType      getType() override  { return DrType::Image; }
    virtual std::string getName() override  { return m_simple_name; }

    // Getters / Setters
    Asset_Category  getAssetCategory()      { return m_category; }
    std::string     getFullPath()           { return m_full_path; }
    std::string     getFilename()           { return m_filename; }
    std::string     getSimplifiedName()     { return m_simple_name; }

    const QImage&   getImage()              { return m_image; }
    QPixmap         getPixmapFromImage()    { return QPixmap::fromImage(m_image); }


};

#endif // DRIMAGE_H











