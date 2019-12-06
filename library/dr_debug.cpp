//
//      Created by Stephens Nunnally on 12/6/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "dr_debug.h"

namespace Dr {


//####################################################################################
//##
//##    Debug Functionality
//##
//####################################################################################
void PrintDebug(std::string message) {
    message += "\n";
    char *cstr = new char [message.length() + 1];
    std::strcpy(cstr, message.c_str());
    fprintf(stderr, "%s", cstr);                        // stderr is the "Error File Stream"
    delete [] cstr;
}


} // end namespace Dr















