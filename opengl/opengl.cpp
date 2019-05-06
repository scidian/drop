//
//      Created by Stephens Nunnally on 5/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "engine/engine.h"
#include "opengl/opengl.h"

//####################################################################################
//##        Constructor / Destructor
//####################################################################################
OpenGL::OpenGL(QWidget *parent, Engine *engine) : QOpenGLWidget(parent), m_engine(engine) {
    // ***** DO NOT perform any OpenGL resource initialization in constructor, use initializeGL() instead
}

OpenGL::~OpenGL() { }

void OpenGL::cleanUp() {
    makeCurrent();

    // Destroy OpenGL Resources
    delete m_engine->t_ball;
    delete m_engine->t_metal_block;
    delete m_engine->t_moon_plant;
    delete m_engine->t_rover_body;
    delete m_engine->t_rover_wheel;
    delete m_engine->t_spare_wheel;

    doneCurrent();
}


//####################################################################################
//##        Handle Window Resizing
//####################################################################################
void OpenGL::resizeGL(int w, int h) {
    glViewport( 0, 0, w, h );
}



















