//
//      Created by Stephens Nunnally on 8/27/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//

#include "colors/colors.h"
#include "style.h"


namespace Dr {


//####################################################################################
//##        Creates a circular mask to limit to QPixmap shape
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



