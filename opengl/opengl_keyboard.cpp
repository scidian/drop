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
//##    Key Press
//####################################################################################
void DrOpenGL::keyPressEvent(QKeyEvent *event) {

           if (event->key() == Qt::Key_W) { g_keyboard_y =  1;
    } else if (event->key() == Qt::Key_A) { g_keyboard_x = -1;
    } else if (event->key() == Qt::Key_D) { g_keyboard_x =  1;
    } else if (event->key() == Qt::Key_S) { g_keyboard_y = -1;

    } else if (event->key() == Qt::Key_Up)      { g_keyboard_y =  1;
    } else if (event->key() == Qt::Key_Down)    { g_keyboard_x = -1;
    } else if (event->key() == Qt::Key_Left)    { g_pedal = Pedal::CounterClockwise;
    } else if (event->key() == Qt::Key_Right)   { g_pedal = Pedal::Clockwise;

    } else if (event->key() == Qt::Key::Key_P) {
        QString file_name = QStandardPaths::writableLocation(QStandardPaths::StandardLocation::DesktopLocation);
        file_name += "/" + m_engine->getProject()->getOption(Project_Options::Name).toString();
        file_name += " - " + QDateTime::currentDateTime().toString("yyyy-MM-dd-hhmmss-zzz") + ".png";
        this->grabFramebuffer().save( file_name );

    }

    QOpenGLWidget::keyPressEvent(event);
}

//####################################################################################
//##    Key Release
//####################################################################################
void DrOpenGL::keyReleaseEvent(QKeyEvent *event) {

           if (event->key() == Qt::Key_W) { if (g_keyboard_y > 0.0) g_keyboard_y = 0;
    } else if (event->key() == Qt::Key_A) { if (g_keyboard_x < 0.0) g_keyboard_x = 0;
    } else if (event->key() == Qt::Key_D) { if (g_keyboard_x > 0.0) g_keyboard_x = 0;
    } else if (event->key() == Qt::Key_S) { if (g_keyboard_y < 0.0) g_keyboard_y = 0;

    } else if (event->key() == Qt::Key_Up)      { if (g_keyboard_y > 0.0) g_keyboard_y = 0;
    } else if (event->key() == Qt::Key_Down)    { if (g_keyboard_x < 0.0) g_keyboard_x = 0;
    } else if (event->key() == Qt::Key_Left)    { g_pedal = Pedal::None;
    } else if (event->key() == Qt::Key_Right)   { g_pedal = Pedal::None;
    }

    QOpenGLWidget::keyReleaseEvent(event);
}










