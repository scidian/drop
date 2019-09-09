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


// Local constants
///const unsigned int c_color_black = 0;
///const unsigned int c_color_white = 4294967295;
const unsigned int c_color_black = QColor(  0,   0,   0,   0).rgba();
const unsigned int c_color_white = QColor(255, 255, 255, 255).rgba();


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
QImage blackAndWhiteFromAlpha(const QImage &from_image, double alpha_tolerance, bool inverse) {
    QImage image = from_image;
    QVector<QRgb*> lines = getScanLines(image);

    unsigned int color1, color2;
    if (inverse) {
        color1 = c_color_white;
        color2 = c_color_black;
    } else {
        color1 = c_color_black;
        color2 = c_color_white;
    }

    // Loop through every pixel
    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            unsigned int pixel = static_cast<unsigned int>(lines[y][x]);
            lines[y][x] = (QColor::fromRgba(pixel).alphaF() < alpha_tolerance) ? color1 : color2;
        }
    }
    return image;
}


//####################################################################################
//##    Compares 2 colors, returns true if they are the same
//####################################################################################
bool isSameColor(QColor color1, QColor color2, double tolerance) {
    return ( Dr::IsCloseTo(color1.redF(),   color2.redF(),   tolerance) &&
             Dr::IsCloseTo(color1.greenF(), color2.greenF(), tolerance) &&
             Dr::IsCloseTo(color1.blueF(),  color2.blueF(),  tolerance) &&
             Dr::IsCloseTo(color1.alphaF(), color2.alphaF(), tolerance) );
}


//####################################################################################
//##    Flood fill
//##        !!!!! #NOTE: Returns flood as it's own image, image passed in is altered!!
//##        tolerance: how similar color should be to continue to fill, 0.0 to 1.0
//####################################################################################
class FillPoint {
public:
    FillPoint(int new_x, int new_y) : x(new_x), y(new_y) { }
    int  x;
    int  y;
};

#define FLOOD_NOT_PROCESSED         0
#define FLOOD_WAS_PROCESSED         1
#define FLOOD_MARKED_FOR_PROCESS    2

QImage floodFill(QImage &from_image, int start_x, int start_y, QColor color, double tolerance) {
    // Get scan lines
    QImage flood =     from_image.copy();
    QImage processed = from_image.copy();
    QVector<QRgb*> image_lines =     getScanLines(from_image);
    QVector<QRgb*> flood_lines =     getScanLines(flood);
    QVector<QRgb*> processed_lines = getScanLines(processed);

    // Get starting color, set processed image to all zeros
    QColor start_color = QColor::fromRgba(image_lines[start_y][start_x]);
    for (int y = 0; y < from_image.height(); ++y) {
        for (int x = 0; x < from_image.width(); ++x) {
            flood_lines[y][x] = 0;
            processed_lines[y][x] = FLOOD_NOT_PROCESSED;
        }
    }

    // Check if start point is in range
    if (start_x < 0 || start_y < 0 || start_x > from_image.width() - 1 || start_y > from_image.height() - 1) return flood;

    QVector<FillPoint> points;
    points.push_back(FillPoint(start_x, start_y));
    do {
        // Go through each point and find new points to fill
        for (auto point: points) {
            if (processed_lines[point.y][point.x] == FLOOD_WAS_PROCESSED) continue;
            image_lines[point.y][point.x] = color.rgba();
            flood_lines[point.y][point.x] = color.rgba();
            processed_lines[point.y][point.x] = FLOOD_WAS_PROCESSED;

            int x_start = (point.x > 0) ?                       point.x - 1 : 0;
            int x_end =   (point.x < from_image.width() - 1)  ? point.x + 1 : from_image.width()  - 1;
            int y_start = (point.y > 0) ?                       point.y - 1 : 0;
            int y_end =   (point.y < from_image.height() - 1) ? point.y + 1 : from_image.height() - 1;

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

    return flood;
}


//####################################################################################
//##    Find Objects
//##        Seperates parts of an image divided by alpha space into seperate images
//####################################################################################
QVector<QImage> findObjectsInImage(const QPixmap &pixmap, double alpha_tolerance) {
    QImage black_white = blackAndWhiteFromAlpha(pixmap.toImage(), alpha_tolerance, true);
    QVector<QRgb*>  lines = getScanLines(black_white);
    QVector<QImage> images;

    QColor white = QColor::fromRgba(c_color_white);
    for (int x = 0; x < black_white.width(); ++x) {
        for (int y = 0; y < black_white.height(); ++y) {
            ///if (black_white.pixel(x, y) == c_color_black) {
            if (lines[y][x] == c_color_black) {
                images.push_back( floodFill(black_white, x, y, white, 0.001) );
            }
        }
    }
    return images;
}



//####################################################################################
//##    Returns a list of points of possible edges of an image
//##        !!!!! #NOTE: Image passed in should be black and white
//##                     (i.e. from DrImageing::blackAndWhiteFromAlpha())
//####################################################################################
QVector<HullPoint> outlinePointList(const QImage &from_image) {
    QImage image = from_image;
    QVector<QRgb*> lines = getScanLines(image);

    QVector<HullPoint> points;
    points.clear();

    // Loop through every pixel to see if is possibly on border
    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            if (lines[y][x] == c_color_black) continue;

            // Run through all pixels this pixel is touching to see if they are transparent (i.e. black)
            int x_start, x_end, y_start, y_end;
            x_start = (x > 0) ? x - 1 : x;
            y_start = (y > 0) ? y - 1 : y;
            x_end =   (x < (image.width() - 1))  ? x + 1 : x;
            y_end =   (y < (image.height() - 1)) ? y + 1 : y;
            bool touching_transparent = false;
            for (int i = x_start; i <= x_end; ++i) {
                for (int j = y_start; j <= y_end; ++j) {
                    if (lines[j][i] == c_color_black) touching_transparent = true;
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
    return points;
}



}   // End Namespace Imaging

















