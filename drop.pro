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
    enums.cpp \
    form_main.cpp \
    main.cpp \
    editor_tree_inspector.cpp \
    editor_tree_assets.cpp \
    editor_tree_advisor.cpp \
    form_main_menu.cpp \
    project.cpp \
    project_world.cpp \
    project_asset.cpp \
    settings.cpp \
    settings_component.cpp \
    settings_component_property.cpp \
    colors.cpp \
    debug.cpp \
    library_misc.cpp \
    editor_tree_inspector_widgets.cpp \
    project_world_stage.cpp \
    project_world_stage_object.cpp \
    editor_item.cpp \
    editor_scene.cpp \
    editor_scene_undo_commands.cpp \
    editor_scene_select.cpp \
    editor_view.cpp \
    editor_view_mouse.cpp \
    editor_view_paint.cpp \
    editor_view_resize.cpp \
    editor_view_tooltip.cpp \
    editor_view_rotate.cpp \
    editor_view_select.cpp \
    project_image.cpp \
    editor_tree_project.cpp \
    constants.cpp \
    editor_view_mouse_move.cpp \
    editor_tree_inspector_update.cpp \
    editor_view_grid.cpp \
    editor_view_key_press.cpp \
    editor_scene_key_press.cpp \
    form_main_interface_editor.cpp \
    interface_editor_relay.cpp \
    globals.cpp \
    image_filter_color.cpp \
    editor_item_item_change.cpp \
    editor_item_paint.cpp \
    editor_view_paint_items.cpp \
    form_main_toolbar.cpp \
    library_style_widget.cpp \
    library_style_sheet.cpp \
    form_main_build.cpp \
    form_main_build_editor.cpp \
    form_main_toolbar_build.cpp \
    widgets.cpp \
    widgets_event_filters.cpp \
    widgets_layout.cpp \
    project_font.cpp \
    form_settings.cpp \
    editor_tree_assets_mouse.cpp \
    form_main_toolbar_button_click.cpp \
    library_style_sheet_toolbar.cpp \
    form_atlas.cpp \
    form_fonts.cpp \
    form_popup.cpp \
    form_blank.cpp

HEADERS += \
    enums.h \
    form_main.h \
    editor_tree_inspector.h \
    editor_tree_assets.h \
    editor_tree_advisor.h \
    project.h \
    project_world.h \
    project_asset.h \
    settings.h \
    settings_component.h \
    settings_component_property.h \
    library.h \
    colors.h \
    debug.h \
    project_world_stage.h \
    project_world_stage_object.h \
    editor_item.h \
    editor_scene.h \
    editor_view.h \
    editor_scene_undo_commands.h \
    project_image.h \
    editor_tree_project.h \
    constants.h \
    interface_editor_relay.h \
    globals.h \
    image_filter_color.h \
    enums_form_main.h \
    widgets.h \
    widgets_event_filters.h \
    widgets_layout.h \
    project_font.h \
    form_settings.h \
    form_atlas.h \
    form_fonts.h \
    form_popup.h \
    form_blank.h

FORMS +=

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    drop.qrc

DISTFILES +=
