//
//      Created by Stephens Nunnally on 12/6/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//  Note:
//      To be Engine compatible, this file does NOT, and should NOT #include any Qt Library types
//
#ifndef DR_MATH_H
#define DR_MATH_H

// Forward Declarations
class DrPoint;
class DrPointF;

// Local Defines
#define EPSILON 0.00001
#define DR_PI   3.141592653589793238463

namespace Dr {


//####################################################################################
//##    FuzzyCompare
//##        Returns true if 'number_a' is within +-'EPSILON' of 'number_b'
//##    IsCloseTo
//##        Returns true if 'number_desired' is within +-'tolerance' of 'number_to_check'
//############################
template<class T> bool  FuzzyCompare(const T& number_a, const T& number_b) {
    return ( (number_a <= (number_b + EPSILON)) && (number_a >= (number_b - EPSILON)) ); }

template<class T> bool  IsCloseTo(const T& number_desired, const T& number_to_check, const T& tolerance) {
    return ( (number_to_check <= (number_desired + tolerance)) && (number_to_check >= (number_desired - tolerance)) ); }


//####################################################################################
//##    Comparison Functions
//############################
// Returns number_to_check fit to within the bounds of min / max
template<class T> T      Clamp(const T& number_to_check, const T& min, const T& max) {
    if (number_to_check < min) return min;
    if (number_to_check > max) return max;
    return number_to_check;
}

// Return the Max of two values
template<class T> T      Max(const T& a, const T& b) { return (a > b) ? a : b; }

// Return the Min of two values
template<class T> T      Min(const T& a, const T& b) { return (a < b) ? a : b; }

// Linear Interpolation between two values
template<class T> T      Lerp(const T& f1, const T& f2, const T& t) { return (f1 * (static_cast<T>(1.0) - t)) + (f2 * t); }

// Linear Interpolation between two values by no more than d
template<class T> T      LerpConst(const T& f1, const T& f2, const T& d) { return f1 + Clamp(f2 - f1, -d, d); }

// Swaps two values
template<class T> void   Swap(T& number1, T& number2) { T temp = number1; number1 = number2; number2 = temp; }


//####################################################################################
//##    Angle Functions
//############################
template<class T> T      RadiansToDegrees(const T& rad) { return (rad * 57.295779513082321); }           // == (180.0 / 3.141592653589793238463);
template<class T> T      DegreesToRadians(const T& degrees) { return degrees * (DR_PI / 180.0); }

// Equalizes x, y, and z angles to within 0 to 360
template<class T> T      EqualizeAngle0to360(const T& angle) {
    T equalized = angle;
    while (equalized <   0) { equalized += 360; }
    while (equalized > 360) { equalized -= 360; }
    return equalized;
}

// Finds closest angle within 180 degrees of angle (both angles must be between 0 to 360)
template<class T> T      FindClosestAngle180(const T& start, const T& angle) {
    T closest = angle;
    if (closest - start > 180)
        closest -= 360;
    else if (start - closest > 180)
        closest += 360;
    return closest;
}

double       CalcRotationAngleInDegrees(DrPointF center_point, DrPointF target_point);
double       Closest90DegreeAngle(double angle, double angle_to_find);
double       DifferenceBetween2Angles(double angle1, double angle2);
bool         IsSimilarAngle(double angle1, double angle2, double tolerance = 0.001);
bool         IsSquare(double check_angle);
bool         IsRectangle(DrPointF p1, DrPointF p2, DrPointF p3, DrPointF p4);
DrPointF     RotatePointAroundOrigin(DrPointF point, DrPointF origin, double angle, bool angle_is_in_radians = false);




}   // end namespace Dr

#endif // DR_MATH_H

















