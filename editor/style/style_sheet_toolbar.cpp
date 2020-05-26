//
//      Created by Stephens Nunnally on 3/24/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "core/colors/colors.h"
#include "editor/helper_library.h"
#include "editor/style/style.h"


namespace Dr {


//################################################################################
//##    ToolBar Style Sheet
//################################################################################
std::string StyleSheetToolBar() {
    return
    // ToolBar
    " QToolBar { background: " + Dr::GetColor(Window_Colors::Button_Dark).name() + "; "
    "       border-bottom: 0 solid; } "

    // ToolBar Push Buttons
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

    // ToolBar Selectable Buttons
    +
    StyleSheetToolBarModeButton("buttonModeWorldGraph", "toolbar_world_graph.png",   "toolbar_world_graph_faded.png",    5) +
    StyleSheetToolBarModeButton("buttonModeWorldEdit",  "toolbar_world_editor.png",  "toolbar_world_editor_faded.png",   5) +
    StyleSheetToolBarModeButton("buttonModeUIEdit",     "toolbar_ui_editor.png",     "toolbar_ui_editor_faded.png",      5) +

    " QToolButton {            border: 1px solid; "
    "       background: " +    Dr::GetColor(Window_Colors::Button_Light).name() + "; "
    "       border-color: " +  Dr::GetColor(Window_Colors::Button_Light).lighter(130).name() +
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
    StyleSheetToolBarButtons("buttonAddThing",          "add.png",              "add.png",              "add_gray.png",             4, 4, 4, 4, 4, false) +
    StyleSheetToolBarButtons("buttonDuplicate",         "duplicate.png",        "duplicate.png",        "duplicate_gray.png",       4, 4, 4, 4, 4, false) +
    StyleSheetToolBarButtons("buttonDeleteThing",       "trash.png",            "trash.png",            "trash_gray.png",           4, 4, 4, 4, 4, false) +
            " QToolButton#buttonDeleteThing                 { padding-right: 5px; padding-top: 3px; } " +

    StyleSheetToolBarButtons("buttonSendToBack",        "sendtoback.png",       "sendtoback.png",       "sendtoback_gray.png",      4, 4, 0, 4, 0, false) +
    StyleSheetToolBarButtons("buttonSendBackward",      "sendbackward.png",     "sendbackward.png",     "sendbackward_gray.png",    4, 0, 0, 0, 0, false) +
    StyleSheetToolBarButtons("buttonSendForward",       "sendforward.png",      "sendforward.png",      "sendforward_gray.png",     4, 0, 0, 0, 0, false) +
    StyleSheetToolBarButtons("buttonSendToFront",       "sendtofront.png",      "sendtofront.png",      "sendtofront_gray.png",     4, 0, 4, 0, 4, false) +

    StyleSheetToolBarButtons("buttonResetObject",       "reset.png",            "reset.png",            "reset_gray.png",           4, 4, 0, 4, 0, false) +
    StyleSheetToolBarButtons("buttonTransformFlipH",    "flip_h.png",           "flip_h.png",           "flip_h_gray.png",          4, 0, 0, 0, 0, false) +
    StyleSheetToolBarButtons("buttonTransformFlipV",    "flip_v.png",           "flip_v.png",           "flip_v_gray.png",          4, 0, 0, 0, 0, false) +
            " QToolButton#buttonTransformFlipV              { padding-bottom: 5px; } " +
    StyleSheetToolBarButtons("buttonTransformRotateL",  "rotate_l.png",         "rotate_l.png",         "rotate_l_gray.png",        4, 0, 0, 0, 0, false) +
    StyleSheetToolBarButtons("buttonTransformRotateR",  "rotate_r.png",         "rotate_r.png",         "rotate_r_gray.png",        4, 0, 4, 0, 4, false) +

    StyleSheetToolBarButtons("buttonGridOnTop",     "grid_ontop_uncheck.png",   "grid_ontop.png",       "grid_ontop_gray.png",      4, 4, 0, 4, 0, true)  +
    StyleSheetToolBarButtons("buttonResizeToGrid",  "grid_resize_uncheck.png",  "grid_resize.png",      "grid_resize.png",          4, 0, 0, 0, 0, true)  +
    StyleSheetToolBarButtons("buttonSnapToGrid",    "grid_snap_uncheck.png",    "grid_snap.png",      "toolbar_grid_snap_gray.png", 4, 0, 0, 0, 0, true)  +
            " QToolButton#buttonSnapToGrid:checked          { padding-bottom: 3px; } " +
    StyleSheetToolBarButtons("buttonSnapOptions",   "arrow.png",                "arrow.png",            "arrow.png",                2, 0, 4, 0, 4, false) +
            " QToolButton#buttonSnapOptions                 { padding-left:   2px; } "
            " QToolButton#buttonSnapOptions::menu-indicator { image: none; } " +

    StyleSheetToolBarButtons("buttonSnapToMap",     "grid_snap_uncheck.png",    "grid_snap.png",      "toolbar_grid_snap_gray.png", 4, 4, 4, 4, 4, true)  +
            " QToolButton#buttonSnapToMap:checked           { padding-bottom: 3px; } " +

    StyleSheetToolBarButtons("buttonPlay",              "play.png",             "play.png",             "play.png",                 4, 4, 4, 4, 4, false) +
    StyleSheetToolBarButtons("buttonPlayStage",         "play_stage.png",       "play_stage.png",       "play_stage.png",           4, 4, 4, 4, 4, false) +
            " QToolButton#buttonPlayStage                   { padding-left: 3px; padding-bottom: 2px; padding-top: 3px; } " +

    StyleSheetToolBarButtons("buttonPlayground",        "playground.png",       "playground.png",       "playground.png",           4, 4, 4, 4, 4, false) +
    StyleSheetToolBarButtons("buttonImageViewer",       "atlas.png",            "atlas.png",            "atlas.png",                4, 4, 4, 4, 4, false) +
    StyleSheetToolBarButtons("buttonSoundFiles",        "sound.png",            "sound.png",            "sound.png",                4, 4, 4, 4, 4, false) +
    StyleSheetToolBarButtons("buttonFontBuilder",       "fonts.png",            "fonts.png",            "fonts.png",                4, 4, 4, 4, 4, false) +
            " QToolButton#buttonFontBuilder                 { padding-left: 3px; padding-right: 3px; } " +
    StyleSheetToolBarButtons("buttonSettingsEditor",    "settings.png",         "settings.png",         "settings.png",             4, 4, 4, 4, 4, false) +
            " QToolButton#buttonSettingsEditor              { padding-left: 3px; padding-bottom: 3px; } " +


    // View ToolBar Buttons
    StyleSheetToolBarModeButton("mousePointer", "toolbar_mouse_pointer.png",    "toolbar_mouse_pointer_faded.png",      5) +
    StyleSheetToolBarModeButton("mouseHand",    "toolbar_mouse_hand.png",       "toolbar_mouse_hand_faded.png",         5) +
    StyleSheetToolBarModeButton("mouseMagnify", "toolbar_mouse_magnify.png",    "toolbar_mouse_magnify_faded.png",      5) +
    StyleSheetToolBarButtons(   "cameraOnOff",  "camera.png",                   "camera.png",           "camera.png",               4, 4, 4, 4, 4, true) +
            " QToolButton#cameraOnOff                       { padding-left: 3px; padding-bottom: 4px; padding-right: 3px; padding-top: 4px; } " +
    StyleSheetToolBarButtons(   "debugOnOff",   "debug_shape.png",              "debug_shape.png",      "debug_shape.png",          4, 4, 4, 4, 4, true) +
            " QToolButton#debugOnOff                        { padding-left: 3px; padding-bottom: 4px; padding-right: 3px; padding-top: 4px; } " +


    // Popup Menus
    " QMenu#menuToolBarOptions { "
    "      padding-top: 4px; padding-bottom: 4px; "
    "      color: " + Dr::GetColor(Window_Colors::Text).name() + "; "
    "      font-size: " + std::to_string(Dr::FontSize()) + "px; "
    "      border: " + Dr::BorderWidth() + " solid; margin: 0px; "
    "      border-color: " + Dr::GetColor(Window_Colors::Icon_Dark).name() + "; "
    "      background: " + Dr::GetColor(Window_Colors::Shadow).name() + "; }"
    " QMenu#menuToolBarOptions::item { padding-top: 2px; padding-bottom: 3px; } "
    " QMenu#menuToolBarOptions::item:selected { "
    "       padding-left: 2px; "
    "       color: " + Dr::GetColor(Window_Colors::Highlight).name() + "; "
    "       background: " + Dr::GetColor(Window_Colors::Button_Light).name() + "; } "
    " QMenu#menuToolBarOptions::indicator:checked { left: 5px; top: -3px; } "
    " QMenu#menuToolBarOptions::item:checked { "
    "       color: " + Dr::GetColor(Window_Colors::Highlight).name() + "; }"

    // FormEngine Buttons
    " QToolButton#engineButton {            border: 1px solid; border-radius: 4px; "
    "       color: " +         Dr::GetColor(Window_Colors::Text_Light).name() + "; "
    "       background: " +    Dr::GetColor(Window_Colors::Button_Light).name() + "; "
    "       border-color: " +  Dr::GetColor(Window_Colors::Button_Light).lighter(130).name() +
                               Dr::GetColor(Window_Colors::Button_Light).name() +
                               Dr::GetColor(Window_Colors::Button_Light).name() +
                               Dr::GetColor(Window_Colors::Button_Light).name() +"; } "
    " QToolButton#engineButton:hover {      border: 1px solid; "
    "       color: " +         Dr::GetColor(Window_Colors::Highlight).name() + "; "
    "       background: " +    Dr::GetColor(Window_Colors::Midlight).name() + "; "
    "       border-color: " +  Dr::GetColor(Window_Colors::Midlight).lighter(150).name() +
                               Dr::GetColor(Window_Colors::Midlight).name() +
                               Dr::GetColor(Window_Colors::Midlight).name() +
                               Dr::GetColor(Window_Colors::Midlight).name() +"; } "
    " QToolButton#engineButton:pressed {    border: 2px solid; "
    "       color: " +         Dr::GetColor(Window_Colors::Text).name() + "; "
    "       background: " +    Dr::GetColor(Window_Colors::Background_Dark).name() + "; "
    "       border-color: " +  Dr::GetColor(Window_Colors::Icon_Light).name() + "; } "
    " QToolButton#engineButton:disabled {   border: 1px solid; "
    "       color: " +         Dr::GetColor(Window_Colors::Background_Light).name() + "; } "

    "";
}


//################################################################################
//##    Formatting for "Mode" toolbar buttons
//################################################################################
std::string StyleSheetToolBarModeButton(std::string button_name, std::string icon_name, std::string icon_faded_name, int padding) {
    std::string pad = std::to_string(padding);
    return
    " QToolButton#" + button_name + " {                  border-radius: 4px; border: 1px solid;  "
    "       background: " +   Dr::GetColor(Window_Colors::Button_Dark).name() + "; "
    "       border-color: " + Dr::GetColor(Window_Colors::Button_Dark).name() + "; } "
    " QToolButton#" + button_name + ":hover:!checked {   border-radius: 4px; border: 1px solid; "
    "       background: " +   Dr::GetColor(Window_Colors::Button_Dark).name() + "; "
    "       border-color: " + Dr::GetColor(Window_Colors::Button_Dark).name() + "; } "
    " QToolButton#" + button_name + ":checked {          border-radius: 4px; border: 1px solid; " + StyleSheetRecessedBackgroundBorder(9, 95) +  "; } "
    " QToolButton#" + button_name + ":hover:checked {    border-radius: 4px; border: 1px solid; " + StyleSheetRecessedBackgroundBorder(9, 95) +  "; } "
    " QToolButton#" + button_name + "  {                image: url(:/assets/toolbar_icons/" + icon_name + "); padding: " + pad + "px; } "
    " QToolButton#" + button_name + ":!checked {        image: url(:/assets/toolbar_icons/" + icon_faded_name + "); padding: " + pad + "px; } "
    " QToolButton#" + button_name + ":pressed {         image: url(:/assets/toolbar_icons/" + icon_faded_name + "); padding: " + pad + "px; } "
    " QToolButton#" + button_name + ":checked {         image: url(:/assets/toolbar_icons/" + icon_name + "); padding: " + pad + "px; } "
    " QToolButton#" + button_name + ":hover:checked {   image: url(:/assets/toolbar_icons/" + icon_name + "); padding: " + pad + "px; } "
    " QToolButton#" + button_name + ":hover:!checked {  image: url(:/assets/toolbar_icons/" + icon_name + "); padding: " + pad + "px; } ";
}


//################################################################################
//##    Formatting for normal toolbar buttons
//################################################################################
std::string StyleSheetToolBarButtons(std::string button_name, std::string icon_unchecked, std::string icon_checked, std::string icon_disabled,
                                 int padding, int tl, int tr, int bl, int br, bool checkable) {
    std::string text =
        " QToolButton#" + button_name + "          { image: url(:/assets/toolbar_icons/toolbar_" + icon_unchecked + "); "
                                        "            padding: " + std::to_string(padding) + "px; } "
        " QToolButton#" + button_name + ":checked  { image: url(:/assets/toolbar_icons/toolbar_" + icon_checked + ");    } "
        " QToolButton#" + button_name + ":disabled { image: url(:/assets/toolbar_icons/toolbar_" + icon_disabled + ");   } "
        " QToolButton#" + button_name + " { border-top-left-radius: " +     std::to_string(tl) + "px;  "
                                        "   border-top-right-radius: " +    std::to_string(tr) + "px;  "
                                        "   border-bottom-left-radius: " +  std::to_string(bl) + "px;  "
                                        "   border-bottom-right-radius: " + std::to_string(br) + "px; } ";

    if (checkable) {
        if (Dr::GetColorScheme() == Color_Scheme::Light) {
            text +=
                " QToolButton#" + button_name + ":checked { border: 1px solid; "
                "       background: " +    Dr::GetColor(Window_Colors::Background_Light).name() + "; "
                "       border-color: " +  Dr::GetColor(Window_Colors::Background_Light).darker(200).name() + " " +
                                           Dr::GetColor(Window_Colors::Background_Light).name() + " " +
                                           Dr::GetColor(Window_Colors::Background_Light).name() + " " +
                                           Dr::GetColor(Window_Colors::Background_Light).name() +"; } ";
        } else {
            text +=
                " QToolButton#" + button_name + ":checked { border: 1px solid; "
                "       background: " +    Dr::GetColor(Window_Colors::Background_Dark).name() + "; "
                "       border-color: " +  Dr::GetColor(Window_Colors::Background_Dark).darker(150).name() + " " +
                                           Dr::GetColor(Window_Colors::Background_Dark).name() + " " +
                                           Dr::GetColor(Window_Colors::Background_Dark).darker(150).name() + " " +
                                           Dr::GetColor(Window_Colors::Background_Dark).name() +"; } ";
        }
    }
    return text;
}






}   // namespace Dr








