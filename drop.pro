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
    main.cpp \
    enums.cpp \
    01_project.cpp \
    02_world.cpp \
    10_asset.cpp \
    03_scene.cpp \
    04_object.cpp \
    31_component.cpp \
    32_property.cpp \
    30_settings.cpp \
    globals.cpp \
    form_main.cpp \
    form_main_setup.cpp \
    form_main_scene_list.cpp \
    form_main_inspector.cpp

HEADERS += \
    enums.h \
    01_project.h \
    02_world.h \
    03_scene.h \
    04_object.h \
    31_component.h \
    32_property.h \
    10_asset.h \
    30_settings.h \
    form_main.h \
    globals.h

FORMS +=

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    drop.qrc

DISTFILES +=
