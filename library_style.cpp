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
        // Some custom class coloring
        " .ColorSplitter { background: " + Dr::GetColor(Window_Colors::Seperator).name() + "; }"
        " QFrame#statusBar { background: " + Dr::GetColor(Window_Colors::Seperator).name() + "; } "
        " QWidget#widgetToolbar { background: " + Dr::GetColor(Window_Colors::Background_Dark).name() + "; } "

        // Main window background and seperator
        " QMainWindow { background: " + Dr::GetColor(Window_Colors::Seperator).name() + "; }" +
        " QMainWindow::separator { border: 1px solid " + Dr::GetColor(Window_Colors::Seperator).name() + "; }"

        // Horizontal and vertical splitters
        " QSplitter { width: 5px; height: 5px; } "
        " QSplitter::handle:vertical { image: url(:/gui_misc/splitter_v.png); } "
        " QSplitter::handle:horizontal { image: url(:/gui_misc/splitter_h.png); } "

        // Scroll bars, used mostly in QGraphicsView
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

        // Toolbar buttons up top
        " QPushButton#toolbarButton { color: " + Dr::GetColor(Window_Colors::Text).name() + "; "
        "       background: qlineargradient(spread:pad, x1:0 y1:0, x2:0 y2:1, "
        "                   stop:0 " + Dr::GetColor(Window_Colors::Button_Light).name() + ", "
        "                   stop:1 " + Dr::GetColor(Window_Colors::Button_Dark).name() + "); "
        "       border: none; border-radius: 6px; }"
        " QPushButton#toolbarButton:hover:!pressed { color: " + Dr::GetColor(Window_Colors::Highlight).name() + "; "
        "       background: " + Dr::GetColor(Window_Colors::Button_Light).name() + "; }"
        " QPushButton#toolbarButton:pressed { color: " + Dr::GetColor(Window_Colors::Highlight).name() + "; "
        "       background: " + Dr::GetColor(Window_Colors::Button_Dark).name() + "; }"

        // Asset, Stage, Inspector, Etc Trees
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

        // Header of Stage Tree (where little lock is)
        " QHeaderView::section { "
        "       background-color: " + Dr::GetColor(Window_Colors::Background_Dark).name() + "; "
        "       border: 0px; } "

        // Tab bar that appears when you stack multiple DockWidgets
        " QTabBar::tab { color: " + Dr::GetColor(Window_Colors::Text).name() + "; "
        "       background: " + Dr::GetColor(Window_Colors::Background_Dark).name() + "; "
        "       border-radius: 6px; padding-top: 2px; padding-bottom: 3px; padding-left: 8px; padding-right: 8px; } "
        " QTabBar::tab:selected { color: " + Dr::GetColor(Window_Colors::Text).name() + "; "
        "       background: " + Dr::GetColor(Window_Colors::Button_Light).name() + "; "
        "       border-radius: 6px; padding-top: 2px; padding-bottom: 3px; padding-left: 8px; padding-right: 8px; } "

        // Dock Widgets
        //" QDockWidget { titlebar-close-icon: url(close.png); } "
        " QDockWidget { font-size: " + QString::number(Dr::FontSize()) + "px; font: bold; "
        "               color: " + Dr::GetColor(Window_Colors::Text).name() + "; } "
        " QDockWidget::title { text-align: center; "
        "       background: qlineargradient(x1:0 y1:0, x2:0 y2:1, "
        "                   stop:0 " + Dr::GetColor(Window_Colors::Icon_Light).name() + ", "
        "                   stop:1 " + Dr::GetColor(Window_Colors::Background_Dark).name() + "); } "

        // DrView display area, ::corner removes right botttom little box
        " QGraphicsView { background: " + Dr::GetColor(Window_Colors::Background_Light).name() + "; }"
        " QGraphicsView::corner { background: transparent; } "

        // Bottom toolbar area
        " QScrollArea { background: " + Dr::GetColor(Window_Colors::Background_Dark).name() + "; }"

        // Mostly debug labels
        " QLabel { color: " + Dr::GetColor(Window_Colors::Text).name() + "; padding: 0px; margin: 0px; border: 0px; } "

        // Check boxes, mostly in object inspector
        " QCheckBox#checkInspector::indicator:checked {         image: url(:/inspector_icons/check_checked.png); }"
        " QCheckBox#checkInspector::indicator:unchecked {       image: url(:/inspector_icons/check_unchecked.png); }"
        " QCheckBox#checkInspector::indicator:checked:hover {   image: url(:/inspector_icons/check_hover_checked.png); }"
        " QCheckBox#checkInspector::indicator:unchecked:hover { image: url(:/inspector_icons/check_hover_unchecked.png); }"

        // Spin boxes, mostly in object inspector
        " QAbstractSpinBox { "
        "       color: " + Dr::GetColor(Window_Colors::Text).name() + "; "
        "       background: " + Dr::GetColor(Window_Colors::Background_Light).name() + "; "
        "       border: " + Dr::BorderWidth() + " solid; "
        "       border-color: " + Dr::GetColor(Window_Colors::Button_Light).name() + "; "
        "       border-radius: 4px; height: 20px;"
        "       selection-color: " + Dr::GetColor(Window_Colors::Shadow).name() + "; "
        "       selection-background-color: " + Dr::GetColor(Window_Colors::Highlight).name() + "; "
        " } "
        " QAbstractSpinBox:hover { "
        "       color: " + Dr::GetColor(Window_Colors::Text_Light).name() + "; "
        "       background: " + Dr::GetColor(Window_Colors::Button_Light).name() + "; "
        "       border: " + Dr::BorderWidth() + " solid " + Dr::GetColor(Window_Colors::Background_Light).name() + "; "
        "       border-radius: 4px; }"

        " QAbstractSpinBox:focus { "
        "       color: " + Dr::GetColor(Window_Colors::Highlight).name() + "; "
        "       background: " + Dr::GetColor(Window_Colors::Shadow).name() + "; "
        "       border: " + Dr::BorderWidth() + " solid " + Dr::GetColor(Window_Colors::Icon_Dark).name() + "; "
        "       border-radius: 0px; }"

        // Line Edit, mostly in object inspector
        " QLineEdit { "
        "       color: " + Dr::GetColor(Window_Colors::Text).name() + "; "
        "       background: " + Dr::GetColor(Window_Colors::Background_Light).name() + "; "
        "       border: " + Dr::BorderWidth() + " solid; "
        "       border-color: " + Dr::GetColor(Window_Colors::Button_Light).name() + "; "
        "       border-radius: 4px; height: 20px;"
        "       selection-color: " + Dr::GetColor(Window_Colors::Shadow).name() + "; "
        "       selection-background-color: " + Dr::GetColor(Window_Colors::Highlight).name() + "; "
        " } "
        " QLineEdit:hover { "
        "       color: " + Dr::GetColor(Window_Colors::Text_Light).name() + "; "
        "       background: " + Dr::GetColor(Window_Colors::Button_Light).name() + "; "
        "       border: " + Dr::BorderWidth() + " solid " + Dr::GetColor(Window_Colors::Background_Light).name() + "; "
        "       border-radius: 4px; }"

        " QLineEdit:focus { "
        "       color: " + Dr::GetColor(Window_Colors::Highlight).name() + "; "
        "       background: " + Dr::GetColor(Window_Colors::Shadow).name() + "; "
        "       border: " + Dr::BorderWidth() + " solid " + Dr::GetColor(Window_Colors::Icon_Dark).name() + "; "
        "       border-radius: 0px; }"

        // Drop down Combo Box, mostly in object inspector
        " QComboBox { "
        "       color: " + Dr::GetColor(Window_Colors::Text).name() + "; "
        "       background: " + Dr::GetColor(Window_Colors::Background_Light).name() + "; "
        "       border: " + Dr::BorderWidth() + " solid; "
        "       border-color: " + Dr::GetColor(Window_Colors::Button_Light).name() + "; "
        "       border-radius: 4px; height: 20px; }"
        " QComboBox:hover { "
        "       color: " + Dr::GetColor(Window_Colors::Text_Light).name() + "; "
        "       background: " + Dr::GetColor(Window_Colors::Button_Light).name() + "; "
        "       border: " + Dr::BorderWidth() + " solid " + Dr::GetColor(Window_Colors::Background_Light).name() + "; "
        "       border-radius: 4px; }"
        " QComboBox::drop-down { border: 0px; width: 20px; }"
        " QComboBox::down-arrow { image: url(:/gui_misc/drop_arrow.png); }"
        // QComboBox gets the "on" state when the popup is open
        " QComboBox:on {"
        "       color: " + Dr::GetColor(Window_Colors::Text).name() + "; "
        "       background: " + Dr::GetColor(Window_Colors::Background_Light).name() + "; "
        "       border: " + Dr::BorderWidth() + " solid; "
        "       border-color: " + Dr::GetColor(Window_Colors::Icon_Dark).name() + "; "
        "       border-bottom-right-radius: 0px;  "
        "       border-bottom-left-radius: 0px; } "

        // Drop down PushButtons
        " QPushButton { "
        "       color: " + Dr::GetColor(Window_Colors::Text).name() + "; "
        "       background: " + Dr::GetColor(Window_Colors::Background_Light).name() + "; "
        "       border: " + Dr::BorderWidth() + " solid; "
        "       border-color: " + Dr::GetColor(Window_Colors::Button_Light).name() + "; "
        "       border-radius: 4px; height: 20px;"
        "       padding-left: 10px; "
        "       text-align: left; "
        " } "
        " QPushButton::menu-indicator { image: none; }"
        " QPushButton:hover { "
        "       color: " + Dr::GetColor(Window_Colors::Text_Light).name() + "; "
        "       background: " + Dr::GetColor(Window_Colors::Button_Light).name() + "; "
        "       border: " + Dr::BorderWidth() + " solid " + Dr::GetColor(Window_Colors::Background_Light).name() + "; "
        "       border-radius: 4px; }"
        " QPushButton:pressed { "
        "       color: " + Dr::GetColor(Window_Colors::Highlight).name() + "; "
        "       background: " + Dr::GetColor(Window_Colors::Shadow).name() + "; "
        "       border: " + Dr::BorderWidth() + " solid " + Dr::GetColor(Window_Colors::Icon_Dark).name() + "; "
        "       border-bottom-right-radius: 0px; "
        "       border-bottom-left-radius: 0px;  }"

        // Asset Item Frames
        " QFrame#assetFrame { border: " + Dr::BorderWidth() + " solid #777777; "
        "       padding: 1px; margin-top: 2px; margin-bottom: 2px; margin-left: 4px; margin-right: 4px; "
        "       border-radius: 6px; } "

        " QFrame#assetFrame:hover { border: " + Dr::BorderWidth() + " solid " + Dr::GetColor(Window_Colors::Icon_Dark).name()  + "; "
        "       padding: 1px; margin-top: 2px; margin-bottom: 2px; margin-left: 4px; margin-right: 4px; "
        "       border-radius: 6px; } "

    );

    widget->setStyleSheet(style_sheet);
}



}











