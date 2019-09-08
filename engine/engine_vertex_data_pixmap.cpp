//
//      Created by Stephens Nunnally on 9/8/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QtMath>
#include <QDebug>
#include <QPainter>
#include <QPixmap>
#include <QMatrix4x4>

#include "engine_texture.h"
#include "engine_vertex_data.h"
#include "helper.h"
#include "imaging/imaging.h"



//####################################################################################
//##    Simplifies a list of points, reducing multiple points along the same slope
//##        NOTE: If ((y2-y1) / (x2-x1)) == ((y3-y1)/(x3-x1)), then slope is same and is along same line
//##        tolerance:  how similar slop should be, bigger numbers causes less points
//##        test_count: how many points to test before we just go ahead and add a point
//####################################################################################
QVector<HullPoint> DrEngineVertexData::simplifyPoints(const QVector<HullPoint> &from_points, double tolerance, int test_count) {
    QVector<HullPoint> simple_points;
    if (from_points.count()  > 0) simple_points.push_back(from_points[0]);        // Add first point
    if (from_points.count() == 2) simple_points.push_back(from_points[1]);        // Add second / last point if only two points
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
                double x3 = from_points[check_point].x;
                double y3 = from_points[check_point].y;
                ++count;

                // Check if slope is no longer the same, which means the line has changed direction
                bool slope_is_the_same = Dr::IsCloseTo(((y2-y1) / (x2-x1)), ((y3-y1) / (x3-x1)), tolerance);
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

// Smooths points, neigbors is in each direction (so 1 is index +/- 1 more point in each direction
QVector<HullPoint> DrEngineVertexData::smoothPoints(const QVector<HullPoint> &from_points, int neighbors, double neighbor_distance, double weight) {
    QVector<HullPoint> smooth_points;

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
//##    Builds an Extruded Pixmap
//####################################################################################
void DrEngineVertexData::initializeExtrudedPixmap(QPixmap &pixmap) {
    m_data.resize(100 * c_vertex_length);

    // ***** Get list of possible outline points
    QVector<HullPoint> image_points =           DrImaging::outlinePointList(pixmap.toImage(), 0.9);

    // ***** Find concave hull for extrusion points
    QVector<HullPoint> concave_hull_face =      HullFinder::FindConcaveHull(image_points, 3.5);
    QVector<HullPoint> concave_hull_extrude =   concave_hull_face;  //HullFinder::FindConcaveHull(image_points, 0.9);

    // ***** Simplify point list
    QVector<HullPoint> simple_points_face =     simplifyPoints(concave_hull_face,   0.000001, 10);
    QVector<HullPoint> simple_points_extrude =  simple_points_face; //simplifyPoints(concave_hull_extrude, 0.000001, 5);

    // ***** Smooths / curves points in the list
    QVector<HullPoint> smooth_points_face =     smoothPoints(simple_points_face,    5, 5.0, 0.5);
    QVector<HullPoint> smooth_points_extrude =  smooth_points_face; //smoothPoints(simple_points_extrude, 5, 5.0, 0.5);


    // ***** Triangulate concave hull
    // Copy HullPoints into TPPLPoly
    std::list<TPPLPoly> testpolys, result;
    TPPLPoly poly; poly.Init(smooth_points_face.count());
    for (int i = 0; i < smooth_points_face.count(); i++) {
        poly[i].x = smooth_points_face[i].x;
        poly[i].y = smooth_points_face[i].y;
    }
    testpolys.push_back( poly );

    // Run triangulation, add triangles to vertex data
    TPPLPartition pp;
    pp.Triangulate_EC( &testpolys, &result );
    ///pp.Triangulate_OPT(  &testpolys, &result);
    ///pp.Triangulate_MONO( &testpolys, &result);
    int width =  pixmap.width();
    int height = pixmap.height();
    double w2d = width  / 2.0;
    double h2d = height / 2.0;
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


    // ***** Add extruded triangles
    for (int i = 0; i < smooth_points_extrude.count(); i++) {
        int point1, point2;
        if (i == smooth_points_extrude.count() - 1) {
            point1 = 0;         point2 = i;
        } else {
            point1 = i + 1;     point2 = i;
        }

        GLfloat  x1 = static_cast<GLfloat>(         smooth_points_extrude[point1].x);
        GLfloat  y1 = static_cast<GLfloat>(height - smooth_points_extrude[point1].y);
        GLfloat tx1 = static_cast<GLfloat>(      smooth_points_extrude[point1].x / width);
        GLfloat ty1 = static_cast<GLfloat>(1.0 - smooth_points_extrude[point1].y / height);

        GLfloat  x2 = static_cast<GLfloat>(         smooth_points_extrude[point2].x);
        GLfloat  y2 = static_cast<GLfloat>(height - smooth_points_extrude[point2].y);
        GLfloat tx2 = static_cast<GLfloat>(      smooth_points_extrude[point2].x / width);
        GLfloat ty2 = static_cast<GLfloat>(1.0 - smooth_points_extrude[point2].y / height);

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
                 x2, y2, tx2, ty2);
    }
}









