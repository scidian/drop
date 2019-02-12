//
//      Created by Stephens Nunnally on 2/11/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QFileInfo>

#include "project_image.h"

// Constructor
DrImage::DrImage(DrProject *parent_project, long key, QString image_path) :
    m_parent_project(parent_project),
    m_key(key)
{

    QFileInfo file_info(image_path);

    m_full_path =   file_info.path();
    m_filename =    file_info.fileName();
    m_simple_name = file_info.baseName();

    m_simple_name.replace("_", " ");

    m_image = QImage(image_path).convertToFormat(QImage::Format::Format_ARGB32);

}
