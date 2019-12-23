//
//      Created by Stephens Nunnally on 12/10/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "core/dr_string.h"

namespace Dr {


//####################################################################################
//##
//##    String Functions
//##
//####################################################################################
// Returns (length) number of characters from the left side of a string
std::string Left(const std::string &source, const size_t length) {
    if (length >= source.size()) { return source; }
    return source.substr(0, length);
}

// Returns (length) number of characters from the right side of a string
std::string Right(const std::string &source, const size_t length) {
    if (length >= source.size()) { return source; }
    return source.substr(source.size() - length);
}

// Returns true if string is a positive integer, otherwise false
bool IsInteger(const std::string &source) {
    return !source.empty() && std::find_if(source.begin(),
        source.end(), [](char c) { return !std::isdigit(c); }) == source.end();
}


}   // end namespace Dr








