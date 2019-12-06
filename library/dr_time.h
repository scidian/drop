//
//      Created by Stephens Nunnally on 12/6/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      A library of helpful Time functions within the global Dr namespace
//  Note:
//      To be Engine compatible, this file does NOT, and should NOT #include any Qt Library types
//
#ifndef DR_TIME_H
#define DR_TIME_H

#include <chrono>

// Type Definitions
typedef std::chrono::high_resolution_clock Clock;
typedef Clock::time_point DrTime;

namespace Dr {


//####################################################################################
//##    Time Functions
//############################
double                  MillisecondsElapsed(const DrTime &timer);
double                  MillisecondsSinceStartOfDay();
void                    ResetTimer(DrTime &timer);




}   // end namespace Dr

#endif // DR_TIME_H






