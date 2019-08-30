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
#include "library/hullfinder.h"

// Image Constants
const int   c_image_size = 600;             // Size of graphic to use for effects (water, mirror, fire, fisheye, swirl, etc.)
const int   c_image_border = 6;             // Border used for mirror, fire, swirl, etc...


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


//####################################################################################
//##    Image editing declarations
//############################
namespace DrImaging {

    QPixmap applySinglePixelFilter( Image_Filter_Type filter, const QPixmap& from_pixmap, int value);
    QImage  applySinglePixelFilter( Image_Filter_Type filter, const QImage&  from_image,  int value );

    QPixmap applyPixelation( const QPixmap& from_pixmap, QPointF data_pair );
    QImage  applyPixelation( const QImage&  from_image,  QPointF data_pair );

    QPixmap drawFibonacci(QColor background_color, QColor pen_color);

    QPixmap drawFire(QColor color_1, QColor color_2, QColor smoke, Fire_Mask mask);
    QPixmap drawFisheye(QColor color);
    QPixmap drawLight(QColor color, int diameter, float cone_start, float cone_end, float intensity, float blur);
    QPixmap drawMirror(QColor top_color, QColor bottom_color);
    QPixmap drawSwirl(QColor top_color, double angle);
    QPixmap drawWater(QColor top_color, QColor bottom_color);

    float*  imageBitsAsFloat(const QImage &from_image);
    QImage  imageMask(const QImage& from_image, QColor mask_color, int max_alpha);

    QVector<HullPoint> outlinePointList(const QImage& from_image, double alpha_tolerance );

}

#endif // IMAGE_FILTER_H












