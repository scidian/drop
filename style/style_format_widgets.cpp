//
//      Created by Stephens Nunnally on 3/13/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QGraphicsDropShadowEffect>
#include <QGraphicsEffect>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGuiApplication>
#include <QScreen>
#include <QStyle>
#include <QWindow>

#include "colors/colors.h"
#include "style/style.h"


namespace Dr {


//####################################################################################
//##    Centers form_to_center on screen,
//##        Tries to use whatever screen parent_to_find_screen_from is on, if it can't
//##        find a valid screen, it defaults to centering the window on the primary screen
//####################################################################################
void CenterFormOnScreen(QWidget *parent_to_find_screen_from, QWidget *form_to_center,
                        double width_percentage, double height_percentage) {
    QScreen *screen = nullptr;
    QRect    screen_geometry;
    screen = FindScreenFromWidget(parent_to_find_screen_from);

    // If couldnt find screen from parent, try to grab screen from cursor position, else default to primary or first screen
    if (screen == nullptr) screen = QGuiApplication::screenAt( QCursor::pos() );
    if (screen == nullptr) screen = QGuiApplication::primaryScreen();
    if (screen == nullptr) screen = QGuiApplication::screens().first();
    screen_geometry = screen->availableGeometry();

    QSize new_size = form_to_center->size();
    if (width_percentage  > 0 && width_percentage  <= 100)
        new_size.setWidth( static_cast<int>( screen_geometry.width()  * (width_percentage  / 100.0) ) );
    if (height_percentage > 0 && height_percentage <= 100)
        new_size.setHeight(static_cast<int>( screen_geometry.height() * (height_percentage / 100.0) ) );
    form_to_center->setGeometry(QStyle::alignedRect( Qt::LeftToRight, Qt::AlignCenter, new_size, screen_geometry ));
}

QScreen* FindScreenFromWidget(QWidget *widget) {
    if (widget == nullptr)                              return nullptr;
    if (widget->window() == nullptr)                    return nullptr;
    if (widget->window()->windowHandle() == nullptr)    return nullptr;
    return widget->window()->windowHandle()->screen();
}


//####################################################################################
//##    Gives widget drop shadow
//####################################################################################
void ApplyDropShadow(QWidget *target_widget, qreal blur_radius, qreal offset_x, qreal offset_y, QColor shadow_color) {
    QGraphicsDropShadowEffect *shadow_effect;
    shadow_effect = new QGraphicsDropShadowEffect();
    shadow_effect->setBlurRadius(blur_radius);
    shadow_effect->setOffset(offset_x, offset_y);
    shadow_effect->setColor(shadow_color);
    target_widget->setGraphicsEffect(shadow_effect);
}

void ApplyDropShadowByType(QWidget *target_widget, Shadow_Types shadow_type) {
    switch (shadow_type) {
        case Shadow_Types::Button_Shadow:   ApplyDropShadow(target_widget, 6,  0,  3, Dr::GetColor(Window_Colors::Shadow) );        break;
        case Shadow_Types::Tool_Tip_Shadow: ApplyDropShadow(target_widget, 4,  0,  3, Dr::GetColor(Window_Colors::Shadow) );        break;
    }
}


//####################################################################################
//##    Applies QGraphicsEffect to QImage
//####################################################################################
QImage ApplyEffectToImage(QImage src, QGraphicsEffect *effect, int extent) {
    if (src.isNull()) return QImage();
    if (!effect) return src;

    QGraphicsScene scene;
    QGraphicsPixmapItem item;
        item.setPixmap(QPixmap::fromImage(src));
        item.setGraphicsEffect(effect);
    scene.addItem(&item);
    QImage image(src.size() + QSize(extent * 2, extent * 2), QImage::Format_ARGB32);
    image.fill(Qt::transparent);
    QPainter painter(&image);
    scene.render(&painter, QRectF(), QRectF(-extent, -extent, src.width() + extent*2, src.height() + extent*2));
    return image;
}



//####################################################################################
//##    Gives widget rounded corners,
//####################################################################################
// Radius is absolute size
void ApplyRoundedCornerMask(QWidget *widget, int x_radius, int y_radius, int method) {
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
void ApplyPopupMask(QWidget *widget, int x_radius, int y_radius, bool below) {
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
    paint.end();

    if (!below) pixmap = pixmap.transformed(QTransform().rotate(180));
    widget->clearMask();
    widget->setMask(pixmap.createMaskFromColor(Qt::green));
}




}   // namespace Dr












