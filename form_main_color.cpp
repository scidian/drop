//
//      Created by Stephens Nunnally on 1/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Colors and styles Form Main
//
//

#include "editor_scene_scene.h"
#include "editor_tree_advisor.h"
#include "editor_tree_assets.h"
#include "editor_tree_inspector.h"
#include "editor_tree_scene.h"
#include "editor_scene_view.h"

#include "form_main.h"


//####################################################################################
//##        Apply palette / coloring / styling to children widgets
//##
void FormMain::applyColoring()
{
    QString style_sheet = QString(
        " QMainWindow { background: " + globals->getColor(Window_Colors::Background_Light).name() + "; }" +
        " QMainWindow::separator { border: 1px solid " + globals->getColor(Window_Colors::Background_Light).name() + "; }"

        " QSplitter { width: 4px; } "
        " QSplitter::handle:vertical { image: url(:/tree_icons/splitter_v.png); } "
        " QSplitter::handle:horizontal { image: url(:/tree_icons/splitter_h.png); } "

        " QScrollBar:vertical { width: 12px; margin: 0px; border-radius: 6px; "
        "       background: " + globals->getColor(Window_Colors::Button_Light).name() + "; } "
        " QScrollBar::handle:vertical { margin: 2px; border-radius: 4px; "
        "       background: qlineargradient(spread:pad, x1:0 y1:0, x2:0 y2:1, "
        "                   stop:0 " + globals->getColor(Window_Colors::Icon_Dark).name() + ", "
        "                   stop:1 " + globals->getColor(Window_Colors::Background_Dark).name() + "); } "
        " QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0px; } "
        " QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical { height: 0px; } "

        " QScrollBar:horizontal { height: 12px; margin: 0px; border-radius: 6px; "
        "       background: " + globals->getColor(Window_Colors::Button_Light).name() + " ;} "
        " QScrollBar::handle:horizontal {      margin: 2px; border-radius: 4px; "
        "       background: qlineargradient(spread:pad, x1:0 y1:0, x2:1 y2:0, "
        "                   stop:0 " + globals->getColor(Window_Colors::Icon_Dark).name() + ", "
        "                   stop:1 " + globals->getColor(Window_Colors::Background_Dark).name() + "); } "
        " QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { width: 0px; } "
        " QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal { width: 0px; } "

        " QPushButton { color: " + globals->getColor(Window_Colors::Text).name() + "; "
        "       background: qlineargradient(spread:pad, x1:0 y1:0, x2:0 y2:1, "
        "                   stop:0 " + globals->getColor(Window_Colors::Button_Light).name() + ", "
        "                   stop:1 " + globals->getColor(Window_Colors::Button_Dark).name() + "); "
        "       border: none; border-radius: 6px; }"
        " QPushButton:hover:!pressed { color: " + globals->getColor(Window_Colors::Highlight).name() + "; "
        "       background: " + globals->getColor(Window_Colors::Button_Light).name() + "; }"
        " QPushButton:pressed { color: " + globals->getColor(Window_Colors::Highlight).name() + "; "
        "       background: " + globals->getColor(Window_Colors::Button_Dark).name() + "; }"

        " QTreeWidget { icon-size: 14px 14px; }"
        " QTreeWidget { color: " + globals->getColor(Window_Colors::Text).name() + ";  "
        "       background: " + globals->getColor(Window_Colors::Background_Dark).name() + "; "
        "       selection-background-color: " + globals->getColor(Window_Colors::Midlight).name() + "; "
        "       show-decoration-selected: 1; }"
        " QTreeWidget::item:selected { color: " + globals->getColor(Window_Colors::Icon_Dark).name() + "; "
        "       background: " + globals->getColor(Window_Colors::Midlight).name() + "; }"
        " QTreeWidget::item:hover:selected { color: " + globals->getColor(Window_Colors::Icon_Light).name() + "; "
        "       background: " + globals->getColor(Window_Colors::Midlight).name() + "; }"
        " QTreeWidget::item:hover:!selected { color: " + globals->getColor(Window_Colors::Highlight).name() + "; "
        "       background: " + globals->getColor(Window_Colors::Background_Dark).name() + "; }"

        " QHeaderView::section { "
        "       background-color: " + globals->getColor(Window_Colors::Background_Dark).name() + "; "
        "       border: 0px; }"

        " QDockWidget { font-size: 11px; color: " + globals->getColor(Window_Colors::Text).name() + "; } "
        " QDockWidget::title { text-align: center; "
        "       background: qlineargradient(x1:0 y1:0, x2:0 y2:1, "
        "                   stop:0 " + globals->getColor(Window_Colors::Icon_Light).name() + ", "
        "                   stop:1 " + globals->getColor(Window_Colors::Background_Dark).name() + "); } "

        " QGraphicsView { background: " + globals->getColor(Window_Colors::Background_Light).name() + "; }"
        " QGraphicsView::corner { background: transparent; } "

        " QScrollArea { background: " + globals->getColor(Window_Colors::Background_Dark).name() + "; }"

        " QLabel { color : " + globals->getColor(Window_Colors::Text).name() + "; } "
    );

    this->setStyleSheet(style_sheet);
}

void FormMain::applyDropShadowByType(QWidget *target_widget, Shadow_Types shadow_type)
{
    switch (shadow_type) {
    case Shadow_Types::Button_Shadow:   applyDropShadow(target_widget, 6, 0, 3, QColor(10, 10, 10));
    }
}
void FormMain::applyDropShadow(QWidget *target_widget, qreal blur_radius, qreal offset_x, qreal offset_y, QColor shadow_color)
{
    QGraphicsDropShadowEffect *shadow_effect;
    shadow_effect = new QGraphicsDropShadowEffect();
    shadow_effect->setBlurRadius(blur_radius);
    shadow_effect->setOffset(offset_x, offset_y);
    shadow_effect->setColor(shadow_color);
    target_widget->setGraphicsEffect(shadow_effect);
}










