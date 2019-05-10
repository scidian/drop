//
//      Created by Stephens Nunnally on 5/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QKeyEvent>

#include "engine/engine.h"
#include "opengl/opengl.h"
#include "helper.h"

//####################################################################################
//##        Key Events
//####################################################################################
void OpenGL::keyPressEvent(QKeyEvent *event) {

    if (event->key() == Qt::Key::Key_A) {
        m_engine->keyboard_x = -1;

    } else if (event->key() == Qt::Key::Key_D) {
        m_engine->keyboard_x =  1;

    } else {
        QOpenGLWidget::keyPressEvent(event);
    }
}

void OpenGL::keyReleaseEvent(QKeyEvent *event) {

    if (event->key() == Qt::Key::Key_A) {
        if (m_engine->keyboard_x < 0.0)
            m_engine->keyboard_x = 0;

    } else if (event->key() == Qt::Key::Key_D) {
        if (m_engine->keyboard_x > 0.0)
            m_engine->keyboard_x = 0;

    } else {
        QOpenGLWidget::keyReleaseEvent(event);
    }

}
