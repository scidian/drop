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
#include <QString>

#include "settings/settings.h"

// Forward declarations
class DrProject;

//####################################################################################
//##    DrImage
//##        Class to hold an immage for use in a project, can be pulled for asset or things
//############################
class DrImage : public DrSettings
{
private:
    // External Borrowed Pointers
    DrProject       *m_parent_project;                  // Holds reference to parent Project class that handles key generation for project

    // Local Variables
    QString          m_full_path;                       // Full path,  i.e.  ":/images/more/pretty_tree_1.png"
    QString          m_filename;                        // Image name, i.e.  "pretty_tree_1.png"
    QString          m_simple_name;                     // Simple name, i.e. "pretty tree 1

    QImage           m_image;                           // Stored image

public:
    DrImage(DrProject *parent_project, long key, QString image_path);
    DrImage(DrProject *parent_project, long key, QString full_path, QString filename, QString simple_name, QImage &image);

    // Getters and setters
    virtual DrType  getType() override      { return DrType::Image; }

    DrProject*      getParentProject()      { return m_parent_project; }

    QString         getFullPath()           { return m_full_path; }
    QString         getFilename()           { return m_filename; }
    QString         getSimplifiedName()     { return m_simple_name; }

    const QImage&   getImage()              { return m_image; }
    QPixmap         getPixmapFromImage()    { return QPixmap::fromImage(m_image); }


};

#endif // DRIMAGE_H
