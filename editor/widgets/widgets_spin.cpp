//
//      Created by Stephens Nunnally on 1/8/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "core/dr_math.h"
#include "editor/widgets/widgets.h"


//####################################################################################
//##    Integer Spin Box, Non signal activating setValue
//####################################################################################
void DrQSpinSlot::updateValue(int value) {
    bool before_block = this->signalsBlocked();
    this->blockSignals(true);
    this->setValue( value );
    this->blockSignals(before_block);
}


//####################################################################################
//##    Floating Point Spin Box, Non signal activating setValue
//####################################################################################
void DrQDoubleSpinSlot::updateValue(double value) {
    bool before_block = this->signalsBlocked();
    this->blockSignals(true);
    if ( Dr::IsCloseTo(value, this->value(), m_update_tolerance) == false) {
        this->setValue( value );
    }
    this->blockSignals(before_block);
}
