//
//      Created by Stephens Nunnally on 5/5/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "engine/engine.h"
#include "opengl/opengl.h"
#include "project/project.h"
#include "project/project_asset.h"
#include "settings/settings.h"
#include "settings/settings_component_property.h"

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

    // Load test resources
    m_engine->addTexture(Test_Textures::Ball,  ":/assets/test_images/ball_1.png");          // -1
    m_engine->addTexture(Test_Textures::Block, ":/assets/test_images/metal_block.png");     // -2
    m_engine->addTexture(Test_Textures::Plant, ":/assets/test_images/moon_plant_6.png");    // -3
    m_engine->addTexture(Test_Textures::Rover, ":/assets/test_images/rover_body.png");      // -4
    m_engine->addTexture(Test_Textures::Wheel, ":/assets/test_images/rover_wheel.png");     // -5
    m_engine->addTexture(Test_Textures::Spare, ":/assets/test_images/spare_wheel.png");     // -6

    // Load resources from project
    for (auto asset_pair : m_engine->getProject()->getAssetMap() ) {
        DrAsset *asset = asset_pair.second;

        if (asset->getAssetType() == DrAssetType::Object) {
            QPixmap pixmap = asset->getComponentProperty(Components::Asset_Animation, Properties::Asset_Animation_Default)->getValue().value<QPixmap>();
            m_engine->addTexture( asset->getKey(), pixmap);
        }
    }


    // Initialize our basic shader, shaders have 2 parts, a Vertex shader followed by a Fragment shader
    QOpenGLShader vShader( QOpenGLShader::Vertex );     vShader.compileSourceFile( ":/shaders/default_vert.glsl" );
    QOpenGLShader fShader( QOpenGLShader::Fragment );   fShader.compileSourceFile( ":/shaders/default_frag.glsl" );
    m_shader.addShader( &vShader );
    m_shader.addShader( &fShader );
    m_shader.link();

    m_attribute_vertex =    m_shader.attributeLocation( "vertex" );
    m_attribute_tex_coord = m_shader.attributeLocation( "texCoord" );
    m_uniform_matrix =      m_shader.uniformLocation(   "matrix" );
    m_uniform_texture =     m_shader.uniformLocation(   "tex" );
    m_uniform_alpha =       m_shader.uniformLocation(   "alpha" );

}













