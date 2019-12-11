//
//      Created by Stephens Nunnally on 12/6/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "library/dr_time.h"


namespace Dr {


//####################################################################################
//##
//##    High-Res Timer Functions
//##
//####################################################################################
double MillisecondsElapsed(const DrTime &timer) {
    return (std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now() - timer).count() /  1000000.0);
}
double MillisecondsSinceStartOfDay() {
    auto now =    std::chrono::system_clock::now();
    time_t tnow = std::chrono::system_clock::to_time_t(now);
    tm *date = std::localtime(&tnow);
    date->tm_hour = 0;
    date->tm_min =  0;
    date->tm_sec =  0;
    auto midnight = std::chrono::system_clock::from_time_t(std::mktime(date));
    return (std::chrono::duration_cast<std::chrono::nanoseconds>(now - midnight).count() / 1000000.0);
}
void ResetTimer(DrTime &timer) { timer = Clock::now(); }




}   // namespace Dr











