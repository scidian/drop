//
//      Created by Stephens Nunnally on 2/3/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef CONSTANTS_SIGNALS_H
#define CONSTANTS_SIGNALS_H

#include <string>


//####################################################################################
//##    DrEngineSignal Signals      !!!!! #NOTE: All uncommented Signals should be implemented. When adding uncommented
//##                                             Signals, make sure to implement them to some degree.
//############################
namespace Signals {

    // ############################ THING EVENTS
    const std::string ThingCollide              = "Thing Collide";              // struct:      Collision_Info
    const std::string ThingSeperate             = "Thing Seperate";             // struct:      Collision_Info
    const std::string ThingDamaged              = "Thing Damaged";              // double:      amount +/- damaged
    const std::string ThingDied                 = "Thing Died";                 // double:      current health


    // ############################ THING COMMANDS
    const std::string ThingCancelCollision      = "Cancel Collision";           // struct:      Collision_Info


    // ############################ INPUT
    // Mouse
    const std::string MouseLeftDown             = "Mouse Left Down";            // DrPointF:    screen position
    const std::string MouseLeftUp               = "Mouse Left Up";              // DrPointF:    screen position
    const std::string MouseMiddleDown           = "Mouse Middle Down";          // DrPointF:    screen position
    const std::string MouseMiddleUp             = "Mouse Middle Up";            // DrPointF:    screen position
    const std::string MouseRightDown            = "Mouse Right Down";           // DrPointF:    screen position
    const std::string MouseRightUp              = "Mouse Right Up";             // DrPointF:    screen position

    const std::string MouseScroll               = "Mouse Scroll";               // int:         amount scrolled
    const std::string MouseMove                 = "Mouse Move";                 // DrPointF:    screen position
    ///const std::string MouseClick                = "Mouse Click";                // bool:    true
    ///const std::string MouseDoubleClick          = "Mouse Double Click";         // bool:    true

    // Touch
    ///const std::string TouchBegin                = "Touch Begin";                // int:     touch number
    ///const std::string TouchEnd                  = "Touch End";                  // int:     touch number
    ///const std::string TouchMove                 = "Touch Move";                 // int:     touch number


}   // End namespace Signals


//####################################################################################
//##    DrNode Slots
//##
//############################
namespace Input_Slots {
    const std::string Start             = "_Start";
    const std::string Load              = "_Load";
}

namespace Output_Slots {
    const std::string UI                = "_UI";
    const std::string Game_Over         = "_GameOver";
}



#endif // CONSTANTS_SIGNALS_H



















