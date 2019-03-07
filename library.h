//
//      Created by Stephens Nunnally on 1/21/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Misc Helpful Math, Output, and QWidget / QObject functions
//
//
#ifndef LIBRARY_WIDGET_H
#define LIBRARY_WIDGET_H

#include <QBitmap>
#include <QPainter>
#include <QMessageBox>
#include <QWidget>


//####################################################################################
//##    Local Enumerations
//############################
enum class Shadow_Types
{
    Button_Shadow,
    Tool_Tip_Shadow,
};


//####################################################################################
//##    Dr Namespace
//##        A library of helpful functions in a global space
//############################
namespace Dr {


// Form / Widget Functions
void    ApplyColoring(QWidget *widget);
void    ApplyDropShadow(QWidget *target_widget, qreal blur_radius, qreal offset_x, qreal offset_y, QColor shadow_color);
void    ApplyDropShadowByType(QWidget *target_widget, Shadow_Types shadow_type);
void    ApplyRoundedCornerMask(QWidget *widget, int x_radius, int y_radius);


// Comparison Functions
double  CheckScaleNotZero(double scale_to_check);
bool    IsCloseTo(double number_desired, double number_to_check, double tolerance);
QString RemoveTrailingDecimals(double value, int max_decimal_places);

// Comparison Template Function, Returns number_to_check fit to within the bounds of min / max
//template<class T>
//inline const T& FitToRange(const T& number_to_check, const T& min, const T& max) {
//    if      (number_to_check < min) return min;
//    else if (number_to_check > max) return max;
//    else                            return number_to_check;
//}

template<class T>
const T& FitToRange(const T& number_to_check, const T& min, const T& max);

template<class T>
const T& FitToRange(const T& number_to_check, const T& min, const T& max) {
    if      (number_to_check < min) return min;
    else if (number_to_check > max) return max;
    else                            return number_to_check;
}


// Angle Functions
bool    IsSimilarAngle(double angle1, double angle2);
bool    IsSquare(double check_angle);


// User Interaction
QString CurrentTimeAsString();
void    ShowErrorMessage(QString function_name, QString error_message);
void    ShowMessageBox(QString new_message, QPixmap pixmap = QPixmap());


}

#endif // LIBRARY_WIDGET_H










