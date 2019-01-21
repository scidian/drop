//
//      Created by Stephens Nunnally on 1/21/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Helpful QWidget / QObject functions
//
//

#include "library.h"

namespace Dr {


//####################################################################################
//##        Returns true if 'number_desired' is within +-'tolerance' of 'number_to_check'
//####################################################################################
//
bool IsCloseTo(double number_desired, double number_to_check, double tolerance)
{
    return ( (number_to_check <= (number_desired + tolerance)) && (number_to_check >= (number_desired - tolerance)) );
}



//####################################################################################
//##        Used to show a modal messagebox
//####################################################################################
void ShowMessageBox(QString new_message)
{
    QMessageBox *msgBox = new QMessageBox(nullptr);
    msgBox->setText(new_message);
    msgBox->exec();
}


//####################################################################################
//##        Gives widget form shape rounded corners
//####################################################################################
// Radius is percent of total rect size (0 - 100)
void WidgetMaskRoundedCorners(QWidget *widget, int x_radius, int y_radius)
{
    ///// ALTERNATE: Creates a rounded path aand applies as mask, produces jagged corners on small rects
    ///QPainterPath path;
    ///path.addRoundedRect(widget->rect(), x_radius, y_radius, Qt::SizeMode::RelativeSize);
    ///QRegion mask = QRegion(path.toFillPolygon().toPolygon());
    ///widget->setMask(mask);

    QRectF rect = widget->rect();
    int   width = static_cast<int>(rect.width());
    int  height = static_cast<int>(rect.height());

    QPixmap pixmap(width, height);
    pixmap.fill();
    QPainter paint(&pixmap);
    paint.setPen( QPen(Qt::NoPen) );
    paint.setBrush( QBrush( Qt::green ));
    paint.fillRect(QRectF( 0, 0, rect.width(), rect.height()), Qt::green);
    paint.setBrush( QBrush( QColor(255, 0, 0), Qt::BrushStyle::SolidPattern ));

    // Draw the rounded rect, this will be the part we keep
    paint.drawRoundRect(0, 0, width, height, x_radius, y_radius);
    widget->setMask(pixmap.createMaskFromColor(Qt::green));
}


}


