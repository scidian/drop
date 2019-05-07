//
//      Created by Stephens Nunnally on 5/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "engine/engine.h"
#include "engine/engine_texture.h"
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
    m_engine->has_scene = false;
    m_engine->deleteResources();

    doneCurrent();
}


//####################################################################################
//##        Handle Window Resizing
//####################################################################################
void OpenGL::resizeGL(int w, int h) {
    glViewport(0, 0, w * devicePixelRatio(), h * devicePixelRatio());
}



















