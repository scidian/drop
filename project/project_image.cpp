//
//      Created by Stephens Nunnally on 2/11/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QFileInfo>

#include "project_image.h"

//####################################################################################
//##    Constructors
//####################################################################################
DrImage::DrImage(DrProject *parent_project, long key, QString image_path) : DrSettings(parent_project) {
    this->setKey(key);

    QFileInfo file_info(image_path);

    m_full_path =   file_info.path();
    m_filename =    file_info.fileName();
    m_simple_name = file_info.baseName();

    m_simple_name.replace("_", " ");

    m_image = QImage(image_path).convertToFormat(QImage::Format::Format_ARGB32);

}

// File Loading Constructor
DrImage::DrImage(DrProject *parent_project, long key, QString full_path, QString filename, QString simple_name, QImage &image) : DrSettings(parent_project) {
    this->setKey(key);
    m_full_path = full_path;
    m_filename = filename;
    m_simple_name = simple_name;
    m_image = image;
}
