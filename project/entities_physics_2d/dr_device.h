//
//      Created by Stephens Nunnally on 11/3/2019, (c) 2019 Scidian Studios, All Rights Reserved
//
//  File:
//
//
//
#ifndef DRDEVICE_H
#define DRDEVICE_H

#include "project/settings/settings.h"

// Forward Declarations
class DrProject;


//####################################################################################
//##    DrDevice
//##        Class to hold a Device for use in a Project
//############################
class DrDevice : public DrSettings
{
private:
    // Local Variables
    DrDeviceType        m_device_type;                      // Device Type as DrDeviceType

public:
    // Constructor
    DrDevice(DrProject *parent_project, long key, DrDeviceType device_type);

    // DrSettings Overrides
    virtual DrType      getType() override      { return DrType::Device; }

    // Getters / Setters
    DrDeviceType        getDeviceType()         { return m_device_type; }

};

#endif // DRDEVICE_H
