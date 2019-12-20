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
#include <QPainter>
#include <QPixmap>
#include <QRgb>
#include <QTime>
#include <QVector2D>
#include <QVector3D>

#include "editor/globals_editor.h"
#include "editor/pixmap/pixmap.h"
#include "library/colors/colors.h"
#include "library/dr_math.h"
#include "library/dr_random.h"

namespace Dr
{


// Internal Constants
const float PI =    3.14159f;
const float RAD =   2.0f * PI;      // 2.0 * PI is 360 in radians
const float DEG =   0.0174533f;     // One Degree in Radians is 0.0174533


//####################################################################################
//##    Draws a DrEffectType::Light as a Pixmap
//##        !! Implemented same algorithm as 2d_light_frag.glsl
//####################################################################################
QPixmap DrawLight(QColor color, int diameter, float cone_start, float cone_end, float intensity, float blur) {
    QPixmap light(diameter, diameter);
    light.fill(Qt::transparent);

    QImage image = light.toImage();
    std::vector<QRgb*> lines = GetScanLines(image);

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
    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {

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
            double amount = Dr::Lerp(1.0, 0.0, static_cast<double>(r)) * static_cast<double>(intensity);

            // Add some noise to reduce rings
            double random_r = (Dr::RandomDouble(0.0, 10.0) - 5.0);
            double random_g = (Dr::RandomDouble(0.0, 10.0) - 5.0);
            double random_b = (Dr::RandomDouble(0.0, 10.0) - 5.0);
            double random_a = (Dr::RandomDouble(0.0, 10.0) - 5.0);

            // Multiply by light color
            int red =   Dr::Clamp(static_cast<int>((color.red() * amount)   + random_r),  0, 255 );
            int green = Dr::Clamp(static_cast<int>((color.green() * amount) + random_g),  0, 255 );
            int blue =  Dr::Clamp(static_cast<int>((color.blue() * amount)  + random_b),  0, 255 );
            int alpha = Dr::Clamp(static_cast<int>((255.0 * static_cast<double>(opacity) * amount) + random_a), 0, 255 );

            QColor new_color = QColor(red, green, blue, 255);
            new_color.setAlpha(alpha);

            lines[y][x] = static_cast<uint>(new_color.rgba());
        }
    }

    return QPixmap::fromImage(image);
}


//####################################################################################
//##    Draws a DrEffectType::Fire as a Pixmap
//####################################################################################
QPixmap DrawFire(QColor color_1, QColor color_2, QColor smoke, Fire_Mask mask) {
    int width =  static_cast<int>(c_image_size * 0.625);
    int height = c_image_size;
    int border = c_image_border;

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
        case Fire_Mask::Square:
            painter.drawRect(border, border, width - (border * 2), (height * 2)  - (border * 2));
            break;
        case Fire_Mask::Triangle: {
            QPainterPath flame;
            flame.moveTo( QPointF( width / 2, border) );
            flame.lineTo( QPointF( 0,     height + border) );
            flame.lineTo( QPointF( width, height + border) );
            flame.lineTo( QPointF( width / 2, border) );
            painter.drawPath(flame);
            break;
        }
    }

    return fire;
}


//####################################################################################
//##    Draws a DrEffectType::Fisheye as a Pixmap
//####################################################################################
QPixmap DrawFisheye(QColor color) {
    int width =  c_image_size;
    int height = c_image_size;
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
//##    Draws a DrEffectType::Mirror as a Pixmap
//####################################################################################
QPixmap DrawMirror(QColor top_color, QColor bottom_color) {
    int width =  c_image_size;
    int height = c_image_size;
    int border = c_image_border;
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
//##    Draws a Fibonacci Swirl, was the original draw routine for DrEngineSwirl
//####################################################################################
QPixmap DrawFibonacci(QColor background_color, QColor pen_color) {
    int width =  c_image_size;
    int height = c_image_size;
    QPixmap swirl(width, height);
    swirl.fill(Qt::transparent);

    // Draws a circle
    background_color.setAlphaF(background_color.alphaF() * 0.5);                  // Decrease lens opacity by half
    QPainter painter(&swirl);
    painter.setPen(Qt::NoPen);
    painter.setBrush(background_color);
    painter.drawEllipse(0, 0, width, height);
    painter.end();

    // Prep painter
    painter.begin(&swirl);
    painter.translate(width / 2, height / 2);
    painter.setBrush(Qt::NoBrush);
    painter.setPen( QPen(pen_color, c_image_border) );
    QPainterPath spiral;
    spiral.moveTo(0, 0);

    // Create a fibonacci sequence
    std::vector<double> fib;
    fib.push_back(1.0);
    fib.push_back(2.0);
    double adjust = 1.00;                                   // Minimum of 0.5
    for (size_t i = 2; i < 40; ++i) {
        fib.push_back( (fib[i - 2] + fib[i - 1]) * adjust );
    }

    // Draw the fibonacci spiral
    int angle = 180;
    for (size_t i = 0; i < fib.size() - 1; ++i) {
        QRectF arc = QRectF( spiral.currentPosition().x() - fib[i], spiral.currentPosition().y() - fib[i], fib[i] * 2.0, fib[i] * 2.0 );
        painter.drawArc( arc, angle * 16, 90 * 16 );
        spiral.arcTo( arc, angle, 90.0 );

        double move = fib[i + 1];
        if (i % 4 == 0) {
            spiral.moveTo( spiral.currentPosition().x(),        spiral.currentPosition().y() - move );
        } else if (i % 4 == 1) {
            spiral.moveTo( spiral.currentPosition().x() - move, spiral.currentPosition().y());
        } else if (i % 4 == 2) {
            spiral.moveTo( spiral.currentPosition().x(),        spiral.currentPosition().y() + move );
        } else if (i % 4 == 3) {
            spiral.moveTo( spiral.currentPosition().x() + move, spiral.currentPosition().y());
        }
        angle += 90;
    }
    ///painter.drawPath(spiral);
    painter.end();

    // Create a mask to limit to circle shape
    Dr::SetMaskCircleShape(swirl);

    return swirl;
}


//####################################################################################
//##    Draws a DrEffectType::Swirl as a Pixmap
//####################################################################################
QPixmap DrawSwirl(QColor color, double angle) {
    // Initialize pixmap
    int width =  c_image_size;
    int height = c_image_size;
    QPixmap swirl(width, height);
    color.setAlphaF(color.alphaF() * 0.5);                  // Decrease lens opacity by half
    swirl.fill(color);

    // Draws a cross that can be swirled
    QPainter painter(&swirl);
    painter.setPen( QPen(Qt::black, c_image_border));
    painter.drawLine(0, 0, width, height);
    painter.drawLine(width, 0, 0, height);
    painter.end();

    // Swirl the image
    QImage source = swirl.toImage();
    QImage dest   = swirl.toImage();
    std::vector<QRgb*> source_lines =   GetScanLines(source);
    std::vector<QRgb*> dest_lines =     GetScanLines(dest);

    // Loop through every pixel and perform swirl
    double radius = width / 2.0;
    angle /= 100.0;
    for (size_t y = 0; y < static_cast<size_t>(source.height()); ++y ) {
        for (size_t x = 0; x < static_cast<size_t>(source.width()); ++x ) {
            QPointF center   { width / 2.0, height / 2.0 };
            QPointF tex_size { width / 1.0, height / 1.0 };
            QPointF tc { double(x), double(y) };
            tc -= center;
            double dist = QLineF(0, 0, tc.x(), tc.y()).length();
            double percent = (radius - dist) / radius;
            double theta = percent * percent * angle * 8.0;
            float s = static_cast<float>(sin(theta));
            float c = static_cast<float>(cos(theta));
            tc = QPointF( static_cast<double>(QVector3D::dotProduct( QVector3D(float(tc.x()), float(tc.y()), 0.0), QVector3D(c, -s, 0.0))),
                          static_cast<double>(QVector3D::dotProduct( QVector3D(float(tc.x()), float(tc.y()), 0.0), QVector3D(s,  c, 0.0))) );
            tc += center;

            int fx = static_cast<int>(tc.x());
            int fy = static_cast<int>(tc.y());
            if (fx >= 0 && fx < source.width() && fy >= 0 && fy < source.height())
                dest_lines[y][x] = source_lines[static_cast<size_t>(fx)][static_cast<size_t>(fy)];
        }
    }

    // Copy image back into pixmap, apply circular mask
    swirl = QPixmap::fromImage(dest);
    Dr::SetMaskCircleShape(swirl);

    return swirl;
}


//####################################################################################
//##    Draws a DrEffectType::Water as a Pixmap
//####################################################################################
QPixmap DrawWater(QColor top_color, QColor bottom_color) {
    int width =  c_image_size;
    int height = c_image_size;
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




}   // End Dr Namespace







