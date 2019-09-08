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
class FillPoint {
public:
    FillPoint(int new_x, int new_y) : x(new_x), y(new_y) { }
    int  x;
    int  y;
};

bool isSameColor(QColor color1, QColor color2, double tolerance) {
    return ( Dr::IsCloseTo(color1.redF(),   color2.redF(),   tolerance) &&
             Dr::IsCloseTo(color1.greenF(), color2.greenF(), tolerance) &&
             Dr::IsCloseTo(color1.blueF(),  color2.blueF(),  tolerance) &&
             Dr::IsCloseTo(color1.alphaF(), color2.alphaF(), tolerance));
}

#define FLOOD_NOT_PROCESSED         0
#define FLOOD_WAS_PROCESSED         1
#define FLOOD_MARKED_FOR_PROCESS    2

QImage floodFill(const QImage& from_image, int start_x, int start_y, QColor color, double tolerance) {
    // Get scan lines, check if start point is in range
    QImage image =     from_image.copy();
    QImage processed = from_image.copy();
    QVector<QRgb*> image_lines =     getScanLines(image);
    QVector<QRgb*> processed_lines = getScanLines(processed);
    if (start_x < 0 || start_y < 0 || start_x > image.width() - 1 || start_y > image.height() - 1) return image;

    // Get starting color, set processed image to all zeros
    QColor start_color = QColor::fromRgba(image_lines[start_y][start_x]);
    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            processed_lines[y][x] = FLOOD_NOT_PROCESSED;
        }
    }

    QVector<FillPoint> points;
    points.push_back(FillPoint(start_x, start_y));
    do {
        // Go through each point and find new points to fill
        for (auto point: points) {
            if (processed_lines[point.y][point.x] == FLOOD_WAS_PROCESSED) continue;
            image_lines[point.y][point.x] = color.rgba();
            processed_lines[point.y][point.x] = FLOOD_WAS_PROCESSED;

            int x_start = (point.x > 0) ?                  point.x - 1 : 0;
            int x_end =   (point.x < image.width() - 1)  ? point.x + 1 : image.width()  - 1;
            int y_start = (point.y > 0) ?                  point.y - 1 : 0;
            int y_end =   (point.y < image.height() - 1) ? point.y + 1 : image.height() - 1;

            for (int x = x_start; x <= x_end; ++x) {
                for (int y = y_start; y <= y_end; ++y) {
                    if (x == point.x && y == point.y) continue;

                    if (processed_lines[y][x] == FLOOD_NOT_PROCESSED) {
                        if (isSameColor(start_color, QColor::fromRgba(image_lines[y][x]), tolerance)) {
                            points.push_back(FillPoint(x, y));
                            processed_lines[y][x] = FLOOD_MARKED_FOR_PROCESS;
                        }
                    }
                }
            }
        }

        // Remove any points that have been processed
        auto it = points.begin();
        while (it != points.end()) {
            if (processed_lines[it->y][it->x] == FLOOD_WAS_PROCESSED)
                it = points.erase(it);
            else
                ++it;
        }
    } while (points.count() > 0);

    return image;
}










}   // End Namespace Imaging








