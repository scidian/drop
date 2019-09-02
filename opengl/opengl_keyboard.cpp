//
//      Created by Stephens Nunnally on 5/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QKeyEvent>
#include <QStandardPaths>
#include <QDateTime>

#include "engine/engine.h"
#include "engine_things/engine_thing_object.h"
#include "helper.h"
#include "opengl/opengl.h"
#include "project/project.h"

//####################################################################################
//##        Key Events
//####################################################################################
void DrOpenGL::keyPressEvent(QKeyEvent *event) {

    if (event->key() == Qt::Key::Key_A) {
        g_keyboard_x = -1;

    } else if (event->key() == Qt::Key::Key_D) {
        g_keyboard_x =  1;

    } else if (event->key() == Qt::Key::Key_W) {
        g_keyboard_y =  1;

    } else if (event->key() == Qt::Key::Key_S) {
        g_keyboard_y = -1;

    } else if (event->key() == Qt::Key::Key_P) {
        QString file_name = QStandardPaths::writableLocation(QStandardPaths::StandardLocation::DesktopLocation);
        file_name += "/" + m_engine->getProject()->getOption(Project_Options::Name).toString();
        file_name += " - " + QDateTime::currentDateTime().toString("yyyy-MM-dd-hhmmss-zzz") + ".png";
        this->grabFramebuffer().save( file_name );

    } else {
        QOpenGLWidget::keyPressEvent(event);
    }
}

void DrOpenGL::keyReleaseEvent(QKeyEvent *event) {

    if (event->key() == Qt::Key::Key_A) {
        if (g_keyboard_x < 0.0)
            g_keyboard_x = 0;

    } else if (event->key() == Qt::Key::Key_D) {
        if (g_keyboard_x > 0.0)
            g_keyboard_x = 0;

    } else if (event->key() == Qt::Key::Key_W) {
        if (g_keyboard_y > 0.0)
            g_keyboard_y = 0;

    } else if (event->key() == Qt::Key::Key_S) {
        if (g_keyboard_y < 0.0)
            g_keyboard_y = 0;

    } else {
        QOpenGLWidget::keyReleaseEvent(event);
    }
}










