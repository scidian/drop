//
//      Created by Stephens Nunnally on 8/22/2019, (c) 2019 Scidian Software, All Rights Reserved
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
//##        Draws a DrEffectType::Light as a Pixmap
//##            !! Implemented same algorithm as 2d_light_frag.glsl
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
//##        Draws a DrEffectType::Fire as a Pixmap
//####################################################################################
QPixmap drawFire(QColor color_1, QColor color_2, QColor smoke, Fire_Mask mask) {
    int width =  250;
    int height = 400;
    int border = 6;

    QPixmap fire(width, height);
    fire.fill(Qt::transparent);

    QLinearGradient gradient(0, 0, 0, height);
    gradient.setColorAt(0.00, color_1);
    gradient.setColorAt(1.00, color_2);

    QPainter painter(&fire);
    painter.setPen( QPen(smoke, border * 2 ));
    painter.setBrush(gradient);

    switch (mask) {
        case Fire_Mask::Torch:
            painter.drawEllipse(border, border, width - (border * 2), (height * 2)  - (border * 2));
            break;
        case Fire_Mask::Candle: {
            ///QPainterPath flame;
            ///flame.moveTo(width / 2, height);
            ///flame.cubicTo( -width / 2.2, height * 0.75, width / 4, height * 0.75, width / 2, 0);
            ///flame.moveTo(width / 2, height);
            ///flame.cubicTo( width + (width / 2.2), height * 0.75, width - (width / 4), height * 0.75, width / 2, 0);

            QPointF tl =    QPointF(width / 10,             0);
            QPointF bl =    QPointF(width / 40,             height - height / 40);
            QPointF br =    QPointF(width - (width / 40),   height - height / 40);
            QPointF tr =    QPointF(width - (width / 10),   0);

            QPointF top =   QPointF(width / 2,      border);
            QPointF left =  QPointF(border,         height - (height / 3));
            QPointF bot =   QPointF(width / 2,      height - border);
            QPointF right = QPointF(width - border, height - (height / 3));

            QPainterPath flame(top);
            flame.quadTo(tl, left);
            flame.quadTo(bl, bot);
            flame.quadTo(br, right);
            flame.quadTo(tr, top);
            painter.drawPath(flame);
            break;
        }
    }

    return fire;
}


//####################################################################################
//##        Draws a DrEffectType::Fisheye as a Pixmap
//####################################################################################
QPixmap drawFisheye(QColor color) {
    int width =  400;
    int height = 400;
    QPixmap lens(width, height);
    lens.fill(Qt::transparent);
    color.setAlphaF(color.alphaF() * 0.5);                  // Decrease lens opacity by half

    QPainter painter(&lens);
    painter.setPen(Qt::NoPen);
    painter.setBrush(color);
    painter.drawEllipse(0, 0, width, height);

    return lens;
}

//####################################################################################
//##        Draws a DrEffectType::Mirror as a Pixmap
//####################################################################################
QPixmap drawMirror(QColor top_color, QColor bottom_color) {
    int width =  400;
    int height = 400;
    int border = 4;
    QPixmap mirror(width, height);
    mirror.fill(Qt::transparent);

    QLinearGradient gradient(0, 0, 0, height);
    gradient.setColorAt(0.00, top_color);
    gradient.setColorAt(1.00, bottom_color);

    QPainter painter(&mirror);
    painter.setPen( QPen(QColor(176, 118, 17), border * 2));
    painter.setBrush( gradient );
    painter.drawRoundedRect( QRectF(border, border, width - (border * 2), height - (border * 2)), border, border);

    return mirror;
}


//####################################################################################
//##        Draws a DrEffectType::Swirl as a Pixmap
//####################################################################################
QPixmap drawSwirl(QColor color) {
    int width =  400;
    int height = 400;
    QPixmap swirl(width, height);
    swirl.fill(Qt::transparent);
    color.setAlphaF(color.alphaF() * 0.5);                  // Decrease lens opacity by half

    QPainter painter(&swirl);
    painter.setPen(Qt::NoPen);
    painter.setBrush(color);
    painter.drawEllipse(0, 0, width, height);

    return swirl;
}


//####################################################################################
//##        Draws a DrEffectType::Water as a Pixmap
//####################################################################################
QPixmap drawWater(QColor top_color, QColor bottom_color) {
    int width =  400;
    int height = 400;
    QPixmap water(width, height);
    water.fill(top_color);

    QLinearGradient gradient(0, 0, 0, height);
    gradient.setColorAt(0.00, top_color);
    gradient.setColorAt(1.00, bottom_color);

    QPainter painter(&water);
    painter.setPen(Qt::NoPen);
    painter.setBrush(gradient);
    painter.drawRect(0, 0, width, height);

    return water;
}




}   // End DrImaging Namespace







