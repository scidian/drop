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
#include <QLabel>
#include <QPainter>
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
void    ApplyCustomStyleSheetFormatting(QWidget *widget);
void    ApplyDropShadow(QWidget *target_widget, qreal blur_radius, qreal offset_x, qreal offset_y, QColor shadow_color);
void    ApplyDropShadowByType(QWidget *target_widget, Shadow_Types shadow_type);
void    ApplyRoundedCornerMask(QWidget *widget, int x_radius, int y_radius, int method = 0);
int     CheckFontWidth(QFont font, QString text_to_check);
QString FitStringToWidth(QFont font, QString text_to_check, int max_width = 80, bool use_dots = true);

QString StyleSheetColorButton(QColor color, QColor text_color, QColor highlight);
QString StyleSheetToolBar();
QString StyleSheetToolBarButtons(QString button_name, QString icon_unchecked, QString icon_checked, QString icon_disabled,
                                 int padding, int tl, int tr, int bl, int br, bool checkable);
QString StyleSheetToolBarModeButton(QString button_name, QString icon_name, QString icon_faded_name);
QString StyleSheetRecessedBackgroundBorder(int top_percent, int bottom_percent, bool highlight = false);
QString StyleSheetPoppedOutBackgroundBorder(QColor background_color, QColor border_color, int top_percent, int bottom_percent, bool highlight = false);


// Comparison Functions
double  CheckScaleNotZero(double scale_to_check);
bool    IsCloseTo(double number_desired, double number_to_check, double tolerance);
QString RemoveTrailingDecimals(double value, int max_decimal_places);


// Comparison Template Function, Returns number_to_check fit to within the bounds of min / max
template<class T>
const T& Clamp(const T& number_to_check, const T& min, const T& max) {
    if      (number_to_check < min) return min;
    else if (number_to_check > max) return max;
    else                            return number_to_check;
}


// Angle Functions
double  Closest90DegreeAngle(double angle, double angle_to_find);
bool    IsSimilarAngle(double angle1, double angle2, double tolerance = .001);
bool    IsSquare(double check_angle);


// User Interaction
QString CurrentTimeAsString();
void    ShowErrorMessage(QString function_name, QString error_message);
void    ShowMessageBox(QString new_message, QPixmap pixmap = QPixmap());


}

#endif // LIBRARY_WIDGET_H










