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

namespace Signals {



    //############################ THINGS
    ///const std::string ThingCollide              = "Thing Collide";              //
    const std::string ThingDamaged              = "Thing Damaged";              // double:  amount +/- damaged
    const std::string ThingDied                 = "Thing Died";                 // double:  current health


    //############################ INPUT
    // Mouse
    const std::string MouseLeftDown             = "Mouse Left Down";            // bool:    true
    const std::string MouseLeftUp               = "Mouse Left Up";              // bool:    true
    const std::string MouseMiddleDown           = "Mouse Middle Down";          // bool:    true
    const std::string MouseMiddleUp             = "Mouse Middle Up";            // bool:    true
    const std::string MouseRightDown            = "Mouse Right Down";           // bool:    true
    const std::string MouseRightUp              = "Mouse Right Up";             // bool:    true
    const std::string MouseScroll               = "Mouse Scroll";               // int:     amount scrolled
    ///const std::string MouseMove                 = "Mouse Move";                 // bool:    true
    ///const std::string MouseClick                = "Mouse Click";                // bool:    true
    ///const std::string MouseDoubleClick          = "Mouse Double Click";         // bool:    true

    // Touch
    ///const std::string TouchBegin                = "Touch Begin";                // int:     touch number
    ///const std::string TouchEnd                  = "Touch End";                  // int:     touch number
    ///const std::string TouchMove                 = "Touch Move";                 // int:     touch number




}   // End namespace Signals




#endif // CONSTANTS_SIGNALS_H
