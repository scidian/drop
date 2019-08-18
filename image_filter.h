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


//####################################################################################
//##    Different types of filters that can be applied to a QPixmap
//############################
enum class Image_Filter_Type {
    Brightness,                     // -255 to 255
    Contrast,                       // Contrast is multiplied by 100 in order to avoid floating point numbers
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

    QPixmap drawFire(QColor color_1, QColor color_2, QColor smoke, Fire_Mask mask);
    QPixmap drawFisheye(QColor color);
    QPixmap drawLight(QColor color, int diameter, float cone_start, float cone_end, float intensity, float blur);
    QPixmap drawWater(QColor top_color, QColor bottom_color);

}

#endif // IMAGE_FILTER_H




