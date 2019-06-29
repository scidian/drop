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

//####################################################################################
//##    Different types of filters that can be applied to a QPixmap
//############################
enum class Image_Filter_Type {
    Brightness,
    Contrast,
    Saturation,
    Hue,
    Grayscale,
    Negative,
    Pixelation,

    Opacity,
};


//####################################################################################
//##    Image editing declarations
//############################
namespace DrImaging {

    QPixmap changeBrightness(const QPixmap& pixmap, int brightness);
    QPixmap changeContrast(const QPixmap& pixmap, int contrast);
    QPixmap changeSaturation(const QPixmap& pixmap, int saturation);
    QPixmap changeHue(const QPixmap& pixmap, int hue);
    QPixmap changeToGrayscale(const QPixmap& pixmap);
    QPixmap changeToNegative(const QPixmap& pixmap);
    QPixmap changePixelation(const QPixmap& pixmap, QPointF pixelation);

    QPixmap changeOpacity(const QPixmap& pixmap, int opacity);

    QPixmap drawLight(QColor color, int diameter, float cone_start, float cone_end, float intensity, float blur);

}

#endif // IMAGE_FILTER_COLOR_H
