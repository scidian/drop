//
//      Created by Stephens Nunnally on 2/3/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "engine_signal.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
DrEngineSignal::DrEngineSignal(std::string name, DrVariant value, std::string thing_name, long thing_asset_key, long thing_engine_key) {
    m_name =    name;
    m_value =   value;
    m_life =    Signal_Life::Born;

    m_thing_name =          thing_name;
    m_thing_asset_key =     thing_asset_key;
    m_thing_engine_key =    thing_engine_key;
}

DrEngineSignal::~DrEngineSignal() {

}
