//
//      Created by Stephens Nunnally on 1/21/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Helpful QWidget / QObject formating functions
//
//
#include "colors.h"
#include "library.h"

namespace Dr {


//####################################################################################
//##        Apply palette / coloring / styling to children widgets
//####################################################################################
void ApplyCustomStyleSheetFormatting(QWidget *widget)
{
    QString style_sheet = QString(
        // Some custom class coloring
        " .ColorSplitter {   background: " + Dr::GetColor(Window_Colors::Seperator).name() + "; }"
        " QFrame#statusBar { background: " + Dr::GetColor(Window_Colors::Seperator).name() + "; } "

        // Main window background and seperator
        " QMainWindow { background: " + Dr::GetColor(Window_Colors::Background_Light).name() + "; }" +
        " QMainWindow::separator { border: 1px solid " + Dr::GetColor(Window_Colors::Seperator).name() + "; }"

        // Shared Empty Widgets
        " QWidget#widgetCentral { background: " + Dr::GetColor(Window_Colors::Background_Dark).name() + "; }"

        " QWidget#childForm     { background: " + Dr::GetColor(Window_Colors::Shadow).name() + "; }"
        " QWidget#innerWidget     { "
        "       background: " + Dr::GetColor(Window_Colors::Button_Dark).name() + "; "
        "       border: 1px solid; border-radius: 7px; "
        "       border-color: " + Dr::GetColor(Window_Colors::Midlight).name() + "; } "

        " QWidget#innerWidgetPopup     { "
        "       background: " + Dr::GetColor(Window_Colors::Button_Dark).name() + "; "
        "       border: 1px solid; border-bottom-left-radius: 7px; border-bottom-right-radius: 7px;"
        "       border-top-color: " +    Dr::GetColor(Window_Colors::Button_Dark).name() + ";  "
        "       border-left-color: " +   Dr::GetColor(Window_Colors::Midlight).name() + ";  "
        "       border-right-color: " +  Dr::GetColor(Window_Colors::Midlight).name() + ";  "
        "       border-bottom-color: " + Dr::GetColor(Window_Colors::Midlight).name() + "; } "

        // Horizontal and vertical splitters
        " QSplitter { width: 5px; height: 5px; } "
        " QSplitter::handle:vertical { image: url(:/gui_misc/splitter_v.png); } "
        " QSplitter::handle:horizontal { image: url(:/gui_misc/splitter_h.png); } "

        // Scroll bars, used mostly in QGraphicsView
        " QScrollBar:vertical { width: 10px; margin: 0px; border-radius: 0px; "
        "       background: " + Dr::GetColor(Window_Colors::Button_Dark).name() + "; } "
        " QScrollBar::handle:vertical { margin: 2px; border-radius: 3px; "
        "       background: qlineargradient(spread:pad, x1:0 y1:0, x2:0 y2:1, "
        "                   stop:0 " + Dr::GetColor(Window_Colors::Icon_Dark).name() + ", "
        "                   stop:1 " + Dr::GetColor(Window_Colors::Background_Dark).name() + "); } "
        " QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0px; } "
        " QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical { height: 0px; } "

        " QScrollBar:horizontal { height: 10px; margin: 0px; border-radius: 0px; "
        "       background: " + Dr::GetColor(Window_Colors::Button_Dark).name() + " ; } "
        " QScrollBar::handle:horizontal {      margin: 2px; border-radius: 3px; "
        "       background: qlineargradient(spread:pad, x1:0 y1:0, x2:1 y2:0, "
        "                   stop:0 " + Dr::GetColor(Window_Colors::Icon_Dark).name() + ", "
        "                   stop:1 " + Dr::GetColor(Window_Colors::Background_Dark).name() + "); } "
        " QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { width: 0px; } "
        " QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal { width: 0px; } "

        // For all scrollable QWidgets, ::corner removes right botttom little box that appears when there are two scrollbars
        " QWidget::corner { background: " + Dr::GetColor(Window_Colors::Button_Dark).name() + "; } " ///transparent; } "

        // Asset, Stage, Inspector Trees
        " QTreeWidget { icon-size: 14px 14px; }"
        " QTreeWidget { color: " + Dr::GetColor(Window_Colors::Text).name() + ";  "
        "               background: " + Dr::GetColor(Window_Colors::Background_Light).name() + "; }"

        // Project Tree
        " QTreeWidget#treeProjectEditor { icon-size: 14px 14px; }"
        " QTreeWidget#treeProjectEditor { color: " + Dr::GetColor(Window_Colors::Text).name() + ";  "
        "       background: " + Dr::GetColor(Window_Colors::Background_Dark).name() + "; "
        "       selection-background-color: " + Dr::GetColor(Window_Colors::Button_Dark).name() + "; "
        "       show-decoration-selected: 1; }"
        " QTreeWidget#treeProjectEditor::item:selected { "
        "       color: " + Dr::GetColor(Window_Colors::Icon_Dark).name() + "; "
        "       background: " + Dr::GetColor(Window_Colors::Button_Dark).name() + "; }"
        " QTreeWidget#treeProjectEditor::item:hover:selected { "
        "       color: " + Dr::GetColor(Window_Colors::Icon_Light).name() + "; "
        "       background: " + Dr::GetColor(Window_Colors::Button_Dark).name() + "; }"
        " QTreeWidget#treeProjectEditor::item:hover:!selected { "
        "       color: " + Dr::GetColor(Window_Colors::Highlight).name() + "; "
        "       background: " + Dr::GetColor(Window_Colors::Background_Dark).name() + "; }"
        // Header of Project Tree (where little lock is)
        " QHeaderView::section { "
        "       background-color: " + Dr::GetColor(Window_Colors::Background_Dark).name() + "; "
        "       border: 0px; } "

        // Tab bar that appears when you stack multiple DockWidgets
        " QTabBar { "
        "       background: " + Dr::GetColor(Window_Colors::Background_Dark).name() + "; } "
        " QTabBar::tab { "
        "       color: " +      Dr::GetColor(Window_Colors::Text_Dark).name() + "; "
        "       background: " + Dr::GetColor(Window_Colors::Background_Dark).name() + "; "
        "       padding-top: 2px; padding-bottom: 3px; padding-left: 4px; padding-right: 4px; "
        "       font-size: " + QString::number(Dr::FontSize()) + "px; } "
        " QTabBar::tab:selected { "
        "       color: " +      Dr::GetColor(Window_Colors::Text_Light).name() + "; "
        "       background: " + Dr::GetColor(Window_Colors::Background_Light).name() + "; "
        "       padding-top: 2px; padding-bottom: 3px; padding-left: 4px; padding-right: 4px; "
        "       font-size: " + QString::number(Dr::FontSize()) + "px; } "

        // Dock Widgets
        " QDockWidget { font-size: " + QString::number(Dr::FontSize()) + "px; font: bold; "
        "               color: " + Dr::GetColor(Window_Colors::Text).name() + "; } "
        " QDockWidget::title { "
        "       text-align: center; "
        "       background: qlineargradient(x1:0 y1:0, x2:0 y2:1, "
        "                   stop:0 " + Dr::GetColor(Window_Colors::Icon_Light).name() + ", "
        "                   stop:1 " + Dr::GetColor(Window_Colors::Background_Dark).name() + "); } "

        // DrView display area
        " QGraphicsView { background: " + Dr::GetColor(Window_Colors::Background_Dark).name() + "; }"

        // Bottom label area
        " QScrollArea { " //border-left: 2px solid; border-right: 1px solid; "
        "   border-color: " + Dr::GetColor(Window_Colors::Background_Dark).name() + "; "
        "   background: " + Dr::GetColor(Window_Colors::Background_Dark).name() + "; }"

        // Mostly debug labels
        " QLabel { color: " + Dr::GetColor(Window_Colors::Text).name() + "; padding: 0px; margin: 0px; border: 0px; } "

        // Generic Push Buttons
        " QPushButton#button { "
        "       color: " + Dr::GetColor(Window_Colors::Text).name() + "; "
        "       background: " + Dr::GetColor(Window_Colors::Button_Light).name() + "; "
        "       border-top: 1 solid; border-color: " + Dr::GetColor(Window_Colors::Button_Dark).name() + "; "
        "       border-radius: 4px; padding: 6px; }"
        " QPushButton#button:hover:!pressed { color: " + Dr::GetColor(Window_Colors::Highlight).name() + "; "
        "       background: " + Dr::GetColor(Window_Colors::Midlight).name() + "; }"
        " QPushButton#button:pressed { color: " + Dr::GetColor(Window_Colors::Highlight).name() + "; "
        "       background: " + Dr::GetColor(Window_Colors::Background_Dark).name() + "; }"

        // Asset Item Frames
        " QFrame#assetFrame { border: " + Dr::BorderWidth() + " solid; " + StyleSheetRecessedBackgroundBorder(5, 95) +
        "       margin-top: 2px; margin-bottom: 2px; margin-left: 5px; margin-right: 0px; "
        "       border-radius: 6px; } "

        " QFrame#assetFrame:hover { "
        "       border: " + Dr::BorderWidth() + " solid " + Dr::GetColor(Window_Colors::Icon_Dark).name()  + "; }"

        " QWidget#assetSearchWidget {"
        "       background: " + Dr::GetColor(Window_Colors::Background_Light).name() + "; } "
        " QLineEdit#assetSearchBar { "
        "       background-image: url(:/gui_misc/search.png); "
        "       background-repeat: no-repeat; "
        "       background-position: left; "
        "       padding: 2 2 3 22; } "


        //###############################################################################
        //##    StatusBar
        //################################################################################
        " QFrame#statusBar { "
        "       background: " + Dr::GetColor(Window_Colors::Button_Dark).name() + "; "
        "       border-top:    1 solid " + Dr::GetColor(Window_Colors::Seperator).name() + "; "
        "       border-bottom: 1 solid " + Dr::GetColor(Window_Colors::Button_Dark).name() + "; } "


        //###############################################################################
        //##    Inspector Widgets
        //################################################################################
        // Check boxes, mostly in object inspector
        " QCheckBox#checkInspector::indicator { height: 22px; width: 32px; }"

        // Spin boxes, mostly in object inspector
        " QAbstractSpinBox {        border: " + Dr::BorderWidth() + " solid; height: 20px; "
        "       border-top-left-radius: 4px; border-bottom-left-radius: 6px; border-top-right-radius: 4px; border-bottom-right-radius: 6px;"
        "       color: " + Dr::GetColor(Window_Colors::Text).name() + "; " + StyleSheetRecessedBackgroundBorder(15, 92) +
        "       selection-color: " + Dr::GetColor(Window_Colors::Shadow).name() + "; "
        "       selection-background-color: " + Dr::GetColor(Window_Colors::Highlight).name() + "; } "
        " QAbstractSpinBox:hover {  border: " + Dr::BorderWidth() + " solid; "
        "       color: " + Dr::GetColor(Window_Colors::Text_Light).name() + "; " + StyleSheetRecessedBackgroundBorder(15, 92, true) + " }"
        " QAbstractSpinBox:focus {  border: " + Dr::BorderWidth() + " solid; border-radius: 0px; "
        "       color: " +        Dr::GetColor(Window_Colors::Highlight).name() + "; "
        "       background: " +   Dr::GetColor(Window_Colors::Shadow).name() + "; "
        "       border-color: " + Dr::GetColor(Window_Colors::Icon_Dark).name() + "; }"
        " QAbstractSpinBox::!enabled { background: transparent; border: none; "
        "       color: " + Dr::GetColor(Window_Colors::Text_Dark).name() + "; } "

        // Line Edit, mostly in object inspector
        " QLineEdit {               border: " + Dr::BorderWidth() + " solid; height: 20px; "
        "       border-top-left-radius: 4px; border-bottom-left-radius: 6px; border-top-right-radius: 4px; border-bottom-right-radius: 6px;"
        "       color: " + Dr::GetColor(Window_Colors::Text).name() + "; " + StyleSheetRecessedBackgroundBorder(15, 92) +
        "       selection-color: " + Dr::GetColor(Window_Colors::Shadow).name() + "; "
        "       selection-background-color: " + Dr::GetColor(Window_Colors::Highlight).name() + "; } "
        " QLineEdit:hover {         border: " + Dr::BorderWidth() + " solid; "
        "       color: " + Dr::GetColor(Window_Colors::Text_Light).name() + "; " + StyleSheetRecessedBackgroundBorder(15, 92, true) + " }"
        " QLineEdit:focus { "
        "       color: " + Dr::GetColor(Window_Colors::Highlight).name() + "; "
        "       background: " + Dr::GetColor(Window_Colors::Shadow).name() + "; "
        "       border: " + Dr::BorderWidth() + " solid " + Dr::GetColor(Window_Colors::Icon_Dark).name() + "; "
        "       border-radius: 0px; }"
        " QLineEdit::!enabled { background: transparent; border: none; "
        "       color: " + Dr::GetColor(Window_Colors::Text_Dark).name() + "; } "


        // Drop down PushButtons
        " QPushButton#buttonDropDown { border: " + Dr::BorderWidth() + " solid; height: 20px; "
        "       border-top-left-radius: 4px; border-bottom-left-radius: 6px; border-top-right-radius: 4px; border-bottom-right-radius: 6px;"
        "       color: " + Dr::GetColor(Window_Colors::Text).name() + "; " +
                StyleSheetPoppedOutBackgroundBorder(Dr::GetColor(Window_Colors::Background_Dark), Dr::GetColor(Window_Colors::Background_Dark), 9, 92) +
        "       padding-left: 10px; "
        "       text-align: left; } "
        " QPushButton#buttonDropDown::menu-indicator { left: -8px; top: -7px; }"
        " QPushButton#buttonDropDown:hover { border: " + Dr::BorderWidth() + " solid; "
        "       color: " + Dr::GetColor(Window_Colors::Text_Light).name() + "; " +
                StyleSheetPoppedOutBackgroundBorder(Dr::GetColor(Window_Colors::Background_Dark), Dr::GetColor(Window_Colors::Background_Dark), 10, 92, true) + " }"
        " QPushButton#buttonDropDown:pressed { "
        "       color: " + Dr::GetColor(Window_Colors::Highlight).name() + "; "
        "       background: " + Dr::GetColor(Window_Colors::Shadow).name() + "; "
        "       border: " + Dr::BorderWidth() + " solid " + Dr::GetColor(Window_Colors::Icon_Dark).name() + "; "
        "       border-radius: 0px; }"
        // Popup Menu for PushButton
        " QMenu#menuComboBox { "
        "      padding-top: 4px; padding-bottom: 4px; "
        "      color: " + Dr::GetColor(Window_Colors::Text).name() + "; "
        "      font-size: " + QString::number(Dr::FontSize()) + "px; "
        "      border: " + Dr::BorderWidth() + " solid; margin: 0px; "
        "      border-color: " + Dr::GetColor(Window_Colors::Icon_Dark).name() + "; "
        "      background: " + Dr::GetColor(Window_Colors::Shadow).name() + "; }"
        " QMenu#menuComboBox::item { padding-top: 2px; padding-bottom: 3px; } "
        " QMenu#menuComboBox::item:selected { "
        "       padding-left: 2px; "
        "       color: " + Dr::GetColor(Window_Colors::Highlight).name() + "; "
        "       background: " + Dr::GetColor(Window_Colors::Button_Light).name() + "; } "
        " QMenu#menuComboBox::indicator:checked { left: 5px; top: -3px; } "
        " QMenu#menuComboBox::item:checked { "
        "       color: " + Dr::GetColor(Window_Colors::Highlight).name() + "; }"



    );

    style_sheet += Dr::StyleSheetToolBar();

    widget->setStyleSheet(style_sheet);
    widget->update();
}


// Cool sunken box kinda look
QString StyleSheetRecessedBackgroundBorder(int top_percent, int bottom_percent, bool highlight) {
    QString top1 = QString::number(double(top_percent)        / 100.0);
    QString top2 = QString::number(double(top_percent + 1)    / 100.0);
    QString bot1 = QString::number(double(bottom_percent)     / 100.0);
    QString bot2 = QString::number(double(bottom_percent + 1) / 100.0);
    QColor middle, top;
    if (!highlight) {
        middle = Dr::GetColor(Window_Colors::Background_Dark);
        top =    Dr::GetColor(Window_Colors::Background_Dark).darker(150);
    } else {
        middle = Dr::GetColor(Window_Colors::Background_Dark).lighter(150);
        top =    Dr::GetColor(Window_Colors::Background_Dark);
    }
    return " background: qlineargradient(spread:pad, x1:0 y1:0, x2:0 y2:1, "
           "            stop:0 " +      top.name() + ", "
           "            stop:" + top1 + top.name() + ", "
           "            stop:" + top2 + middle.name() + ", "
           "            stop:" + bot1 + middle.name() + ", "
           "            stop:" + bot2 + Dr::GetColor(Window_Colors::Background_Dark).darker(150).name() + ", "
           "            stop:1 " +      Dr::GetColor(Window_Colors::Background_Dark).darker(150).name() + "); "
           "          border-color: " + Dr::GetColor(Window_Colors::Background_Dark).darker(150).name() + " " +
                                        Dr::GetColor(Window_Colors::Background_Dark).darker(150).name()+ " " +
                                        Dr::GetColor(Window_Colors::Background_Dark).lighter(200).name() + " " +
                                        Dr::GetColor(Window_Colors::Background_Dark).darker(150).name() + "; ";
}


// Cool popped out box kinda look
QString StyleSheetPoppedOutBackgroundBorder(QColor background_color, QColor border_color, int top_percent, int bottom_percent, bool highlight) {
    QString top1 = QString::number(double(top_percent)        / 100.0);
    QString top2 = QString::number(double(top_percent + 1)    / 100.0);
    QString bot1 = QString::number(double(bottom_percent)     / 100.0);
    QString bot2 = QString::number(double(bottom_percent + 1) / 100.0);
    QColor middle;
    if (!highlight) middle = background_color;
    else            middle = background_color;
    return " background: qlineargradient(spread:pad, x1:0 y1:0, x2:0 y2:1, "
           "            stop:0 " +      background_color.lighter(200).name() + ", "
           "            stop:" + top1 + background_color.lighter(200).name() + ", "
           "            stop:" + top2 + middle.name() + ", "
           "            stop:" + bot1 + middle.name() + ", "
           "            stop:" + bot2 + background_color.darker(250).name() + ", "
           "            stop:1 " +      background_color.darker(250).name() + "); "
           "          border-color: " + border_color.darker(250).name() + " " +
                                        border_color.darker(250).name()+ " " +
                                        border_color.darker(150).name() + " " +
                                        border_color.darker(250).name() + "; ";
}


// Generates the style sheet for use with a Inspector Color Button
QString StyleSheetColorButton(QColor color, QColor text_color, QColor highlight)
{
    return
    " QPushButton { height: 20px; padding-top: 0px;"
    "   color: " + text_color.name() + "; "
    "   border: " + Dr::BorderWidth() + " solid; "
    "   border-radius: 4px; "
    "   background: qlineargradient(spread:pad, x1:0 y1:0, x2:0 y2:1, "
    "       stop:0.00 " + color.lighter(150).name() + ", "
    "       stop:0.08 " + color.lighter(150).name() + ", "
    "       stop:0.20 " + color.name() + ", "
    "       stop:0.88 " + color.name() + ", "
    "       stop:0.92 " + color.darker(300).name() + ", "
    "       stop:1.00 " + color.darker(300).name() + "); "
    "   border-color: " + color.darker(300).name() + "; "
    "   border-bottom-color: " + color.darker(400).name() + "; } "
    " QPushButton:hover {   color: " + highlight.name() + "; } "
    " QPushButton:pressed { padding-top: 1px; "
    "   background: qlineargradient(spread:pad, x1:0 y1:0, x2:0 y2:1, "
    "       stop:0.00 " + color.darker(300).name() + ", "
    "       stop:0.08 " + color.darker(300).name() + ", "
    "       stop:0.16 " + color.name() + ", "
    "       stop:0.92 " + color.name() + ", "
    "       stop:0.93 " + color.darker(300).name() + ", "
    "       stop:1.00 " + color.darker(300).name() + "); "
    "   border-color: " + color.darker(300).name() + "; "
    "   border-bottom-color: " + Dr::GetColor(Window_Colors::Background_Dark).lighter(200).name() + "; } ";
}




}   // namespace Dr











