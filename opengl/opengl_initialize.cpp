//
//      Created by Stephens Nunnally on 5/5/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "engine/engine.h"
#include "opengl/opengl.h"

//####################################################################################
//##        Initialize OpenGL Resources
//####################################################################################
void OpenGL::initializeGL() {
    // Connect to aboutToBeDestroyed signal to make sure we clean up our resources
    // context() and QOpenGLContext::currentContext() are equivalent when called from initializeGL or paintGL.
    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &OpenGL::cleanUp);

    // Set up the rendering context, load shaders and other resources, etc.:
    initializeOpenGLFunctions();
    glClearColor(0.0, 0.0, 0.0, 1.0f);

    // Load resources
    m_engine->addTexture(":/assets/test_images/ball_1.png");            // 0
    m_engine->addTexture(":/assets/test_images/metal_block.png");       // 1
    m_engine->addTexture(":/assets/test_images/moon_plant_6.png");      // 2
    m_engine->addTexture(":/assets/test_images/rover_body.png");        // 3
    m_engine->addTexture(":/assets/test_images/rover_wheel.png");       // 4
    m_engine->addTexture(":/assets/test_images/spare_wheel.png");       // 5

    // Initialize our basic shader, shaders have 2 parts, a Vertex shader followed by a Fragment shader
    QOpenGLShader vShader( QOpenGLShader::Vertex );     vShader.compileSourceFile( ":/shaders/vShader.glsl" );
    QOpenGLShader fShader( QOpenGLShader::Fragment );   fShader.compileSourceFile( ":/shaders/fShader.glsl" );
    m_program.addShader( &vShader );
    m_program.addShader( &fShader );
    m_program.link();

    m_vertexAttr =      m_program.attributeLocation( "vertex" );
    m_texCoordAttr =    m_program.attributeLocation( "texCoord" );
    m_matrixUniform =   m_program.uniformLocation(   "matrix" );
    m_texUniform =      m_program.uniformLocation(   "tex" );
}













