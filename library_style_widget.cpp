//
//      Created by Stephens Nunnally on 3/13/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QGraphicsDropShadowEffect>
#include <QGuiApplication>
#include <QScreen>
#include <QStyle>
#include <QWindow>

#include "colors.h"
#include "library.h"

namespace Dr {


//####################################################################################
//##        Centers form_to_center on screen,
//##            Tries to use whatever screen parent_to_find_screen_from is on
//####################################################################################
void CenterFormOnScreen(QWidget *parent_to_find_screen_from, QWidget *form_to_center)
{
    QWidget *parent = parent_to_find_screen_from;
    QScreen *screen = nullptr;
    if (parent) {
        if (parent->window()) {
            if (parent->window()->windowHandle()) {
                screen = parent->window()->windowHandle()->screen();
            }
        }
    }
    QRect screen_geometry;
    if (screen) screen_geometry = screen->availableGeometry();
    else        screen_geometry = QGuiApplication::screens().first()->geometry();
    form_to_center->setGeometry(QStyle::alignedRect( Qt::LeftToRight, Qt::AlignCenter, form_to_center->size(), screen_geometry ));


    /// !! To find screen at mouse position:
    ///QGuiApplication::screenAt(QPoint(0, 0));

    /// !! To get the pixel somewhere on the screen:
    /**
    QPixmap *a = new QPixmap;
    *a = QPixmap::grabWindow(QApplication::desktop()->winId());
    QImage *img = new QImage;
    *img = a->toImage();
    QRgb b = img->pixel(10,33);
    QColor *c = new QColor;
    c->setRgb(b);
    QString color_name = c->name();
    */

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
    case Shadow_Types::Button_Shadow:   ApplyDropShadow(target_widget, 6,  0,  3, Dr::GetColor(Window_Colors::Shadow) );        break;
    case Shadow_Types::Tool_Tip_Shadow: ApplyDropShadow(target_widget, 4,  0,  3, Dr::GetColor(Window_Colors::Shadow) );        break;
    }
}



//####################################################################################
//##        Gives widget rounded corners,
//####################################################################################
// Radius is absolute size
void ApplyRoundedCornerMask(QWidget *widget, int x_radius, int y_radius, int method)
{
    QPixmap pixmap(widget->rect().width(), widget->rect().height());
    pixmap.fill(Qt::green);
    QPainter paint(&pixmap);
    paint.setRenderHint(QPainter::Antialiasing, true);
    paint.setPen( QPen(Qt::NoPen) );

    // Draw the rounded rect, this will be the part we keep. Played around with two ways to do the same thing
    if (method == 0) {
        QPainterPath path;
        path.addRoundedRect(pixmap.rect(), x_radius, y_radius, Qt::SizeMode::AbsoluteSize);
        paint.fillPath(path, Qt::red);
    } else {
        paint.setBrush( Qt::red );
        paint.drawRoundedRect(0, 0, widget->rect().width(), widget->rect().height(), x_radius, y_radius);
    }

    widget->setMask(pixmap.createMaskFromColor(Qt::green));
}


//####################################################################################
//##        Gives widget rounded corners, and a pointed top
//####################################################################################
// Radius is absolute size
void ApplyPopupMask(QWidget *widget, int x_radius, int y_radius)
{
    QPixmap pixmap(widget->rect().width(), widget->rect().height());
    pixmap.fill(Qt::green);
    QPainter paint(&pixmap);
    paint.setRenderHint(QPainter::Antialiasing, true);
    paint.setPen( QPen(Qt::NoPen) );

    paint.setBrush( Qt::red );
    paint.drawRoundedRect(0, 10, widget->rect().width(), widget->rect().height() - 10, x_radius, y_radius);

    QPoint points[3];
    points[0] = QPoint(widget->rect().width() / 2, 0);
    points[1] = QPoint((widget->rect().width() / 2) - 30, 30);
    points[2] = QPoint((widget->rect().width() / 2) + 30, 30);
    paint.drawPolygon(points, 3);

    widget->setMask(pixmap.createMaskFromColor(Qt::green));
}


//####################################################################################
//##        These functions check the width of text that would be drawn
//##        with a particular font
//####################################################################################
int CheckFontWidth(QFont font, QString text_to_check)
{
    QFontMetrics font_metrics { font };
    return font_metrics.horizontalAdvance( text_to_check );
}

// Shortens label text to fit within a widget (max_width), used in editor_tree_assets frames
QString FitStringToWidth(QFont font, QString text_to_check, int max_width, bool use_dots)
{
    QString text = text_to_check;
    QFontMetrics font_metrics { font };
    int width = font_metrics.horizontalAdvance( text_to_check );

    int length = text.length();
    while (width > max_width && length >= 1) {
        --length;
        text = text_to_check.left(length);
        if (use_dots) text += "...";
        width = font_metrics.horizontalAdvance( text );
    }
    return text;
}



}   // namespace Dr












