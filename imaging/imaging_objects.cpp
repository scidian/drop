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
#include "editor_view/editor_view.h"
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


// Local Interger Point Class
class IntPoint {
public:
    int  x = 0;
    int  y = 0;
    IntPoint() { }
    IntPoint(int _x, int _y) : x(_x), y(_y) { }
};


//####################################################################################
//##    Returns black / white image
//##        alpha_tolerance is from 0.0 to 1.0
//##        NORMAL  (inverse == false): transparent areas are black, objects are white
//##        INVERSE (inverse == true) : transparent areas are white, objects are black
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

    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            lines[y][x] = (QColor::fromRgba(lines[y][x]).alphaF() < alpha_tolerance) ? color1 : color2;
        }
    }
    return image;
}


//####################################################################################
//##    Flood fill
//##        !!!!! #NOTE: Returns Flood QImage, 'from_image' passed in by reference is altered !!!!!
//##
//##        tolerance: how similar color should be to continue to fill, 0.0 to 1.0
//##        REFERENCE RETURN
//##            int flood_pixel_count - returns number of total pixels in flood
//####################################################################################
#define FLOOD_NOT_PROCESSED         0
#define FLOOD_WAS_PROCESSED         1
#define FLOOD_MARKED_FOR_PROCESS    2

QImage floodFill(QImage &from_image, int at_x, int at_y, QColor color, double tolerance, Flood_Fill_Type type, int &flood_pixel_count) {
    flood_pixel_count = 0;

    // Get scan lines
    QImage flood =      from_image.copy();
    QImage processed =  from_image.copy();
    QVector<QRgb*> image_lines =     getScanLines(from_image);
    QVector<QRgb*> flood_lines =     getScanLines(flood);
    QVector<QRgb*> processed_lines = getScanLines(processed);

    // Check if start point is in range
    if (at_x < 0 || at_y < 0 || at_x > from_image.width() - 1 || at_y > from_image.height() - 1) return QImage(0, 0, QImage::Format::Format_ARGB32);
    if (from_image.width() < 1 || from_image.height() < 1) return QImage(0, 0, QImage::Format::Format_ARGB32);

    // Get starting color, set processed image to all zeros
    QColor start_color = QColor::fromRgba(image_lines[at_y][at_x]);
    for (int y = 0; y < from_image.height(); ++y) {
        for (int x = 0; x < from_image.width(); ++x) {
            flood_lines[y][x] = 0;
            processed_lines[y][x] = FLOOD_NOT_PROCESSED;
        }
    }

    QVector<IntPoint> points;
    points.push_back(IntPoint(at_x, at_y));
    do {
        // Go through each point and find new points to fill
        for (auto point: points) {
            if (processed_lines[point.y][point.x] == FLOOD_WAS_PROCESSED) continue;
            image_lines[point.y][point.x] = color.rgba();
            flood_lines[point.y][point.x] = color.rgba();
            processed_lines[point.y][point.x] = FLOOD_WAS_PROCESSED;
            ++flood_pixel_count;

            int x_start = (point.x > 0) ?                       point.x - 1 : 0;
            int x_end =   (point.x < from_image.width() - 1)  ? point.x + 1 : from_image.width()  - 1;
            int y_start = (point.y > 0) ?                       point.y - 1 : 0;
            int y_end =   (point.y < from_image.height() - 1) ? point.y + 1 : from_image.height() - 1;

            for (int x = x_start; x <= x_end; ++x) {
                for (int y = y_start; y <= y_end; ++y) {
                    if (x == point.x && y == point.y) continue;

                    if (type == Flood_Fill_Type::Compare_4) {
                        if ( (x == point.x - 1) && (y == point.y - 1) ) continue;
                        if ( (x == point.x + 1) && (y == point.y - 1) ) continue;
                        if ( (x == point.x - 1) && (y == point.y + 1) ) continue;
                        if ( (x == point.x + 1) && (y == point.y + 1) ) continue;
                    }

                    if (processed_lines[y][x] == FLOOD_NOT_PROCESSED) {
                        if (Dr::IsSameColor(start_color, QColor::fromRgba(image_lines[y][x]), tolerance)) {
                            points.push_back(IntPoint(x, y));
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
//##        Seperates parts of an image divided by alpha space into seperate images, the
//##        resulting array of images are black and white. Black where around the ouside of
//##        of the object, and the object itself is white
//####################################################################################
QVector<QImage> findObjectsInImage(const QPixmap &pixmap, double alpha_tolerance) {
    QImage black_white = blackAndWhiteFromAlpha(pixmap.toImage(), alpha_tolerance, true);
    QVector<QRgb*>  lines = getScanLines(black_white);
    QVector<QImage> images;

    // Loop through every pixel in image, if we find a spot that has an object, flood fill that
    // spot and add the resulting image shape to the array of object images
    QColor white = QColor::fromRgba(c_color_white);
    for (int x = 0; x < black_white.width(); ++x) {
        for (int y = 0; y < black_white.height(); ++y) {
            if (lines[y][x] == c_color_black) {
                int flood_pixel_count;
                QImage flood_fill = floodFill(black_white, x, y, white, 0.001, Flood_Fill_Type::Compare_4, flood_pixel_count);
                if (flood_pixel_count > 0)
                    images.push_back( flood_fill );
            }
        }
    }
    return images;
}


//####################################################################################
//##    Returns a clockwise list of points representing an alpha outline of an image
//##        !!!!! #NOTE: Image passed in should be black and white,
//##                     probably from DrImageing::blackAndWhiteFromAlpha()
//####################################################################################
#define TRACE_NOT_PROCESSED         0           // Pixels that can be part of the border
#define TRACE_PROCESSED_ONCE        1           // Pixels that added to the border once
#define TRACE_PROCESSED_TWICE       2           // Pixels that added to the border twice (after a there and back again trace)
#define TRACE_NOT_BORDER            5           // Pixels that are not near the edge
#define TRACE_START_PIXEL          10           // Starting pixel

QVector<HullPoint> traceImageOutline(const QImage &from_image) {
    // Initialize images
    QImage image = from_image;
    QImage processed =  from_image.copy();
    QVector<QRgb*> image_lines =        getScanLines(image);
    QVector<QRgb*> processed_lines =    getScanLines(processed);

    // Initialize point array, verify image size
    QVector<IntPoint> points; points.clear();
    if (image.width() < 1 || image.height() < 1) return QVector<HullPoint> { };

    // ***** Find starting point, and also set processed image bits
    bool has_start_point = false;
    IntPoint last_point;
    for (int x = 0; x < image.width(); ++x) {
        for (int y = 0; y < image.height(); ++y) {
            // If pixel is black, it cannot be part of the border
            if (image_lines[y][x] == c_color_black) {
                processed_lines[y][x] = TRACE_NOT_BORDER;
                continue;
            }

            // Run through all pixels this pixel is touching to see if any are transparent (i.e. black)
            bool can_be_border = false;
            if (x == 0 || y == 0 || x == (image.width() - 1) || y == (image.height() - 1)) {
                can_be_border = true;
            } else {
                for (int i = x - 1; i <= x + 1; ++i) {
                    for (int j = y  - 1; j <= y + 1; ++j) {
                        if (image_lines[j][i] == c_color_black) can_be_border = true;
                    }
                }
            }

            // If not touching any transparent pixels, and not on edge, cannot be part of border
            if (!can_be_border) {
                processed_lines[y][x] = TRACE_NOT_BORDER;
                continue;
            }

            // Otherwise mark it as not processed and check if we have a start point
            processed_lines[y][x] = TRACE_NOT_PROCESSED;
            if (!has_start_point) {
                points.push_back(IntPoint(x, y));
                last_point = IntPoint(x - 1, y);
                processed_lines[y][x] = TRACE_START_PIXEL;
                has_start_point = true;
            }
        }
    }

    // ***** Find outline points
    QVector<IntPoint> surround;
    do {
        // Collect list of points around current point
        surround.clear();
        IntPoint current_point = points.last();
        int x_start = (current_point.x > 0) ?                       current_point.x - 1 : 0;
        int x_end =   (current_point.x < from_image.width() - 1)  ? current_point.x + 1 : from_image.width()  - 1;
        int y_start = (current_point.y > 0) ?                       current_point.y - 1 : 0;
        int y_end =   (current_point.y < from_image.height() - 1) ? current_point.y + 1 : from_image.height() - 1;
        for (int x = x_start; x <= x_end; ++x) {
            for (int y = y_start; y <= y_end; ++y) {
                if (x == current_point.x && y == current_point.y) continue;
                if (processed_lines[y][x] != TRACE_PROCESSED_TWICE && processed_lines[y][x] != TRACE_NOT_BORDER) {
                    surround.push_back(IntPoint(x, y));
                }
            }
        }

        // Compare surrounding points to see which one has the greatest angle measured clockwise from the last set of points
        double compare_angle = DrView::calcRotationAngleInDegrees(QPointF(current_point.x, current_point.y), QPointF(last_point.x, last_point.y));
        double angle = -1;
        IntPoint next_point;
        for (auto point : surround) {
            double check_angle = DrView::calcRotationAngleInDegrees(QPointF(current_point.x, current_point.y), QPointF(point.x, point.y));
            while (check_angle > 0) {             check_angle -= 360.0; }
            while (check_angle < compare_angle) { check_angle += 360.0; }
            if (check_angle > angle) {
                angle = check_angle;
                next_point = point;
            }
        }

        // If we found an angle, we found our next point, add it to the list
        if (angle > -1) {
            if (processed_lines[current_point.y][current_point.x] == TRACE_NOT_PROCESSED)
                processed_lines[current_point.y][current_point.x] = TRACE_PROCESSED_ONCE;
            else if (processed_lines[current_point.y][current_point.x] == TRACE_PROCESSED_ONCE)
                processed_lines[current_point.y][current_point.x] = TRACE_PROCESSED_TWICE;
            last_point = current_point;
            points.push_back(next_point);
        }

    } while ((surround.count() > 0) && (processed_lines[points.last().y][points.last().x] != TRACE_START_PIXEL));

    // ***** Convert to HullPoint array and return
    QVector<HullPoint> hull_points;
    for (int i = 0; i < points.count(); ++i) {
        hull_points.push_back(HullPoint(points[i].x, points[i].y));
    }
    return hull_points;
}


//####################################################################################
//##    Returns a mostly random list of points of possible edges of an image
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

















