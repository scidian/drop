//
//      Created by Stephens Nunnally on 2/7/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "3rd_party/hull_finder.h"
#include "3rd_party/polyline_simplification.h"
#include "core/dr_debug.h"
#include "core/imaging/imaging.h"
#include "engine/mesh/engine_mesh.h"
#include "engine/mesh/engine_vertex_data.h"
#include "project/constants_component_info.h"
#include "project/entities/dr_image.h"
#include "project/properties/property_collision.h"
#include "project/settings/settings_component.h"
#include "project/settings/settings_component_property.h"


//####################################################################################
//##    Loads list of points for Image and Image Holes
//####################################################################################
void DrImage::autoOutlinePoints() {

    // ***** Break pixmap into seperate images for each object in image
    std::vector<DrBitmap>   bitmaps;
    std::vector<DrRect>     rects;
    Dr::FindObjectsInBitmap(m_bitmap, bitmaps, rects, c_alpha_tolerance);

    // ***** Go through each image (object) and Polygon for it
    for (int image_number = 0; image_number < static_cast<int>(bitmaps.size()); image_number++) {
        DrBitmap &image = bitmaps[image_number];
        if (image.width < 1 || image.height < 1) continue;

        // Trace edge of image
        std::vector<DrPointF> one_poly = Dr::TraceImageOutline(image);
        double plus_one_pixel_percent_x = 1.0 + (1.00 / image.width);       // Add 1.0 pixel
        double plus_one_pixel_percent_y = 1.0 + (1.00 / image.height);      // Add 1.0 pixel
        for (auto &point : one_poly) {
            point.x = point.x * plus_one_pixel_percent_x;
            point.y = point.y * plus_one_pixel_percent_y;
        }

        // Old Way of Simplifying Points on Similar Slopes
        ///int split = wireframe ? int((((image.width() + image.height()) / 2) * 0.2) / 5) : 1000;      // Splits longest lines of outline into 5 triangles
        ///one_poly = simplifyPoints(one_poly, 0.030,     5, true );        // First run with averaging points to reduce triangles among similar slopes
        ///one_poly = simplifyPoints(one_poly, 0.001, split, false);        // Run again with smaller tolerance to reduce triangles along straight lines

        // Smooth point list
        one_poly = DrEngineVertexData::smoothPoints( one_poly, 5, 20.0, 1.0 );

        // Run Polyline Simplification algorithm
        one_poly = PolylineSimplification::RamerDouglasPeucker(one_poly, 0.1);

        // If we only have a couple points left, add shape as a box of the original image, otherwise use PolylineSimplification points
        if (one_poly.size() < 5) {
            ///points = HullFinder::FindConcaveHull(points, 5.0);
            one_poly.clear();
            one_poly.push_back( DrPointF(rects[image_number].topLeft().x,        rects[image_number].topLeft().y) );
            one_poly.push_back( DrPointF(rects[image_number].topRight().x,       rects[image_number].topRight().y) );
            one_poly.push_back( DrPointF(rects[image_number].bottomRight().x,    rects[image_number].bottomRight().y) );
            one_poly.push_back( DrPointF(rects[image_number].bottomLeft().x,     rects[image_number].bottomLeft().y) );
            one_poly.push_back( DrPointF(rects[image_number].topLeft().x,        rects[image_number].topLeft().y) );
        }

        // Remove duplicate first point
        one_poly.pop_back();

        // Check winding
        HullFinder::EnsureWindingOrientation(one_poly, Winding_Orientation::CounterClockwise);

        // Add polygon to list of polygons in shape
        m_poly_list.push_back(one_poly);


        // ***** Copy image and finds holes as seperate outlines
        DrBitmap holes = image.copy(rects[image_number]);
        Dr::FillBorder(holes, Dr::white, holes.rect());                     // Ensures only holes are left as black spots

        // Breaks holes into seperate images for each Hole
        std::vector<DrBitmap> hole_images;
        std::vector<DrRect>   hole_rects;
        Dr::FindObjectsInBitmap(holes, hole_images, hole_rects, c_alpha_tolerance, false);

        // Go through each image (Hole) create list for it
        std::vector<std::vector<DrPointF>> hole_list;
        for (int hole_number = 0; hole_number < static_cast<int>(hole_images.size()); hole_number++) {
            DrBitmap &hole = hole_images[hole_number];
            if (hole.width < 1 || hole.height < 1) continue;
            std::vector<DrPointF> one_hole = Dr::TraceImageOutline(hole);
            // Add in sub image offset to points
            for (auto &point : one_hole) {
                point.x += rects[image_number].left();
                point.y += rects[image_number].top();
            }
            one_hole = DrEngineVertexData::smoothPoints( one_hole, 5, 20.0, 1.0 );
            one_hole = PolylineSimplification::RamerDouglasPeucker(one_hole, 0.1);
            HullFinder::EnsureWindingOrientation(one_hole, Winding_Orientation::Clockwise);
            hole_list.push_back(one_hole);
        }
        m_hole_list.push_back(hole_list);
    }

}























