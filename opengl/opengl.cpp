//
//      Created by Stephens Nunnally on 5/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QSurfaceFormat>

#include "engine/engine.h"
#include "engine/engine_texture.h"
#include "engine/engine_world.h"
#include "engine/form_engine.h"
#include "engine_mesh/engine_vertex_data.h"
#include "engine_things/engine_thing_object.h"
#include "opengl/opengl.h"

//####################################################################################
//##    Constructor / Destructor
//####################################################################################
DrOpenGL::DrOpenGL(QWidget *parent, FormEngine *form_engine, DrEngine *engine) : QOpenGLWidget(parent), m_form_engine(form_engine), m_engine(engine) {
    // ***** DO NOT perform any OpenGL resource initialization in constructor, use initializeGL() instead    
    setFocusPolicy(Qt::FocusPolicy::StrongFocus);            // Must setFocus to accept KeyPress events
}

// Destroy OpenGL Resources
DrOpenGL::~DrOpenGL() {
    m_engine->clearWorlds();

    makeCurrent();
        // Delete textures
        m_engine->deleteTextures();

        // Delete frame buffer objects
        delete m_render_fbo;
        delete m_texture_fbo;
        delete m_glow_fbo;
        delete m_occluder_fbo;
        for (auto fbo_pair : m_occluders)
            delete fbo_pair.second;
        for (auto fbo_pair : m_shadows)
            delete fbo_pair.second;

        // Delete vertex buffer objects
        for (auto vbo_pair : m_texture_vbos) {
            vbo_pair.second->destroy();
            delete vbo_pair.second;
        }
        for (auto data_pair : m_texture_data)
            delete data_pair.second;
        m_cube_vbo->destroy();
        delete m_cube_vbo;
        delete m_cube_data;

    doneCurrent();
}


//####################################################################################
//##    Handle Window Resizing
//####################################################################################
void DrOpenGL::resizeGL(int w, int h) {
    glViewport(0, 0, w * devicePixelRatio(), h * devicePixelRatio());
}














