//
//      Created by Stephens Nunnally on 1/21/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Misc helpful math, string and output functions
//
//
#ifndef HELPER_WIDGET_H
#define HELPER_WIDGET_H

#include <QBitmap>
#include <QLabel>
#include <QPainter>
#include <QScreen>
#include <QWidget>
#include <chrono>

typedef std::chrono::high_resolution_clock Clock;
typedef Clock::time_point DrTime;

//####################################################################################
//##    A library of helpful functions within the global Dr namespace
//############################
namespace Dr {

// Comparison Template Function, Returns number_to_check fit to within the bounds of min / max
template<class T> T Clamp(const T& number_to_check, const T& min, const T& max) {
    if      (number_to_check < min) return min;
    else if (number_to_check > max) return max;
    else                            return number_to_check;
}

// Casts an Enum value to an int
template<class T> int EnumToInt(const T& enum_to_convert) { return static_cast<int>(enum_to_convert); }

// Return the Max of two values
template<class T> T Max(const T& a, const T& b) { return (a > b) ? a : b; }

// Return the Min of two values
template<class T> T Min(const T& a, const T& b) { return (a < b) ? a : b; }

// Linear Interpolation between two values
template<class T> T Lerp(const T& f1, const T& f2, const T& t) { return (f1 * (static_cast<T>(1.0) - t)) + (f2 * t); }


// Time Functions
QString     CurrentTimeAsString();
double      MillisecondsElapsed(const DrTime &timer);
void        ResetTimer(DrTime &timer);

// Comparison Functions
double      CheckScaleNotZero(double scale_to_check);
bool        IsCloseTo(double number_desired, double number_to_check, double tolerance);
QString     RemoveTrailingDecimals(double value, int max_decimal_places);

// Angle Functions
double      Closest90DegreeAngle(double angle, double angle_to_find);
bool        IsSimilarAngle(double angle1, double angle2, double tolerance = .001);
bool        IsSquare(double check_angle);

// Custom Font Functions
QFont       CustomFont();
QString     FontFamily();
int         FontSize();
void        LoadCustomFonts();

// String Functions
int         CheckFontWidth(QFont font, QString text_to_check);
QString     FitStringToWidth(QFont font, QString text_to_check, int max_width = 80, bool use_dots = true);
QString     StringFromBool(bool boolean);

// User Interaction
void        ShowErrorMessage(QString function_name, QString error_message);
void        ShowMessageBox(QString new_message, QPixmap pixmap = QPixmap());


}

#endif // HELPER_WIDGET_H















