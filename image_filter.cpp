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
    image.detach();
    if ( image.format() != QImage::Format::Format_ARGB32 )
        image = image.convertToFormat( QImage::Format_ARGB32 );

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
            for( int y = 0; y < image.height(); ++y ) {
                QRgb* line = reinterpret_cast<QRgb*>( image.scanLine( y ));
                for( int x = 0; x < image.width(); ++x ) {

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
    image.detach();
    if ( image.format() != QImage::Format::Format_ARGB32 )
        image = image.convertToFormat( QImage::Format_ARGB32 );

    // Truecolor Rgba
    if (image.colorCount() == 0 ) {
        if ( image.hasAlphaChannel() ) {

            // Grab all the scan lines
            QVector<QRgb*> lines;
            for( int y = 0; y < image.height(); ++y ) {
                lines.append( reinterpret_cast<QRgb*>(image.scanLine(y)) );
            }

            // Loop through every pixel
            for( int y = 0; y < image.height(); ++y ) {
                for( int x = 0; x < image.width(); ++x ) {
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
//##        Draws a DrEngineLight as a Pixmap, implemented same algorithm as 2d_light_frag.glsl
//####################################################################################
// Constants
const float PI =    3.14159f;
const float RAD =   2.0f * PI;      // 2.0 * PI is 360 in radians
const float DEG =   0.0174533f;     // One Degree in Radians is 0.0174533

QPixmap drawLight(QColor color, int diameter, float cone_start, float cone_end, float intensity, float blur) {


    QPixmap light(diameter, diameter);
    light.fill(Qt::transparent);

    QImage image = light.toImage();
    image.detach();
    if ( image.format() != QImage::Format::Format_ARGB32 )
        image = image.convertToFormat( QImage::Format_ARGB32 );

    // Truecolor Rgba
    if (image.colorCount() == 0 ) {
        if ( image.hasAlphaChannel() ) {

            // Grab all the scan lines
            QVector<QRgb*> lines;
            for( int y = 0; y < image.height(); ++y ) {
                lines.append( reinterpret_cast<QRgb*>(image.scanLine(y)) );
            }

            float cone_1 = qDegreesToRadians(cone_start);
            float cone_2 = qDegreesToRadians(cone_end);
            if (cone_1 < 0.0f) cone_1 += RAD;
            if (cone_2 < 0.0f) cone_2 += RAD;

            if (intensity >= 50.0f) {
                intensity = (intensity - 40.0f) / 10.f;
            } else {
                intensity /= 50.0f;
            }
            blur +=         0.001f;
            float alpha =   1.0f;

            // Loop through every pixel
            for( int y = 0; y < image.height(); ++y ) {
                for( int x = 0; x < image.width(); ++x ) {

                    // Rectangular to Polar
                    QVector2D norm = QVector2D( x - (image.width() / 2.0f), (image.height() / 2.0f) - y );
                    float theta =    static_cast<float>(qAtan2( static_cast<double>(norm.y()), static_cast<double>(norm.x()) ));
                    float r =        norm.length() / (image.width() / 2.0f);
                    float opacity =  alpha;

                    // Check that pixel is within allowed light cone
                    if (theta < 0.0f) theta += RAD;                             // Add 360 degrees in radians if theta is less than zero
                    if (cone_1 > cone_2) {
                        if (theta < cone_1 && theta > cone_2) {
                            float diff_x = Dr::Lerp(0.0f, 1.0f, ((cone_1 - theta) / DEG) * (1.0f/blur));
                            float diff_y = Dr::Lerp(0.0f, 1.0f, ((theta - cone_2) / DEG) * (1.0f/blur));
                            opacity -= Dr::Min(diff_x, diff_y);
                            if (opacity <= 0.01f) continue;
                        }
                    } else {
                        if (theta < cone_1 || theta > cone_2) {
                            float diff_x = 0.0f, diff_y = 0.0f;
                            if (theta > cone_2) {
                                diff_x = Dr::Lerp(0.0f, 1.0f, ((cone_1 + RAD - theta) / DEG) * (1.0f/blur));
                                diff_y = Dr::Lerp(0.0f, 1.0f, ((theta - cone_2)       / DEG) * (1.0f/blur));
                            } else if (theta < cone_1) {
                                diff_x = Dr::Lerp(0.0f, 1.0f, ((cone_1 - theta)       / DEG) * (1.0f/blur));
                                diff_y = Dr::Lerp(0.0f, 1.0f, ((theta + RAD - cone_2) / DEG) * (1.0f/blur));
                            }
                            opacity -= Dr::Min(diff_x, diff_y);
                            if (opacity <= 0.01f) continue;
                        }
                    }

                    // Multiply the intensity by our distance, which gives us a radial falloff
                    float amount = Dr::Lerp(1.0f, 0.0f, r) * intensity;

                    // Multiply by light color
                    int red =   Dr::Clamp(static_cast<int>(color.red() * amount),      0, 255 );
                    int green = Dr::Clamp(static_cast<int>(color.green() * amount),    0, 255 );
                    int blue =  Dr::Clamp(static_cast<int>(color.blue() * amount),     0, 255 );
                    int alpha = Dr::Clamp(static_cast<int>(255.0f * opacity * amount), 0, 255 );

                    QColor new_color = QColor(red, green, blue, 255);
                    new_color.setAlpha(alpha);

                    lines[y][x] = static_cast<uint>(new_color.rgba());
                }
            }

        } else {    Dr::ShowMessageBox("Image missing alpha channel!"); }
    } else {    Dr::ShowMessageBox("Image only has 256 colors!"); }

    return QPixmap::fromImage(image);
}


//####################################################################################
//##        Draws a DrEngineWater as a Pixmap
//####################################################################################
QPixmap drawWater(QColor top_color, QColor bottom_color) {
    QPixmap water(400, 400);
    water.fill(top_color);

    QLinearGradient gradient(0, 0, 0, 400);
    gradient.setColorAt(0.00, top_color);
    gradient.setColorAt(1.00, bottom_color);

    QPainter painter(&water);
    painter.setPen(Qt::NoPen);
    painter.setBrush(gradient);
    painter.drawRect(0, 0, 400, 400);

    return water;
}


//####################################################################################
//##        Draws a DrEngineFisheye as a Pixmap
//####################################################################################
QPixmap drawFisheye(QColor color) {
    QPixmap lens(400, 400);
    lens.fill(Qt::transparent);
    color.setAlphaF(color.alphaF() * 0.5);          // Decrease lens opacity by half

    QPainter painter(&lens);
    painter.setPen(Qt::NoPen);
    painter.setBrush(color);
    painter.drawEllipse(0, 0, 400, 400);

    return lens;
}



}   // End DrImaging Namespace


















