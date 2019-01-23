//
//      Created by Stephens Nunnally on 1/21/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Helpful QWidget / QObject formating functions
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
    case Shadow_Types::Button_Shadow:   ApplyDropShadow(target_widget, 6,  0,  3, Dr::GetColor(Window_Colors::Shadow) );    break;
    case Shadow_Types::Tool_Tip_Shadow: ApplyDropShadow(target_widget, 4,  0,  3, Dr::GetColor(Window_Colors::Shadow) );    break;
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



//####################################################################################
//##        Apply palette / coloring / styling to children widgets
//####################################################################################
void ApplyColoring(QWidget *widget)
{
    QString style_sheet = QString(
        " .ColorSplitter { background: " + Dr::GetColor(Window_Colors::Background_Light).name() + "; }"
        " QFrame#statusBar { background: " + Dr::GetColor(Window_Colors::Background_Dark).name() + "; } "

        " QMainWindow { background: " + Dr::GetColor(Window_Colors::Background_Light).name() + "; }" +
        " QMainWindow::separator { border: 1px solid " + Dr::GetColor(Window_Colors::Background_Light).name() + "; }"

        " QSplitter { width: 5px; height: 5px; } "
        " QSplitter::handle:vertical { image: url(:/gui_misc/splitter_v.png); } "
        " QSplitter::handle:horizontal { image: url(:/gui_misc/splitter_h.png); } "

        " QScrollBar:vertical { width: 12px; margin: 0px; border-radius: 6px; "
        "       background: " + Dr::GetColor(Window_Colors::Button_Light).name() + "; } "
        " QScrollBar::handle:vertical { margin: 2px; border-radius: 4px; "
        "       background: qlineargradient(spread:pad, x1:0 y1:0, x2:0 y2:1, "
        "                   stop:0 " + Dr::GetColor(Window_Colors::Icon_Dark).name() + ", "
        "                   stop:1 " + Dr::GetColor(Window_Colors::Background_Dark).name() + "); } "
        " QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0px; } "
        " QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical { height: 0px; } "

        " QScrollBar:horizontal { height: 12px; margin: 0px; border-radius: 6px; "
        "       background: " + Dr::GetColor(Window_Colors::Button_Light).name() + " ;} "
        " QScrollBar::handle:horizontal {      margin: 2px; border-radius: 4px; "
        "       background: qlineargradient(spread:pad, x1:0 y1:0, x2:1 y2:0, "
        "                   stop:0 " + Dr::GetColor(Window_Colors::Icon_Dark).name() + ", "
        "                   stop:1 " + Dr::GetColor(Window_Colors::Background_Dark).name() + "); } "
        " QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { width: 0px; } "
        " QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal { width: 0px; } "

        " QPushButton { color: " + Dr::GetColor(Window_Colors::Text).name() + "; "
        "       background: qlineargradient(spread:pad, x1:0 y1:0, x2:0 y2:1, "
        "                   stop:0 " + Dr::GetColor(Window_Colors::Button_Light).name() + ", "
        "                   stop:1 " + Dr::GetColor(Window_Colors::Button_Dark).name() + "); "
        "       border: none; border-radius: 6px; }"
        " QPushButton:hover:!pressed { color: " + Dr::GetColor(Window_Colors::Highlight).name() + "; "
        "       background: " + Dr::GetColor(Window_Colors::Button_Light).name() + "; }"
        " QPushButton:pressed { color: " + Dr::GetColor(Window_Colors::Highlight).name() + "; "
        "       background: " + Dr::GetColor(Window_Colors::Button_Dark).name() + "; }"

        " QTreeWidget { icon-size: 14px 14px; }"
        " QTreeWidget { color: " + Dr::GetColor(Window_Colors::Text).name() + ";  "
        "       background: " + Dr::GetColor(Window_Colors::Background_Dark).name() + "; "
        "       selection-background-color: " + Dr::GetColor(Window_Colors::Midlight).name() + "; "
        "       show-decoration-selected: 1; }"
        " QTreeWidget::item:selected { color: " + Dr::GetColor(Window_Colors::Icon_Dark).name() + "; "
        "       background: " + Dr::GetColor(Window_Colors::Midlight).name() + "; }"
        " QTreeWidget::item:hover:selected { color: " + Dr::GetColor(Window_Colors::Icon_Light).name() + "; "
        "       background: " + Dr::GetColor(Window_Colors::Midlight).name() + "; }"
        " QTreeWidget::item:hover:!selected { color: " + Dr::GetColor(Window_Colors::Highlight).name() + "; "
        "       background: " + Dr::GetColor(Window_Colors::Background_Dark).name() + "; }"

        " QHeaderView::section { "
        "       background-color: " + Dr::GetColor(Window_Colors::Background_Dark).name() + "; "
        "       border: 0px; } "

        //" QDockWidget { titlebar-close-icon: url(close.png); } "
        " QDockWidget { font-size: 11px; color: " + Dr::GetColor(Window_Colors::Text).name() + "; } "
        " QDockWidget::title { text-align: center; "
        "       background: qlineargradient(x1:0 y1:0, x2:0 y2:1, "
        "                   stop:0 " + Dr::GetColor(Window_Colors::Icon_Light).name() + ", "
        "                   stop:1 " + Dr::GetColor(Window_Colors::Background_Dark).name() + "); } "

        " QGraphicsView { background: " + Dr::GetColor(Window_Colors::Background_Light).name() + "; }"
        " QGraphicsView::corner { background: transparent; } "

        " QScrollArea { background: " + Dr::GetColor(Window_Colors::Background_Dark).name() + "; }"

        " QLabel { color : " + Dr::GetColor(Window_Colors::Text).name() + "; } "

    );

    widget->setStyleSheet(style_sheet);
}



}


