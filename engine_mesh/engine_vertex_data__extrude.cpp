//
//      Created by Stephens Nunnally on 9/8/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QtMath>
#include <QDateTime>
#include <QDebug>
#include <QMatrix4x4>
#include <QPainter>
#include <QPixmap>
#include <QRandomGenerator>
#include <QStandardPaths>

#include <algorithm>
#include <limits>

#include "3rd_party/delaunator.h"
#include "3rd_party/hull_finder.h"
#include "3rd_party/poly_partition.h"
#include "3rd_party/polyline_simplification.h"
#include "3rd_party_chipmunk/chipmunk.h"
#include "engine/engine_texture.h"
#include "engine_mesh/engine_vertex_data.h"
#include "helper.h"
#include "imaging/imaging.h"
#include "types/point.h"
#include "types/pointf.h"


//####################################################################################
//##    Builds an Extruded Pixmap Model
//####################################################################################
void DrEngineVertexData::initializeExtrudedPixmap(QPixmap &pixmap, bool wireframe) {
    m_data.resize(100 * c_vertex_length);

    // ***** Break pixmap into seperate images for each object in image
    QVector<QImage> images;
    QVector<QRect>  rects;
    DrImaging::findObjectsInImage(pixmap.toImage(), images, rects, 0.9);
    ///images.push_back( DrImaging::blackAndWhiteFromAlpha(pixmap.toImage(), 0.9, false));

    // ***** Go through each image (object) and add triangles for it
    for (int image_number = 0; image_number < images.count(); image_number++) {
        QImage &image = images[image_number];
        if (image.width() < 1 || image.height() < 1) continue;

        // ***** Trace edge of image
        QVector<DrPointF> points = DrImaging::traceImageOutline(image);

        // Smooth point list
        points = smoothPoints( points, 5, 5.0, 0.5 );

        // Run Polyline Simplification algorithm
        points = QVector<DrPointF>::fromStdVector( PolylineSimplification::RamerDouglasPeucker(points.toStdVector(), 0.1) );

        // Check winding
        HullFinder::EnsureWindingOrientation(points, Winding_Orientation::CounterClockwise);

        // Old Way of Simplifying Points on Similar Slopes
        ///int split = wireframe ? int((((image.width() + image.height()) / 2) * 0.2) / 5) : 1000;      // Splits longest lines of outline into 5 triangles
        ///points = simplifyPoints(points, 0.030,     5, true);             // First run with averaging points to reduce triangles among similar slopes
        ///points = simplifyPoints(points, 0.001, split, false);            // Run again with smaller tolerance to reduce triangles along straight lines

        // ***** Copy image and finds holes as seperate outlines
        QImage holes = image.copy(rects[image_number]);
        DrImaging::fillBorder(holes, c_color_white, holes.rect());          // Ensures only holes are left as black spots

        // Breaks holes into seperate images for each Hole
        QVector<QImage> hole_images;
        QVector<QRect>  hole_rects;
        DrImaging::findObjectsInImage(holes, hole_images, hole_rects, 0.9, false);

        // Go through each image (Hole) create list for it
        QVector<QVector<DrPointF>> hole_list;
        for (int hole_number = 0; hole_number < hole_images.count(); hole_number++) {
            QImage &hole = hole_images[hole_number];
            if (hole.width() < 1 || hole.height() < 1) continue;
            QVector<DrPointF> one_hole = DrImaging::traceImageOutline(hole);
            // Add in sub image offset to points
            for (auto &point : one_hole) {
                point.x += rects[image_number].x();
                point.y += rects[image_number].y();
            }
            one_hole = smoothPoints( one_hole, 5, 5.0, 0.5 );
            one_hole = QVector<DrPointF>::fromStdVector( PolylineSimplification::RamerDouglasPeucker(one_hole.toStdVector(), 0.1) );
            HullFinder::EnsureWindingOrientation(one_hole, Winding_Orientation::Clockwise);
            hole_list.push_back(one_hole);
        }


        // ***** Triangulate Concave Hull
        ///triangulateFace(points, hole_list, image, wireframe, Trianglulation::Ear_Clipping);
        ///triangulateFace(points, hole_list, image, wireframe, Trianglulation::Optimal_Polygon);
        ///triangulateFace(points, hole_list, image, wireframe, Trianglulation::Monotone);
        triangulateFace(points, hole_list, image, wireframe, Trianglulation::Delaunay);

        // ***** Add extruded triangles from Hull and Holes
        int slices = wireframe ? 2 : 1;
        extrudeFacePolygon(points, image.width(), image.height(), slices);
        for (auto hole : hole_list) {
            extrudeFacePolygon(hole, image.width(), image.height(), slices);
        }
    }

    // ***** Smooth Vertices
    ///smoothVertices(1.0f);
}


//####################################################################################
//##    #NOTE: This function superseeded by "3rd_party/polyline_simplification.h"
//##
//##    Simplifies a list of points representing a 2D outline, reducing multiple points along the same slope
//##        NOTE: If ((y2-y1) / (x2-x1)) == ((y3-y1)/(x3-x1)), then slope is same and is along same line
//##        tolerance:  how similar slop should be, bigger numbers causes less points
//##                    0.01 looks nice for most objects, 1.0 looks good for #KEYWORD: "low poly", "low-poly"
//##        test_count: how many points to test before we just go ahead and add a point
//####################################################################################
QVector<DrPointF> DrEngineVertexData::simplifyPoints(const QVector<DrPointF> &outline_points, double tolerance, int test_count, bool average) {
    QVector<DrPointF> simple_points;
    if (outline_points.count()  > 0) simple_points.push_back(outline_points[0]);            // Add first point
    if (outline_points.count() == 2) simple_points.push_back(outline_points[1]);            // Add second / last point if only two points
    if (outline_points.count()  > 2) {
        int at_point =      0;
        int next_point =    1;
        int check_point  =  2;

        // Loop through points finding lines and eliminating duplicate points among them
        while (at_point < outline_points.count() - 1) {
            bool found_next_end_point =false;
            int  count = 0;
            do {
                double x1 = outline_points[at_point].x;
                double y1 = outline_points[at_point].y;

                double x2 = outline_points[next_point].x;
                double y2 = outline_points[next_point].y;
                if (average && (next_point - at_point > 1)) {
                    int diff = next_point - at_point;
                    int index = at_point + (diff / 2);
                    x2 = outline_points[index].x;
                    y2 = outline_points[index].y;
                }

                double x3 = outline_points[check_point].x;
                double y3 = outline_points[check_point].y;
                ++count;

                // Check if slope is no longer the same, which means the line has changed direction
                double slope1 = (y2-y1) / (x2-x1);
                double slope2 = (y3-y1) / (x3-x1);
                bool   slope_is_the_same = false;
                if (Dr::FuzzyCompare(y1, y2) && Dr::FuzzyCompare(y2, y3))
                    slope_is_the_same = true;
                else if (Dr::FuzzyCompare(x1, x2) && Dr::FuzzyCompare(x2, x3))
                    slope_is_the_same = true;
                else if (Dr::IsCloseTo(slope1, slope2, tolerance))
                    slope_is_the_same = true;


                if (!slope_is_the_same || count > test_count) {
                    found_next_end_point = true;
                    at_point = next_point;
                    simple_points.push_back(outline_points[at_point]);
                }

                ++next_point;
                ++check_point;

                // Check point is at the end
                if (check_point == outline_points.count()) {
                    found_next_end_point = true;
                    at_point = check_point - 1;
                    simple_points.push_back(outline_points[at_point]);
                }

            } while (found_next_end_point == false);
        }
    }
    return simple_points;
}


//####################################################################################
//##    Smooth / Curve a collection of points representing a 2D outline
//####################################################################################
// Returns a point from a vector with wrap around coverage of vector indices
DrPointF pointAt(const QVector<DrPointF> &point_list, int index) {
    if (index < 0)
        return point_list[index + point_list.count()];
    else if (index >= point_list.count())
        return point_list[index - point_list.count()];
    else
        return point_list[index];
}

const double c_smooth_min_size = 50.0;

// Smooths points, neighbors is in each direction (so 1 is index +/- 1 more point in each direction
QVector<DrPointF> DrEngineVertexData::smoothPoints(const QVector<DrPointF> &outline_points, int neighbors, double neighbor_distance, double weight) {
    QVector<DrPointF> smooth_points;

    // Check size of polygon, accomodate smoothing for small images
    double x_min = outline_points[0].x;
    double x_max = outline_points[0].x;
    double y_min = outline_points[0].y;
    double y_max = outline_points[0].y;
    for (int i = 0; i < outline_points.count(); i++) {
        if (outline_points[i].x < x_min) x_min = outline_points[i].x;
        if (outline_points[i].x > x_max) x_max = outline_points[i].x;
        if (outline_points[i].y < y_min) y_min = outline_points[i].y;
        if (outline_points[i].y > y_max) y_max = outline_points[i].y;
    }
    double x_size = x_max - x_min;
    double y_size = y_max - y_min;
    if (x_size < c_smooth_min_size) {
        neighbor_distance /= (c_smooth_min_size / x_size);
    } else if (y_size < c_smooth_min_size) {
        neighbor_distance /= (c_smooth_min_size / y_size);
    }

    // If not enough neighbors, just return starting polygon
    if (outline_points.count() <= neighbors) {
        for (int i = 0; i < outline_points.count(); ++i)
            smooth_points.push_back(outline_points[i]);
        return smooth_points;
    }

    // Go through and smooth the points (simple average)
    const double c_sharp_angle = 120.0;

    for (int i = 0; i < outline_points.count(); i++) {
        // Current Point
        DrPointF this_point = outline_points[i];
        double total_used = 1.0;
        double x = this_point.x;
        double y = this_point.y;
        double angle_reduction = 1.0;

        // Check if current point is a sharp angle, if so add to list and continue
        DrPointF start_check_point =   pointAt(outline_points, i);
        double   start_check_angle_1 = Dr::CalcRotationAngleInDegrees(start_check_point, pointAt(outline_points, i - 1));
        double   start_check_angle_2 = Dr::CalcRotationAngleInDegrees(start_check_point, pointAt(outline_points, i + 1));
        double   start_diff =          Dr::DifferenceBetween2Angles(start_check_angle_1, start_check_angle_2);
        if (start_diff <= c_sharp_angle) {
            smooth_points.push_back( this_point );
            continue;
        }

        // Check neighbors in both directions for sharp angles, don't include these neighbors for averaging,
        // This allows us to keep sharper corners on square objects
        int average_from = i - neighbors;
        int average_to =   i + neighbors;
        for (int j = i - 1; j >= i - neighbors; j--) {
            DrPointF check_point = pointAt(outline_points, j);
            double check_angle_1 = Dr::CalcRotationAngleInDegrees(check_point, pointAt(outline_points, j - 1));
            double check_angle_2 = Dr::CalcRotationAngleInDegrees(check_point, pointAt(outline_points, j + 1));
            double diff =          Dr::DifferenceBetween2Angles(check_angle_1, check_angle_2);
            if (diff <= c_sharp_angle) { average_from = j + 0 /*1*/; break; }
        }
        for (int j = i + 1; j <= i + neighbors; j++) {
            DrPointF check_point = pointAt(outline_points, j);
            double check_angle_1 = Dr::CalcRotationAngleInDegrees(check_point, pointAt(outline_points, j - 1));
            double check_angle_2 = Dr::CalcRotationAngleInDegrees(check_point, pointAt(outline_points, j + 1));
            double diff =          Dr::DifferenceBetween2Angles(check_angle_1, check_angle_2);
            if (diff <= c_sharp_angle) { average_to =   j - 0 /*1*/; break; }
        }

        // Smooth point
        for (int j = average_from; j <= average_to; j++) {
            // Skip point we're on from adding into average, already added it
            if (j != i) {
                DrPointF check_point = pointAt(outline_points, j);
                if (QLineF(this_point.x, this_point.y, check_point.x, check_point.y).length() < neighbor_distance) {
                    x += (check_point.x * weight * angle_reduction);
                    y += (check_point.y * weight * angle_reduction);
                    total_used +=        (weight * angle_reduction);
                }
            }
        }

        // Add to array
        smooth_points.push_back(DrPointF(x / total_used, y / total_used));
    }
    return smooth_points;
}


//####################################################################################
//##    Triangulate Face and add Triangles to Vertex Data
//####################################################################################
void DrEngineVertexData::triangulateFace(const QVector<DrPointF> &outline_points, const QVector<QVector<DrPointF>> &hole_list,
                                         const QImage &black_and_white, bool wireframe, Trianglulation type) {
    int width =  black_and_white.width();
    int height = black_and_white.height();
    double w2d = width  / 2.0;
    double h2d = height / 2.0;

    // ***** Copy DrPointFs into TPPLPoly
    if (outline_points.count() < 3) return;
    std::list<TPPLPoly> testpolys, result;
    TPPLPoly poly; poly.Init(outline_points.count());
    for (int i = 0; i < outline_points.count(); i++) {
        poly[i].x = outline_points[i].x;
        poly[i].y = outline_points[i].y;
    }
    testpolys.push_back( poly );

    // ***** Run triangulation
    TPPLPartition pp;
    switch (type) {
        case Trianglulation::Ear_Clipping:      pp.Triangulate_EC(  &testpolys, &result);       break;
        case Trianglulation::Monotone:          pp.Triangulate_MONO(&testpolys, &result);       break;
        case Trianglulation::Delaunay: {
            result.push_back( poly );
            ///pp.ConvexPartition_OPT(&(*testpolys.begin()), &result);
            ///pp.ConvexPartition_HM(&(*testpolys.begin()), &result);
            break;
        }
    }

    // ***** Add triangulated convex hull to vertex data
    if (type != Trianglulation::Delaunay) {
        for (auto poly : result) {
            GLfloat x1 = static_cast<GLfloat>(         poly[0].x - w2d);
            GLfloat y1 = static_cast<GLfloat>(height - poly[0].y - h2d);
            GLfloat x2 = static_cast<GLfloat>(         poly[1].x - w2d);
            GLfloat y2 = static_cast<GLfloat>(height - poly[1].y - h2d);
            GLfloat x3 = static_cast<GLfloat>(         poly[2].x - w2d);
            GLfloat y3 = static_cast<GLfloat>(height - poly[2].y - h2d);

            GLfloat tx1 = static_cast<GLfloat>(      poly[0].x / width);
            GLfloat ty1 = static_cast<GLfloat>(1.0 - poly[0].y / height);
            GLfloat tx2 = static_cast<GLfloat>(      poly[1].x / width);
            GLfloat ty2 = static_cast<GLfloat>(1.0 - poly[1].y / height);
            GLfloat tx3 = static_cast<GLfloat>(      poly[2].x / width);
            GLfloat ty3 = static_cast<GLfloat>(1.0 - poly[2].y / height);

            triangle( x1, y1, tx1, ty1,
                      x3, y3, tx3, ty3,
                      x2, y2, tx2, ty2);
        }


    // ***** After splitting concave hull into convex polygons, add Delaunay Triangulation to vertex data
    } else {

        // Copy Outline Points into coordinate list
        std::vector<double> coords;
        for (auto poly : result) {
            for (int i = 0; i < poly.GetNumPoints(); i++) {
                coords.push_back(poly[i].x);
                coords.push_back(poly[i].y);
            }
        }

        // Copy Hole Points coordinate list
        for (auto hole : hole_list) {
            for (auto point : hole) {
                coords.push_back(point.x);
                coords.push_back(point.y);
            }
        }

        // Add some uniform points, 4 points looks great and keeps triangles low
        if (wireframe) {
            int x_add, y_add;
            x_add = width /  4;
            y_add = height / 4;
            if (x_add < 1) x_add = 1;
            if (y_add < 1) y_add = 1;
            for (int i = (x_add / 2); i < width; i += x_add) {
                for (int j = (y_add / 2); j < height; j += y_add) {
                    if (black_and_white.pixel(i, j) != c_color_black) {
                        coords.push_back( i );
                        coords.push_back( j );
                    }
                }
            }
        }

        // Check list for duplicates before running triangulation (run with STEP of 2 as coords is stored in x, y pairs)
        std::vector<double> no_duplicates;
        for (std::size_t i = 0; i < coords.size(); i += 2) {
            bool has_it = false;
            for (std::size_t j = i + 2; j < coords.size(); j += 2) {
                if (Dr::FuzzyCompare(coords[i], coords[j]) && Dr::FuzzyCompare(coords[i+1], coords[j+1])) {
                    has_it = true;
                    break;
                }
            }
            if (!has_it) {
                no_duplicates.push_back(coords[i]);
                no_duplicates.push_back(coords[i+1]);
            }
        }
        if (no_duplicates.size() < 6) return;                                           // We need at least 3 points!!


        // Run triangulation, add triangles to vertex data
        Delaunator::Delaunator d(no_duplicates);

        // Delaunay Trianglulation returns a collection of triangles filling a convex hull of a collection of points.
        // So no we have to go through the triangles returned and remove any that are over transparent areas of our object.
        for (std::size_t i = 0; i < d.triangles.size(); i += 3) {
            double x1 = d.coords[2 * d.triangles[i + 0]];
            double y1 = d.coords[2 * d.triangles[i + 0] + 1];
            double x2 = d.coords[2 * d.triangles[i + 1]];
            double y2 = d.coords[2 * d.triangles[i + 1] + 1];
            double x3 = d.coords[2 * d.triangles[i + 2]];
            double y3 = d.coords[2 * d.triangles[i + 2] + 1];

            // Check each triangle to see if mid-points lie outside concave hull by comparing object image
            DrPoint mid12( (x1 + x2) / 2.0, (y1 + y2) / 2.0 );
            DrPoint mid23( (x2 + x3) / 2.0, (y2 + y3) / 2.0 );
            DrPoint mid13( (x1 + x3) / 2.0, (y1 + y3) / 2.0 );
            int transparent_count = 0;
            if (black_and_white.pixel(mid12.x, mid12.y) == c_color_black) ++transparent_count;
            if (black_and_white.pixel(mid23.x, mid23.y) == c_color_black) ++transparent_count;
            if (black_and_white.pixel(mid13.x, mid13.y) == c_color_black) ++transparent_count;
            if (transparent_count > 1) continue;

            // Add triangle
            GLfloat ix1 = static_cast<GLfloat>(         x1 - w2d);
            GLfloat iy1 = static_cast<GLfloat>(height - y1 - h2d);
            GLfloat ix2 = static_cast<GLfloat>(         x2 - w2d);
            GLfloat iy2 = static_cast<GLfloat>(height - y2 - h2d);
            GLfloat ix3 = static_cast<GLfloat>(         x3 - w2d);
            GLfloat iy3 = static_cast<GLfloat>(height - y3 - h2d);

            GLfloat tx1 = static_cast<GLfloat>(      x1 / width);
            GLfloat ty1 = static_cast<GLfloat>(1.0 - y1 / height);
            GLfloat tx2 = static_cast<GLfloat>(      x2 / width);
            GLfloat ty2 = static_cast<GLfloat>(1.0 - y2 / height);
            GLfloat tx3 = static_cast<GLfloat>(      x3 / width);
            GLfloat ty3 = static_cast<GLfloat>(1.0 - y3 / height);

            triangle( ix1, iy1, tx1, ty1,
                      ix2, iy2, tx2, ty2,
                      ix3, iy3, tx3, ty3);
        }   // end for int i

    }   // end if

}


//####################################################################################
//##    Add Extrusion Triangles to Vertex Data
//####################################################################################
void DrEngineVertexData::extrudeFacePolygon(const QVector<DrPointF> &outline_points, int width, int height, int steps) {
    double w2d = width  / 2.0;
    double h2d = height / 2.0;

    for (int i = 0; i < outline_points.count(); i++) {
        int point1 = i + 1;
        int point2 = i;
        if (point1 >= outline_points.count()) point1 = 0;

        GLfloat  x1 = static_cast<GLfloat>(         outline_points[point1].x);
        GLfloat  y1 = static_cast<GLfloat>(height - outline_points[point1].y);
        GLfloat tx1 = static_cast<GLfloat>(         outline_points[point1].x / width);
        GLfloat ty1 = static_cast<GLfloat>(1.0 -    outline_points[point1].y / height);

        GLfloat  x2 = static_cast<GLfloat>(         outline_points[point2].x);
        GLfloat  y2 = static_cast<GLfloat>(height - outline_points[point2].y);
        GLfloat tx2 = static_cast<GLfloat>(         outline_points[point2].x / width);
        GLfloat ty2 = static_cast<GLfloat>(1.0 -    outline_points[point2].y / height);

        x1 -= static_cast<GLfloat>(w2d);
        x2 -= static_cast<GLfloat>(w2d);
        y1 -= static_cast<GLfloat>(h2d);
        y2 -= static_cast<GLfloat>(h2d);

        float pixel_w = (1.0f / width);
        float pixel_h = (1.0f / height);
        if (tx1 > 0.5f) x1 -= pixel_w; else x1 += pixel_w;
        if (tx2 > 0.5f) x2 -= pixel_w; else x2 += pixel_w;
        if (ty1 > 0.5f) y1 -= pixel_h; else y1 += pixel_h;
        if (ty2 > 0.5f) y2 -= pixel_h; else y2 += pixel_h;

        extrude( x1, y1, tx1, ty1,
                 x2, y2, tx2, ty2, steps);
    }
}











