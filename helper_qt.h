//
//      Created by Stephens Nunnally on 9/25/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      To be used with Qt Library types
//
//
#ifndef HELPER_QT_H
#define HELPER_QT_H

#include <QBitmap>
#include <QLabel>
#include <QPainter>
#include <QScreen>
#include <QWidget>


//####################################################################################
//##    A library of helpful functions within the global Dr namespace
//############################
namespace Dr {

// Time Functions
QString     CurrentTimeAsString();

// Comparison Functions
double      CheckScaleNotZero(double scale_to_check);
QString     RemoveTrailingDecimals(double value, int max_decimal_places);

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
void        ShowMessageBox(QString new_message, QPixmap pixmap = QPixmap(), bool modal = true);


}


#endif // HELPER_QT_H
