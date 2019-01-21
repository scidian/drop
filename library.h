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

namespace Dr {


bool    IsCloseTo(double number_desired, double number_to_check, double tolerance);

void    ShowMessageBox(QString new_message);

void    WidgetMaskRoundedCorners(QWidget *widget, int x_radius, int y_radius);


}

#endif // LIBRARY_WIDGET_H










