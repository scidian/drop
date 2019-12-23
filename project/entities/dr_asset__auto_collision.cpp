//
//      Created by Stephens Nunnally on 11/23/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "3rd_party/hull_finder.h"
#include "3rd_party/polyline_simplification.h"
#include "core/imaging/imaging.h"
#include "project/properties/property_collision.h"
#include "project/dr_project.h"
#include "project/entities/dr_animation.h"
#include "project/entities/dr_asset.h"


//####################################################################################
//##    Determines automatic collision shapes based on image
//####################################################################################
DrPropertyCollision autoCollisionShape(const DrBitmap &bitmap) {
    DrPropertyCollision shapes;

    // ***** Break pixmap into seperate images for each object in image
    std::vector<DrBitmap> bitmaps;
    std::vector<DrRect>   rects;
    Dr::FindObjectsInBitmap(bitmap, bitmaps, rects, 0.9);

    // ***** Creates a box polygon for use with complicated / not complicated enough images
    std::vector<DrPointF> box;
    box.clear();
    box.push_back( DrPointF(0,                0) );
    box.push_back( DrPointF(bitmap.width - 1, 0) );
    box.push_back( DrPointF(bitmap.width - 1, bitmap.height - 1) );
    box.push_back( DrPointF(0,                bitmap.height - 1) );

    // ***** Go through each image (object) and Polygon for it
    if (bitmaps.size() < 50) {
        for (int image_number = 0; image_number < static_cast<int>(bitmaps.size()); image_number++) {
            DrBitmap &image = bitmaps[image_number];
            if (image.width < 1 || image.height < 1) continue;

            // Trace edge of image
            std::vector<DrPointF> points = Dr::TraceImageOutline(image);
            double plus_one_pixel_percent_x = 1.0 + (1.50 / image.width);
            double plus_one_pixel_percent_y = 1.0 + (1.50 / image.height);
            for (auto &point : points) {
                point.x = point.x * plus_one_pixel_percent_x;
                point.y = point.y * plus_one_pixel_percent_y;
            }

            // Run Polyline Simplification algorithm
            std::vector<DrPointF> simple_points;
            simple_points = PolylineSimplification::RamerDouglasPeucker(points, 2.0);

            // If we only have a couple points left, add shape as a box of the original image, otherwise use PolylineSimplification points
            if ((simple_points.size() < 4)) {
                ///points = HullFinder::FindConcaveHull(points, 5.0);
                points.clear();
                points.push_back( DrPointF(rects[image_number].topLeft().x,        rects[image_number].topLeft().y) );
                points.push_back( DrPointF(rects[image_number].topRight().x,       rects[image_number].topRight().y) );
                points.push_back( DrPointF(rects[image_number].bottomRight().x,    rects[image_number].bottomRight().y) );
                points.push_back( DrPointF(rects[image_number].bottomLeft().x,     rects[image_number].bottomLeft().y) );
                points.push_back( DrPointF(rects[image_number].topLeft().x,        rects[image_number].topLeft().y) );
            } else {
                points = simple_points;
            }

            // Check we still have 3 points, remove duplicate first point
            if (points.size() < 4) continue;
            points.pop_back();

            // Check winding
            HullFinder::EnsureWindingOrientation(points, Winding_Orientation::CounterClockwise);

            // Add polygon to list of polygons in shape
            shapes.addPolygon( points );
        }
    }

    // If we don't have polygons by this point, just add a simple box
    if (shapes.getPolygons().size() < 1) {
        shapes.addPolygon( box );
    }

    // Adjust points in Boxes / Polygons so that (0, 0) is the center of the image
    for (auto &shape : shapes.getPolygons()) {
        for (auto &point : shape) {
            point.x = point.x - (bitmap.width / 2.0);
            point.y = (bitmap.height - point.y) - (bitmap.height / 2.0);
        }
    }

    return shapes;
}














