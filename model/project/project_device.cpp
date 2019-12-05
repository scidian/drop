//
//      Created by Stephens Nunnally on 11/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QPixmap>

#include "model/project/project.h"
#include "model/project/project_device.h"
#include "settings/settings_component_property.h"


//####################################################################################
//##    Constructor, Destructor
//####################################################################################
DrDevice::DrDevice(DrProject *parent_project, long key, DrDeviceType device_type) : DrSettings(parent_project) {
    this->setKey(key);

    m_device_type = device_type;

    // ***** Initialize Device Settings
    DrProperty *property_name = getComponentProperty(Components::Entity_Settings, Properties::Entity_Name);
    property_name->setEditable(false);
    property_name->setDisplayName("Device Name");
    property_name->setDescription("Name of this Device Asset.");
    property_name->setValue(Dr::StringFromDeviceType(device_type));
}


//####################################################################################
//##    Creates a Pixmap for this Effect
//####################################################################################
QPixmap DrDevice::getPixmap() {
    switch (m_device_type) {
        case DrDeviceType::Camera:      return QPixmap(":/assets/asset_types/camera.png");
    }
}


