//
//      Created by Stephens Nunnally on 1/21/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Helpful QWidget / QObject functions
//
//

#include <QGraphicsDropShadowEffect>

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
//##        Gives widget drop shadow
//####################################################################################
void ApplyDropShadow(QWidget *target_widget, qreal blur_radius, qreal offset_x, qreal offset_y, QColor shadow_color)
{
    QGraphicsDropShadowEffect *shadow_effect;
    shadow_effect = new QGraphicsDropShadowEffect();
    shadow_effect->setBlurRadius(blur_radius);
    shadow_effect->setOffset(offset_x, offset_y);
    shadow_effect->setColor(shadow_color);
    target_widget->setGraphicsEffect(shadow_effect);
}
void ApplyDropShadowByType(QWidget *target_widget, Shadow_Types shadow_type)
{
    switch (shadow_type) {
    case Shadow_Types::Button_Shadow:   ApplyDropShadow(target_widget, 6, 0, 3, QColor(10, 10, 10));
    }
}



//####################################################################################
//##        Gives widget rounded corners
//####################################################################################
// Radius is percent of total rect size (0 - 100)
void ApplyRoundedCornerMask(QWidget *widget, int x_radius, int y_radius)
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


