//
//      Created by Stephens Nunnally on 12/6/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "core/dr_debug.h"

namespace Dr {


//####################################################################################
//##
//##    Debug Functionality
//##
//####################################################################################
void PrintDebug(const std::string message) {
    std::string msg = message + std::string("\n");
    char *cstr = new char [msg.length() + 1];
    std::strcpy(cstr, msg.c_str());
    fprintf(stderr, "%s", cstr);                        // stderr is the "Error File Stream"
    delete [] cstr;
}


} // end namespace Dr















