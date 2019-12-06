//
//      Created by Stephens Nunnally on 9/20/2019, (c) 2019 Scidian Software, All Rights Reserved
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

#include "editor/colors/colors.h"
#include "editor/globals_editor.h"
#include "editor/imaging/imaging.h"
#include "library/helper.h"


namespace DrImaging
{

//####################################################################################
//##    Copies rect from source_image to dest_image
//####################################################################################
void copyImage(QImage &dest_image, QImage &source_image, QRect rect) {
    QVector<QRgb*> dest_lines =   getScanLines(dest_image);
    QVector<QRgb*> source_lines = getScanLines(source_image);

    for (int y = rect.y(); y < rect.y() + rect.height(); ++y) {
        for (int x = rect.x(); x < rect.x() + rect.width(); ++x) {
            dest_lines[y][x] = source_lines[y][x];
        }
    }
}

//####################################################################################
//##    Fills image with color inside rect
//####################################################################################
void fillImage(QImage &image, QColor color, QRect rect) {
    QVector<QRgb*> lines = getScanLines(image);

    for (int y = rect.y(); y < rect.y() + rect.height(); ++y) {
        for (int x = rect.x(); x < rect.x() + rect.width(); ++x) {
            lines[y][x] = color.rgba();
        }
    }
}




}   // End namespace DrImaging
