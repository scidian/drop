//
//      Created by Stephens Nunnally on 1/21/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Helpful QWidget / QObject formating functions
//
//
#include "colors/colors.h"
#include "style/style.h"

#include "helper.h"
#include "helper_qt.h"



namespace Dr {

//####################################################################################
//##    Apply palette / coloring / styling to children widgets
//####################################################################################
QString CustomStyleSheetFormatting() {

    QString style_sheet = QString(

        //################################################################################
        //##    Misc Styling
        //################################################################################
        // Example of some custom class coloring
        ///" .ClassNameHere {   background: " + Dr::GetColor(Window_Colors::Seperator).name() + "; }"

        // Splitters
        " QSplitter#splitterVertical {      background: " + Dr::GetColor(Window_Colors::Seperator).name() + "; }"
        " QSplitter#splitterHorizontal {    background: " + Dr::GetColor(Window_Colors::Seperator).name() + "; }"

        // Tool tip
        " QToolTip { border: 1px solid; "
        "       border-color: " +     Dr::GetColor(Window_Colors::Shadow).name() + "; "
        "       color: " +            Dr::GetColor(Window_Colors::Shadow).name() + "; "
        "       background-color: " + Dr::GetColor(Window_Colors::Text).name() + "; }"

        // Main window background and seperator
        " QMainWindow { background: " + Dr::GetColor(Window_Colors::Background_Light).name() + "; }" +
        " QMainWindow::separator { border: 1px solid " + Dr::GetColor(Window_Colors::Seperator).name() + "; }"

        // For all scrollable QWidgets, ::corner removes right botttom little box that appears when there are two scrollbars
        " QWidget::corner { background: " + Dr::GetColor(Window_Colors::Button_Dark).name() + "; } " ///transparent; } "

        // Shared Empty Widgets
        " QWidget#widgetCentral { background: " + Dr::GetColor(Window_Colors::Background_Dark).name() + "; }"

        " QWidget#childForm     { background: " + Dr::GetColor(Window_Colors::Background_Dark).name() + "; }"
        " QWidget#innerWidget     { "
        "       background: " + Dr::GetColor(Window_Colors::Button_Dark).name() + "; "
        "       border: 1px solid; border-radius: 7px; "
        "       border-color: " + Dr::GetColor(Window_Colors::Midlight).name() + "; } "

        " QWidget#innerWidgetPopupBelow    { "
        "       background: " + Dr::GetColor(Window_Colors::Button_Dark).name() + "; "
        "       border: 1px solid; border-bottom-left-radius: 7px; border-bottom-right-radius: 7px;"
        "       border-top-color: " +    Dr::GetColor(Window_Colors::Button_Dark).name() + ";  "
        "       border-left-color: " +   Dr::GetColor(Window_Colors::Midlight).name() + ";  "
        "       border-right-color: " +  Dr::GetColor(Window_Colors::Midlight).name() + ";  "
        "       border-bottom-color: " + Dr::GetColor(Window_Colors::Midlight).name() + "; } "
        " QWidget#innerWidgetPopupAbove    { "
        "       background: " + Dr::GetColor(Window_Colors::Button_Dark).name() + "; "
        "       border: 1px solid; border-top-left-radius: 7px; border-top-right-radius: 7px;"
        "       border-top-color: " +    Dr::GetColor(Window_Colors::Midlight).name() + ";  "
        "       border-left-color: " +   Dr::GetColor(Window_Colors::Midlight).name() + ";  "
        "       border-right-color: " +  Dr::GetColor(Window_Colors::Midlight).name() + ";  "
        "       border-bottom-color: " + Dr::GetColor(Window_Colors::Button_Dark).name() + "; } "

        // Horizontal and vertical splitters
        " QSplitter { width: 5px; height: 5px; } "
        " QSplitter::handle:vertical { image: url(:/assets/gui_misc/splitter_v.png); } "
        " QSplitter::handle:horizontal { image: url(:/assets/gui_misc/splitter_h.png); } "

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
        "       border-radius: 4px; "
        "       padding-left: 20px; padding-right:  20px; "
        "       padding-top:   6px; padding-bottom:  6px; }"
        " QPushButton#button:hover:!pressed { color: " + Dr::GetColor(Window_Colors::Highlight).name() + "; "
        "       background: " + Dr::GetColor(Window_Colors::Midlight).name() + "; }"
        " QPushButton#button:pressed { color: " + Dr::GetColor(Window_Colors::Highlight).name() + "; "
        "       background: " + Dr::GetColor(Window_Colors::Background_Dark).name() + "; }"
        " QPushButton#button:disabled { color: " + Dr::GetColor(Window_Colors::Text_Dark).name() + "; "
        "       background: " + Dr::GetColor(Window_Colors::Background_Dark).name() + "; }"


        //################################################################################
        //##    Scroll bars, used mostly in QGraphicsView
        //################################################################################
        " QScrollBar:vertical { width: 10px; margin: 0px; border-radius: 0px; "
        "       background: " + Dr::GetColor(Window_Colors::Button_Dark).name() + "; } "
        " QScrollBar::handle:vertical { margin: 2px; border-radius: 3px; "
        "       background: qlineargradient(spread:pad, x1:0 y1:0, x2:0 y2:1, "
        "                   stop:0 " + Dr::GetColor(Window_Colors::Icon_Dark).name() + ", "
        "                   stop:1 " + Dr::GetColor(Window_Colors::Header_Fade).name() + "); } "
        " QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0px; } "
        " QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical { height: 0px; } "

        " QScrollBar:horizontal { height: 10px; margin: 0px; border-radius: 0px; "
        "       background: " + Dr::GetColor(Window_Colors::Button_Dark).name() + " ; } "
        " QScrollBar::handle:horizontal {      margin: 2px; border-radius: 3px; "
        "       background: qlineargradient(spread:pad, x1:0 y1:0, x2:1 y2:0, "
        "                   stop:0 " + Dr::GetColor(Window_Colors::Icon_Dark).name() + ", "
        "                   stop:1 " + Dr::GetColor(Window_Colors::Header_Fade).name() + "); } "
        " QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { width: 0px; } "
        " QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal { width: 0px; } "


        //################################################################################
        //##    Message Box / About Box Coloring
        //################################################################################
        " QMessageBox { background-color: " + Dr::GetColor(Window_Colors::Background_Dark).name() + "; } "
        " QMessageBox QAbstractButton { color: #FF0000; }"

        " QMessageBox QAbstractButton { "
        "       color: " + Dr::GetColor(Window_Colors::Text).name() + "; "
        "       background: " + Dr::GetColor(Window_Colors::Button_Light).name() + "; "
        "       border-top: 1 solid; border-color: " + Dr::GetColor(Window_Colors::Button_Dark).name() + "; "
        "       border-radius: 4px; "
        "       padding-left: 20px; padding-right:  20px; "
        "       padding-top:   6px; padding-bottom:  6px; }"
        " QMessageBox QAbstractButton:hover:!pressed { color: " + Dr::GetColor(Window_Colors::Highlight).name() + "; "
        "       background: " + Dr::GetColor(Window_Colors::Midlight).name() + "; }"
        " QMessageBox QAbstractButton:pressed { color: " + Dr::GetColor(Window_Colors::Highlight).name() + "; "
        "       background: " + Dr::GetColor(Window_Colors::Background_Dark).name() + "; }"
        " QMessageBox QAbstractButton:disabled { color: " + Dr::GetColor(Window_Colors::Text_Dark).name() + "; "
        "       background: " + Dr::GetColor(Window_Colors::Background_Dark).name() + "; }"

        //################################################################################
        //##    Asset, Stage, Inspector Trees
        //################################################################################
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
        "       color: " + Dr::GetColor(Window_Colors::Background_Dark).name() + "; "
        "       background-color: " + Dr::GetColor(Window_Colors::Background_Dark).name() + "; "
        "       border: 0px; } "


        //################################################################################
        //##    Dock Widgets - Stacked Dock TabBars
        //################################################################################
        " QDockWidget { font-size: " + QString::number(Dr::FontSize()) + "px; font: bold; "
        "               color: " + Dr::GetColor(Window_Colors::Header_Text).name() + "; } "
        " QDockWidget::title { "
        "       text-align: center; "
        "       background: qlineargradient(x1:0 y1:0, x2:0 y2:1, "
        "                   stop:0 " + Dr::GetColor(Window_Colors::Icon_Light).name() + ", "
        "                   stop:1 " + Dr::GetColor(Window_Colors::Header_Fade).name() + "); } "

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


        //################################################################################
        //##    Asset Tree - Item Frames, Search Bar
        //################################################################################
        " QFrame#assetFrame { border: " + Dr::BorderWidth() + " solid; " + StyleSheetRecessedBackgroundBorder(5, 95) +
        "       margin-top: 2px; margin-bottom: 2px; margin-left: 5px; margin-right: 0px; "
        "       border-radius: 6px; } "
        " QFrame#assetFrame:hover { " + StyleSheetRecessedBackgroundBorder(5, 95, true) + "; } "
        ///"       border: " + Dr::BorderWidth() + " solid " + Dr::GetColor(Window_Colors::Icon_Dark).name()  + "; }"

        " QFrame#inspectorImageFrame { border: " + Dr::BorderWidth() + " solid; " + StyleSheetRecessedBackgroundBorder(5, 95) +
        "       margin-top: 2px; margin-bottom: 2px; margin-left: 0px; margin-right: 0px; "
        "       border-radius: 6px; } "
        " QFrame#inspectorImageFrame:hover { " + StyleSheetRecessedBackgroundBorder(5, 95, true) + "; } "

        " QPushButton#buttonAssetCategory { height: 22px; font: 13px; text-align: left; "
        "               color: " + Dr::GetColor(Window_Colors::Text).name() + "; "
        "               border: none; "
        "               border-style: solid; "
        "               border-top-width: " + Dr::BorderWidth() + "; "
        "               border-bottom-width: " + Dr::BorderWidth() + "; "
        "               border-color: " + Dr::GetColor(Window_Colors::Header_Fade).name() + "; "
        "               background: qlineargradient(spread:pad, x1:0 y1:0, x2:0 y2:1, "
        "                           stop:0.00 " + Dr::GetColor(Window_Colors::Midlight).name() +        ///Button_Light).lighter(150).name() +
        "                           stop:0.05 " + Dr::GetColor(Window_Colors::Midlight).name() +        ///Button_Light).lighter(150).name() +
        "                           stop:0.10 " + Dr::GetColor(Window_Colors::Button_Light).name() +
        "                         , stop:0.90 " + Dr::GetColor(Window_Colors::Background_Light).name() +///Button_Light).darker(200).name() +
        "                         , stop:0.95 " + Dr::GetColor(Window_Colors::Background_Light).name() +///Button_Light).darker(400).name() +
        "                         , stop:1.00 " + Dr::GetColor(Window_Colors::Shadow).name() + "); }"   ///Window_Colors::Button_Light).darker(400).name() + "); } "
        " QPushButton#buttonAssetCategory:hover:!pressed { color: " + Dr::GetColor(Window_Colors::Highlight).name() + "; } "
        " QPushButton#buttonAssetCategory:pressed  { color: " + Dr::GetColor(Window_Colors::Text_Dark).name() + "; } "
        " QPushButton#buttonAssetCategory:disabled { color: " + Dr::GetColor(Window_Colors::Text_Dark).name() + "; } "

        " QWidget#assetSearchWidget {"
        "       background: " + Dr::GetColor(Window_Colors::Background_Light).name() + "; } "
        " QLineEdit#assetSearchBar { "
        "       background-image: url(:/assets/gui_misc/search.png); "
        "       background-repeat: no-repeat; "
        "       background-position: left; "
        "       padding: 2 2 3 22; } "


        //################################################################################
        //##    StatusBar
        //################################################################################
        " QFrame#statusBar { "
        "       background: " + Dr::GetColor(Window_Colors::Button_Dark).name() + "; "
        "       border-top:    1 solid " + Dr::GetColor(Window_Colors::Seperator).name() + "; "
        "       border-bottom: 1 solid " + Dr::GetColor(Window_Colors::Button_Dark).name() + "; } "


        //################################################################################
        //##    Inspector Widgets - Spinboxes, LineEdits, Pushbutton Menus, Sliders
        //################################################################################
        // Check boxes, mostly in Inspector
        " QCheckBox#checkInspector::indicator { height: 27px; width: 32px; }"

        // Spin boxes, mostly in Inspector
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
        " QAbstractSpinBox:!enabled { background: transparent; border: none; "
        "       color: " + Dr::GetColor(Window_Colors::Text_Dark).name() + "; } "
        " QAbstractSpinBox:readOnly { background: transparent; border: none; "
        "       color: " + Dr::GetColor(Window_Colors::Text_Dark).name() + "; } "

        " QAbstractSpinBox#spinBool:!enabled { color: transparent; }"

        // Right click menu, mostly in SpinBox context menus
        " QMenu { "
        "       border: 1px solid black; "
        "       color: " + Dr::GetColor(Window_Colors::Text).name() + "; "
        "       background: " +   Dr::GetColor(Window_Colors::Button_Dark).name() + "; "
        "       selection-color: " + Dr::GetColor(Window_Colors::Shadow).name() + "; "
        "       selection-background-color: " + Dr::GetColor(Window_Colors::Highlight).name() + ";"
        " } "
        " QMenu::item:!enabled { "
        "       color: " + Dr::GetColor(Window_Colors::Text_Dark).name() + "; "
        "}"


        // Line Edit, mostly in Inspector
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

        // Text Edit, mostly in Inspector
        " QTextEdit {               border: " + Dr::BorderWidth() + " solid; height: 20px; "
        "       border-top-left-radius: 4px; border-bottom-left-radius: 6px; border-top-right-radius: 4px; border-bottom-right-radius: 6px;"
        "       color: " + Dr::GetColor(Window_Colors::Text).name() + "; " + StyleSheetRecessedBackgroundBorder(5, 95) +
        "       selection-color: " + Dr::GetColor(Window_Colors::Shadow).name() + "; "
        "       selection-background-color: " + Dr::GetColor(Window_Colors::Highlight).name() + "; } "
        " QTextEdit:hover {         border: " + Dr::BorderWidth() + " solid; "
        "       color: " + Dr::GetColor(Window_Colors::Text_Light).name() + "; " + StyleSheetRecessedBackgroundBorder(5, 95, true) + " }"
        " QTextEdit:focus { "
        "       color: " + Dr::GetColor(Window_Colors::Highlight).name() + "; "
        "       background: " + Dr::GetColor(Window_Colors::Shadow).name() + "; "
        "       border: " + Dr::BorderWidth() + " solid " + Dr::GetColor(Window_Colors::Icon_Dark).name() + "; "
        "       border-radius: 0px; }"
        " QTextEdit::!enabled { background: transparent; border: none; "
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
                StyleSheetPoppedOutBackgroundBorder(Dr::GetColor(Window_Colors::Background_Light), Dr::GetColor(Window_Colors::Background_Light), 10, 92, true) + " }"
        " QPushButton#buttonDropDown:pressed { "
        "       color: " + Dr::GetColor(Window_Colors::Highlight).name() + "; "
        "       background: " + Dr::GetColor(Window_Colors::Shadow).name() + "; "
        "       border: " + Dr::BorderWidth() + " solid " + Dr::GetColor(Window_Colors::Icon_Dark).name() + "; "
        "       border-radius: 0px; }"
        " QPushButton#buttonDropDown:!enabled { "
        "       color: " + Dr::GetColor(Window_Colors::Text_Dark).name() + "; "
        "       background: transparent; border: none; }"

        // Little Edit / Delete Buttons on Inspector Image Property
//        " QPushButton#buttonImageMiniButton { border: " + Dr::BorderWidth() + " solid; height: 20px; border-radius: 4px; "
//        "       color: " + Dr::GetColor(Window_Colors::Text).name() + "; " +
//                StyleSheetPoppedOutBackgroundBorder(Dr::GetColor(Window_Colors::Background_Dark),
//                                                    Dr::GetColor(Window_Colors::Background_Dark), 9, 92) +
//        "       padding: 0px; } "
//        " QPushButton#buttonImageMiniButton:hover { border: " + Dr::BorderWidth() + " solid; "
//        "       color: " + Dr::GetColor(Window_Colors::Text_Light).name() + "; " +
//                StyleSheetPoppedOutBackgroundBorder(Dr::GetColor(Window_Colors::Background_Light),
//                                                    Dr::GetColor(Window_Colors::Background_Light), 10, 92, true) + " }"
//        " QPushButton#buttonImageMiniButton:pressed { "
//        ///"       border: " + Dr::BorderWidth() + " solid; "
//        ///"       color: " + Dr::GetColor(Window_Colors::Text_Light).name() + "; " + StyleSheetRecessedBackgroundBorder(10, 92) + " }"
//        "       color: " + Dr::GetColor(Window_Colors::Highlight).name() + "; "
//        "       background: " + Dr::GetColor(Window_Colors::Shadow).name() + "; "
//        "       border: " + Dr::BorderWidth() + " solid " + Dr::GetColor(Window_Colors::Icon_Dark).name() + "; "
//        "       border-radius: 0px; }"
        " QPushButton#buttonImageMiniButton {            border: 1px solid; border-radius: 4px; "
        "       background: " +    Dr::GetColor(Window_Colors::Background_Light).name() + "; "
        "       border-color: " +
                (Dr::GetColorScheme() == Color_Scheme::Light ? Dr::GetColor(Window_Colors::Background_Light).darker(130) :
                                                               Dr::GetColor(Window_Colors::Background_Light).lighter(200)).name() +
                                   Dr::GetColor(Window_Colors::Background_Light).darker(130).name() +
                                   Dr::GetColor(Window_Colors::Background_Light).name() +
                                   Dr::GetColor(Window_Colors::Background_Light).darker(130).name() +"; } "
        " QPushButton#buttonImageMiniButton:hover {      border: 1px solid; "
        "       background: " +    Dr::GetColor(Window_Colors::Button_Light).name() + "; "
        "       border-color: " +
                (Dr::GetColorScheme() == Color_Scheme::Light ? Dr::GetColor(Window_Colors::Button_Light).darker(130) :
                                                               Dr::GetColor(Window_Colors::Button_Light).lighter(200)).name() +
                                   Dr::GetColor(Window_Colors::Button_Light).darker(130).name() +
                                   Dr::GetColor(Window_Colors::Button_Light).name() +
                                   Dr::GetColor(Window_Colors::Button_Light).darker(130).name() +"; } "
        " QPushButton#buttonImageMiniButton:pressed {    border: 1px solid; "
        "       background: " +    Dr::GetColor(Window_Colors::Background_Dark).name() + "; "
        "       border-color: " +  Dr::GetColor(Window_Colors::Background_Dark).darker(150).name() +
                                   Dr::GetColor(Window_Colors::Background_Dark).name() +
                                   Dr::GetColor(Window_Colors::Background_Dark).darker(150).name() +
                                   Dr::GetColor(Window_Colors::Background_Dark).name() +"; } "

        // Popup Menu for PushButton
        " QMenu#menuComboBox { "
        "       padding-top: 4px; padding-bottom: 4px; "
        "       color: " + Dr::GetColor(Window_Colors::Text).name() + "; "
        "       font-size: " + QString::number(Dr::FontSize()) + "px; "
        "       border: " + Dr::BorderWidth() + " solid; margin: 0px; "
        "       border-color: " + Dr::GetColor(Window_Colors::Icon_Dark).name() + "; "
        "       background: " + Dr::GetColor(Window_Colors::Shadow).name() + "; }"
        " QMenu#menuComboBox::item { "
        "       padding-left: 18px; padding-top: 2px; padding-bottom: 3px; } "
        " QMenu#menuComboBox::item:selected { "
        "       padding-left: 22px; "
        "       color: " + Dr::GetColor(Window_Colors::Highlight).name() + "; "
        "       background: " + Dr::GetColor(Window_Colors::Button_Light).name() + "; } "
        " QMenu#menuComboBox::indicator:checked { left: 5px; top: -3px; } "
        " QMenu#menuComboBox::item:checked { "
        "       color: " + Dr::GetColor(Window_Colors::Highlight).name() + "; }"

        // Slider for Opacity, etc.
        " QSlider::groove:horizontal { height: 4px; background: black; "
        "       background: " + Dr::GetColor(Window_Colors::Background_Light).name() + "; } "
        " QSlider::handle:horizontal {"
        "       border: 2px solid; "
        "       border-color: " + Dr::GetColor(Window_Colors::Background_Dark).name() + "; "
        "       background: " +   Dr::GetColor(Window_Colors::Text).name() + "; "
        "       width: 12px;"
        "       margin: -6px 0; "                  // Expand outside the groove (ie "height")
        "       border-radius: 8px;"               // Turn into circle
        " } "
        " QSlider::handle:horizontal:hover {"
        "       background: " +   Dr::GetColor(Window_Colors::Highlight).name() + "; "
        " } "
        " QSlider::add-page:horizontal { border-radius: 2px; border-top: 1px solid; margin: 0 2; "
        "       background: " +   Dr::GetColor(Window_Colors::Background_Dark).name() + "; "
        "       border-color: " + Dr::GetColor(Window_Colors::Shadow).name() + "; } "
        " QSlider::sub-page:horizontal { border-radius: 2px; border-top: 1px solid; "
        "       background: " +   Dr::GetColor(Window_Colors::Icon_Dark).name() + "; "
        "       border-color: " + Dr::GetColor(Window_Colors::Icon_Light).name() + "; } "
        // Disabled Slider
        " QSlider::sub-page:horizontal:!enabled { border-radius: 2px; border-top: 1px solid; "
        "       background: " +   Dr::GetColor(Window_Colors::Text_Dark).name() + "; "
        "       border-color: " + Dr::GetColor(Window_Colors::Text_Dark).name() + "; } "
        " QSlider::handle:horizontal:!enabled {"
        "       background: " +   Dr::GetColor(Window_Colors::Text_Dark).name() + "; } "


        //################################################################################
        //##    FormPopup Styling
        //################################################################################
        // Popup menu RadioButton list
        " QRadioButton#popupRadio         { color: " + Dr::GetColor(Window_Colors::Text).name() + "; } "
        " QRadioButton#popupRadio:checked { color: " + Dr::GetColor(Window_Colors::Highlight).name() + "; } "
        " QRadioButton#popupRadio:hover   { color: " + Dr::GetColor(Window_Colors::Highlight).name() + "; "
        "       padding-left: 2px; } "
        " QRadioButton::indicator::checked       { image: url(:/assets/gui_misc/check.png); padding-left: 5px;  padding-right: 4px;  } "
        " QRadioButton::indicator::checked:hover { image: url(:/assets/gui_misc/check.png); padding-left: 3px;  padding-right: 6px; } "
        " QRadioButton::indicator::unchecked     { image: none; } "

        // Color picker and dialog buttons
        + Dr::StyleSheetColorButton(Dr::GetColor(Window_Colors::Background_Dark),
                                    Dr::GetColor(Window_Colors::Background_Dark),
                                    Dr::GetColor(Window_Colors::Background_Dark), 0, 0, 0, 0, false, true, "#buttonColorPicker") +
        " QPushButton#buttonColorPicker       { image: url(:/assets/gui_misc/dropper.png);       padding: 3px; } "
        " QPushButton#buttonColorPicker:hover { " +
        StyleSheetPoppedOutBackgroundBorder(Dr::GetColor(Window_Colors::Background_Light), Dr::GetColor(Window_Colors::Background_Light), 10, 92, true) + " }"

        + Dr::StyleSheetColorButton(Dr::GetColor(Window_Colors::Background_Dark),
                                    Dr::GetColor(Window_Colors::Background_Dark),
                                    Dr::GetColor(Window_Colors::Background_Dark), 0, 4, 0, 4, false, true, "#buttonColorDialog") +
        " QPushButton#buttonColorDialog       { image: url(:/assets/gui_misc/color_wheel.png);   padding: 3px; } "
        " QPushButton#buttonColorDialog:hover { " +
        StyleSheetPoppedOutBackgroundBorder(Dr::GetColor(Window_Colors::Background_Light), Dr::GetColor(Window_Colors::Background_Light), 10, 92, true) + " }"
        " QPushButton#buttonColorDialog:pressed { padding-left: 3px; padding-right: 3px; padding-top: 4px; padding-bottom: 2px; " +
        StyleSheetRecessedBackgroundBorder(10, 92, false) + " }"

        // Color popup Palette button
        + Dr::StyleSheetColorButton(Dr::GetColor(Window_Colors::Background_Dark),
                                    Dr::GetColor(Window_Colors::Background_Dark),
                                    Dr::GetColor(Window_Colors::Background_Dark), 3, 3, 3, 3, true, true, "#buttonColorPalette") +
        " QPushButton#buttonColorPalette       { image: url(:/assets/gui_misc/color_palette.png);   padding: 5px; margin-bottom: 1px; } "
        " QPushButton#buttonColorPalette:hover { " +
        StyleSheetPoppedOutBackgroundBorder(Dr::GetColor(Window_Colors::Background_Light), Dr::GetColor(Window_Colors::Background_Light), 10, 92, true) + " }"
        " QPushButton#buttonColorPalette:pressed { padding-left: 5px; padding-right: 5px; padding-top: 6px; padding-bottom: 4px; " +
        StyleSheetRecessedBackgroundBorder(10, 92, false) + " }"
        " QPushButton#buttonColorPalette:checked { padding-left: 5px; padding-right: 5px; padding-top: 6px; padding-bottom: 4px; " +
        StyleSheetRecessedBackgroundBorder(10, 92, false) + " }"

        // Color popup History button
        + Dr::StyleSheetColorButton(Dr::GetColor(Window_Colors::Background_Dark),
                                    Dr::GetColor(Window_Colors::Background_Dark),
                                    Dr::GetColor(Window_Colors::Background_Dark), 3, 3, 3, 3, true, true, "#buttonColorHistory") +
        " QPushButton#buttonColorHistory       { image: url(:/assets/gui_misc/color_history.png);   padding: 5px; } "
        " QPushButton#buttonColorHistory:hover { " +
        StyleSheetPoppedOutBackgroundBorder(Dr::GetColor(Window_Colors::Background_Light), Dr::GetColor(Window_Colors::Background_Light), 10, 92, true) + " }"
        " QPushButton#buttonColorHistory:pressed { padding-left: 5px; padding-right: 5px; padding-top: 6px; padding-bottom: 4px; " +
        StyleSheetRecessedBackgroundBorder(10, 92, false) + " }"
        " QPushButton#buttonColorHistory:checked { padding-left: 5px; padding-right: 5px; padding-top: 6px; padding-bottom: 4px; " +
        StyleSheetRecessedBackgroundBorder(10, 92, false) + " }"


    );

    style_sheet += Dr::StyleSheetToolBar();

    return style_sheet;
}


//################################################################################
//##    Cool sunken box kinda look
//################################################################################
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
                                        Dr::GetColor(Window_Colors::Background_Light).name() + " " +
                                        Dr::GetColor(Window_Colors::Background_Dark).darker(150).name() + "; ";
}

//################################################################################
//##    Cool popped out box kinda look
//################################################################################
QString StyleSheetPoppedOutBackgroundBorder(QColor background_color, QColor border_color, int top_percent, int bottom_percent, bool highlight) {
    QString top1 = QString::number(double(top_percent)        / 100.0);
    QString top2 = QString::number(double(top_percent + 1)    / 100.0);
    QString bot1 = QString::number(double(bottom_percent)     / 100.0);
    QString bot2 = QString::number(double(bottom_percent + 1) / 100.0);
    QColor middle;
    if (!highlight) middle = background_color;
    else            middle = background_color;
    return " background: qlineargradient(spread:pad, x1:0 y1:0, x2:0 y2:1, "
           "            stop:0 " +      background_color.lighter(175).name() + ", "
           "            stop:" + top1 + background_color.lighter(175).name() + ", "
           "            stop:" + top2 + middle.name() + ", "
           "            stop:" + bot1 + middle.name() + ", "
           "            stop:" + bot2 + background_color.darker(250).name() + ", "
           "            stop:1 " +      background_color.darker(250).name() + "); "
           "          border-color: " + border_color.darker(250).name() + "; "
           "          border-bottom-color: " +  border_color.darker(150).name() + "; ";
}

//################################################################################
//##    Generates the style sheet for use with Color, Color Dropper and Color Dialog buttons
//################################################################################
QString StyleSheetColorButton(QColor color, QColor text_color, QColor highlight,
                              int tl_radius, int tr_radius, int bl_radius, int br_radius,
                              bool left_border, bool right_border, QString name) {
    QString style =
    " QPushButton" + name + " { height: 20px; padding-top: 0px;"
    "   color: " + text_color.name() + "; "
    "   border-top: " +    Dr::BorderWidth() + " solid; "
    "   border-bottom: " + Dr::BorderWidth() + " solid; ";

    if (left_border)  style += "   border-left: " +   Dr::BorderWidth() + " solid; ";
    else              style += "   border-left: 0px; ";
    if (right_border) style += "   border-right: " +   Dr::BorderWidth() + " solid; ";
    else              style += "   border-right: 0px; ";

    style +=
    "   border-top-left-radius:     " + QString::number(tl_radius) + "px; "
    "   border-bottom-left-radius:  " + QString::number(bl_radius) + "px; "
    "   border-top-right-radius:    " + QString::number(tr_radius) + "px; "
    "   border-bottom-right-radius: " + QString::number(br_radius) + "px; "
    "   background: qlineargradient(spread:pad, x1:0 y1:0, x2:0 y2:1, "
    "       stop:0.00 " + color.lighter(200).name() + ", "
    "       stop:0.09 " + color.lighter(200).name() + ", "
    "       stop:0.10 " + color.name() + ", "
    "       stop:0.92 " + color.name() + ", "
    "       stop:0.93 " + color.darker(300).name() + ", "
    "       stop:1.00 " + color.darker(300).name() + "); "
    "   border-color: " + color.darker(300).name() + "; "
    "   border-bottom-color: " + color.darker(400).name() + "; } "
    " QPushButton" + name + ":hover {   color: " + highlight.name() + "; } "
    " QPushButton" + name + ":pressed { padding-top: 1px; "
    "   background: qlineargradient(spread:pad, x1:0 y1:0, x2:0 y2:1, "
    "       stop:0.00 " + color.darker(300).name() + ", "
    "       stop:0.09 " + color.darker(300).name() + ", "
    "       stop:0.10 " + color.name() + ", "
    "       stop:0.92 " + color.name() + ", "
    "       stop:0.93 " + color.darker(300).name() + ", "
    "       stop:1.00 " + color.darker(300).name() + "); "
    "   border-color: " + color.darker(300).name() + "; "
    "   border-bottom-color: " + Dr::GetColor(Window_Colors::Background_Dark).lighter(200).name() + "; } ";

    return style;
}




}   // namespace Dr








