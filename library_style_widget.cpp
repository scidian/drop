//
//      Created by Stephens Nunnally on 3/13/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QGraphicsDropShadowEffect>


#include "colors.h"
#include "library.h"

namespace Dr {


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
    case Shadow_Types::Button_Shadow:   ApplyDropShadow(target_widget, 6,  0,  3, Dr::GetColor(Window_Colors::Shadow) );        break;
    case Shadow_Types::Tool_Tip_Shadow: ApplyDropShadow(target_widget, 4,  0,  3, Dr::GetColor(Window_Colors::Shadow) );        break;
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
    paint.setRenderHint(QPainter::Antialiasing, true);
    paint.setPen( QPen(Qt::NoPen) );
    paint.setBrush( QBrush( Qt::green ));
    paint.fillRect(QRectF( 0, 0, rect.width(), rect.height()), Qt::green);
    paint.setBrush( QBrush( QColor(255, 0, 0), Qt::BrushStyle::SolidPattern ));

    // Draw the rounded rect, this will be the part we keep
    paint.drawRoundedRect(0, 0, width, height, x_radius, y_radius);
    widget->setMask(pixmap.createMaskFromColor(Qt::green));
}


// Shortens label text to fit within a widget (i.e. asset frame)
QString CheckFontWidth(QFont font, QString text_to_check, int max_width)
{
    QString text = text_to_check;
    QFontMetrics fm { font };
    int width = fm.width( text );

    int length = text.length();
    while (width > max_width && length >= 1) {
        --length;
        text = text_to_check.left(length) + "...";
        width = fm.width( text );
    }
    return text;
}


}   // namespace Dr












