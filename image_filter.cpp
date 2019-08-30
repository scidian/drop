//
//      Created by Stephens Nunnally on 3/7/2019, (c) 2019 Scidian Software, All Rights Reserved
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
#include <QTime>
#include <QVector2D>

#include "colors/colors.h"
#include "globals.h"
#include "image_filter.h"
#include "helper.h"


namespace DrImaging
{


//####################################################################################
//##        Loops through image and changes one pixel at a time based on a
//##        premultiplied table
//####################################################################################
QPixmap applySinglePixelFilter( Image_Filter_Type filter, const QPixmap& from_pixmap, int value) {
    return QPixmap::fromImage( applySinglePixelFilter(filter, from_pixmap.toImage(), value ));
}

QImage applySinglePixelFilter( Image_Filter_Type filter, const QImage& from_image, int value) {

    QImage image = from_image;
    if ( image.format() != QImage::Format::Format_ARGB32 )
        image = image.convertToFormat( QImage::Format_ARGB32 );
    image.detach();

    QTime time_it;
    time_it.restart();

    // Truecolor Rgba
    if (image.colorCount() == 0 ) {

        int table[ 256 ];
        for ( int i = 0; i < 256; ++i ) {
            switch (filter) {
                case Image_Filter_Type::Brightness:   table[i] = Dr::Clamp( i + value, 0, 255 );                                      break;
                case Image_Filter_Type::Contrast:     table[i] = Dr::Clamp( (( i - 127 ) * (value + 128) / 128 ) + 127, 0, 255 );     break;
                default: ;
            }
        }

        if ( image.hasAlphaChannel() ) {
            // Loop through every pixel
            for (int y = 0; y < image.height(); ++y) {
                QRgb* line = reinterpret_cast<QRgb*>( image.scanLine( y ));
                for (int x = 0; x < image.width(); ++x) {

                    // Grab the current pixel color
                    QColor color = QColor::fromRgba( line[x] );

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
                            qreal r = color.redF() *   color.alphaF();
                            qreal g = color.greenF() * color.alphaF();
                            qreal b = color.blueF() *  color.alphaF();
                            color.setRedF(   r );
                            color.setGreenF( g );
                            color.setBlueF(  b );
                            break;
                        }
                        case Image_Filter_Type::Pixelation: ;                                           // Different Function
                    }

                    // Sets the new pixel color
                    line[x] = color.rgba();

                }
            }


        } else {
            // Error, no alpha channel
            Dr::ShowMessageBox("Image missing alpha channel!");
        }
    } else {
        // Error, only 256 color format
        Dr::ShowMessageBox("Image only has 256 colors!");
    }

    // !!!!! #DEBUG: Timer that shows how long it took to apply filter
    //if (filter == Image_Filter_Type::Hue)
    //    Dr::SetLabelText(Label_Names::Label_2, "   Hue  Time: " + QString::number(time_it.elapsed()) );
    //else
    //    Dr::SetLabelText(Label_Names::Label_1, " Filter Time: " + QString::number(time_it.elapsed()) );

    return image;
}



//####################################################################################
//##        Pixelates Image
//####################################################################################
QPixmap applyPixelation( const QPixmap& from_pixmap, QPointF data_pair ) {
    return QPixmap::fromImage( applyPixelation(from_pixmap.toImage(), data_pair ));
}

QImage applyPixelation(const QImage& from_image, QPointF data_pair ) {
    QImage image = from_image;
    if (image.format() != QImage::Format::Format_ARGB32)
        image = image.convertToFormat( QImage::Format_ARGB32 );
    image.detach();

    // Truecolor Rgba
    if (image.colorCount() == 0 ) {
        if ( image.hasAlphaChannel() ) {

            // Grab all the scan lines
            QVector<QRgb*> lines;
            for (int y = 0; y < image.height(); ++y) {
                lines.append( reinterpret_cast<QRgb*>(image.scanLine(y)) );
            }

            // Loop through every pixel
            for (int y = 0; y < image.height(); ++y) {
                for (int x = 0; x < image.width(); ++x) {
                    double dx = data_pair.x();
                    double dy = data_pair.y();

                    double x_coord = Dr::Clamp( dx * floor(x / dx) + (dx / 2.0), 0.0, image.width() - 1.0);
                    double y_coord = Dr::Clamp( dy * floor(y / dy) + (dy / 2.0), 0.0, image.height() - 1.0);

                    lines[y][x] = lines[static_cast<int>(y_coord)][static_cast<int>(x_coord)];
                }
            }

        } else {    Dr::ShowMessageBox("Image missing alpha channel!"); }
    } else {    Dr::ShowMessageBox("Image only has 256 colors!"); }

    return image;
}



//####################################################################################
//##        Turns Image into a Mask of the spcified color
//####################################################################################
QImage imageMask(const QImage &from_image, QColor mask_color, int max_alpha) {
    QImage image = from_image;
    if ( image.format() != QImage::Format::Format_ARGB32 )
        image = image.convertToFormat( QImage::Format_ARGB32 );
    image.detach();

    // Truecolor Rgba
    if (image.colorCount() == 0 ) {
        if ( image.hasAlphaChannel() ) {

            // Loop through every pixel, if alpha is below threshold, set to mask color
            for (int y = 0; y < image.height(); ++y) {
                QRgb* line = reinterpret_cast<QRgb*>( image.scanLine( y ));
                for (int x = 0; x < image.width(); ++x) {
                    QColor color = QColor::fromRgba( line[x] );
                    line[x] = (color.alpha() < max_alpha) ? mask_color.light().rgba() : mask_color.rgba();
                }
            }

        } else {    Dr::ShowMessageBox("Image missing alpha channel!"); }
    } else {    Dr::ShowMessageBox("Image only has 256 colors!"); }
    return image;
}


//####################################################################################
//##        Returns an array of bits from an Image as aRgb float values
//####################################################################################
float* imageBitsAsFloat(const QImage &from_image) {
    QImage image = from_image;
    if ( image.format() != QImage::Format::Format_ARGB32 )
        image = image.convertToFormat( QImage::Format_ARGB32 );
    image.detach();

    float *out = static_cast<float*>( malloc(static_cast<unsigned long>(image.width() * image.height()) * sizeof(float)) );

    // Truecolor Rgba
    if (image.colorCount() == 0 ) {
        if ( image.hasAlphaChannel() ) {

            // Loop through every pixel, if alpha is below threshold, set to mask color
            int index = 0;
            for (int y = 0; y < image.height(); ++y) {
                QRgb* line = reinterpret_cast<QRgb*>( image.scanLine( y ));
                for (int x = 0; x < image.width(); ++x) {
                    out[index] = line[x];
                    index++;
                }
            }

        } else {    Dr::ShowMessageBox("Image missing alpha channel!"); }
    } else {    Dr::ShowMessageBox("Image only has 256 colors!"); }
    return out;
}



}   // End DrImaging Namespace
















