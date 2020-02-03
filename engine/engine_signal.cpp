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
DrEngineSignal::DrEngineSignal(std::string name, DrVariant value,
                               std::string thing_a_name, long thing_a_asset_key, long thing_a_engine_key,
                               std::string thing_b_name, long thing_b_asset_key, long thing_b_engine_key) {
    m_name =    name;
    m_value =   value;
    m_life =    Signal_Life::Born;

    m_thing_a_name =        thing_a_name;
    m_thing_a_asset_key =   thing_a_asset_key;
    m_thing_a_engine_key =  thing_a_engine_key;

    m_thing_b_name =        thing_b_name;
    m_thing_b_asset_key =   thing_b_asset_key;
    m_thing_b_engine_key =  thing_b_engine_key;
}

DrEngineSignal::~DrEngineSignal() {

}
