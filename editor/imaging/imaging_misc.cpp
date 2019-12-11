//
//      Created by Stephens Nunnally on 9/2/2019, (c) 2019 Scidian Software, All Rights Reserved
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
#include <QTime>
#include <QVector2D>

#include <limits>

#include "editor/colors/colors.h"
#include "editor/globals_editor.h"
#include "editor/helper_library.h"
#include "editor/imaging/imaging.h"
#include "library/dr_math.h"


namespace DrImaging
{


//####################################################################################
//##    Returns array of scanlines that are a direct access to QImage pixels
//####################################################################################
std::vector<QRgb*> GetScanLines(QImage &image) {
    std::vector<QRgb*> lines;

    if (image.format() != QImage::Format::Format_ARGB32)
        image = image.convertToFormat( QImage::Format_ARGB32 );
    image.detach();

    // Truecolor Rgba
    if (image.colorCount() == 0) {
        if (image.hasAlphaChannel()) {

            // Grab all the scan lines
            for (int y = 0; y < image.height(); ++y)
                lines.push_back( reinterpret_cast<QRgb*>(image.scanLine(y)) );

        } else {    Dr::ShowMessageBox("Image missing alpha channel!"); }
    } else {    Dr::ShowMessageBox("Image only has 256 colors!"); }

    return lines;
}


//####################################################################################
//##    Returns average color of a Pixmap
//####################################################################################
QColor AverageColor(const QPixmap &pixmap, bool screen_shot) {
    QPixmap  one(20, 20);
             one.fill(Qt::transparent);
    QPainter painter(&one);
    painter.drawPixmap(one.rect(), pixmap, pixmap.rect());
    painter.end();

    QImage image = one.toImage();
    double r = 0, g = 0, b = 0;
    double count = 0;
    for (int i = 0; i < one.width(); ++i) {
        for (int j = 0; j < one.height(); ++j) {
            QColor pixel = image.pixelColor(i, j);
            r += pixel.red() *   pixel.alphaF();
            g += pixel.green() * pixel.alphaF();
            b += pixel.blue() *  pixel.alphaF();
            count += pixel.alphaF();
        }
    }

    if (screen_shot) {
        QString file_name = QStandardPaths::writableLocation(QStandardPaths::StandardLocation::DesktopLocation);
        file_name += "/average_color";
        file_name += " - " + QDateTime::currentDateTime().toString("yyyy-MM-dd-hhmmss-zzz") + ".png";
        image.save( file_name );
    }

    int red =   static_cast<int>(r / count);
    int green = static_cast<int>(g / count);
    int blue =  static_cast<int>(b / count);

    red =   Dr::Clamp(red,   0, 255);
    green = Dr::Clamp(green, 0, 255);
    blue =  Dr::Clamp(blue,  0, 255);

    return QColor(red, green, blue);
}


//####################################################################################
//##    Returns an array of bits from an Image as aRgb float values
//####################################################################################
float* ImageBitsAsFloat(const QImage &from_image) {
    QImage image = from_image;
    std::vector<QRgb*> lines = GetScanLines(image);

    // Method #1
    ///float *out = static_cast<float*>( malloc(static_cast<std::size_t>(image.width() * image.height()) * sizeof(float)) );
    ///int index = 0;
    ///for (size_t y = 0; y < image.height(); ++y) {
    ///    for (size_t x = 0; x < image.width(); ++x) {
    ///        out[index] = lines[y][x];
    ///        index++;
    ///    }
    ///}

    // Method #2
    float *out = reinterpret_cast<float*>(image.bits());

    return out;
}



}   // End Namespce DrImaging







