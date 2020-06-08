//
//      Created by Stephens Nunnally on 3/24/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QDebug>

#include "core/colors/colors.h"
#include "editor/helper_library.h"
#include "editor/style/style.h"

namespace Dr {

// Local Constants
const int   c_toolbar_height =          40;                 // Main Toolbar height
const int   c_view_toolbar_height =     34;                 // View Toolbar height
const int   c_mode_size_w =             22;                 // Mode button width
const int   c_mode_size_h =             18;                 // Mode button height
const int   c_button_size_w =           20;                 // Normal tool button width
const int   c_button_size_h =           16;                 // Normal tool button height


//################################################################################
//##    ToolBar Style Sheet
//################################################################################
std::string StyleSheetToolBar() {
    return
    // ToolBar
    " QToolBar { "
    "       padding:    4px;"
    "       height:     " + std::to_string(Dr::Scale(c_toolbar_height)) + "px; "
    "       background: " + Dr::GetColor(Window_Colors::Button_Dark).name() + "; "
    "       border-bottom: 0 solid; } "
    " QWidget#widgetToolBar {           height: " + std::to_string(Dr::Scale(c_toolbar_height)) + "px; } "

    // Shared Groups
    " QWidget#widgetGroupMode {         height: " + std::to_string(Dr::Scale(c_toolbar_height)) + "px; } "
    " QWidget#widgetGroupEdit {         height: " + std::to_string(Dr::Scale(c_toolbar_height)) + "px; } "
    " QWidget#widgetGroupPlay {         height: " + std::to_string(Dr::Scale(c_toolbar_height)) + "px; } "
    " QWidget#widgetGroupSettings {     height: " + std::to_string(Dr::Scale(c_toolbar_height)) + "px; } "

    // View "Editor" Groups
    " QWidget#widgetGroupLayering {     height: " + std::to_string(Dr::Scale(c_toolbar_height)) + "px; } "
    " QWidget#widgetGroupTransform {    height: " + std::to_string(Dr::Scale(c_toolbar_height)) + "px; } "
    " QWidget#widgetGroupGridFull {     height: " + std::to_string(Dr::Scale(c_toolbar_height)) + "px; } "

    // View "Node" Groups
    " QWidget#widgetGroupGridSimple {   height: " + std::to_string(Dr::Scale(c_toolbar_height)) + "px; } "

    // View "Mixer" Groups
    " QWidget#widgetGroupVisual {       height: " + std::to_string(Dr::Scale(c_toolbar_height)) + "px; } "


    // Master Volume Dial
    " QDial#dialMasterVolume { qproperty-dialSize: " + std::to_string(Dr::Scale(c_toolbar_height - 8)) + "; }"



    // ToolBar Push Buttons
    " QPushButton#toolbarButton { "
    "       color: " + Dr::GetColor(Window_Colors::Text).name() + "; "
    "       background: " + Dr::GetColor(Window_Colors::Button_Light).name() + "; "
    "       border-top: " + std::to_string(Dr::Scale(1)) + "px solid; border-color: " + Dr::GetColor(Window_Colors::Button_Dark).name() + "; "
    "       border-radius: 4px; }"
    " QPushButton#toolbarButton:hover:!pressed { color: " + Dr::GetColor(Window_Colors::Highlight).name() + "; "
    "       background: " + Dr::GetColor(Window_Colors::Midlight).name() + "; }"
    " QPushButton#toolbarButton:pressed { color: " + Dr::GetColor(Window_Colors::Highlight).name() + "; "
    "       background: " + Dr::GetColor(Window_Colors::Background_Dark).name() + "; }"

    // QLabel Spacer
    " QLabel#labelSpacer {           border-left: " + std::to_string(Dr::Scale(1)) + "px solid; border-color: " + Dr::GetColor(Window_Colors::Shadow).name() + "; } "
    " QLabel#labelSpacerNotVisible { border-left: " + std::to_string(Dr::Scale(1)) + "px solid; border-color: transparent; } " +

    // ToolBar Selectable Buttons
    StyleSheetToolBarModeButton("buttonModeWorldGraph",     "toolbar_world_graph.png",      "toolbar_world_graph_faded.png",    5) +
    StyleSheetToolBarModeButton("buttonModeWorldEdit",      "toolbar_world_editor.png",     "toolbar_world_editor_faded.png",   5) +
    StyleSheetToolBarModeButton("buttonModeUIEdit",         "toolbar_ui_editor.png",        "toolbar_ui_editor_faded.png",      5) +
    StyleSheetToolBarModeButton("buttonModeSound",          "toolbar_sound.png",            "toolbar_sound_faded.png",          5) +

    " QToolButton {            border: " + std::to_string(Dr::Scale(1)) + "px solid; "
    "       background: " +    Dr::GetColor(Window_Colors::Button_Light).name() + "; "
    "       border-color: " +  Dr::GetColor(Window_Colors::Button_Light).lighter(130).name() +
                               Dr::GetColor(Window_Colors::Button_Light).name() +
                               Dr::GetColor(Window_Colors::Button_Light).name() +
                               Dr::GetColor(Window_Colors::Button_Light).name() +"; } "
    " QToolButton:hover {      border: " + std::to_string(Dr::Scale(1)) + "px solid; "
    "       background: " +    Dr::GetColor(Window_Colors::Midlight).name() + "; "
    "       border-color: " +  Dr::GetColor(Window_Colors::Midlight).lighter(150).name() +
                               Dr::GetColor(Window_Colors::Midlight).name() +
                               Dr::GetColor(Window_Colors::Midlight).name() +
                               Dr::GetColor(Window_Colors::Midlight).name() +"; } "
    " QToolButton:pressed {    border: " + std::to_string(Dr::Scale(1)) + "px solid; "
    "       background: " +    Dr::GetColor(Window_Colors::Background_Dark).name() + "; "
    "       border-color: " +  Dr::GetColor(Window_Colors::Background_Dark).darker(150).name() +
                               Dr::GetColor(Window_Colors::Background_Dark).name() +
                               Dr::GetColor(Window_Colors::Background_Dark).darker(150).name() +
                               Dr::GetColor(Window_Colors::Background_Dark).name() +"; } " +

    StyleSheetToolBarButtons("buttonAddThing",          "add.png",              "add.png",              "add_gray.png",                 4, 4, 4, 4, 4, false, 36) +
    StyleSheetToolBarButtons("buttonDuplicate",         "duplicate.png",        "duplicate.png",        "duplicate_gray.png",           4, 4, 4, 4, 4, false) +
    StyleSheetToolBarButtons("buttonDeleteThing",       "trash.png",            "trash.png",            "trash_gray.png",               4, 4, 4, 4, 4, false) +

    StyleSheetToolBarButtons("buttonSendToBack",        "sendtoback.png",       "sendtoback.png",       "sendtoback_gray.png",          4, 4, 0, 4, 0, false) +
    StyleSheetToolBarButtons("buttonSendBackward",      "sendbackward.png",     "sendbackward.png",     "sendbackward_gray.png",        4, 0, 0, 0, 0, false) +
    StyleSheetToolBarButtons("buttonSendForward",       "sendforward.png",      "sendforward.png",      "sendforward_gray.png",         4, 0, 0, 0, 0, false) +
    StyleSheetToolBarButtons("buttonSendToFront",       "sendtofront.png",      "sendtofront.png",      "sendtofront_gray.png",         4, 0, 4, 0, 4, false) +

    StyleSheetToolBarButtons("buttonResetObject",       "reset.png",            "reset.png",            "reset_gray.png",               4, 4, 0, 4, 0, false) +
    StyleSheetToolBarButtons("buttonTransformFlipH",    "flip_h.png",           "flip_h.png",           "flip_h_gray.png",              4, 0, 0, 0, 0, false) +
    StyleSheetToolBarButtons("buttonTransformFlipV",    "flip_v.png",           "flip_v.png",           "flip_v_gray.png",              4, 0, 0, 0, 0, false) +
    StyleSheetToolBarButtons("buttonTransformRotateL",  "rotate_l.png",         "rotate_l.png",         "rotate_l_gray.png",            4, 0, 0, 0, 0, false) +
    StyleSheetToolBarButtons("buttonTransformRotateR",  "rotate_r.png",         "rotate_r.png",         "rotate_r_gray.png",            4, 0, 4, 0, 4, false) +

    StyleSheetToolBarButtons("buttonGridOnTop",     "grid_ontop_uncheck.png",   "grid_ontop.png",       "grid_ontop_gray.png",          4, 4, 0, 4, 0, true)  +
    StyleSheetToolBarButtons("buttonResizeToGrid",  "grid_resize_uncheck.png",  "grid_resize.png",      "grid_resize.png",              4, 0, 0, 0, 0, true)  +
    StyleSheetToolBarButtons("buttonSnapToGrid",    "grid_snap_uncheck.png",    "grid_snap.png",        "toolbar_grid_snap_gray.png",   4, 0, 0, 0, 0, true)  +
    StyleSheetToolBarButtons("buttonSnapOptions",   "arrow.png",                "arrow.png",            "arrow.png",                    4, 0, 4, 0, 4, false, 10) +
            " QToolButton#buttonSnapOptions::menu-indicator { image: none; } " +

    StyleSheetToolBarButtons("buttonSnapToMap",     "grid_snap_uncheck.png",    "grid_snap.png",        "toolbar_grid_snap_gray.png",   4, 4, 4, 4, 4, true)  +

    StyleSheetToolBarButtons("buttonPlay",              "play.png",             "play.png",             "play.png",                     4, 4, 4, 4, 4, false, 36) +
    StyleSheetToolBarButtons("buttonPlayStage",         "play_stage.png",       "play_stage.png",       "play_stage.png",               4, 4, 4, 4, 4, false) +

    StyleSheetToolBarButtons("buttonPlayground",        "playground.png",       "playground.png",       "playground.png",               4, 4, 4, 4, 4, false) +
    StyleSheetToolBarButtons("buttonImageViewer",       "atlas.png",            "atlas.png",            "atlas.png",                    4, 4, 4, 4, 4, false) +
    StyleSheetToolBarButtons("buttonSoundFiles",        "sound.png",            "sound.png",            "sound.png",                    4, 4, 4, 4, 4, false) +
    StyleSheetToolBarButtons("buttonFontBuilder",       "fonts.png",            "fonts.png",            "fonts.png",                    4, 4, 4, 4, 4, false) +
    StyleSheetToolBarButtons("buttonSettingsEditor",    "settings.png",         "settings.png",         "settings.png",                 4, 4, 4, 4, 4, false) +


    // View ToolBar
    " QFrame#viewToolBar { "
    "       padding:    2px;"
    "       height:     " + std::to_string(Dr::Scale(c_view_toolbar_height)) + "px; "
    "       background: " + Dr::GetColor(Window_Colors::Button_Dark).name() + "; "
    " } "
    " QWidget#widgetGroupMouse {        height: " + std::to_string(Dr::Scale(c_view_toolbar_height)) + "px; } "
    " QWidget#widgetGroupHandTool {     height: " + std::to_string(Dr::Scale(c_view_toolbar_height)) + "px; } "
    " QWidget#widgetGroupZoomTool {     height: " + std::to_string(Dr::Scale(c_view_toolbar_height)) + "px; } "
    " QWidget#widgetGroupToggle {       height: " + std::to_string(Dr::Scale(c_view_toolbar_height)) + "px; } " +

    // View ToolBar Buttons
    StyleSheetToolBarModeButton("mousePointer",     "toolbar_mouse_pointer.png",    "toolbar_mouse_pointer_faded.png",      5, c_mode_size_w - 4, c_mode_size_h - 4) +
    StyleSheetToolBarModeButton("mouseHand",        "toolbar_mouse_hand.png",       "toolbar_mouse_hand_faded.png",         5, c_mode_size_w - 4, c_mode_size_h - 4) +
    StyleSheetToolBarModeButton("mouseMagnify",     "toolbar_mouse_magnify.png",    "toolbar_mouse_magnify_faded.png",      5, c_mode_size_w - 4, c_mode_size_h - 4) +

    StyleSheetToolBarButtons(   "cameraOnOff",      "camera.png",                   "camera.png",           "camera.png",               4, 4, 4, 4, 4, true) +
    StyleSheetToolBarButtons(   "debugOnOff",       "debug_shape.png",              "debug_shape.png",      "debug_shape.png",          4, 4, 4, 4, 4, true) +


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
std::string StyleSheetToolBarModeButton(std::string button_name, std::string icon_name, std::string icon_faded_name, int padding, int custom_width, int custom_height) {
    std::string pad = std::to_string(padding);
    int width =  c_mode_size_w;
    int height = c_mode_size_h;
    if (custom_width  != 0) width  = custom_width;
    if (custom_height != 0) height = custom_height;
    return
    " QToolButton#" + button_name + " { "
    "       width:  " + std::to_string(Dr::Scale(width)) + "px; "
    "       height: " + std::to_string(Dr::Scale(height)) + "px; "
    "       border-radius: 4px; border: " + std::to_string(Dr::Scale(1)) + "px solid;  "
    "       background: " +   Dr::GetColor(Window_Colors::Button_Dark).name() + "; "
    "       border-color: " + Dr::GetColor(Window_Colors::Button_Dark).name() + "; } "
    " QToolButton#" + button_name + ":hover:!checked { "
    "       border-radius: 4px; border: " + std::to_string(Dr::Scale(1)) + "px solid; "
    "       background: " +   Dr::GetColor(Window_Colors::Button_Dark).name() + "; "
    "       border-color: " + Dr::GetColor(Window_Colors::Button_Dark).name() + "; } "
    " QToolButton#" + button_name + ":checked {          border-radius: 4px; border: " + std::to_string(Dr::Scale(1)) + "px solid; " + StyleSheetRecessedBackgroundBorder(9, 95) +  "; } "
    " QToolButton#" + button_name + ":hover:checked {    border-radius: 4px; border: " + std::to_string(Dr::Scale(1)) + "px solid; " + StyleSheetRecessedBackgroundBorder(9, 95) +  "; } "
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
                                     int padding, int tl, int tr, int bl, int br, bool checkable, int custom_width, int custom_height) {
    int width =  c_button_size_w;
    int height = c_button_size_h;
    if (custom_width  != 0) width  = custom_width;
    if (custom_height != 0) height = custom_height;

    std::string text =
        " QToolButton#" + button_name + " { "
        "       width:  " + std::to_string(Dr::Scale(width))  + "px; "
        "       height: " + std::to_string(Dr::Scale(height)) + "px; "
        "       image: url(:/assets/toolbar_icons/toolbar_" + icon_unchecked + "); "
        "       padding: " + std::to_string(padding) + "px; } "
        " QToolButton#" + button_name + ":checked  { image: url(:/assets/toolbar_icons/toolbar_" + icon_checked + ");    } "
        " QToolButton#" + button_name + ":disabled { image: url(:/assets/toolbar_icons/toolbar_" + icon_disabled + ");   } "
        " QToolButton#" + button_name + " { border-top-left-radius: " +     std::to_string(tl) + "px;  "
                                        "   border-top-right-radius: " +    std::to_string(tr) + "px;  "
                                        "   border-bottom-left-radius: " +  std::to_string(bl) + "px;  "
                                        "   border-bottom-right-radius: " + std::to_string(br) + "px; } ";

    if (checkable) {
        if (Dr::GetColorScheme() == Color_Scheme::Light) {
            text +=
                " QToolButton#" + button_name + ":checked { "
                "       border: " + std::to_string(Dr::Scale(1)) + "px solid; "
                "       background: " +    Dr::GetColor(Window_Colors::Background_Light).name() + "; "
                "       border-color: " +  Dr::GetColor(Window_Colors::Background_Light).darker(200).name() + " " +
                                           Dr::GetColor(Window_Colors::Background_Light).name() + " " +
                                           Dr::GetColor(Window_Colors::Background_Light).name() + " " +
                                           Dr::GetColor(Window_Colors::Background_Light).name() +"; } ";
        } else {
            text +=
                " QToolButton#" + button_name + ":checked { "
                "       border: " + std::to_string(Dr::Scale(1)) + "px solid; "
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








