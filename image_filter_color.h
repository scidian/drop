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


namespace DrFilter
{

    QPixmap changeBrightness(const QPixmap& image, int brightness);
    QPixmap changeContrast(const QPixmap& image, int contrast);
    QPixmap changeGamma(const QPixmap& image, int gamma);

}

#endif // IMAGE_FILTER_COLOR_H
