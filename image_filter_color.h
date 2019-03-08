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
    Hue,
    Saturation,
};


namespace DrFilter
{


    QPixmap changeBrightness(const QPixmap& pixmap, int brightness);
    QPixmap changeContrast(const QPixmap& pixmap, int contrast);
    QPixmap changeHue(const QPixmap& pixmap, int hue);
    QPixmap changeSaturation(const QPixmap& pixmap, int saturation);

}

#endif // IMAGE_FILTER_COLOR_H
