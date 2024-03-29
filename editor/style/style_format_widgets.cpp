//
//      Created by Stephens Nunnally on 3/13/2019, (c) 2019 Scidian Studios, All Rights Reserved
//
//  File:
//
//
//
#include <QDebug>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsEffect>
#include <QGuiApplication>
#include <QLayout>
#include <QLayoutItem>
#include <QPainterPath>
#include <QScreen>
#include <QStandardPaths>
#include <QStyle>
#include <QWindow>

#include "core/colors/colors.h"
#include "core/dr_debug.h"
#include "editor/helper_library.h"
#include "editor/pixmap/pixmap.h"
#include "editor/style/style.h"

namespace Dr {


//####################################################################################
//##    Clears all widgets and layouts from a QWidget
//####################################################################################
void ClearWidget(QWidget *widget) {
    DeleteLayout(widget->layout());
    while (QWidget *child = widget->findChild<QWidget*>()) {
        ///Dr::PrintDebug("Deleting QWidget: " + child->objectName().toStdString());
        delete child;
    }
}

void DeleteLayout(QLayout *layout) {
    QLayoutItem *item;
    QLayout *sublayout;
    QWidget *widget;
    while ((item = layout->takeAt(0))) {
        if ((sublayout = item->layout()) != nullptr) {
            DeleteLayout(sublayout);
        } else if ((widget = item->widget()) != nullptr) {
            widget->hide();
            ///Dr::PrintDebug("Deleting QWidget: " + widget->objectName().toStdString());
            delete widget;
        } else {
            ///Dr::PrintDebug("Deleting Item...");
            delete item;
        }
    }
    delete layout;
}


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
    if (width_percentage  > 0 && width_percentage  <= 100) {
        int new_width = static_cast<int>(screen_geometry.width()  * (width_percentage  / 100.0));
        if (new_width > 1900) new_width = 1900;
        new_size.setWidth( new_width );
    }
    if (height_percentage > 0 && height_percentage <= 100) {
        int new_height = static_cast<int>(screen_geometry.height() * (height_percentage / 100.0));
        if (new_height > 1200) new_height = 1200;
        new_size.setHeight( new_height );
    }
    form_to_center->setGeometry( QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, new_size, screen_geometry) );
}

QScreen* FindScreenFromWidget(QWidget *widget) {
    if (widget == nullptr)                              return nullptr;
    if (widget->window() == nullptr)                    return nullptr;
    if (widget->window()->windowHandle() == nullptr)    return nullptr;
    return widget->window()->windowHandle()->screen();
}


//####################################################################################
//##    Centers form_to_center on Parent Widget
//####################################################################################
void CenterFormOnParent(QWidget *parent_form, QWidget *form_to_center) {
    if (parent_form == nullptr) return;

    QRect parent_geometry = parent_form->geometry();
    QSize new_size = form_to_center->size();
    form_to_center->setGeometry( QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, new_size, parent_geometry) );
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
        case Shadow_Types::Button_Shadow:
            if (Dr::GetColorScheme() == Color_Scheme::Light)
                ApplyDropShadow(target_widget, 3, Dr::Scale(0), Dr::Scale(2), Dr::ToQColor(Dr::GetColor(Window_Colors::Text)) );
            else
                ApplyDropShadow(target_widget, 3, Dr::Scale(0), Dr::Scale(2), Dr::ToQColor(Dr::GetColor(Window_Colors::Seperator)) );
            break;
        case Shadow_Types::Button_Shadow_Thin:
                ApplyDropShadow(target_widget, 0, Dr::Scale(0), Dr::Scale(1), Dr::ToQColor(Dr::GetColor(Window_Colors::Background_Dark)).darker(150) );
            break;
        case Shadow_Types::Tool_Tip_Shadow:
                ApplyDropShadow(target_widget, 4, Dr::Scale(0), Dr::Scale(3), Dr::ToQColor(Dr::GetColor(Window_Colors::Shadow)) );
            break;
    }
}


//####################################################################################
//##    Gives widget rounded corners,
//####################################################################################
// Radius is absolute size
void ApplyRoundedCornerMask(QWidget *widget, int x_radius, int y_radius, int method) {
    if (widget == nullptr) return;

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

    QBitmap mask = pixmap.createMaskFromColor(Qt::green, Qt::MaskMode::MaskInColor);
    widget->setMask(mask);
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
    paint.setPen(Qt::NoPen);

    paint.setBrush(Qt::red);
    paint.drawRoundedRect(0, 10, widget->rect().width(), widget->rect().height() - 10, x_radius, y_radius);

    QPoint points[3];
    points[0] = QPoint(widget->rect().width() / 2, 0);
    points[1] = QPoint((widget->rect().width() / 2) - 30, 30);
    points[2] = QPoint((widget->rect().width() / 2) + 30, 30);
    paint.drawPolygon(points, 3);
    paint.end();

    if (!below) pixmap = pixmap.transformed(QTransform().rotate(180));

    QBitmap mask = pixmap.createMaskFromColor(Qt::green, Qt::MaskMode::MaskInColor);
    widget->clearMask();
    widget->setMask(mask);

    ///pixmap.save(QStandardPaths::writableLocation(QStandardPaths::StandardLocation::DesktopLocation) + "/mask.png");
    ///qDebug() << "Applying popup mask";
}




}   // namespace Dr












