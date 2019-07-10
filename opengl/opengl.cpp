//
//      Created by Stephens Nunnally on 5/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QSurfaceFormat>

#include "engine/engine.h"
#include "engine/engine_thing_object.h"
#include "engine/engine_texture.h"
#include "engine/engine_world.h"
#include "engine/form_engine.h"
#include "opengl/opengl.h"

//####################################################################################
//##        Constructor / Destructor
//####################################################################################
DrOpenGL::DrOpenGL(QWidget *parent, FormEngine *form_engine, DrEngine *engine) : QOpenGLWidget(parent), m_form_engine(form_engine), m_engine(engine) {
    // ***** DO NOT perform any OpenGL resource initialization in constructor, use initializeGL() instead    
    setFocusPolicy(Qt::FocusPolicy::StrongFocus);            // Must setFocus to accept KeyPress events
}

// Destroy OpenGL Resources
DrOpenGL::~DrOpenGL() {
    m_engine->clearWorlds();

    makeCurrent();
        m_engine->deleteTextures();
        delete m_render_fbo;
        delete m_texture_fbo;
        delete m_glow_fbo;
        delete m_occluder_fbo;

        for (auto fbo_pair : m_occluders)
            delete fbo_pair.second;
        for (auto fbo_pair : m_shadows)
            delete fbo_pair.second;

    doneCurrent();
}


//####################################################################################
//##        Handle Window Resizing
//####################################################################################
void DrOpenGL::resizeGL(int w, int h) {
    glViewport(0, 0, w * devicePixelRatio(), h * devicePixelRatio());
}














