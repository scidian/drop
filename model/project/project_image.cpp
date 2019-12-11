//
//      Created by Stephens Nunnally on 2/11/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QFileInfo>

#include "model/project/project_image.h"
#include "model/settings/settings_component.h"
#include "model/settings/settings_component_property.h"


//####################################################################################
//##    Constructors
//####################################################################################
DrImage::DrImage(DrProject *parent_project, long key, std::string image_path, Asset_Category category)
    : DrSettings(parent_project)
{
    this->setKey(key);
    m_category = category;

    QFileInfo file_info(QString::fromStdString(image_path));

    m_full_path =   file_info.path().toStdString();
    m_filename =    file_info.fileName().toStdString();
    m_simple_name = file_info.baseName().toStdString();

    m_simple_name = QString::fromStdString(m_simple_name).replace("_", " ").toStdString();

    m_image = QImage(QString::fromStdString(image_path)).convertToFormat(QImage::Format::Format_ARGB32);
    if (m_image.isNull()) {
        m_image = QImage(1, 1, QImage::Format::Format_ARGB32);
    }

    // Set name in DrSettings Entity properties
    getComponentProperty(Components::Entity_Settings, Properties::Entity_Name)->setValue(m_simple_name);
    getComponentProperty(Components::Entity_Settings, Properties::Entity_Name)->setEditable(false);
}

// File Loading Constructor
DrImage::DrImage(DrProject *parent_project, long key, std::string full_path, std::string filename, std::string simple_name,
                 QImage &image, Asset_Category category)
    : DrSettings(parent_project)
{
    this->setKey(key);
    m_category = category;

    m_full_path = full_path;
    m_filename = filename;
    m_simple_name = simple_name;
    m_image = image;

    // Set name in DrSettings Entity properties
    getComponentProperty(Components::Entity_Settings, Properties::Entity_Name)->setValue(m_simple_name);
    getComponentProperty(Components::Entity_Settings, Properties::Entity_Name)->setEditable(false);
}










