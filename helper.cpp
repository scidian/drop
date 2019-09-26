//
//      Created by Stephens Nunnally on 1/21/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Helpful misc functions
//
//
#include <QApplication>
#include <QFontDatabase>
#include <QGridLayout>
#include <QMessageBox>
#include <QSpacerItem>
#include <QTime>
#include <cmath>

#include "colors/colors.h"
#include "helper.h"
#include "widgets/widgets.h"

namespace Dr {


//####################################################################################
//##    High-Res Timer Functions
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
    return std::chrono::duration_cast<std::chrono::milliseconds>(now - midnight).count();
}
void ResetTimer(DrTime &timer) { timer = Clock::now(); }



//####################################################################################
//##    Angle Comparision Functions
//####################################################################################
// Returns true of the two angles are parrallel or perpendicular
bool IsSimilarAngle(double angle1, double angle2, double tolerance) {
    while (angle1 >= 90) { angle1 -= 90; }
    while (angle1 <   0) { angle1 += 90; }
    while (angle2 >= 90) { angle2 -= 90; }
    while (angle2 <   0) { angle2 += 90; }
    return IsCloseTo(angle1, angle2, tolerance);
}

// Returns true is 'check_angle' in equal to 0, 90, 180, or 270, i.e. "square" angle
bool IsSquare(double check_angle) {
    check_angle = abs(check_angle);
    while (check_angle >= 360) check_angle -= 360;
    if (qFuzzyCompare(check_angle, 0))   return true;
    if (qFuzzyCompare(check_angle, 90))  return true;
    if (qFuzzyCompare(check_angle, 180)) return true;
    if (qFuzzyCompare(check_angle, 270)) return true;
    return false;
}

// Returns 'angle' rounded to the nearest 90 multiple of 'angle_to_find'
double Closest90DegreeAngle(double angle, double angle_to_find) {
    double closest_angle =    0;
    double distance_apart = 180;
    for (double d = angle_to_find - 1080; d < angle_to_find + 1080; d += 90.0) {
        if (abs(d - angle) < distance_apart) {
            closest_angle = d;
            distance_apart = abs(d - angle);
        }
    }
    return closest_angle;
}



}   // namespace Dr













