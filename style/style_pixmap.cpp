//
//      Created by Stephens Nunnally on 8/27/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "colors/colors.h"
#include "helper.h"
#include "imaging/imaging.h"
#include "style.h"


namespace Dr {



//####################################################################################
//##    Turns Image into a Mask of the spcified color
//####################################################################################
QImage ImageMask(const QImage &from_image, QColor mask_color, int max_alpha) {
    QImage image = from_image;
    QVector<QRgb*> lines = DrImaging::getScanLines(image);

    // Loop through every pixel, if alpha is below threshold, set to mask color
    for (int y = 0; y < image.height(); ++y) {
        QRgb* line = reinterpret_cast<QRgb*>( image.scanLine( y ));
        for (int x = 0; x < image.width(); ++x) {
            QColor color = QColor::fromRgba( line[x] );
            line[x] = (color.alpha() < max_alpha) ? 0 : mask_color.rgba();
        }
    }
    return image;
}

//####################################################################################
//##    Creates a circular mask to limit to QPixmap shape
//####################################################################################
void SetMaskCircleShape (QPixmap &pix) {
    QPixmap mask(pix.width(), pix.height());
    mask.fill(Qt::green);
    QPainter painter(&mask);
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::red);
    painter.drawEllipse(0, 0, pix.width(), pix.height());
    painter.end();
    pix.setMask( mask.createMaskFromColor(Qt::green) );
}



}   // End namespace Dr



