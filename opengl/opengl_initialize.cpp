//
//      Created by Stephens Nunnally on 5/5/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "engine/engine.h"
#include "engine/engine_object.h"
#include "opengl/opengl.h"
#include "project/project.h"
#include "project/project_asset.h"
#include "settings/settings.h"
#include "settings/settings_component_property.h"

//####################################################################################
//##        Initialize OpenGL Resources
//####################################################################################
void OpenGL::initializeGL() {

    // Set up the rendering context, load shaders and other resources, etc.:
    initializeOpenGLFunctions();
    glClearColor(0.0, 0.0, 0.0, 1.0f);

    // Load test resources
    m_engine->addTexture(Asset_Textures::Numbers,   ":/assets/engine/numbers.png");             // -1
    m_engine->addTexture(Asset_Textures::Ball,      ":/assets/test_images/ball_1.png");         // -4
    m_engine->addTexture(Asset_Textures::Block,     ":/assets/test_images/metal_block.png");    // -5
    m_engine->addTexture(Asset_Textures::Plant,     ":/assets/test_images/moon_plant_6.png");   // -6
    m_engine->addTexture(Asset_Textures::Rover,     ":/assets/test_images/rover_body.png");     // -7
    m_engine->addTexture(Asset_Textures::Wheel,     ":/assets/test_images/rover_wheel.png");    // -8
    m_engine->addTexture(Asset_Textures::Spare,     ":/assets/test_images/spare_wheel.png");    // -9

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

    // Vertex Shader Input
    m_attribute_vertex =    m_shader.attributeLocation( "vertex" );
    m_attribute_tex_coord = m_shader.attributeLocation( "texture_coordinates" );
    m_uniform_matrix =      m_shader.uniformLocation(   "u_matrix" );

    // Fragment Shader Input
    m_uniform_texture =     m_shader.uniformLocation(   "u_tex" );
    m_uniform_alpha =       m_shader.uniformLocation(   "u_alpha" );    
    m_uniform_width =       m_shader.uniformLocation(   "u_width" );
    m_uniform_height =      m_shader.uniformLocation(   "u_height" );
    m_uniform_pixel_x =     m_shader.uniformLocation(   "u_pixel_x" );
    m_uniform_pixel_y =     m_shader.uniformLocation(   "u_pixel_y" );
    m_uniform_negative =    m_shader.uniformLocation(   "u_negative" );
    m_uniform_grayscale =   m_shader.uniformLocation(   "u_grayscale" );
    m_uniform_hue =         m_shader.uniformLocation(   "u_hue" );
    m_uniform_saturation =  m_shader.uniformLocation(   "u_saturation" );
    m_uniform_contrast =    m_shader.uniformLocation(   "u_contrast" );
    m_uniform_brightness =  m_shader.uniformLocation(   "u_brightness" );
    m_uniform_kernel =      m_shader.uniformLocation(   "u_kernel" );

}













