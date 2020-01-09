//
//      Created by Stephens Nunnally on 1/8/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "editor/widgets/widgets.h"


//####################################################################################
//##    Non signal activating setValue
//####################################################################################
void DrQSpinSlot::updateValue(int value) {
    bool before_block = this->signalsBlocked();
    this->blockSignals(true);
    this->setValue( value );
    this->blockSignals(before_block);
}
