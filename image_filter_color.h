//
//      Created by Stephens Nunnally on 3/7/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef IMAGE_FILTER_COLOR_H
#define IMAGE_FILTER_COLOR_H

#include <QPixmap>


enum class Filter_Type
{
    Brightness,
    Contrast,
    Saturation,
    Hue,
    Grayscale,
    Negative,
};


namespace DrFilter
{


    QPixmap changeBrightness(const QPixmap& pixmap, int brightness);
    QPixmap changeContrast(const QPixmap& pixmap, int contrast);
    QPixmap changeSaturation(const QPixmap& pixmap, int saturation);
    QPixmap changeHue(const QPixmap& pixmap, int hue);
    QPixmap changeToGrayscale(const QPixmap& pixmap);
    QPixmap changeToNegative(const QPixmap& pixmap);


}

#endif // IMAGE_FILTER_COLOR_H
