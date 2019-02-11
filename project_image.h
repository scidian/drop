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

class DrProject;

class DrImage
{
private:
    DrProject       *m_parent_project;                  // holds reference to parent Project class that handles key generation for project
    long             m_key;                             // holds unique key for each object within DrProject

    QString          m_full_path;                       // full path,  i.e.  ":/images/more/pretty_tree_1.png"
    QString          m_filename;                        // image name, i.e.  "pretty_tree_1.png"
    QString          m_simple_name;                     // simple name, i.e. "pretty tree 1

    QImage           m_image;                           // stored image

public:
    DrImage(DrProject *parent_project, long key, QString image_path);

    // Getters and setters
    DrProject*      getParentProject()  { return m_parent_project; }
    long            getKey()            { return m_key; }

    QString         getFullPath()       { return m_full_path; }
    QString         getFilename()       { return m_filename; }
    QString         getSimplifiedName() { return m_simple_name; }

    QImage          getImage()          { return m_image; }
    QPixmap         getPixmap()         { return QPixmap::fromImage(m_image); }


};

#endif // DRIMAGE_H
