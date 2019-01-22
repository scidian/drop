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

enum class Shadow_Types
{
    Button_Shadow,
    Tool_Tip_Shadow,
};


namespace Dr {


// Form / Widget Functions
void    ApplyColoring(QWidget *widget);
void    ApplyDropShadow(QWidget *target_widget, qreal blur_radius, qreal offset_x, qreal offset_y, QColor shadow_color);
void    ApplyDropShadowByType(QWidget *target_widget, Shadow_Types shadow_type);
void    ApplyRoundedCornerMask(QWidget *widget, int x_radius, int y_radius);


// Comparison Functions
bool    IsCloseTo(double number_desired, double number_to_check, double tolerance);


// User Interaction
void    ShowMessageBox(QString new_message);


}

#endif // LIBRARY_WIDGET_H










