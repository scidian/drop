//
//      Created by Stephens Nunnally on 5/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QSurfaceFormat>

#include "engine/engine.h"
#include "engine/engine_object.h"
#include "engine/engine_texture.h"
#include "engine/form_engine.h"
#include "opengl/opengl.h"

//####################################################################################
//##        Constructor / Destructor
//####################################################################################
OpenGL::OpenGL(QWidget *parent, FormEngine *form_engine, DrEngine *engine) : QOpenGLWidget(parent), m_form_engine(form_engine), m_engine(engine) {
    // ***** DO NOT perform any OpenGL resource initialization in constructor, use initializeGL() instead    
    setFocusPolicy(Qt::FocusPolicy::StrongFocus);            // Must setFocus to accept KeyPress events
}

// Destroy OpenGL Resources
OpenGL::~OpenGL() {
    m_engine->clearWorlds();

    makeCurrent();
        m_engine->deleteTextures();
        delete m_render_fbo;
        delete m_texture_fbo;
        delete m_occluder_fbo;
        delete m_lights_fbo;
    doneCurrent();
}


//####################################################################################
//##        Handle Window Resizing
//####################################################################################
void OpenGL::resizeGL(int w, int h) {
    glViewport(0, 0, w * devicePixelRatio(), h * devicePixelRatio());
}














