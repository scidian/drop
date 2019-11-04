//
//      Created by Stephens Nunnally on 11/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QtMath>
#include <QDebug>
#include <QImage>
#include <QPixmap>
#include <QRgb>
#include <QTime>
#include <QVector2D>
#include <QVector3D>

#include "colors/colors.h"
#include "globals.h"
#include "imaging.h"
#include "helper.h"
#include "style/style.h"


namespace DrImaging
{


//####################################################################################
//##    Draws a DrDeviceType::Camera as a Pixmap
//####################################################################################
QPixmap drawCamera() {
    QColor color = Qt::red;

    int width =  c_image_size;
    int height = c_image_size;
    QPixmap lens(width, height);
    lens.fill(Qt::transparent);
    color.setAlphaF(color.alphaF() * 0.5);                  // Decrease lens opacity by half

    QPainter painter(&lens);
    painter.setPen(Qt::NoPen);
    painter.setBrush(color);
    painter.drawEllipse(0, 0, width, height);

    return lens;
}




}   // End DrImaging Namespace




