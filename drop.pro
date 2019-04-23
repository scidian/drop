#-------------------------------------------------
#
# Project created by QtCreator 2018-11-27T11:20:40
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Drop
TEMPLATE = app

# The following define makes your compiler show warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
    colors/colors.cpp \
    colors/colors_palette_blank.cpp \
    colors/colors_palette_material.cpp \
    colors/colors_palette_rocky_rover.cpp \
    colors/colors_palette_window_themes.cpp \
    editor/editor_item.cpp \
    editor/editor_item_item_change.cpp \
    editor/editor_item_paint.cpp \
    editor/editor_scene.cpp \
    editor/editor_scene_key_press.cpp \
    editor/editor_scene_select.cpp \
    editor/editor_scene_undo_commands.cpp \
    editor/editor_scene_update.cpp \
    editor/editor_tree_advisor.cpp \
    editor/editor_tree_assets.cpp \
    editor/editor_tree_assets_mouse.cpp \
    editor/editor_tree_assets_update.cpp \
    editor/editor_tree_inspector.cpp \
    editor/editor_tree_inspector_update.cpp \
    editor/editor_tree_inspector_widgets.cpp \
    editor/editor_tree_inspector_widgets2.cpp \
    editor/editor_tree_project.cpp \
    editor/editor_tree_project_drag_drop.cpp \
    editor/editor_tree_project_selection.cpp \
    editor/editor_tree_project_update.cpp \
    editor/editor_view.cpp \
    editor/editor_view_drag_drop.cpp \
    editor/editor_view_grid.cpp \
    editor/editor_view_key_press.cpp \
    editor/editor_view_mouse.cpp \
    editor/editor_view_mouse_move.cpp \
    editor/editor_view_paint.cpp \
    editor/editor_view_paint_items.cpp \
    editor/editor_view_resize.cpp \
    editor/editor_view_rotate.cpp \
    editor/editor_view_select.cpp \
    editor/editor_view_tooltip.cpp \
    enums.cpp \
    forms/form_atlas.cpp \
    forms/form_blank.cpp \
    forms/form_color_magnifier.cpp \
    forms/form_fonts.cpp \
    forms/form_main.cpp \
    forms/form_main_build.cpp \
    forms/form_main_build_editor.cpp \
    forms/form_main_interface_editor.cpp \
    forms/form_main_menu.cpp \
    forms/form_main_toolbar.cpp \
    forms/form_main_toolbar_build.cpp \
    forms/form_main_toolbar_button_click.cpp \
    forms/form_popup.cpp \
    forms/form_popup_colors.cpp \
    forms/form_popup_grid_snap.cpp \
    forms/form_settings.cpp \
    main.cpp \
    project/project.cpp \
    project/project_asset.cpp \
    project/project_font.cpp \
    project/project_image.cpp \
    project/project_world.cpp \
    project/project_world_stage.cpp \
    project/project_world_stage_object.cpp \
    settings/settings.cpp \
    settings/settings_component.cpp \
    settings/settings_component_property.cpp \
    debug.cpp \
    library_misc.cpp \
    constants.cpp \
    interface_editor_relay.cpp \
    globals.cpp \
    image_filter_color.cpp \
    library_style_widget.cpp \
    library_style_sheet.cpp \
    widgets/widgets.cpp \
    widgets/widgets_event_filters.cpp \
    widgets/widgets_layout.cpp \
    library_style_sheet_toolbar.cpp

HEADERS += \
    colors/colors.h \
    editor/editor_item.h \
    editor/editor_scene.h \
    editor/editor_scene_undo_commands.h \
    editor/editor_tree_advisor.h \
    editor/editor_tree_assets.h \
    editor/editor_tree_inspector.h \
    editor/editor_tree_project.h \
    editor/editor_view.h \
    enums.h \
    forms/form_atlas.h \
    forms/form_blank.h \
    forms/form_color_magnifier.h \
    forms/form_fonts.h \
    forms/form_main.h \
    forms/form_popup.h \
    forms/form_settings.h \
    project/project.h \
    project/project_asset.h \
    project/project_font.h \
    project/project_image.h \
    project/project_world.h \
    project/project_world_stage.h \
    project/project_world_stage_object.h \
    settings/settings.h \
    settings/settings_component.h \
    settings/settings_component_property.h \
    library.h \
    debug.h \
    constants.h \
    interface_editor_relay.h \
    globals.h \
    image_filter_color.h \
    enums_form_main.h \
    widgets/widgets.h \
    widgets/widgets_event_filters.h \
    widgets/widgets_layout.h

FORMS +=

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    drop.qrc

DISTFILES +=
