//
//      Created by Stephens Nunnally on 12/19/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QtMath>
#include <QDebug>
#include <QImage>
#include <QPixmap>
#include <QRgb>
#include <QStandardPaths>
#include <QVector2D>

#include "core/colors/colors.h"
#include "core/dr_math.h"
#include "editor/pixmap/pixmap.h"
#include "editor/preferences.h"

namespace Dr
{


//####################################################################################
//##    Loops through image and changes one pixel at a time based on a
//##    premultiplied table
//####################################################################################
QPixmap ApplySinglePixelFilter(Image_Filter_Type filter, const QPixmap &from_pixmap, int value) {
    return QPixmap::fromImage(ApplySinglePixelFilter(filter, from_pixmap.toImage(), value));
}

QImage ApplySinglePixelFilter(Image_Filter_Type filter, const QImage &from_image, int value) {
    QImage image = from_image;
    std::vector<QRgb*> lines = Dr::GetScanLines(image);

    int table[ 256 ];
    for ( int i = 0; i < 256; ++i ) {
        switch (filter) {
            case Image_Filter_Type::Brightness:   table[i] = Dr::Clamp( i + value, 0, 255 );                                    break;
            case Image_Filter_Type::Contrast:     table[i] = Dr::Clamp( (( i - 127 ) * (value + 128) / 128 ) + 127, 0, 255 );   break;
            default: ;
        }
    }

    for (size_t y = 0; y < static_cast<size_t>(image.height()); ++y) {
        for (size_t x = 0; x < static_cast<size_t>(image.width()); ++x) {

            // Grab the current pixel color
            QColor color = QColor::fromRgba( lines[y][x] );

            switch (filter) {
                case Image_Filter_Type::Brightness:
                case Image_Filter_Type::Contrast:
                    color.setRed(   table[color.red()]   );
                    color.setGreen( table[color.green()] );
                    color.setBlue(  table[color.blue()]  );
                    break;
                case Image_Filter_Type::Saturation: {
                    // !!!!! #NOTE: QColor returns -1 if image is grayscale
                    //              If thats the case give it a default hue of 0 (red) to match shader
                    int hue = (color.hue() == -1) ? 0 : color.hue();
                    color.setHsv(hue, Dr::Clamp(color.saturation() + value, 0, 255), color.value(), color.alpha());
                    break;
                }
                case Image_Filter_Type::Hue:
                    color.setHsv(Dr::Clamp(color.hue() + value, 0, 720), color.saturation(), color.value(), color.alpha());
                    break;
                case Image_Filter_Type::Grayscale: {
                    double temp = (color.redF() * 0.2126) + (color.greenF() * 0.7152) + (color.blueF() * 0.0722);
                    color.setRgbF(temp, temp, temp, color.alphaF());
                    break;
                }
                case Image_Filter_Type::Negative:
                    color.setRgbF(1.0 - color.redF(), 1.0 - color.greenF(), 1.0 - color.blueF(), color.alphaF());
                    break;
                case Image_Filter_Type::Opacity:
                    color.setAlpha( Dr::Clamp(color.alpha() + value, 0, 255) );
                    break;
                case Image_Filter_Type::Premultiplied_Alpha: {
                    double r = color.redF() *   color.alphaF();
                    double g = color.greenF() * color.alphaF();
                    double b = color.blueF() *  color.alphaF();
                    color.setRedF(   r );
                    color.setGreenF( g );
                    color.setBlueF(  b );
                    break;
                }
                case Image_Filter_Type::Pixelation: ;                                           // Different Function
                    break;
                case Image_Filter_Type::Bitrate:
                    double bit_depth = static_cast<double>(value);
                    // = vec3(floor(frag_rgb.r * bit_depth), floor(frag_rgb.g * bit_depth), floor(frag_rgb.b * bit_depth)) / bit_depth;
                    double r = static_cast<double>(static_cast<int>((color.redF() *     bit_depth))) / bit_depth;
                    double g = static_cast<double>(static_cast<int>((color.greenF() *   bit_depth))) / bit_depth;
                    double b = static_cast<double>(static_cast<int>((color.blueF() *    bit_depth))) / bit_depth;
                    color.setRedF(   r );
                    color.setGreenF( g );
                    color.setBlueF(  b );
                    break;
            }

            // Sets the new pixel color
            lines[y][x] = color.rgba();
        }
    }
    return image;
}



//####################################################################################
//##    Pixelates Image
//####################################################################################
void ApplyPixelation(QImage &image, QPointF data_pair) {
    std::vector<QRgb*> lines = GetScanLines(image);

    for (size_t y = 0; y < static_cast<size_t>(image.height()); ++y) {
        for (size_t x = 0; x < static_cast<size_t>(image.width()); ++x) {
            double dx = data_pair.x();
            double dy = data_pair.y();

            double x_coord = Dr::Clamp( dx * floor(x / dx) + (dx / 2.0), 0.0, image.width() - 1.0);
            double y_coord = Dr::Clamp( dy * floor(y / dy) + (dy / 2.0), 0.0, image.height() - 1.0);

            lines[y][x] = lines[static_cast<size_t>(y_coord)][static_cast<size_t>(x_coord)];
        }
    }
}


//####################################################################################
//##    Replace all colors in image to new_color
//####################################################################################
QImage ColorizeImage(const QImage &from_image, QColor new_color) {
    QImage image = from_image;
    std::vector<QRgb*> lines = GetScanLines(image);

    for (size_t y = 0; y < static_cast<size_t>(image.height()); ++y) {
        for (size_t x = 0; x < static_cast<size_t>(image.width()); ++x) {
            QColor color = QColor::fromRgba( lines[y][x] );

            if (color.alpha() > 10) {
                color.setRed(   new_color.red() );
                color.setGreen( new_color.green() );
                color.setBlue(  new_color.blue() );
                lines[y][x] = color.rgba();
            }
        }
    }
    return image;
}


//####################################################################################
//##    Tints an image towards tint_color
//####################################################################################
QImage TintImage(const QImage &from_image, QColor tint_color) {
    QImage image = from_image;
    std::vector<QRgb*> lines = GetScanLines(image);

    for (size_t y = 0; y < static_cast<size_t>(image.height()); ++y) {
        for (size_t x = 0; x < static_cast<size_t>(image.width()); ++x) {
            QColor color = QColor::fromRgba( lines[y][x] );

            // Convert to grayscale
            double temp = (color.redF() * 0.2126) + (color.greenF() * 0.7152) + (color.blueF() * 0.0722);

            // Colorize
            color.setRedF(   Dr::Clamp(temp * tint_color.redF()   * 2.0, 0.0, 1.0) );
            color.setGreenF( Dr::Clamp(temp * tint_color.greenF() * 2.0, 0.0, 1.0) );
            color.setBlueF(  Dr::Clamp(temp * tint_color.blueF()  * 2.0, 0.0, 1.0) );
            lines[y][x] = color.rgba();
        }
    }
    return image;
}


//####################################################################################
//##    Sets any pixels <= 2 opacity to zero for easier GraphicsItem shapes
//####################################################################################
QImage CheckOpacityTolerance(const QImage &from_image) {
    QImage image = from_image;
    std::vector<QRgb*> lines = Dr::GetScanLines(image);

    for (size_t y = 0; y < static_cast<size_t>(image.height()); ++y) {
        for (size_t x = 0; x < static_cast<size_t>(image.width()); ++x) {
            QColor color = QColor::fromRgba( lines[y][x] );

            if (color.alpha() <= 2) {
                color.setAlpha(0);
                lines[y][x] = color.rgba();
            }
        }
    }
    return image;
}




}   // End namespace Dr
















