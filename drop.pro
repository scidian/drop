#-------------------------------------------------
#
# Project created by QtCreator 2018-11-27T11:20:40
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Drop
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
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
    form_main_setup.cpp \
    main.cpp \
    editor_tree_inspector.cpp \
    editor_tree_assets.cpp \
    editor_tree_scene.cpp \
    editor_tree_advisor.cpp \
    editor_scene_scene.cpp \
    editor_scene_view.cpp \
    form_main_menu.cpp \
    editor_scene_view_paint.cpp \
    editor_scene_view_rotate.cpp \
    editor_scene_view_resize.cpp \
    editor_scene_view_select.cpp \
    project.cpp \
    project_world.cpp \
    project_world_scene.cpp \
    project_world_scene_object.cpp \
    project_asset.cpp \
    settings.cpp \
    settings_component.cpp \
    settings_component_property.cpp \
    editor_scene_item.cpp \
    editor_scene_view_mouse.cpp \
    editor_scene_scene_select.cpp \
    editor_scene_view_tooltip.cpp \
    colors.cpp \
    debug.cpp \
    interface_relay.cpp \
    library_misc.cpp \
    library_style.cpp \
    editor_scene_scene_undo.cpp \
    editor_tree_inspector_widgets.cpp

HEADERS += \
    enums.h \
    form_main.h \
    editor_tree_inspector.h \
    interface_relay.h \
    editor_tree_assets.h \
    editor_tree_scene.h \
    editor_tree_advisor.h \
    editor_scene_scene.h \
    editor_scene_view.h \
    project.h \
    project_world.h \
    project_world_scene.h \
    project_world_scene_object.h \
    project_asset.h \
    settings.h \
    settings_component.h \
    settings_component_property.h \
    editor_scene_item.h \
    library.h \
    colors.h \
    debug.h \
    editor_scene_scene_undo.h

FORMS +=

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    drop.qrc

DISTFILES += \
    README.md
