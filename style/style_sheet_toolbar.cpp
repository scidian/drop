//
//      Created by Stephens Nunnally on 3/24/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "colors/colors.h"
#include "helper.h"
#include "style.h"

namespace Dr {


//################################################################################
//##    Toolbar Style Sheet
//################################################################################
QString StyleSheetToolBar() {
    return
    // Toolbar
    " QToolBar { background: " + Dr::GetColor(Window_Colors::Button_Dark).name() + "; "
    "       border-bottom: 0 solid; } "

    // Toolbar Push Buttons
    " QPushButton#toolbarButton { "
    "       color: " + Dr::GetColor(Window_Colors::Text).name() + "; "
    "       background: " + Dr::GetColor(Window_Colors::Button_Light).name() + "; "
    "       border-top: 1 solid; border-color: " + Dr::GetColor(Window_Colors::Button_Dark).name() + "; "
    "       border-radius: 4px; }"
    " QPushButton#toolbarButton:hover:!pressed { color: " + Dr::GetColor(Window_Colors::Highlight).name() + "; "
    "       background: " + Dr::GetColor(Window_Colors::Midlight).name() + "; }"
    " QPushButton#toolbarButton:pressed { color: " + Dr::GetColor(Window_Colors::Highlight).name() + "; "
    "       background: " + Dr::GetColor(Window_Colors::Background_Dark).name() + "; }"

    // QLabel Spacer
    " QLabel#labelSpacer {           border-left: 1px solid; border-color: " + Dr::GetColor(Window_Colors::Shadow).name() + "; } "
    " QLabel#labelSpacerNotVisible { border-left: 1px solid; border-color: transparent; } "

    // Toobar Selectable Buttons
    +
    StyleSheetToolBarModeButton("buttonModeWorldMap",  "toolbar_world_map.png",     "toolbar_world_map_faded.png") +
    StyleSheetToolBarModeButton("buttonModeWorldEdit", "toolbar_world_editor.png",  "toolbar_world_editor_faded.png") +
    StyleSheetToolBarModeButton("buttonModeUIEdit",    "toolbar_ui_editor.png",     "toolbar_ui_editor_faded.png") +

    " QToolButton {            border: 1px solid; "
    "       background: " +    Dr::GetColor(Window_Colors::Button_Light).name() + "; "
    "       border-color: " +  Dr::GetColor(Window_Colors::Button_Light).lighter(150).name() +
                               Dr::GetColor(Window_Colors::Button_Light).name() +
                               Dr::GetColor(Window_Colors::Button_Light).name() +
                               Dr::GetColor(Window_Colors::Button_Light).name() +"; } "
    " QToolButton:hover {      border: 1px solid; "
    "       background: " +    Dr::GetColor(Window_Colors::Midlight).name() + "; "
    "       border-color: " +  Dr::GetColor(Window_Colors::Midlight).lighter(150).name() +
                               Dr::GetColor(Window_Colors::Midlight).name() +
                               Dr::GetColor(Window_Colors::Midlight).name() +
                               Dr::GetColor(Window_Colors::Midlight).name() +"; } "
    " QToolButton:pressed {    border: 1px solid; "
    "       background: " +    Dr::GetColor(Window_Colors::Background_Dark).name() + "; "
    "       border-color: " +  Dr::GetColor(Window_Colors::Background_Dark).darker(150).name() +
                               Dr::GetColor(Window_Colors::Background_Dark).name() +
                               Dr::GetColor(Window_Colors::Background_Dark).darker(150).name() +
                               Dr::GetColor(Window_Colors::Background_Dark).name() +"; } "

    +
    StyleSheetToolBarButtons("buttonSendToBack",        "sendtoback.png",       "sendtoback.png",       "sendtoback_gray.png",   4, 4, 0, 4, 0, false) +
    StyleSheetToolBarButtons("buttonSendBackward",      "sendbackward.png",     "sendbackward.png",     "sendbackward_gray.png", 4, 0, 0, 0, 0, false) +
    StyleSheetToolBarButtons("buttonSendForward",       "sendforward.png",      "sendforward.png",      "sendforward_gray.png",  4, 0, 0, 0, 0, false) +
    StyleSheetToolBarButtons("buttonSendToFront",       "sendtofront.png",      "sendtofront.png",      "sendtofront_gray.png",  4, 0, 4, 0, 4, false) +

    StyleSheetToolBarButtons("buttonResetObject",       "reset.png",            "reset.png",            "reset_gray.png",        4, 4, 0, 4, 0, false) +
    StyleSheetToolBarButtons("buttonTransformFlipH",    "flip_h.png",           "flip_h.png",           "flip_h_gray.png",       4, 0, 0, 0, 0, false) +
    StyleSheetToolBarButtons("buttonTransformFlipV",    "flip_v.png",           "flip_v.png",           "flip_v_gray.png",       4, 0, 0, 0, 0, false) +
            " QToolButton#buttonTransformFlipV              { padding-bottom: 5px; } " +
    StyleSheetToolBarButtons("buttonTransformRotateL",  "rotate_l.png",         "rotate_l.png",         "rotate_l_gray.png",     4, 0, 0, 0, 0, false) +
    StyleSheetToolBarButtons("buttonTransformRotateR",  "rotate_r.png",         "rotate_r.png",         "rotate_r_gray.png",     4, 0, 4, 0, 4, false) +

    StyleSheetToolBarButtons("buttonGridOnTop",     "grid_ontop_uncheck.png",   "grid_ontop.png",       "grid_ontop_gray.png",   4, 4, 0, 4, 0, true)  +
    StyleSheetToolBarButtons("buttonResizeToGrid",  "grid_resize_uncheck.png",  "grid_resize.png",      "grid_resize.png",       4, 0, 0, 0, 0, true)  +
    StyleSheetToolBarButtons("buttonSnapToGrid",    "grid_snap_uncheck.png",    "grid_snap.png",        "toolbar_grid_snap_gray.png", 4, 0, 0, 0, 0, true)  +
            " QToolButton#buttonSnapToGrid:checked          { padding-bottom: 3px; } " +
    StyleSheetToolBarButtons("buttonSnapOptions",   "arrow.png",                "arrow.png",            "arrow.png",             2, 0, 4, 0, 4, false) +
            " QToolButton#buttonSnapOptions                 { margin-left: -1px; } "
            " QToolButton#buttonSnapOptions::menu-indicator { image: none; } " +

    StyleSheetToolBarButtons("buttonPlay",              "play.png",             "play.png",             "play.png",              4, 4, 4, 4, 4, false) +

    StyleSheetToolBarButtons("buttonAtlasViewer",       "atlas.png",            "atlas.png",            "atlas.png",             4, 4, 4, 4, 4, false) +
    StyleSheetToolBarButtons("buttonFontBuilder",       "fonts.png",            "fonts.png",            "fonts.png",             4, 4, 4, 4, 4, false) +
    StyleSheetToolBarButtons("buttonSettingsEditor",    "settings.png",         "settings.png",         "settings.png",          4, 4, 4, 4, 4, false) +


    // Popup Menus
    " QMenu#menuToolbarOptions { "
    "      padding-top: 4px; padding-bottom: 4px; "
    "      color: " + Dr::GetColor(Window_Colors::Text).name() + "; "
    "      font-size: " + QString::number(Dr::FontSize()) + "px; "
    "      border: " + Dr::BorderWidth() + " solid; margin: 0px; "
    "      border-color: " + Dr::GetColor(Window_Colors::Icon_Dark).name() + "; "
    "      background: " + Dr::GetColor(Window_Colors::Shadow).name() + "; }"
    " QMenu#menuToolbarOptions::item { padding-top: 2px; padding-bottom: 3px; } "
    " QMenu#menuToolbarOptions::item:selected { "
    "       padding-left: 2px; "
    "       color: " + Dr::GetColor(Window_Colors::Highlight).name() + "; "
    "       background: " + Dr::GetColor(Window_Colors::Button_Light).name() + "; } "
    " QMenu#menuToolbarOptions::indicator:checked { left: 5px; top: -3px; } "
    " QMenu#menuToolbarOptions::item:checked { "
    "       color: " + Dr::GetColor(Window_Colors::Highlight).name() + "; }"


    "";
}


//################################################################################
//##    Formatting for "Mode" toolbar buttons
//################################################################################
QString StyleSheetToolBarModeButton(QString button_name, QString icon_name, QString icon_faded_name) {
    return
    " QToolButton#" + button_name + " {                  border-radius: 4px; border: 1px solid;  "
    "       background: " +   Dr::GetColor(Window_Colors::Button_Dark).name() + "; "
    "       border-color: " + Dr::GetColor(Window_Colors::Button_Dark).name() + "; } "
    " QToolButton#" + button_name + ":hover:!checked {   border-radius: 4px; border: 1px solid; "
    "       background: " +   Dr::GetColor(Window_Colors::Button_Dark).name() + "; "
    "       border-color: " + Dr::GetColor(Window_Colors::Button_Dark).name() + "; } "
    " QToolButton#" + button_name + ":checked {          border-radius: 4px; border: 1px solid; " + StyleSheetRecessedBackgroundBorder(12, 93) +  "; } "
    " QToolButton#" + button_name + ":hover:checked {    border-radius: 4px; border: 1px solid; " + StyleSheetRecessedBackgroundBorder(12, 93) +  "; } "
    " QToolButton#" + button_name + "  {                image: url(:/assets/toolbar_icons/" + icon_name + "); padding: 7px; } "
    " QToolButton#" + button_name + ":pressed {         image: url(:/assets/toolbar_icons/" + icon_faded_name + "); padding: 7px; } "
    " QToolButton#" + button_name + ":checked {         image: url(:/assets/toolbar_icons/" + icon_name + "); padding: 7px; } "
    " QToolButton#" + button_name + ":hover:checked {   image: url(:/assets/toolbar_icons/" + icon_name + "); padding: 7px; } "
    " QToolButton#" + button_name + ":hover:!checked {  image: url(:/assets/toolbar_icons/" + icon_name + "); padding: 7px; } ";
}


//################################################################################
//##    Formatting for normal toolbar buttons
//################################################################################
QString StyleSheetToolBarButtons(QString button_name, QString icon_unchecked, QString icon_checked, QString icon_disabled,
                                 int padding, int tl, int tr, int bl, int br, bool checkable) {
    QString text =
        " QToolButton#" + button_name + "          { image: url(:/assets/toolbar_icons/toolbar_" + icon_unchecked + "); "
                                        "            padding: " + QString::number(padding) + "px; } "
        " QToolButton#" + button_name + ":checked  { image: url(:/assets/toolbar_icons/toolbar_" + icon_checked + ");    } "
        " QToolButton#" + button_name + ":disabled { image: url(:/assets/toolbar_icons/toolbar_" + icon_disabled + ");   } "
        " QToolButton#" + button_name + " { border-top-left-radius: " +     QString::number(tl) + "px;  "
                                        "   border-top-right-radius: " +    QString::number(tr) + "px;  "
                                        "   border-bottom-left-radius: " +  QString::number(bl) + "px;  "
                                        "   border-bottom-right-radius: " + QString::number(br) + "px; } ";

    if (checkable) { text +=
        " QToolButton#" + button_name + ":checked { border: 1px solid; "
        "       background: " +    Dr::GetColor(Window_Colors::Background_Dark).name() + "; "
        "       border-color: " +  Dr::GetColor(Window_Colors::Background_Dark).darker(150).name() + " " +
                                   Dr::GetColor(Window_Colors::Background_Dark).name() + " " +
                                   Dr::GetColor(Window_Colors::Background_Dark).darker(150).name() + " " +
                                   Dr::GetColor(Window_Colors::Background_Dark).name() +"; } ";
    }
    return text;
}






}   // namespace Dr








