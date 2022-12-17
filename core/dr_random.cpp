//
//      Created by Stephens Nunnally on 12/6/2019, (c) 2019 Scidian Studios, All Rights Reserved
//
//  File:
//
//
//
#include <chrono>

#include "core/dr_random.h"

namespace Dr {


//####################################################################################
//##
//##    Random Functions
//##
//####################################################################################
// Returns a number between lower (inclusive) and upper (exclusive)
int RandomInt(int lower, int upper) {
    static unsigned seed = static_cast<unsigned>(std::chrono::system_clock::now().time_since_epoch().count());
    static std::default_random_engine generator (seed);

    std::uniform_int_distribution<int> distribution(lower, upper - 1);
    return distribution(generator);
}

// Returns a number between lower (inclusive) and upper (inclusive)
double RandomDouble(double lower, double upper) {
    static unsigned seed = static_cast<unsigned>(std::chrono::system_clock::now().time_since_epoch().count());
    static std::default_random_engine generator (seed);

    std::uniform_int_distribution<double> distribution(lower * 10000.0, upper * 10000.0);
    return (distribution(generator) / 10000.0);
}

// Returns a random true / false
bool RandomBool() {
    static unsigned seed = static_cast<unsigned>(std::chrono::system_clock::now().time_since_epoch().count());
    static std::default_random_engine generator (seed);

    std::uniform_int_distribution<int> distribution(1, 2);
    return (distribution(generator) == 1);
}


}   // End namespace Dr









