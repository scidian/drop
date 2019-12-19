﻿//
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

#include "editor/colors/colors.h"
#include "editor/globals_editor.h"
#include "editor/helper_library.h"
#include "editor/imaging/imaging.h"
#include "editor/pixmap/pixmap.h"
#include "library/dr_math.h"
#include "library/types/dr_point.h"
#include "library/types/dr_pointf.h"


namespace Dr
{


//####################################################################################
//##    Returns black / white image
//##        alpha_tolerance is from 0.0 to 1.0
//##        NORMAL  (inverse == false): transparent areas are black, objects are white
//##        INVERSE (inverse == true) : transparent areas are white, objects are black
//####################################################################################
QImage BlackAndWhiteFromAlpha(const QImage &from_image, double alpha_tolerance, bool inverse) {
    QImage image = from_image;
    std::vector<QRgb*> lines = GetScanLines(image);

    unsigned int color1, color2;
    if (inverse) {
        color1 = c_color_white;
        color2 = c_color_black;
    } else {
        color1 = c_color_black;
        color2 = c_color_white;
    }

    for (size_t y = 0; y < static_cast<size_t>(image.height()); ++y) {
        for (size_t x = 0; x < static_cast<size_t>(image.width()); ++x) {
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

QImage FloodFill(QImage &from_image, int at_x, int at_y, QColor fill_color, double tolerance, Flood_Fill_Type type,
                 int &flood_pixel_count, QRect &flood_rect) {
    flood_pixel_count = 0;

    // Get scan lines
    QImage flood =      from_image.copy();
    QImage processed =  from_image.copy();
    std::vector<QRgb*> image_lines =     GetScanLines(from_image);
    std::vector<QRgb*> flood_lines =     GetScanLines(flood);
    std::vector<QRgb*> processed_lines = GetScanLines(processed);

    // Check if start point is in range    
    flood_rect = QRect(0, 0, 0, 0);
    if (at_x < 0 || at_y < 0 || at_x > from_image.width() - 1 || at_y > from_image.height() - 1) {
        return QImage(0, 0, QImage::Format::Format_ARGB32);
    } else if (from_image.width() < 1 || from_image.height() < 1) {
        return QImage(0, 0, QImage::Format::Format_ARGB32);
    } else if ((from_image.width()) == 1 && (from_image.height() == 1)) {
        image_lines[0][0] = fill_color.rgba();
        flood_lines[0][0] = fill_color.rgba();
        flood_rect = QRect(at_x, at_y, 1, 1);
        return flood;
    }

    // Get starting color, set processed image to all zeros
    QColor start_color = QColor::fromRgba(image_lines[at_y][at_x]);

    for (size_t y = 0; y < static_cast<size_t>(from_image.height()); ++y) {
        for (size_t x = 0; x < static_cast<size_t>(from_image.width()); ++x) {
            flood_lines[y][x] = 0;
            processed_lines[y][x] = FLOOD_NOT_PROCESSED;
        }
    }

    // Push starting point onto vector
    std::vector<DrPoint> points; points.clear();
    points.push_back(DrPoint(at_x, at_y));
    bool processed_some;

    int min_x = at_x, max_x = at_x;
    int min_y = at_y, max_y = at_y;
    do {
        // Go through each point and find new points to fill
        processed_some = false;
        for (auto point: points) {
            if (processed_lines[point.y][point.x] == FLOOD_WAS_PROCESSED) continue;
            image_lines[point.y][point.x] = fill_color.rgba();
            flood_lines[point.y][point.x] = fill_color.rgba();
            processed_lines[point.y][point.x] = FLOOD_WAS_PROCESSED;
            if (point.x < min_x) min_x = point.x;
            if (point.x > max_x) max_x = point.x;
            if (point.y < min_y) min_y = point.y;
            if (point.y > max_y) max_y = point.y;
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
                        if (Dr::SameQColor(start_color, QColor::fromRgba(image_lines[y][x]), tolerance)) {
                            points.push_back(DrPoint(x, y));
                            processed_lines[y][x] = FLOOD_MARKED_FOR_PROCESS;
                            processed_some = true;
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
    } while ((points.size() > 0) && processed_some);

    flood_rect = QRect(min_x, min_y, (max_x - min_x) + 1, (max_y - min_y) + 1);
    return flood;
}


//####################################################################################
//##    Fill border
//##        Traces Border of 'rect' and makes sure to fill in any c_color_black areas with fill_color
//####################################################################################
void FillBorder(QImage &image, QColor fill_color, QRect rect) {
    std::vector<QRgb*> lines = Dr::GetScanLines(image);
    QRect  fill_rect;
    int    fill_qty;

    int y1 = rect.y();
    int y2 = rect.y() + rect.height() - 1;
    for (int x = rect.x(); x < rect.x() + rect.width(); x++) {
        if (lines[y1][x] == c_color_black) {
            Dr::FloodFill(image, x, y1, fill_color, 0.001, Flood_Fill_Type::Compare_4, fill_qty, fill_rect);
        }
        if (lines[y2][x] == c_color_black) {
            Dr::FloodFill(image, x, y2, fill_color, 0.001, Flood_Fill_Type::Compare_4, fill_qty, fill_rect);
        }
    }

    int x1 = rect.x();
    int x2 = rect.x() + rect.width() - 1;
    for (int y = rect.y(); y < rect.y() + rect.height(); y++) {
        if (lines[y][x1] == c_color_black) {
            Dr::FloodFill(image, x1, y, fill_color, 0.001, Flood_Fill_Type::Compare_4, fill_qty, fill_rect);
        }
        if (lines[y][x2] == c_color_black) {
            Dr::FloodFill(image, x2, y, fill_color, 0.001, Flood_Fill_Type::Compare_4, fill_qty, fill_rect);
        }
    }
}


//####################################################################################
//##    Find Objects
//##        Seperates parts of an image divided by alpha space into seperate images, returns image count.
//##        The images are stored into the reference array passed in 'images', the images are black and white.
//##            Black where around the ouside of of the object, and the object itself is white.
//##        Rects of images are returned in 'rects'
//####################################################################################
int FindObjectsInImage(const QImage &image, std::vector<QImage> &images, std::vector<QRect> &rects, double alpha_tolerance, bool convert) {
    QImage black_white;
    if (convert) black_white = BlackAndWhiteFromAlpha(image, alpha_tolerance, true);
    else         black_white = image;
    std::vector<QRgb*> lines = GetScanLines(black_white);
    int object_count = 0;

    // Loop through every pixel in image, if we find a spot that has an object,
    // flood fill that spot and add the resulting image shape to the array of object images
    for (size_t x = 0; x < static_cast<size_t>(black_white.width()); ++x) {
        for (size_t y = 0; y < static_cast<size_t>(black_white.height()); ++y) {
            if (lines[y][x] == c_color_black) {
                QRect  rect;
                int    flood_pixel_count;
                QImage flood_fill = FloodFill(black_white, int(x), int(y), c_color_white, 0.001, Flood_Fill_Type::Compare_4, flood_pixel_count, rect);
                if (flood_pixel_count > 1) {
                    rects.push_back( rect );
                    images.push_back( flood_fill );
                    ++object_count;
                }
            }
        }
    }
    return object_count;
}


//####################################################################################
//##    Returns a clockwise list of points representing an alpha outline of an image
//##        !!!!! #NOTE: Image passed in should be black and white,
//##                     probably from DrImageing::BlackAndWhiteFromAlpha()
//####################################################################################
#define TRACE_NOT_BORDER            0           // Pixels that are not near the edge
#define TRACE_START_PIXEL           1           // Starting pixel
#define TRACE_NOT_PROCESSED         2           // Pixels that can be part of the border
#define TRACE_PROCESSED_ONCE        3           // Pixels that added to the border once
#define TRACE_PROCESSED_TWICE       4           // Pixels that added to the border twice        (after a there and back again trace)

std::vector<DrPointF> TraceImageOutline(const QImage &from_image) {
    // Initialize images
    QImage image = from_image;
    QImage processed = from_image.copy();
    std::vector<QRgb*> image_lines =        GetScanLines(image);
    std::vector<QRgb*> processed_lines =    GetScanLines(processed);
    int border_pixel_count = 0;

    // Initialize point array, verify image size
    std::vector<DrPoint> points; points.clear();
    if (image.width() < 1 || image.height() < 1) return std::vector<DrPointF> { };

    // ***** Find starting point, and also set processed image bits
    //       !!!!! #NOTE: Important that y loop is on top, we need to come at pixel from the left
    DrPoint last_point;
    bool has_start_point = false;

    for (size_t y = 0; y < static_cast<size_t>(image.height()); ++y) {
        for (size_t x = 0; x < static_cast<size_t>(image.width()); ++x) {
            // If pixel is part of the exterior, it cannot be part of the border
            if (image_lines[y][x] == c_color_black) {
                processed_lines[y][x] = TRACE_NOT_BORDER;
                continue;
            }

            // Run through all pixels this pixel is touching to see if any are transparent (i.e. c_color_black)
            bool can_be_border = false;
            if (x == 0 || y == 0 || x == static_cast<size_t>(image.width() - 1) || y == static_cast<size_t>(image.height() - 1)) {
                can_be_border = true;
            } else {
                for (size_t i = x - 1; i <= x + 1; ++i) {
                    for (size_t j = y - 1; j <= y + 1; ++j) {
                        if (image_lines[j][i] == c_color_black) can_be_border = true;
                    }
                }
            }

            // If not touching any transparent pixels, and not on edge, cannot be part of border
            if (!can_be_border) {
                processed_lines[y][x] = TRACE_NOT_BORDER;

            // Otherwise mark it as not processed and check if we have a start point
            } else {
                processed_lines[y][x] = TRACE_NOT_PROCESSED;
                ++border_pixel_count;
                if (!has_start_point) {
                    points.push_back(DrPoint(static_cast<int>(x), static_cast<int>(y)));
                    last_point = DrPoint(static_cast<int>(x - 1), static_cast<int>(y));
                    processed_lines[y][x] = TRACE_START_PIXEL;
                    has_start_point = true;
                }
            }
        }
    }
    if (border_pixel_count < 3) return std::vector<DrPointF> { };


    // ***** Find outline points
    std::vector<DrPoint> surround;
    bool back_at_start;
    do {
        // Collect list of points around current point
        surround.clear();
        DrPoint current_point = points.back();
        int x_start = (current_point.x > 0) ?                       current_point.x - 1 : 0;
        int x_end =   (current_point.x < from_image.width() - 1)  ? current_point.x + 1 : from_image.width()  - 1;
        int y_start = (current_point.y > 0) ?                       current_point.y - 1 : 0;
        int y_end =   (current_point.y < from_image.height() - 1) ? current_point.y + 1 : from_image.height() - 1;
        for (int x = x_start; x <= x_end; ++x) {
            for (int y = y_start; y <= y_end; ++y) {
                if (x == current_point.x && y == current_point.y) continue;
                ///if ( (x == current_point.x - 1) && (y == current_point.y - 1) ) continue;
                ///if ( (x == current_point.x + 1) && (y == current_point.y - 1) ) continue;
                ///if ( (x == current_point.x - 1) && (y == current_point.y + 1) ) continue;
                ///if ( (x == current_point.x + 1) && (y == current_point.y + 1) ) continue;
                if (processed_lines[y][x] != TRACE_PROCESSED_TWICE && processed_lines[y][x] != TRACE_NOT_BORDER) {
                    surround.push_back(DrPoint(x, y));
                }
            }
        }

        // Compare surrounding points to see which one has the greatest angle measured clockwise from the last set of points
        double   last_point_angle = Dr::CalcRotationAngleInDegrees(current_point.toPointF(), last_point.toPointF());
        double   angle_diff = 0;
        bool     first = true;
        DrPoint  next_point;
        for (auto point : surround) {
            // Find angle of point
            double check_angle = Dr::CalcRotationAngleInDegrees(current_point.toPointF(), point.toPointF());
            while (check_angle > 0) {                 check_angle -= 360.0; }
            while (check_angle <= last_point_angle) { check_angle += 360.0; }

            // Add penalty for each time pixel has been checked
            ///if (processed_lines[point.y][point.x] != TRACE_START_PIXEL)
            ///    check_angle += (processed_lines[point.y][point.x] - TRACE_NOT_PROCESSED) * 360.0;

            // See if next closest point
            double check_point_diff = check_angle - last_point_angle;
            if (first || check_point_diff < angle_diff) {
                angle_diff = check_point_diff;
                next_point = point;
                first = false;
            }
        }

        // If we found an angle, we found our next point, add it to the list
        if (surround.size() > 0) {
            if (processed_lines[current_point.y][current_point.x] == TRACE_NOT_PROCESSED)
                processed_lines[current_point.y][current_point.x] = TRACE_PROCESSED_ONCE;
            else if (processed_lines[current_point.y][current_point.x] == TRACE_PROCESSED_ONCE)
                processed_lines[current_point.y][current_point.x] = TRACE_PROCESSED_TWICE;
            last_point = current_point;
            points.push_back(next_point);
        }

        // Check if we're back at start and no more options
        back_at_start = (processed_lines[points.back().y][points.back().x] == TRACE_START_PIXEL);

    } while ((surround.size() > 0) && !back_at_start);

    // ***** Convert to DrPointF array and return
    std::vector<DrPointF> hull_points;
    for (size_t i = 0; i < points.size(); ++i) {
        hull_points.push_back(DrPointF(points[i].x, points[i].y));
    }
    return hull_points;
}


//####################################################################################
//##    Returns a mostly random list of points of possible edges of an image
//##        !!!!! #NOTE: Image passed in should be black and white
//##                     (i.e. from DrImageing::BlackAndWhiteFromAlpha())
//####################################################################################
std::vector<DrPointF> OutlinePointList(const QImage &from_image) {
    QImage image = from_image;
    std::vector<QRgb*> lines = GetScanLines(image);

    std::vector<DrPointF> points;
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
                points.push_back(DrPointF(x, y));
            } else {
                if ((x == 0 && y == 0) ||
                    (x == 0 && y == (image.height() - 1)) ||
                    (x == (image.width() - 1) && y == 0) ||
                    (x == (image.width() - 1) && y == (image.height() - 1))) {
                    points.push_back(DrPointF(x, y));
                }
            }
        }
    }
    return points;
}



}   // End Namespace Imaging

















