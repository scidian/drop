//
//      Created by Stephens Nunnally on 12/18/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QDebug>
#include <QFileInfo>
#include <QImage>
#include <QPixmap>

#include "editor/project/project.h"
#include "project/dr_project.h"
#include "project/entities/dr_asset.h"
#include "project/entities/dr_image.h"
#include "project/entities/dr_stage.h"
#include "project/entities/dr_thing.h"
#include "project/entities/dr_world.h"
#include "project/properties/property_collision.h"

namespace Dr {


//####################################################################################
//##    Adds Singular Image to Project
//####################################################################################
DrImage* AddImage(DrProject *project, QString filename, long key, Asset_Category category) {

    QFileInfo file_info(filename);
        QString full_path =     file_info.path();
        QString file_name =     file_info.fileName();
        QString simple_name =   file_info.baseName().replace("_", " ");

    QImage image = QImage(filename).convertToFormat(QImage::Format::Format_ARGB32);
    if (image.isNull()) {
        image = QImage(1, 1, QImage::Format::Format_ARGB32);
    }
    DrBitmap bitmap = DrBitmap(image.constBits(), static_cast<int>(image.sizeInBytes()), false, image.width(), image.height());
    ///qDebug() << "Bitmap - from size: " << image.sizeInBytes() << ", Width: " << bitmap.width << ", Height: " << bitmap.height;

    DrPropertyCollision image_shapes;
    ///image_shapes = autoCollisionShapes(bitmap);


    return project->addImage(simple_name.toStdString(), bitmap, category, key);
}


//####################################################################################
//##    Adds Built In Images to Project
//####################################################################################
void AddBuiltInImages(DrProject *project) {

    AddImage(project, ":/assets/dr_images/empty.png" ,                          c_key_image_empty,                  Asset_Category::Image);
    AddImage(project, ":/assets/dr_images/circle.png",                          c_key_image_character,              Asset_Category::Image);
    AddImage(project, ":/assets/dr_images/box.png",                             c_key_image_object,                 Asset_Category::Image);

    //##    Built In Images - Basic         300-399
    AddImage(project, ":/assets/dr_images/basic/square_1.png",                  c_key_image_square_1,               Asset_Category::Basic);
    AddImage(project, ":/assets/dr_images/basic/square_2.png",                  c_key_image_square_2,               Asset_Category::Basic);
    AddImage(project, ":/assets/dr_images/basic/square_3.png",                  c_key_image_square_3,               Asset_Category::Basic);
    AddImage(project, ":/assets/dr_images/basic/square_4.png",                  c_key_image_square_4,               Asset_Category::Basic);
    AddImage(project, ":/assets/dr_images/basic/square_5.png",                  c_key_image_square_5,               Asset_Category::Basic);
    AddImage(project, ":/assets/dr_images/basic/square_6.png",                  c_key_image_square_6,               Asset_Category::Basic);
    AddImage(project, ":/assets/dr_images/basic/square_7.png",                  c_key_image_square_7,               Asset_Category::Basic);
    AddImage(project, ":/assets/dr_images/basic/square_bars.png",               c_key_image_square_bars,            Asset_Category::Basic);
    AddImage(project, ":/assets/dr_images/basic/square_flat.png",               c_key_image_square_flat,            Asset_Category::Basic);
    AddImage(project, ":/assets/dr_images/basic/square_stripes.png",            c_key_image_square_stripes,         Asset_Category::Basic);
    AddImage(project, ":/assets/dr_images/basic/rounded_1.png",                 c_key_image_rounded_1,              Asset_Category::Basic);
    AddImage(project, ":/assets/dr_images/basic/rounded_2.png",                 c_key_image_rounded_2,              Asset_Category::Basic);
    AddImage(project, ":/assets/dr_images/basic/rounded_3.png",                 c_key_image_rounded_3,              Asset_Category::Basic);
    AddImage(project, ":/assets/dr_images/basic/rounded_4.png",                 c_key_image_rounded_4,              Asset_Category::Basic);
    AddImage(project, ":/assets/dr_images/basic/rounded_5.png",                 c_key_image_rounded_5,              Asset_Category::Basic);
    AddImage(project, ":/assets/dr_images/basic/rounded_6.png",                 c_key_image_rounded_6,              Asset_Category::Basic);
    AddImage(project, ":/assets/dr_images/basic/rounded_7.png",                 c_key_image_rounded_7,              Asset_Category::Basic);
    AddImage(project, ":/assets/dr_images/basic/rounded_bars.png",              c_key_image_rounded_bars,           Asset_Category::Basic);
    AddImage(project, ":/assets/dr_images/basic/rounded_flat.png",              c_key_image_rounded_flat,           Asset_Category::Basic);
    AddImage(project, ":/assets/dr_images/basic/rounded_stripes.png",           c_key_image_rounded_stripes,        Asset_Category::Basic);
    AddImage(project, ":/assets/dr_images/basic/circle_1.png",                  c_key_image_circle_1,               Asset_Category::Basic);
    AddImage(project, ":/assets/dr_images/basic/circle_2.png",                  c_key_image_circle_2,               Asset_Category::Basic);
    AddImage(project, ":/assets/dr_images/basic/circle_3.png",                  c_key_image_circle_3,               Asset_Category::Basic);
    AddImage(project, ":/assets/dr_images/basic/circle_4.png",                  c_key_image_circle_4,               Asset_Category::Basic);
    AddImage(project, ":/assets/dr_images/basic/circle_5.png",                  c_key_image_circle_5,               Asset_Category::Basic);
    AddImage(project, ":/assets/dr_images/basic/circle_6.png",                  c_key_image_circle_6,               Asset_Category::Basic);
    AddImage(project, ":/assets/dr_images/basic/circle_7.png",                  c_key_image_circle_7,               Asset_Category::Basic);
    AddImage(project, ":/assets/dr_images/basic/circle_bars.png",               c_key_image_circle_bars,            Asset_Category::Basic);
    AddImage(project, ":/assets/dr_images/basic/circle_flat.png",               c_key_image_circle_flat,            Asset_Category::Basic);
    AddImage(project, ":/assets/dr_images/basic/circle_stripes.png",            c_key_image_circle_stripes,         Asset_Category::Basic);
    AddImage(project, ":/assets/dr_images/basic/triangle_1.png",                c_key_image_triangle_1,             Asset_Category::Basic);
    AddImage(project, ":/assets/dr_images/basic/triangle_2.png",                c_key_image_triangle_2,             Asset_Category::Basic);
    AddImage(project, ":/assets/dr_images/basic/triangle_3.png",                c_key_image_triangle_3,             Asset_Category::Basic);
    AddImage(project, ":/assets/dr_images/basic/triangle_4.png",                c_key_image_triangle_4,             Asset_Category::Basic);
    AddImage(project, ":/assets/dr_images/basic/triangle_5.png",                c_key_image_triangle_5,             Asset_Category::Basic);
    AddImage(project, ":/assets/dr_images/basic/triangle_6.png",                c_key_image_triangle_6,             Asset_Category::Basic);
    AddImage(project, ":/assets/dr_images/basic/triangle_7.png",                c_key_image_triangle_7,             Asset_Category::Basic);
    AddImage(project, ":/assets/dr_images/basic/triangle_bars.png",             c_key_image_triangle_bars,          Asset_Category::Basic);
    AddImage(project, ":/assets/dr_images/basic/triangle_flat.png",             c_key_image_triangle_flat,          Asset_Category::Basic);
    AddImage(project, ":/assets/dr_images/basic/triangle_stripes.png",          c_key_image_triangle_stripes,       Asset_Category::Basic);
    AddImage(project, ":/assets/dr_images/basic/diamond_1.png",                 c_key_image_diamond_1,              Asset_Category::Basic);
    AddImage(project, ":/assets/dr_images/basic/diamond_2.png",                 c_key_image_diamond_2,              Asset_Category::Basic);
    AddImage(project, ":/assets/dr_images/basic/diamond_3.png",                 c_key_image_diamond_3,              Asset_Category::Basic);
    AddImage(project, ":/assets/dr_images/basic/diamond_4.png",                 c_key_image_diamond_4,              Asset_Category::Basic);
    AddImage(project, ":/assets/dr_images/basic/diamond_5.png",                 c_key_image_diamond_5,              Asset_Category::Basic);
    AddImage(project, ":/assets/dr_images/basic/diamond_bars.png",              c_key_image_diamond_bars,           Asset_Category::Basic);
    AddImage(project, ":/assets/dr_images/basic/diamond_flat.png",              c_key_image_diamond_flat,           Asset_Category::Basic);
    AddImage(project, ":/assets/dr_images/basic/diamond_stripes.png",           c_key_image_diamond_stripes,        Asset_Category::Basic);
    AddImage(project, ":/assets/dr_images/basic/line_1.png",                    c_key_image_line_1,                 Asset_Category::Basic);
    AddImage(project, ":/assets/dr_images/basic/line_2.png",                    c_key_image_line_2,                 Asset_Category::Basic);
    AddImage(project, ":/assets/dr_images/basic/cross_1.png",                   c_key_image_cross_1,                Asset_Category::Basic);
    AddImage(project, ":/assets/dr_images/basic/cross_2.png",                   c_key_image_cross_2,                Asset_Category::Basic);
    AddImage(project, ":/assets/dr_images/basic/cross_3.png",                   c_key_image_cross_3,                Asset_Category::Basic);
    AddImage(project, ":/assets/dr_images/basic/cross_4.png",                   c_key_image_cross_4,                Asset_Category::Basic);
    AddImage(project, ":/assets/dr_images/basic/cross_5.png",                   c_key_image_cross_5,                Asset_Category::Basic);
    AddImage(project, ":/assets/dr_images/basic/cross_6.png",                   c_key_image_cross_6,                Asset_Category::Basic);
    AddImage(project, ":/assets/dr_images/basic/cross_7.png",                   c_key_image_cross_7,                Asset_Category::Basic);
    AddImage(project, ":/assets/dr_images/basic/cross_bars.png",                c_key_image_cross_bars,             Asset_Category::Basic);
    AddImage(project, ":/assets/dr_images/basic/cross_flat.png",                c_key_image_cross_flat,             Asset_Category::Basic);
    AddImage(project, ":/assets/dr_images/basic/cross_stripes.png",             c_key_image_cross_stripes,          Asset_Category::Basic);
    AddImage(project, ":/assets/dr_images/basic/half_circle_1.png",             c_key_image_half_circle_1,          Asset_Category::Basic);
    AddImage(project, ":/assets/dr_images/basic/half_circle_2.png",             c_key_image_half_circle_2,          Asset_Category::Basic);
    AddImage(project, ":/assets/dr_images/basic/slope_left.png",                c_key_image_slope_left,             Asset_Category::Basic);
    AddImage(project, ":/assets/dr_images/basic/slope_right.png",               c_key_image_slope_right,            Asset_Category::Basic);

    //##    Built In Images - Outline       400-429
    AddImage(project, ":/assets/dr_images/outlines/square_outline_1.png",       c_key_image_square_outline_1,       Asset_Category::Outlines);
    AddImage(project, ":/assets/dr_images/outlines/square_outline_2.png",       c_key_image_square_outline_2,       Asset_Category::Outlines);
    AddImage(project, ":/assets/dr_images/outlines/square_outline_3.png",       c_key_image_square_outline_3,       Asset_Category::Outlines);
    AddImage(project, ":/assets/dr_images/outlines/rounded_outline_1.png",      c_key_image_rounded_outline_1,      Asset_Category::Outlines);
    AddImage(project, ":/assets/dr_images/outlines/rounded_outline_2.png",      c_key_image_rounded_outline_2,      Asset_Category::Outlines);
    AddImage(project, ":/assets/dr_images/outlines/rounded_outline_3.png",      c_key_image_rounded_outline_3,      Asset_Category::Outlines);
    AddImage(project, ":/assets/dr_images/outlines/circle_outline_1.png",       c_key_image_circle_outline_1,       Asset_Category::Outlines);
    AddImage(project, ":/assets/dr_images/outlines/circle_outline_2.png",       c_key_image_circle_outline_2,       Asset_Category::Outlines);
    AddImage(project, ":/assets/dr_images/outlines/circle_outline_3.png",       c_key_image_circle_outline_3,       Asset_Category::Outlines);
    AddImage(project, ":/assets/dr_images/outlines/triangle_outline_1.png",     c_key_image_triangle_outline_1,     Asset_Category::Outlines);
    AddImage(project, ":/assets/dr_images/outlines/triangle_outline_2.png",     c_key_image_triangle_outline_2,     Asset_Category::Outlines);
    AddImage(project, ":/assets/dr_images/outlines/triangle_outline_3.png",     c_key_image_triangle_outline_3,     Asset_Category::Outlines);
    AddImage(project, ":/assets/dr_images/outlines/diamond_outline_1.png",      c_key_image_diamond_outline_1,      Asset_Category::Outlines);
    AddImage(project, ":/assets/dr_images/outlines/diamond_outline_2.png",      c_key_image_diamond_outline_2,      Asset_Category::Outlines);
    AddImage(project, ":/assets/dr_images/outlines/cross_outline_1.png",        c_key_image_cross_outline_1,        Asset_Category::Outlines);
    AddImage(project, ":/assets/dr_images/outlines/cross_outline_2.png",        c_key_image_cross_outline_2,        Asset_Category::Outlines);

    //##    Built In Images - Gradient      430-469
    AddImage(project, ":/assets/dr_images/gradient/circle_linear_1.png",        c_key_image_circle_linear_1,        Asset_Category::Gradient);
    AddImage(project, ":/assets/dr_images/gradient/circle_linear_2.png",        c_key_image_circle_linear_2,        Asset_Category::Gradient);
    AddImage(project, ":/assets/dr_images/gradient/circle_linear_3.png",        c_key_image_circle_linear_3,        Asset_Category::Gradient);
    AddImage(project, ":/assets/dr_images/gradient/circle_radial_1.png",        c_key_image_circle_radial_1,        Asset_Category::Gradient);
    AddImage(project, ":/assets/dr_images/gradient/circle_radial_2.png",        c_key_image_circle_radial_2,        Asset_Category::Gradient);
    AddImage(project, ":/assets/dr_images/gradient/circle_radial_3.png",        c_key_image_circle_radial_3,        Asset_Category::Gradient);
    AddImage(project, ":/assets/dr_images/gradient/square_linear_1.png",        c_key_image_square_linear_1,        Asset_Category::Gradient);
    AddImage(project, ":/assets/dr_images/gradient/square_linear_2.png",        c_key_image_square_linear_2,        Asset_Category::Gradient);
    AddImage(project, ":/assets/dr_images/gradient/square_linear_3.png",        c_key_image_square_linear_3,        Asset_Category::Gradient);
    AddImage(project, ":/assets/dr_images/gradient/square_radial_1.png",        c_key_image_square_radial_1,        Asset_Category::Gradient);
    AddImage(project, ":/assets/dr_images/gradient/square_radial_2.png",        c_key_image_square_radial_2,        Asset_Category::Gradient);
    AddImage(project, ":/assets/dr_images/gradient/square_radial_3.png",        c_key_image_square_radial_3,        Asset_Category::Gradient);
    AddImage(project, ":/assets/dr_images/gradient/triangle_linear_1.png",      c_key_image_triangle_linear_1,      Asset_Category::Gradient);
    AddImage(project, ":/assets/dr_images/gradient/triangle_linear_2.png",      c_key_image_triangle_linear_2,      Asset_Category::Gradient);
    AddImage(project, ":/assets/dr_images/gradient/triangle_linear_3.png",      c_key_image_triangle_linear_3,      Asset_Category::Gradient);
    AddImage(project, ":/assets/dr_images/gradient/triangle_radial_1.png",      c_key_image_triangle_radial_1,      Asset_Category::Gradient);
    AddImage(project, ":/assets/dr_images/gradient/triangle_radial_2.png",      c_key_image_triangle_radial_2,      Asset_Category::Gradient);
    AddImage(project, ":/assets/dr_images/gradient/triangle_radial_3.png",      c_key_image_triangle_radial_3,      Asset_Category::Gradient);

    //##    Built In Images - Polygon       470-499
    AddImage(project, ":/assets/dr_images/polygons/polygon_5_sides.png",        c_key_image_polygon_5_sides,        Asset_Category::Polygons);
    AddImage(project, ":/assets/dr_images/polygons/polygon_6_sides.png",        c_key_image_polygon_6_sides,        Asset_Category::Polygons);
    AddImage(project, ":/assets/dr_images/polygons/polygon_7_sides.png",        c_key_image_polygon_7_sides,        Asset_Category::Polygons);
    AddImage(project, ":/assets/dr_images/polygons/polygon_8_sides.png",        c_key_image_polygon_8_sides,        Asset_Category::Polygons);
    AddImage(project, ":/assets/dr_images/polygons/polygon_9_sides.png",        c_key_image_polygon_9_sides,        Asset_Category::Polygons);
    AddImage(project, ":/assets/dr_images/polygons/polygon_10_sides.png",       c_key_image_polygon_10_sides,       Asset_Category::Polygons);
    AddImage(project, ":/assets/dr_images/polygons/star_6_points.png",          c_key_image_star_6_points,          Asset_Category::Polygons);
    AddImage(project, ":/assets/dr_images/polygons/star_12_points.png",         c_key_image_star_12_points,         Asset_Category::Polygons);
    AddImage(project, ":/assets/dr_images/polygons/star_24_points.png",         c_key_image_star_24_points,         Asset_Category::Polygons);

    //##    Built In Images - Isometric      500-549
    AddImage(project, ":/assets/dr_images/isometric/isometric_cube_1.png",      c_key_image_isometric_cube_1,       Asset_Category::Isometric);
    AddImage(project, ":/assets/dr_images/isometric/isometric_cube_2.png",      c_key_image_isometric_cube_2,       Asset_Category::Isometric);
    AddImage(project, ":/assets/dr_images/isometric/isometric_cube_3.png",      c_key_image_isometric_cube_3,       Asset_Category::Isometric);
    AddImage(project, ":/assets/dr_images/isometric/isometric_cube_grass.png",  c_key_image_isometric_cube_grass,   Asset_Category::Isometric);
    AddImage(project, ":/assets/dr_images/isometric/isometric_cube_ice.png",    c_key_image_isometric_cube_ice,     Asset_Category::Isometric);
    AddImage(project, ":/assets/dr_images/isometric/isometric_pyramid_1.png",   c_key_image_isometric_pyramid_1,    Asset_Category::Isometric);
    AddImage(project, ":/assets/dr_images/isometric/isometric_pyramid_2.png",   c_key_image_isometric_pyramid_2,    Asset_Category::Isometric);
    AddImage(project, ":/assets/dr_images/isometric/isometric_pyramid_3.png",   c_key_image_isometric_pyramid_3,    Asset_Category::Isometric);
    AddImage(project, ":/assets/dr_images/isometric/isometric_sphere_1.png",    c_key_image_isometric_sphere_1,     Asset_Category::Isometric);
    AddImage(project, ":/assets/dr_images/isometric/isometric_sphere_2.png",    c_key_image_isometric_sphere_2,     Asset_Category::Isometric);
    AddImage(project, ":/assets/dr_images/isometric/isometric_diamond_1.png",   c_key_image_isometric_diamond_1,    Asset_Category::Isometric);
    AddImage(project, ":/assets/dr_images/isometric/isometric_spikes.png",      c_key_image_isometric_spikes,       Asset_Category::Isometric);
    AddImage(project, ":/assets/dr_images/isometric/isometric_star.png",        c_key_image_isometric_star,         Asset_Category::Isometric);
    AddImage(project, ":/assets/dr_images/isometric/isometric_column_1.png",    c_key_image_isometric_column_1,     Asset_Category::Isometric);
    AddImage(project, ":/assets/dr_images/isometric/isometric_column_2.png",    c_key_image_isometric_column_2,     Asset_Category::Isometric);
    AddImage(project, ":/assets/dr_images/isometric/isometric_flower_1.png",    c_key_image_isometric_flower_1,     Asset_Category::Isometric);
    AddImage(project, ":/assets/dr_images/isometric/isometric_mountain.png",    c_key_image_isometric_mountain,     Asset_Category::Isometric);
    AddImage(project, ":/assets/dr_images/isometric/isometric_mushroom.png",    c_key_image_isometric_mushroom,     Asset_Category::Isometric);
    AddImage(project, ":/assets/dr_images/isometric/isometric_rock_1.png",      c_key_image_isometric_rock_1,       Asset_Category::Isometric);
    AddImage(project, ":/assets/dr_images/isometric/isometric_rock_2.png",      c_key_image_isometric_rock_2,       Asset_Category::Isometric);
    AddImage(project, ":/assets/dr_images/isometric/isometric_rock_3.png",      c_key_image_isometric_rock_3,       Asset_Category::Isometric);
    AddImage(project, ":/assets/dr_images/isometric/isometric_tree_1.png",      c_key_image_isometric_tree_1,       Asset_Category::Isometric);
    AddImage(project, ":/assets/dr_images/isometric/isometric_tree_2.png",      c_key_image_isometric_tree_2,       Asset_Category::Isometric);

    //##    Built In Images - Shapes        550-599
    AddImage(project, ":/assets/dr_images/shapes/star_1.png",                   c_key_image_star_1,                 Asset_Category::Shapes);
    AddImage(project, ":/assets/dr_images/shapes/star_2.png",                   c_key_image_star_2,                 Asset_Category::Shapes);
    AddImage(project, ":/assets/dr_images/shapes/star_3.png",                   c_key_image_star_3,                 Asset_Category::Shapes);
    AddImage(project, ":/assets/dr_images/shapes/star_4.png",                   c_key_image_star_4,                 Asset_Category::Shapes);
    AddImage(project, ":/assets/dr_images/shapes/heart_1.png",                  c_key_image_heart_1,                Asset_Category::Shapes);
    AddImage(project, ":/assets/dr_images/shapes/heart_2.png",                  c_key_image_heart_2,                Asset_Category::Shapes);
    AddImage(project, ":/assets/dr_images/shapes/heart_half.png",               c_key_image_heart_half,             Asset_Category::Shapes);
    AddImage(project, ":/assets/dr_images/shapes/pie.png",                      c_key_image_pie,                    Asset_Category::Shapes);
    AddImage(project, ":/assets/dr_images/shapes/ghost.png",                    c_key_image_ghost,                  Asset_Category::Shapes);
    AddImage(project, ":/assets/dr_images/shapes/gear_1.png",                   c_key_image_gear_1,                 Asset_Category::Shapes);
    AddImage(project, ":/assets/dr_images/shapes/gear_2.png",                   c_key_image_gear_2,                 Asset_Category::Shapes);
    AddImage(project, ":/assets/dr_images/shapes/thought_bubble_1.png",         c_key_image_thought_bubble_1,       Asset_Category::Shapes);
    AddImage(project, ":/assets/dr_images/shapes/thought_bubble_2.png",         c_key_image_thought_bubble_2,       Asset_Category::Shapes);
    AddImage(project, ":/assets/dr_images/shapes/thought_bubble_3.png",         c_key_image_thought_bubble_3,       Asset_Category::Shapes);
    AddImage(project, ":/assets/dr_images/shapes/exclamation_point.png",        c_key_image_exclamation_point,      Asset_Category::Shapes);
    AddImage(project, ":/assets/dr_images/shapes/question_mark.png",            c_key_image_question_mark,          Asset_Category::Shapes);
    AddImage(project, ":/assets/dr_images/shapes/arrow_1.png",                  c_key_image_arrow_1,                Asset_Category::Shapes);
    AddImage(project, ":/assets/dr_images/shapes/arrow_2.png",                  c_key_image_arrow_2,                Asset_Category::Shapes);
    AddImage(project, ":/assets/dr_images/shapes/arrow_3.png",                  c_key_image_arrow_3,                Asset_Category::Shapes);
    AddImage(project, ":/assets/dr_images/shapes/reset.png",                    c_key_image_reset,                  Asset_Category::Shapes);
    AddImage(project, ":/assets/dr_images/shapes/fish.png",                     c_key_image_fish,                   Asset_Category::Shapes);

    //##    Built In Images - Ground        600-699
    AddImage(project, ":/assets/dr_images/ground/ground_top_1.png",             c_key_image_ground_top_1,           Asset_Category::Ground);
    AddImage(project, ":/assets/dr_images/ground/ground_top_2.png",             c_key_image_ground_top_2,           Asset_Category::Ground);
    AddImage(project, ":/assets/dr_images/ground/ground_top_3.png",             c_key_image_ground_top_3,           Asset_Category::Ground);
    AddImage(project, ":/assets/dr_images/ground/ground_top_4.png",             c_key_image_ground_top_4,           Asset_Category::Ground);
    AddImage(project, ":/assets/dr_images/ground/ground_top_5.png",             c_key_image_ground_top_5,           Asset_Category::Ground);
    AddImage(project, ":/assets/dr_images/ground/ground_top_6.png",             c_key_image_ground_top_6,           Asset_Category::Ground);
    AddImage(project, ":/assets/dr_images/ground/ground_top_7.png",             c_key_image_ground_top_7,           Asset_Category::Ground);
    AddImage(project, ":/assets/dr_images/ground/ground_top_8.png",             c_key_image_ground_top_8,           Asset_Category::Ground);
    AddImage(project, ":/assets/dr_images/ground/ground_top_left.png",          c_key_image_ground_top_left,        Asset_Category::Ground);
    AddImage(project, ":/assets/dr_images/ground/ground_top_right.png",         c_key_image_ground_top_right,       Asset_Category::Ground);
    AddImage(project, ":/assets/dr_images/ground/ground_top_up_left_1.png",     c_key_image_ground_top_up_left_1,   Asset_Category::Ground);
    AddImage(project, ":/assets/dr_images/ground/ground_top_up_left_2.png",     c_key_image_ground_top_up_left_2,   Asset_Category::Ground);
    AddImage(project, ":/assets/dr_images/ground/ground_top_up_right_1.png",    c_key_image_ground_top_up_right_1,  Asset_Category::Ground);
    AddImage(project, ":/assets/dr_images/ground/ground_top_up_right_2.png",    c_key_image_ground_top_up_right_2,  Asset_Category::Ground);
    AddImage(project, ":/assets/dr_images/ground/block_1.png",                  c_key_image_block_1,                Asset_Category::Ground);
    AddImage(project, ":/assets/dr_images/ground/block_2.png",                  c_key_image_block_2,                Asset_Category::Ground);
    AddImage(project, ":/assets/dr_images/ground/block_lock.png",               c_key_image_block_lock,             Asset_Category::Ground);
    AddImage(project, ":/assets/dr_images/ground/box_1.png",                    c_key_image_box_1,                  Asset_Category::Ground);
    AddImage(project, ":/assets/dr_images/ground/box_2.png",                    c_key_image_box_2,                  Asset_Category::Ground);
    AddImage(project, ":/assets/dr_images/ground/bricks.png",                   c_key_image_bricks,                 Asset_Category::Ground);
    AddImage(project, ":/assets/dr_images/ground/bricks_left.png",              c_key_image_bricks_left,            Asset_Category::Ground);
    AddImage(project, ":/assets/dr_images/ground/bricks_right.png",             c_key_image_bricks_right,           Asset_Category::Ground);
    AddImage(project, ":/assets/dr_images/ground/dirt.png",                     c_key_image_dirt,                   Asset_Category::Ground);
    AddImage(project, ":/assets/dr_images/ground/dirt_block.png",               c_key_image_dirt_block,             Asset_Category::Ground);
    AddImage(project, ":/assets/dr_images/ground/dirt_left.png",                c_key_image_dirt_left,              Asset_Category::Ground);
    AddImage(project, ":/assets/dr_images/ground/dirt_right.png",               c_key_image_dirt_right,             Asset_Category::Ground);
    AddImage(project, ":/assets/dr_images/ground/ice.png",                      c_key_image_ice,                    Asset_Category::Ground);
    AddImage(project, ":/assets/dr_images/ground/ice_left.png",                 c_key_image_ice_left,               Asset_Category::Ground);
    AddImage(project, ":/assets/dr_images/ground/ice_right.png",                c_key_image_ice_right,              Asset_Category::Ground);
    AddImage(project, ":/assets/dr_images/ground/sand.png",                     c_key_image_sand,                   Asset_Category::Ground);
    AddImage(project, ":/assets/dr_images/ground/sand_left.png",                c_key_image_sand_left,              Asset_Category::Ground);
    AddImage(project, ":/assets/dr_images/ground/sand_right.png",               c_key_image_sand_right,             Asset_Category::Ground);
    AddImage(project, ":/assets/dr_images/ground/stone.png",                    c_key_image_stone,                  Asset_Category::Ground);
    AddImage(project, ":/assets/dr_images/ground/stone_left.png",               c_key_image_stone_left,             Asset_Category::Ground);
    AddImage(project, ":/assets/dr_images/ground/stone_right.png",              c_key_image_stone_right,            Asset_Category::Ground);
    AddImage(project, ":/assets/dr_images/ground/tile.png",                     c_key_image_tile,                   Asset_Category::Ground);
    AddImage(project, ":/assets/dr_images/ground/tile_left.png",                c_key_image_tile_left,              Asset_Category::Ground);
    AddImage(project, ":/assets/dr_images/ground/tile_right.png",               c_key_image_tile_right,             Asset_Category::Ground);
    AddImage(project, ":/assets/dr_images/ground/water.png",                    c_key_image_water,                  Asset_Category::Ground);
    AddImage(project, ":/assets/dr_images/ground/water_top.png",                c_key_image_water_top,              Asset_Category::Ground);
    AddImage(project, ":/assets/dr_images/ground/wood.png",                     c_key_image_wood,                   Asset_Category::Ground);
    AddImage(project, ":/assets/dr_images/ground/wood_left.png",                c_key_image_wood_left,              Asset_Category::Ground);
    AddImage(project, ":/assets/dr_images/ground/wood_right.png",               c_key_image_wood_right,             Asset_Category::Ground);
    AddImage(project, ":/assets/dr_images/ground/wall_1.png",                   c_key_image_wall_1,                 Asset_Category::Ground);
    AddImage(project, ":/assets/dr_images/ground/wall_2.png",                   c_key_image_wall_2,                 Asset_Category::Ground);
    AddImage(project, ":/assets/dr_images/ground/window.png",                   c_key_image_window,                 Asset_Category::Ground);
    AddImage(project, ":/assets/dr_images/ground/bridge_left.png",              c_key_image_bridge_left,            Asset_Category::Ground);
    AddImage(project, ":/assets/dr_images/ground/bridge_mid.png",               c_key_image_bridge_mid,             Asset_Category::Ground);
    AddImage(project, ":/assets/dr_images/ground/bridge_right.png",             c_key_image_bridge_right,           Asset_Category::Ground);
    AddImage(project, ":/assets/dr_images/ground/platform_left.png",            c_key_image_platform_left,          Asset_Category::Ground);
    AddImage(project, ":/assets/dr_images/ground/platform_mid.png",             c_key_image_platform_mid,           Asset_Category::Ground);
    AddImage(project, ":/assets/dr_images/ground/platform_right.png",           c_key_image_platform_right,         Asset_Category::Ground);
    AddImage(project, ":/assets/dr_images/ground/logs.png",                     c_key_image_logs,                   Asset_Category::Ground);

    //##    Built In Images - Decoration    700-749
    AddImage(project, ":/assets/dr_images/decoration/bush_1.png",               c_key_image_bush_1,                 Asset_Category::Decoration);
    AddImage(project, ":/assets/dr_images/decoration/bush_2.png",               c_key_image_bush_2,                 Asset_Category::Decoration);
    AddImage(project, ":/assets/dr_images/decoration/bush_3.png",               c_key_image_bush_3,                 Asset_Category::Decoration);
    AddImage(project, ":/assets/dr_images/decoration/cloud_1.png",              c_key_image_cloud_1,                Asset_Category::Decoration);
    AddImage(project, ":/assets/dr_images/decoration/cloud_2.png",              c_key_image_cloud_2,                Asset_Category::Decoration);
    AddImage(project, ":/assets/dr_images/decoration/flower_1.png",             c_key_image_flower_1,               Asset_Category::Decoration);
    AddImage(project, ":/assets/dr_images/decoration/grass_1.png",              c_key_image_grass_1,                Asset_Category::Decoration);
    AddImage(project, ":/assets/dr_images/decoration/grass_2.png",              c_key_image_grass_2,                Asset_Category::Decoration);
    AddImage(project, ":/assets/dr_images/decoration/grass_3.png",              c_key_image_grass_3,                Asset_Category::Decoration);
    AddImage(project, ":/assets/dr_images/decoration/ladder_1.png",             c_key_image_ladder_1,               Asset_Category::Decoration);
    AddImage(project, ":/assets/dr_images/decoration/ladder_2.png",             c_key_image_ladder_2,               Asset_Category::Decoration);
    AddImage(project, ":/assets/dr_images/decoration/ladder_top.png",           c_key_image_ladder_top,             Asset_Category::Decoration);
    AddImage(project, ":/assets/dr_images/decoration/mushroom.png",             c_key_image_mushroom,               Asset_Category::Decoration);
    AddImage(project, ":/assets/dr_images/decoration/rock_1.png",               c_key_image_rock_1,                 Asset_Category::Decoration);
    AddImage(project, ":/assets/dr_images/decoration/rock_2.png",               c_key_image_rock_2,                 Asset_Category::Decoration);
    AddImage(project, ":/assets/dr_images/decoration/sign_1.png",               c_key_image_sign_1,                 Asset_Category::Decoration);
    AddImage(project, ":/assets/dr_images/decoration/sign_2.png",               c_key_image_sign_2,                 Asset_Category::Decoration);
    AddImage(project, ":/assets/dr_images/decoration/sign_post.png",            c_key_image_sign_post,              Asset_Category::Decoration);
    AddImage(project, ":/assets/dr_images/decoration/sign_post_top.png",        c_key_image_sign_post_top,          Asset_Category::Decoration);
    AddImage(project, ":/assets/dr_images/decoration/sign_warning.png",         c_key_image_sign_warning,           Asset_Category::Decoration);
    AddImage(project, ":/assets/dr_images/decoration/sun.png",                  c_key_image_sun,                    Asset_Category::Decoration);

}   // End AddBuiltInImages()




}   // End namespace Dr
