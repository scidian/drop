//
//      Created by Stephens Nunnally on 12/6/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//  Note:
//      To be Engine compatible, this file does NOT, and should NOT #include any Qt Library types
//
#ifndef DR_RANDOM_H
#define DR_RANDOM_H

#include <random>

namespace Dr {


    //####################################################################################
    //##    Random Functions
    //############################
    /// @brief: Returns a number between lower (inclusive) and upper (exclusive)
    int     RandomInt(int lower, int upper);
    /// @brief: Returns a number between lower (inclusive) and upper (inclusive)
    double  RandomDouble(double lower, double upper);
    /// @brief: Returns a random boolean
    bool    RandomBool();


}   // End namespace Dr

#endif // DR_RANDOM_H








