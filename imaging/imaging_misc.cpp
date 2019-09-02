//
//      Created by Stephens Nunnally on 9/2/2019, (c) 2019 Scidian Software, All Rights Reserved
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
//##        Returns average color of a Pixmap
//####################################################################################
QColor averageColor(const QPixmap &pixmap, bool screen_shot) {
    QPixmap  one(20, 20);
             one.fill(Qt::transparent);
    QPainter painter(&one);
    painter.drawPixmap(one.rect(), pixmap, pixmap.rect());
    painter.end();

    QImage image = one.toImage();
    double r = 0, g = 0, b = 0;
    double count = 0;
    for (int i = 0; i < one.width(); ++i) {
        for (int j = 0; j < one.height(); ++j) {
            QColor pixel = image.pixelColor(i, j);
            r += pixel.red() *   pixel.alphaF();
            g += pixel.green() * pixel.alphaF();
            b += pixel.blue() *  pixel.alphaF();
            count += pixel.alphaF();
        }
    }

    if (screen_shot) {
        QString file_name = QStandardPaths::writableLocation(QStandardPaths::StandardLocation::DesktopLocation);
        file_name += "/average_color";
        file_name += " - " + QDateTime::currentDateTime().toString("yyyy-MM-dd-hhmmss-zzz") + ".png";
        image.save( file_name );
    }

    return QColor(static_cast<int>(r / count), static_cast<int>(g / count), static_cast<int>(b / count));
}


//####################################################################################
//##        Returns an array of bits from an Image as aRgb float values
//####################################################################################
float* imageBitsAsFloat(const QImage &from_image) {
    QImage image = from_image;
    if ( image.format() != QImage::Format::Format_ARGB32 )
        image = image.convertToFormat( QImage::Format_ARGB32 );
    image.detach();

    float *out = static_cast<float*>( malloc(static_cast<unsigned long>(image.width() * image.height()) * sizeof(float)) );

    // Truecolor Rgba
    if (image.colorCount() == 0 ) {
        if ( image.hasAlphaChannel() ) {

            // Loop through every pixel, if alpha is below threshold, set to mask color
            int index = 0;
            for (int y = 0; y < image.height(); ++y) {
                QRgb* line = reinterpret_cast<QRgb*>( image.scanLine( y ));
                for (int x = 0; x < image.width(); ++x) {
                    out[index] = line[x];
                    index++;
                }
            }

        } else {    Dr::ShowMessageBox("Image missing alpha channel!"); }
    } else {    Dr::ShowMessageBox("Image only has 256 colors!"); }
    return out;
}


//####################################################################################
//##        Returns a list of points of possible edges of an image
//####################################################################################
QVector<HullPoint> outlinePointList(const QImage& from_image, double alpha_tolerance ) {
    QImage image = from_image;
    if (image.format() != QImage::Format::Format_ARGB32)
        image = image.convertToFormat( QImage::Format_ARGB32 );
    image.detach();

    QVector<HullPoint> points;
    points.clear();

    // Truecolor Rgba
    if (image.colorCount() == 0 ) {
        if ( image.hasAlphaChannel() ) {

            // Grab all the scan lines
            QVector<QRgb*> lines;
            for (int y = 0; y < image.height(); ++y) {
                lines.append( reinterpret_cast<QRgb*>(image.scanLine(y)) );
            }

            // Loop through every pixel to see if is possibly on border
            for (int y = 0; y < image.height(); ++y) {
                for (int x = 0; x < image.width(); ++x) {

                    if ( QColor::fromRgba(lines[y][x]).alphaF() < alpha_tolerance) continue;

                    bool touching_transparent = false;

                    // Run through all pixels this pixel is touching to see if they are transparent
                    int x_start, x_end, y_start, y_end;
                    x_start = (x > 0) ? x - 1 : x;
                    y_start = (y > 0) ? y - 1 : y;
                    x_end =   (x < (image.width() - 1))  ? x + 1 : x;
                    y_end =   (y < (image.height() - 1)) ? y + 1 : y;
                    for (int i = x_start; i <= x_end; ++i) {
                        for (int j = y_start; j <= y_end; ++j) {
                            if ( QColor::fromRgba(lines[j][i]).alphaF() < alpha_tolerance)
                                touching_transparent = true;
                            if (touching_transparent) break;
                        }
                        if (touching_transparent) break;
                    }

                    if (touching_transparent) {
                        points.push_back(HullPoint(x, y));
                    } else {
                        if ((x == 0 && y == 0) ||
                            (x == 0 && y == (image.height() - 1)) ||
                            (x == (image.width() - 1) && y == 0) ||
                            (x == (image.width() - 1) && y == (image.height() - 1))) {
                            points.push_back(HullPoint(x, y));
                        }
                    }
                }
            }

        } else {    Dr::ShowMessageBox("Image missing alpha channel!"); }
    } else {    Dr::ShowMessageBox("Image only has 256 colors!"); }

    return points;
}



}   // End Namespce DrImaging







