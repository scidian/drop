//
//      Created by Stephens Nunnally on 1/21/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      An interface class, used to transfer messages, etc around program between widgets / classes
//
//

#include "interface_relay.h"

//####################################################################################
//##        Interface Destructors - Must include definition of a virtual destructor
//####################################################################################
InterfaceRelay::~InterfaceRelay() { }





//####################################################################################
//##        These functions can be used globally without passing around an interface pointer
//##
//##        - Just need to #include "interface_relay.h"
//##        - When program starts, FormMain is set as active 'InterfaceRelay' class
//##        - Make calls like: "Dr::SetLabelText"
//##
//####################################################################################

namespace Dr {

    // Local variable used to store an active InterfaceRelay class instance
    static InterfaceRelay *active_window;


    void SetActiveRelay(InterfaceRelay *new_relay)      { active_window = new_relay; }
    void SetLabelText(Label_Names label, QString text)  { active_window->setLabelText(label, text); }

}
