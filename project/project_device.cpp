//
//      Created by Stephens Nunnally on 11/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "imaging/imaging.h"
#include "project/project.h"
#include "project/project_device.h"


//####################################################################################
//##    Constructor, Destructor
//####################################################################################
DrDevice::DrDevice(DrProject *parent_project, long key, DrDeviceType device_type) : DrSettings(parent_project) {
    this->setKey(key);

    m_device_type = device_type;
}


//####################################################################################
//##    Creates a Pixmap for this Effect
//####################################################################################
QPixmap DrDevice::getPixmap() {
    switch (m_device_type) {
        case DrDeviceType::Camera:      return QPixmap(":/assets/asset_types/camera.png");
    }
}


