//
//      Created by Stephens Nunnally on 3/7/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef IMAGE_FILTER_H
#define IMAGE_FILTER_H

#include <QPixmap>

#include "enums_engine.h"

// Forward Declarations
class DrPointF;

// Image Constants
const int   c_image_size = 600;             // Size of graphic to use for effects (water, mirror, fire, fisheye, swirl, etc.)
const int   c_image_border = 6;             // Border used for mirror, fire, swirl, etc...

// Color constants
///const unsigned int c_color_black = 0;
///const unsigned int c_color_white = 4294967295;
const unsigned int c_color_black =  QColor(  0,   0,   0,   0).rgba();
const unsigned int c_color_white =  QColor(255, 255, 255, 255).rgba();


//####################################################################################
//##    Different types of filters that can be applied to a QPixmap
//############################
enum class Image_Filter_Type {
    Brightness,                             // -255 to 255
    Contrast,                               // Contrast is multiplied by 100 in order to avoid floating point numbers
    Saturation,
    Hue,
    Grayscale,
    Negative,
    Pixelation,

    Opacity,

    Premultiplied_Alpha,
};

enum class Flood_Fill_Type {
    Compare_4,
    Compare_8,
};


//####################################################################################
//##    Image editing declarations
//############################
namespace DrImaging {

    // Basic Drawing
    void    copyImage(QImage &dest_image, QImage &source_image, QRect rect) ;
    void    fillImage(QImage &image, QColor color, QRect rect);


    // Misc Image Functions
    QColor              averageColor(const QPixmap &pixmap, bool screen_shot = false);
    QVector<QRgb*>      getScanLines(QImage &image);
    float*              imageBitsAsFloat(const QImage &from_image);
    QVector<DrPointF>   outlinePointList(const QImage &from_image);
    QVector<DrPointF>   traceImageOutline(const QImage &from_image);


    // Filters
    QPixmap applySinglePixelFilter( Image_Filter_Type filter, const QPixmap &from_pixmap, int value);
    QImage  applySinglePixelFilter( Image_Filter_Type filter, const QImage  &from_image,  int value);

    QPixmap applyPixelation( const QPixmap &from_pixmap, QPointF data_pair );
    QImage  applyPixelation( const QImage  &from_image,  QPointF data_pair );

    QImage  colorizeImage(const QImage &from_image, QColor new_color);


    // Draw Images for DrEffects
    QPixmap drawFibonacci(QColor background_color, QColor pen_color);

    QPixmap drawFire(QColor color_1, QColor color_2, QColor smoke, Fire_Mask mask);
    QPixmap drawFisheye(QColor color);
    QPixmap drawLight(QColor color, int diameter, float cone_start, float cone_end, float intensity, float blur);
    QPixmap drawMirror(QColor top_color, QColor bottom_color);
    QPixmap drawSwirl(QColor top_color, double angle);
    QPixmap drawWater(QColor top_color, QColor bottom_color);


    // Draw Images for DrDevices
    QPixmap drawCamera();


    // Object Counting / Fill (a la Ravens Project)
    QImage  blackAndWhiteFromAlpha(const QImage &from_image, double alpha_tolerance, bool inverse = false);
    void    fillBorder(QImage &image, QColor fill_color, QRect rect);
    int     findObjectsInImage(const QImage &image, QVector<QImage> &images, QVector<QRect> &rects, double alpha_tolerance, bool convert = true);
    QImage  floodFill(QImage &from_image, int at_x, int at_y, QColor fill_color, double tolerance, Flood_Fill_Type type,
                      int &flood_pixel_count, QRect &flood_rect);


}

#endif // IMAGE_FILTER_H












