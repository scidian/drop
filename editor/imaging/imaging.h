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

enum class Flood_Fill_Type {
    Compare_4,
    Compare_8,
};


//####################################################################################
//##    Image editing declarations
//############################
namespace Dr {

    // ***** Misc Image Functions
    std::vector<DrPointF>   OutlinePointList(const QImage &from_image);
    std::vector<DrPointF>   TraceImageOutline(const QImage &from_image);

    // ***** Filters
    QPixmap ApplySinglePixelFilter( Image_Filter_Type filter, const QPixmap &from_pixmap, int value);
    QImage  ApplySinglePixelFilter( Image_Filter_Type filter, const QImage  &from_image,  int value);

    QPixmap ApplyPixelation( const QPixmap &from_pixmap, QPointF data_pair );
    QImage  ApplyPixelation( const QImage  &from_image,  QPointF data_pair );

    QImage  ColorizeImage(const QImage &from_image, QColor new_color);
    QImage  CheckOpacityTolerance(const QImage &from_image);

    // ***** Object Counting / Fill (a la Ravens Project)
    QImage  BlackAndWhiteFromAlpha(const QImage &from_image, double alpha_tolerance, bool inverse = false);
    void    FillBorder(QImage &image, QColor fill_color, QRect rect);
    int     FindObjectsInImage(const QImage &image, std::vector<QImage> &images, std::vector<QRect> &rects, double alpha_tolerance, bool convert = true);
    QImage  FloodFill(QImage &from_image, int at_x, int at_y, QColor fill_color, double tolerance, Flood_Fill_Type type,
                      int &flood_pixel_count, QRect &flood_rect);


}

#endif // IMAGE_FILTER_H












