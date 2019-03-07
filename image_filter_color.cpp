//
//      Created by Stephens Nunnally on 3/7/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QtMath>

#include "globals.h"
#include "image_filter_color.h"
#include "library.h"

#include <QImage>
#include <QPixmap>
#include <QRgb>


namespace DrFilter
{


template<class T>
inline const T& kClamp( const T& x, const T& low, const T& high ) {
    if ( x < low )       return low;
    else if ( high < x ) return high;
    else                 return x;
}

inline int changeBrightness( int value, int brightness )    { return kClamp( value + brightness * 255 / 100, 0, 255 ); }
inline int changeContrast( int value, int contrast )        { return kClamp((( value - 127 ) * contrast / 100 ) + 127, 0, 255 ); }
inline int changeGamma( int value, int gamma )              { return kClamp( int( pow( value / 255.0, 100.0 / gamma ) * 255 ), 0, 255 ); }
inline int changeUsingTable( int value, const int table[] ) { return table[ value ]; }

template< int operation( int, int ) >
static
QImage changeImage( const QImage& image, int value )
{
    QImage im = image;
    im.detach();

    Dr::SetLabelText(Label_Names::Label_1, "Color Count: " + QString::number(im.colorCount()));


    if (im.colorCount() == 0 ) /* truecolor */
    {
        // Just in case
        if ( im.format() != QImage::Format_RGB32 ) {
            im = im.convertToFormat( QImage::Format_RGB32 );
            Dr::SetLabelText(Label_Names::Label_2, "Made it RGB32: " + Dr::CurrentTimeAsString());
        }

        int table[ 256 ];
        for ( int i = 0; i < 256; ++i )
            table[ i ] = operation( i, value );

        if ( im.hasAlphaChannel() ) {
            for( int y = 0; y < im.height(); ++y ) {
                QRgb* line = reinterpret_cast<QRgb*>( im.scanLine( y ));
                for( int x = 0; x < im.width(); ++x )
                    line[ x ] = qRgba( changeUsingTable( qRed(   line[ x ] ), table ),
                                       changeUsingTable( qGreen( line[ x ] ), table ),
                                       changeUsingTable( qBlue(  line[ x ] ), table ),
                                       qAlpha( line[ x ] ));

                                       //changeUsingTable( qAlpha( line[ x ] ), table ));
            }

        } else {
            for( int y = 0; y < im.height(); ++y ) {
                QRgb* line = reinterpret_cast<QRgb*>( im.scanLine( y ));
                for( int x = 0; x < im.width(); ++x )
                    line[ x ] = qRgb( changeUsingTable( qRed(   line[ x ] ), table ),
                                      changeUsingTable( qGreen( line[ x ] ), table ),
                                      changeUsingTable( qBlue(  line[ x ] ), table ));
            }
        }
    } else {
        QVector<QRgb> colors = im.colorTable();

        for( int i = 0; i < im.colorCount(); ++i )
            colors[ i ] = qRgb( operation( qRed(   colors[ i ] ), value ),
                                operation( qGreen( colors[ i ] ), value ),
                                operation( qBlue(  colors[ i ] ), value ) );
    }
    return im;
}


// brightness is multiplied by 100 in order to avoid floating point numbers
QImage changeBrightness( const QImage& image, int brightness )
{
    // Range is -255 to 255
    if ( brightness == 0 ) return image;
    return changeImage< changeBrightness >( image, brightness );
}


// contrast is multiplied by 100 in order to avoid floating point numbers
QImage changeContrast( const QImage& image, int contrast )
{
    if ( contrast == 100 ) return image;
    return changeImage< changeContrast >( image, contrast );
}

// gamma is multiplied by 100 in order to avoid floating point numbers
QImage changeGamma( const QImage& image, int gamma )
{
    if( gamma == 100 ) return image;
    return changeImage< changeGamma >( image, gamma );
}

QPixmap changeBrightness(const QPixmap& pixmap, int brightness)
{
    QImage image = pixmap.toImage();
    return QPixmap::fromImage(changeBrightness(image, brightness));
}

QPixmap changeContrast(const QPixmap& pixmap, int contrast)
{
    QImage image = pixmap.toImage();
    return QPixmap::fromImage(changeContrast(image, contrast));
}

QPixmap changeGamma(const QPixmap& pixmap, int gamma)
{
    QImage image = pixmap.toImage();
    return QPixmap::fromImage(changeBrightness(image, gamma));
}

}












