//
//      Created by Stephens Nunnally on 12/10/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef DR_STRING_H
#define DR_STRING_H

#include <algorithm>
#include <sstream>
#include <string>


//####################################################################################
//##    String Functions
//############################
namespace Dr {


    /// @brief: Returns (length) number of characters from the left side of a string
    std::string Left(const std::string &source, const size_t length);

    /// @brief: Returns (length) number of characters from the right side of a string
    std::string Right(const std::string &source, const size_t length);

    /// @brief: Returns true if string is a positive integer, otherwise false
    bool        IsInteger(const std::string &source);

    /// @brief: Returns number as string with decimal_places precision
    std::string RoundToDecimalPlace(const double number, const int decimal_places);

    /// @brief: Returns interger as hex string
    std::string ToHex(const int integer);

}   // End namespace Dr

#endif // DR_STRING_H







