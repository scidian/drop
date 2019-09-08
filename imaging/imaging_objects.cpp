//
//      Created by Stephens Nunnally on 9/8/2019, (c) 2019 Scidian Software, All Rights Reserved
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
#include <QStandardPaths>
#include <QTime>
#include <QVector2D>

#include "colors/colors.h"
#include "globals.h"
#include "imaging.h"
#include "helper.h"


namespace DrImaging
{


//####################################################################################
//##    Returns array of scanlines that are a direct access to QImage pixels
//####################################################################################
QVector<QRgb*> getScanLines(QImage &image) {
    QVector<QRgb*> lines;

    if (image.format() != QImage::Format::Format_ARGB32)
        image = image.convertToFormat( QImage::Format_ARGB32 );
    image.detach();

    // Truecolor Rgba
    if (image.colorCount() == 0) {
        if (image.hasAlphaChannel()) {

            // Grab all the scan lines
            for (int y = 0; y < image.height(); ++y)
                lines.append( reinterpret_cast<QRgb*>(image.scanLine(y)) );

        } else {    Dr::ShowMessageBox("Image missing alpha channel!"); }
    } else {    Dr::ShowMessageBox("Image only has 256 colors!"); }

    return lines;
}


//####################################################################################
//##    Returns black / white image, white == had pixel, black == was transparent
//##        alpha_tolerance is from 0.0 to 1.0
//####################################################################################
QImage blackAndWhiteFromAlpha(const QImage& from_image, double alpha_tolerance) {
    QImage image = from_image;
    QVector<QRgb*> lines = getScanLines(image);

    // Loop through every pixel
    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            if (QColor::fromRgba(lines[y][x]).alphaF() < alpha_tolerance)
                lines[y][x] = 0.0;
            else
                lines[y][x] = QColor(255, 255, 255, 255).rgba();
        }
    }
    return image;
}



//####################################################################################
//##    Flood fill
//##        tolerance: how similar color should be to continue to fill, 0.0 to 1.0
//####################################################################################
QImage floodFill(const QImage& from_image, int x, int y, QColor color, double tolerance) {
    QImage image = from_image;
    QVector<QRgb*> lines = getScanLines(image);

    if (x < 0 || y < 0 || x > image.width() - 1 || y > image.height() - 1) return image;


    // Loop through every pixel
    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {

            if (QColor::fromRgba(lines[y][x]).alphaF() < tolerance)
                lines[y][x] = 0.0;
            else
                lines[y][x] = QColor(255, 255, 255, 255).rgba();


        }
    }
    return image;
}










}   // End Namespace Imaging








