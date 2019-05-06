//
//      Created by Stephens Nunnally on 5/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QKeyEvent>

#include "forms/form_engine.h"
#include "physics/physics_opengl.h"


//####################################################################################
//##        Key Events
//####################################################################################
void PhysicsOpenGL::keyPressEvent(QKeyEvent *event) {
    // When space bar is down, enabled mouse press and move to translate viewable area
    if (event->key() == Qt::Key::Key_Space)
        m_flag_spacebar = true;
    QOpenGLWidget::keyPressEvent(event);
}

void PhysicsOpenGL::keyReleaseEvent(QKeyEvent *event) {
    // When space bar is released, change mode back to select / move items
    if (event->key() == Qt::Key::Key_Space)
        m_flag_spacebar = false;
    QOpenGLWidget::keyReleaseEvent(event);
}
