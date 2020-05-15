//
//      Created by Stephens Nunnally on 5/15/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "project/settings/settings_component.h"
#include "project/settings/settings_component_signal.h"


//####################################################################################
//##    Constructor
//####################################################################################
DrSignal::DrSignal(DrComponent *parent_component, long signal_key, std::string name, bool is_deletable) {
    m_parent_component = parent_component;
    m_parent_settings =  parent_component->getParentSettings();

    m_signal_key =      signal_key;
    m_signal_name =     name;
    m_is_deletable =    is_deletable;
}















