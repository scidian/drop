//
//      Created by Stephens Nunnally on 11/23/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "3rd_party/hull_finder.h"
#include "3rd_party/polyline_simplification.h"
#include "editor/imaging/imaging.h"
#include "project/project.h"
#include "project/project_animation.h"
#include "project/project_asset.h"
#include "properties/property_collision.h"


//####################################################################################
//##    Determines automatic collision shapes based on image
//####################################################################################
DrPropertyCollision autoCollisionShape(QPixmap pixmap) {
    DrPropertyCollision shapes;

    // ***** Break pixmap into seperate images for each object in image
    QVector<QImage> images;
    QVector<QRect>  rects;
    DrImaging::findObjectsInImage(pixmap.toImage(), images, rects, 0.9);

    // ***** Creates a box polygon for use with complicated / not complicated enough images
    QVector<DrPointF> box;
    box.clear();
    box.push_back( DrPointF(0,                  0) );
    box.push_back( DrPointF(pixmap.width() - 1, 0) );
    box.push_back( DrPointF(pixmap.width() - 1, pixmap.height() - 1) );
    box.push_back( DrPointF(0,                  pixmap.height() - 1) );

    // ***** Go through each image (object) and Polygon for it
    if (images.count() < 50) {
        for (int image_number = 0; image_number < images.count(); image_number++) {
            QImage &image = images[image_number];
            if (image.width() < 1 || image.height() < 1) continue;

            // Trace edge of image
            QVector<DrPointF> points = DrImaging::traceImageOutline(image);
            double plus_one_pixel_percent_x = 1.0 + (1.50 / image.width());
            double plus_one_pixel_percent_y = 1.0 + (1.50 / image.height());
            for (auto &point : points) {
                point.x = point.x * plus_one_pixel_percent_x;
                point.y = point.y * plus_one_pixel_percent_y;
            }

            // Run Polyline Simplification algorithm
            QVector<DrPointF> simple_points;
            simple_points = QVector<DrPointF>::fromStdVector( PolylineSimplification::RamerDouglasPeucker(points.toStdVector(), 2.0) );

            // If we only have a couple points left, add shape as a box of the original image, otherwise use PolylineSimplification points
            if ((simple_points.count() < 4)) {
                ///points = HullFinder::FindConcaveHull(points, 5.0);
                points.clear();
                points.push_back( DrPointF(rects[image_number].topLeft().x(),        rects[image_number].topLeft().y()) );
                points.push_back( DrPointF(rects[image_number].topRight().x(),       rects[image_number].topRight().y()) );
                points.push_back( DrPointF(rects[image_number].bottomRight().x(),    rects[image_number].bottomRight().y()) );
                points.push_back( DrPointF(rects[image_number].bottomLeft().x(),     rects[image_number].bottomLeft().y()) );
                points.push_back( DrPointF(rects[image_number].topLeft().x(),        rects[image_number].topLeft().y()) );
            } else {
                points = simple_points;
            }

            // Check we still have 3 points, remove duplicate first point
            if (points.count() < 4) continue;
            points.pop_back();

            // Check winding
            HullFinder::EnsureWindingOrientation(points, Winding_Orientation::CounterClockwise);

            // Add polygon to list of polygons in shape
            shapes.addPolygon( points.toStdVector() );
        }
    }

    // If we don't have polygons by this point, just add a simple box
    if (shapes.getPolygons().size() < 1) {
        shapes.addPolygon( box.toStdVector() );
    }

    // Adjust points in Boxes / Polygons so that (0, 0) is the center of the image
    for (auto &shape : shapes.getPolygons()) {
        for (auto &point : shape) {
            point.x = point.x - (pixmap.width() / 2.0);
            point.y = (pixmap.height() - point.y) - (pixmap.height() / 2.0);
        }
    }

    return shapes;
}















