//
//      Created by Stephens Nunnally on 3/7/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QtMath>
#include <QImage>
#include <QPixmap>
#include <QRgb>
#include <QTime>

#include "globals.h"
#include "image_filter_color.h"
#include "library.h"


namespace DrFilter
{

// Foward declarations
QImage  applySinglePixelFilter( Filter_Type filter, const QImage& from_image, int value );



//####################################################################################
//##        External filter calls
//####################################################################################
// Range is -255 to 255
QPixmap changeBrightness(const QPixmap& pixmap, int brightness)
{
    QImage image = applySinglePixelFilter( Filter_Type::Brightness, pixmap.toImage(), brightness );
    return QPixmap::fromImage( image );
}

// Contrast is multiplied by 100 in order to avoid floating point numbers
QPixmap changeContrast(const QPixmap& pixmap, int contrast)
{
    QImage image = applySinglePixelFilter( Filter_Type::Contrast, pixmap.toImage(), contrast );
    return QPixmap::fromImage( image );
}

QPixmap changeSaturation(const QPixmap& pixmap, int saturation)
{
    QImage image = applySinglePixelFilter( Filter_Type::Saturation, pixmap.toImage(), saturation );
    return QPixmap::fromImage( image );
}

QPixmap changeHue(const QPixmap& pixmap, int hue)
{
    QImage image = applySinglePixelFilter( Filter_Type::Hue, pixmap.toImage(), hue );
    return QPixmap::fromImage( image );
}

QPixmap changeToGrayscale(const QPixmap& pixmap)
{
    QImage image = applySinglePixelFilter( Filter_Type::Grayscale, pixmap.toImage(), 0 );
    return QPixmap::fromImage( image );
}

QPixmap changeToNegative(const QPixmap& pixmap)
{
    QImage image = applySinglePixelFilter( Filter_Type::Negative, pixmap.toImage(), 0 );
    return QPixmap::fromImage( image );
}


//####################################################################################
//##        Loops through image and changes one pixel at a time based on a
//##        premultiplied table
//####################################################################################
QImage applySinglePixelFilter( Filter_Type filter, const QImage& from_image, int value )
{
    QImage image = from_image;
    image.detach();

    if ( image.format() != QImage::Format::Format_ARGB32 )
        image = image.convertToFormat( QImage::Format_ARGB32 );

    QTime time_it;
    time_it.restart();


    if (image.colorCount() == 0 )      // i.e. Truecolor Rgba
    {
        // Just in case


        int table[ 256 ];
        for ( int i = 0; i < 256; ++i ) {
            switch (filter)
            {
            case Filter_Type::Brightness:   table[i] = Dr::Clamp( i + value, 0, 255 );                                      break;
            case Filter_Type::Contrast:     table[i] = Dr::Clamp( (( i - 127 ) * (value + 100) / 100 ) + 127, 0, 255 );     break;
            default: ;
            }
        }

        if ( image.hasAlphaChannel() ) {

            // Loop through every pixel
            for( int y = 0; y < image.height(); ++y ) {
                QRgb* line = reinterpret_cast<QRgb*>( image.scanLine( y ));
                for( int x = 0; x < image.width(); ++x ) {

                    // Grab the current pixel color
                    QColor color = QColor::fromRgba( line[x] );
                    double temp;

                    switch (filter)
                    {
                    case Filter_Type::Brightness:
                    case Filter_Type::Contrast:
                        color.setRed(   table[color.red()]   );
                        color.setGreen( table[color.green()] );
                        color.setBlue(  table[color.blue()]  );
                        break;
                    case Filter_Type::Saturation:
                        color.setHsv(color.hue(), Dr::Clamp(color.saturation() + value, 0, 255), color.value(), color.alpha());
                        break;
                    case Filter_Type::Hue:
                        color.setHsv(Dr::Clamp(color.hue() + value, 0, 720), color.saturation(), color.value(), color.alpha());
                        break;
                    case Filter_Type::Grayscale:
                        temp = (color.redF() * 0.21) + (color.greenF() * 0.72) + (color.blueF() * 0.07);
                        color.setRgbF(temp, temp, temp, color.alphaF());
                        break;
                    case Filter_Type::Negative:
                        color.setRgbF(1.0 - color.redF(), 1.0 - color.greenF(), 1.0 - color.blueF(), color.alphaF());
                        break;
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

    // !!!!! DEBUG: Timer that shows how long it took to apply filter
    //if (filter == Filter_Type::Hue)
    //    Dr::SetLabelText(Label_Names::Label_2, "   Hue  Time: " + QString::number(time_it.elapsed()) );
    //else
    //    Dr::SetLabelText(Label_Names::Label_1, " Filter Time: " + QString::number(time_it.elapsed()) );

    return image;
}








}













