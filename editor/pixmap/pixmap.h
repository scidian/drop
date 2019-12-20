//
//      Created by Stephens Nunnally on 3/7/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef PIXMAP_H
#define PIXMAP_H

#include <QPixmap>

#include "engine/enums_engine.h"
#include "library/types/dr_color.h"
#include "library/types/dr_pointf.h"


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


// Image Constants
const int   c_image_size =   600;           // Size of graphic to use for effects (water, mirror, fire, fisheye, swirl, etc.)
const int   c_image_border =   6;           // Border used for mirror, fire, swirl, etc...

const int   c_device_size =  400;           // Size used for square Device / Control icons (camera, etc)
const int   c_device_border = 10;           // Border used for camera, etc


//####################################################################################
//##    Pixmap handling / drawing
//############################
namespace Dr {


    // ***** Basic Drawing
    DrColor             AverageColor(const QPixmap &pixmap, bool screen_shot = false);
    void                CopyImage(QImage &dest_image, QImage &source_image, QRect rect) ;
    void                FillImage(QImage &image, QColor color, QRect rect);
    std::vector<QRgb*>  GetScanLines(QImage &image);
    float*              ImageBitsAsFloat(const QImage &from_image);


    // ***** Draw Images for DrDevices
    QPixmap DrawFixedIcon(QPixmap &middle_icon);
    QPixmap DrawCamera();


    // ***** Draw Images for DrEffects
    QPixmap DrawFibonacci(QColor background_color, QColor pen_color);

    QPixmap DrawFire(QColor color_1, QColor color_2, QColor smoke, Fire_Mask mask);
    QPixmap DrawFisheye(QColor color);
    QPixmap DrawLight(QColor color, int diameter, float cone_start, float cone_end, float intensity, float blur);
    QPixmap DrawMirror(QColor top_color, QColor bottom_color);
    QPixmap DrawSwirl(QColor top_color, double angle);
    QPixmap DrawWater(QColor top_color, QColor bottom_color);


    // ***** Filters
    QPixmap ApplySinglePixelFilter( Image_Filter_Type filter, const QPixmap &from_pixmap, int value);
    QImage  ApplySinglePixelFilter( Image_Filter_Type filter, const QImage  &from_image,  int value);

    void    ApplyPixelation(QImage &image,  QPointF data_pair);

    QImage  ColorizeImage(const QImage &from_image, QColor new_color);
    QImage  CheckOpacityTolerance(const QImage &from_image);


    // ***** Masking
    QImage      ImageMask(const QImage& from_image, QColor mask_color, int max_alpha);
    void        SetMaskCircleShape (QPixmap &pix);


}

#endif // PIXMAP_H












