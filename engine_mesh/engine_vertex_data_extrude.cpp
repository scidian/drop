//
//      Created by Stephens Nunnally on 9/8/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QtMath>
#include <QDebug>
#include <QMatrix4x4>
#include <QPainter>
#include <QPixmap>
#include <QRandomGenerator>
#include <algorithm>

#include "3rd_party/delaunator.h"
#include "3rd_party_chipmunk/chipmunk.h"
#include "engine/engine_texture.h"
#include "engine_mesh/engine_vertex_data.h"
#include "helper.h"
#include "imaging/imaging.h"


//####################################################################################
//##    Builds an Extruded Pixmap Model
//####################################################################################
void DrEngineVertexData::initializeExtrudedPixmap(QPixmap &pixmap) {
    m_data.resize(100 * c_vertex_length);

    // Break pixmap into seperate images for each object in image
    QVector<QImage> images;
    images = DrImaging::findObjectsInImage(pixmap, 0.9);
    ///images.push_back( DrImaging::blackAndWhiteFromAlpha(pixmap.toImage(), 0.9, false));

    for (auto image : images) {
        if (image.width() < 1 || image.height() < 1) continue;

        // ***** Get a list of possible outline points, find concave hull
        ///QVector<HullPoint> image_points =   DrImaging::outlinePointList(image);
        ///if (image_points.count() < 1) continue;
        ///QVector<HullPoint> concave_hull =   HullFinder::FindConcaveHull(image_points, 3.5);

        // ***** Trace edge of image
        QVector<HullPoint> points =     DrImaging::traceImageOutline(image);
        Winding_Orientation winding =   HullFinder::FindWindingOrientation(points);
        switch (winding) {
            case Winding_Orientation::Unknown:          continue;
            case Winding_Orientation::Clockwise:        std::reverse(points.begin(), points.end()); break;
            case Winding_Orientation::CounterClockwise: break;
        }

        // ***** Smooth, then Simplify point list, a value of 1.0 looks nice for #KEYWORD: "low poly"
        points =  smoothPoints(  points, 4, 4.0, 0.4);

        points =  simplifyPoints(points, 0.030,    5, true);    // First run with averaging points to reduce triangles among similar slopes
        points =  simplifyPoints(points, 0.001,   20, false);   // Then run again with smaller tolerance to reduce triangles along straight lines
        // or
        ///points =  simplifyPoints(points, 0.030,   10, true);    // Test

        // ***** Triangulate concave hull
        ///triangulateFace(points, image, Trianglulation::Ear_Clipping);
        ///triangulateFace(points, image, Trianglulation::Optimal_Polygon);
        ///triangulateFace(points, image, Trianglulation::Monotone);
        triangulateFace(points, image, Trianglulation::Delaunay);

        // ***** Add extruded triangles
        extrudeFacePolygon(points, image.width(), image.height(), 2);
    }
}


//####################################################################################
//##    Simplifies a list of points, reducing multiple points along the same slope
//##        NOTE: If ((y2-y1) / (x2-x1)) == ((y3-y1)/(x3-x1)), then slope is same and is along same line
//##        tolerance:  how similar slop should be, bigger numbers causes less points
//##                    0.01 looks nice for most objects, 1.0 looks good for #KEYWORD: "low poly", "low-poly"
//##        test_count: how many points to test before we just go ahead and add a point
//####################################################################################
QVector<HullPoint> DrEngineVertexData::simplifyPoints(const QVector<HullPoint> &from_points, double tolerance, int test_count, bool average) {
    QVector<HullPoint> simple_points;
    if (from_points.count()  > 0) simple_points.push_back(from_points[0]);          // Add first point
    if (from_points.count() == 2) simple_points.push_back(from_points[1]);          // Add second / last point if only two points
    if (from_points.count()  > 2) {
        int at_point =      0;
        int next_point =    1;
        int check_point  =  2;

        // Loop through points finding lines and eliminating duplicate points among them
        while (at_point < from_points.count() - 1) {
            bool found_next_end_point =false;
            int  count = 0;
            do {
                double x1 = from_points[at_point].x;
                double y1 = from_points[at_point].y;

                double x2 = from_points[next_point].x;
                double y2 = from_points[next_point].y;
                if (average && (next_point - at_point > 1)) {
                    int diff = next_point - at_point;
                    int index = at_point + (diff / 2);
                    x2 = from_points[index].x;
                    y2 = from_points[index].y;
                }

                double x3 = from_points[check_point].x;
                double y3 = from_points[check_point].y;
                ++count;

                // Check if slope is no longer the same, which means the line has changed direction
                double slope1 = (y2-y1) / (x2-x1);
                double slope2 = (y3-y1) / (x3-x1);
                bool   slope_is_the_same = false;
                if (qFuzzyCompare(y1, y2) && qFuzzyCompare(y2, y3))
                    slope_is_the_same = true;
                else if (qFuzzyCompare(x1, x2) && qFuzzyCompare(x2, x3))
                    slope_is_the_same = true;
                else if (Dr::IsCloseTo(slope1, slope2, tolerance))
                    slope_is_the_same = true;


                if (!slope_is_the_same || count > test_count) {
                    found_next_end_point = true;
                    at_point = next_point;
                    simple_points.push_back(from_points[at_point]);
                }

                ++next_point;
                ++check_point;

                // Check point is at the end
                if (check_point == from_points.count()) {
                    found_next_end_point = true;
                    at_point = check_point - 1;
                    simple_points.push_back(from_points[at_point]);
                }

            } while (found_next_end_point == false);
        }
    }
    return simple_points;
}


//####################################################################################
//##    Smooth / Curve a collection of points
//####################################################################################
// Returns a point from a vector with wrap around coverage of vector indices
HullPoint pointAt(const QVector<HullPoint> &point_list, int index) {
    if (index < 0)
        return point_list[index + point_list.count()];
    else if (index >= point_list.count())
        return point_list[index - point_list.count()];
    else
        return point_list[index];
}

const double c_smooth_min_size = 50.0;

// Smooths points, neigbors is in each direction (so 1 is index +/- 1 more point in each direction
QVector<HullPoint> DrEngineVertexData::smoothPoints(const QVector<HullPoint> &from_points, int neighbors, double neighbor_distance, double weight) {
    QVector<HullPoint> smooth_points;

    // Check size of polygon, accomodate smoothing for small images
    double x_min = from_points[0].x;
    double x_max = from_points[0].x;
    double y_min = from_points[0].y;
    double y_max = from_points[0].y;
    for (int i = 0; i < from_points.count(); i++) {
        if (from_points[i].x < x_min) x_min = from_points[i].x;
        if (from_points[i].x > x_max) x_max = from_points[i].x;
        if (from_points[i].y < y_min) y_min = from_points[i].y;
        if (from_points[i].y > y_max) y_max = from_points[i].y;
    }
    double x_size = x_max - x_min;
    double y_size = y_max - y_min;
    if (x_size < c_smooth_min_size) {
        neighbor_distance /= (c_smooth_min_size / x_size);
    } else if (y_size < c_smooth_min_size) {
        neighbor_distance /= (c_smooth_min_size / y_size);
    }

    // If not enough neighbors, just return starting polygon
    if (from_points.count() <= neighbors) {
        for (int i = 0; i < from_points.count(); ++i) smooth_points.push_back(from_points[i]);
        return smooth_points;
    }

    // Go through and smooth the points (simple average)
    for (int i = 0; i < from_points.count(); i++) {
        HullPoint this_point = from_points[i];
        double total_used = 0;
        double x = 0, y = 0;
        for (int j = i - neighbors; j <= i + neighbors; j++) {
            HullPoint check_point = pointAt(from_points, j);
            if (j == i) {
                x += check_point.x;
                y += check_point.y;
                ++total_used;

            } else if (QLineF(this_point.x, this_point.y, check_point.x, check_point.y).length() < neighbor_distance) {
                x += (check_point.x * weight);
                y += (check_point.y * weight);
                total_used += weight;
            }
        }
        smooth_points.push_back(HullPoint(x / total_used, y / total_used));
    }
    return smooth_points;
}


//####################################################################################
//##    Triangulate Face and add Triangles to Vertex Data
//####################################################################################
void DrEngineVertexData::triangulateFace(const QVector<HullPoint> &from_points, QImage &black_and_white, Trianglulation type) {
    int width =  black_and_white.width();
    int height = black_and_white.height();
    double w2d = width  / 2.0;
    double h2d = height / 2.0;

    // Copy HullPoints into TPPLPoly
    std::list<TPPLPoly> testpolys, result;
    TPPLPoly poly; poly.Init(from_points.count());
    for (int i = 0; i < from_points.count(); i++) {
        poly[i].x = from_points[i].x;
        poly[i].y = from_points[i].y;
    }
    testpolys.push_back( poly );

    // Run triangulation
    TPPLPartition pp;
    switch (type) {
        case Trianglulation::Ear_Clipping:      pp.Triangulate_EC(&(*testpolys.begin()), &result);          break;
        case Trianglulation::Optimal_Polygon:   pp.Triangulate_OPT(&(*testpolys.begin()), &result);         break;
        case Trianglulation::Monotone:          pp.Triangulate_MONO(&(*testpolys.begin()), &result);        break;
        case Trianglulation::Delaunay: {
            result.push_back( poly );
            ///pp.ConvexPartition_OPT(&(*testpolys.begin()), &result);
            ///pp.ConvexPartition_HM(&(*testpolys.begin()), &result);
            break;
        }
    }

    // Add triangulated convex hull to vertex data
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


    // After splitting concave hull into convex polygons, add Delaunay Triangulation to vertex data
    } else {

        for (auto poly : result) {
            if (poly.GetNumPoints() < 3) continue;

            // Copy HullPoints into vector
            std::vector<double> coords;
            for (int i = 0; i < poly.GetNumPoints(); i++) {
                coords.push_back(poly[i].x);
                coords.push_back(poly[i].y);
            }

            // Add some uniform points, 4 points looks great and keeps triangles low
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

            // Add some points just around the edges of the hull
            /**
            for (int i = 0; i < poly.GetNumPoints(); i++) {
                double x_start = poly[i].x - 2;
                double x_end =   poly[i].x + 2;
                double y_start = poly[i].y - 2;
                double y_end =   poly[i].y + 2;
                for (double x = x_start; x <= x_end; x += 1) {
                    for (double y = y_start; y <= y_end; y += 1) {
                        int at_x = static_cast<int>(x);
                        int at_y = static_cast<int>(y);
                        if (at_x == int(poly[i].x) || at_y == int(poly[i].y)) continue;
                        at_x = Dr::Clamp(at_x, 0, width -  1);
                        at_y = Dr::Clamp(at_y, 0, height - 1);
                        if (black_and_white.pixel(at_x, at_y) != c_color_black) {
                            coords.push_back( at_x );
                            coords.push_back( at_y );
                        }
                    }
                }
            }
            */

            // Run triangulation, add triangles to vertex data
            Delaunator::Delaunator d(coords);

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
                IntPoint mid12( (x1 + x2) / 2.0, (y1 + y2) / 2.0 );
                IntPoint mid23( (x2 + x3) / 2.0, (y2 + y3) / 2.0 );
                IntPoint mid13( (x1 + x3) / 2.0, (y1 + y3) / 2.0 );
                int transparent_count = 0;
                if (black_and_white.pixel(mid12.x, mid12.y) == c_color_black) ++transparent_count;
                if (black_and_white.pixel(mid23.x, mid23.y) == c_color_black) ++transparent_count;
                if (black_and_white.pixel(mid13.x, mid13.y) == c_color_black) ++transparent_count;
                if (transparent_count > 1) continue;

                // Check each triangle to see if mid-points lie outside concave hull by checking concave hull intersections
                /**
                HullPoint mid12( (x1 + x2) / 2.0, (y1 + y2) / 2.0 );
                HullPoint mid13( (x1 + x3) / 2.0, (y1 + y3) / 2.0 );
                HullPoint mid23( (x2 + x3) / 2.0, (y2 + y3) / 2.0 );
                QLineF line_12(mid12.x, mid12.y, 100000.0, mid12.y);
                QLineF line_13(mid13.x, mid13.y, 100000.0, mid13.y);
                QLineF line_23(mid23.x, mid23.y, 100000.0, mid23.y);
                int intersect_count_12 = 0;
                int intersect_count_13 = 0;
                int intersect_count_23 = 0;
                for (int i = 0; i < (from_points.count() - 1); ++i) {
                    QLineF  check_line = QLineF(from_points[i].x, from_points[i].y, from_points[i+1].x, from_points[i+1].y);
                    QPointF intersection_point;
                    if (check_line.intersect(line_12, &intersection_point) == QLineF::IntersectType::BoundedIntersection) ++intersect_count_12;
                    if (check_line.intersect(line_13, &intersection_point) == QLineF::IntersectType::BoundedIntersection) ++intersect_count_13;
                    if (check_line.intersect(line_23, &intersection_point) == QLineF::IntersectType::BoundedIntersection) ++intersect_count_23;
                }
                int outside_count = 0;
                if (intersect_count_12 % 2 == 0) ++outside_count;
                if (intersect_count_13 % 2 == 0) ++outside_count;
                if (intersect_count_23 % 2 == 0) ++outside_count;
                if (outside_count > 1) continue;
                */

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
        }   // end for auto poly
    }   // end if

}


//####################################################################################
//##    Add Extrusion Triangles to Vertex Data
//####################################################################################
void DrEngineVertexData::extrudeFacePolygon(const QVector<HullPoint> &from_points, int width, int height, int steps) {
    double w2d = width  / 2.0;
    double h2d = height / 2.0;

    for (int i = 0; i < from_points.count(); i++) {
        int point1, point2;
        if (i == from_points.count() - 1) {
            point1 = 0;         point2 = i;
        } else {
            point1 = i + 1;     point2 = i;
        }

        GLfloat  x1 = static_cast<GLfloat>(         from_points[point1].x);
        GLfloat  y1 = static_cast<GLfloat>(height - from_points[point1].y);
        GLfloat tx1 = static_cast<GLfloat>(         from_points[point1].x / width);
        GLfloat ty1 = static_cast<GLfloat>(1.0 -    from_points[point1].y / height);

        GLfloat  x2 = static_cast<GLfloat>(         from_points[point2].x);
        GLfloat  y2 = static_cast<GLfloat>(height - from_points[point2].y);
        GLfloat tx2 = static_cast<GLfloat>(         from_points[point2].x / width);
        GLfloat ty2 = static_cast<GLfloat>(1.0 -    from_points[point2].y / height);

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











