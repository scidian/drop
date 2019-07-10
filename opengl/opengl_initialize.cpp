//
//      Created by Stephens Nunnally on 5/5/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "engine/engine.h"
#include "engine/engine_thing_object.h"
#include "opengl/opengl.h"
#include "project/project.h"
#include "project/project_asset.h"
#include "settings/settings.h"
#include "settings/settings_component_property.h"

// OpenGL Constant Definitions
int g_max_texture_size =        2048;
int g_max_rays =                2048;
int g_max_occluder_fbo_size =   2048;
int g_max_light_fbo_size =      2048;

//####################################################################################
//##        Initialize OpenGL Resources
//####################################################################################
void DrOpenGL::initializeGL() {

    // ***** Set up the rendering context, load shaders and other resources, etc.:
    initializeOpenGLFunctions();
    glClearColor(0.0, 0.0, 0.0, 0.0f);

    // Load some global OpenGL Info
    glGetIntegerv ( GL_MAX_TEXTURE_SIZE, &g_max_texture_size );
    g_max_rays =              (g_max_texture_size < c_desired_max_rays) ?          g_max_texture_size : c_desired_max_rays;
    g_max_occluder_fbo_size = (g_max_texture_size < c_desired_occluder_fbo_size) ? g_max_texture_size : c_desired_occluder_fbo_size;
    g_max_light_fbo_size =    (g_max_texture_size < c_desired_light_fbo_size) ?    g_max_texture_size : c_desired_light_fbo_size;

    // Load test resources
    m_engine->addTexture(Asset_Textures::Numbers,   ":/assets/engine/numbers.png");             // -1
    m_engine->addTexture(Asset_Textures::Ball,      ":/assets/test_images/ball_1.png");         // -4
    m_engine->addTexture(Asset_Textures::Block,     ":/assets/test_images/metal_block.png");    // -5
    m_engine->addTexture(Asset_Textures::Plant,     ":/assets/test_images/moon_plant_6.png");   // -6
    m_engine->addTexture(Asset_Textures::Rover,     ":/assets/test_images/rover_body.png");     // -7
    m_engine->addTexture(Asset_Textures::Wheel,     ":/assets/test_images/rover_wheel.png");    // -8
    m_engine->addTexture(Asset_Textures::Spare,     ":/assets/test_images/spare_wheel.png");    // -9


    // ***** Load resources from project
    for (auto asset_pair : m_engine->getProject()->getAssetMap() ) {
        DrAsset *asset = asset_pair.second;

        if (asset->getAssetType() == DrAssetType::Object) {
            QPixmap pixmap = asset->getComponentProperty(Components::Asset_Animation, Properties::Asset_Animation_Default)->getValue().value<QPixmap>();
            m_engine->addTexture( asset->getKey(), pixmap);
        }
    }


    // ***** Initialize our basic shader, shaders have 2 parts, a Vertex shader followed by a Fragment shader
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
    m_uniform_bitrate =     m_shader.uniformLocation(   "u_bitrate" );
    m_uniform_pixel_x =     m_shader.uniformLocation(   "u_pixel_x" );
    m_uniform_pixel_y =     m_shader.uniformLocation(   "u_pixel_y" );
    m_uniform_negative =    m_shader.uniformLocation(   "u_negative" );
    m_uniform_grayscale =   m_shader.uniformLocation(   "u_grayscale" );
    m_uniform_hue =         m_shader.uniformLocation(   "u_hue" );
    m_uniform_saturation =  m_shader.uniformLocation(   "u_saturation" );
    m_uniform_contrast =    m_shader.uniformLocation(   "u_contrast" );
    m_uniform_brightness =  m_shader.uniformLocation(   "u_brightness" );
    m_uniform_tint =        m_shader.uniformLocation(   "u_tint" );
    m_uniform_kernel =      m_shader.uniformLocation(   "u_kernel" );


    // ***** Initialize our Occluder Map Shader
    QOpenGLShader vOccluderShader( QOpenGLShader::Vertex );     vOccluderShader.compileSourceFile( ":/shaders/default_vert.glsl" );
    QOpenGLShader fOccluderShader( QOpenGLShader::Fragment );   fOccluderShader.compileSourceFile( ":/shaders/occluder_frag.glsl" );
    m_occluder_shader.addShader( &vOccluderShader );
    m_occluder_shader.addShader( &fOccluderShader );
    m_occluder_shader.link();

    // Shadow Vertex Shader Input
    m_attribute_occluder_vertex =     m_occluder_shader.attributeLocation( "vertex" );
    m_attribute_occluder_tex_coord =  m_occluder_shader.attributeLocation( "texture_coordinates" );
    m_uniform_occluder_matrix =       m_occluder_shader.uniformLocation(   "u_matrix" );

    // Shadow Fragment Shader Input
    m_uniform_occluder_texture =      m_occluder_shader.uniformLocation(   "u_texture" );
    m_uniform_occluder_alpha =        m_occluder_shader.uniformLocation(   "u_alpha" );
    m_uniform_occluder_depth =        m_occluder_shader.uniformLocation(   "u_depth" );


    // ***** Initialize our Shadow Map Shader
    QOpenGLShader vShadowShader( QOpenGLShader::Vertex );     vShadowShader.compileSourceFile( ":/shaders/default_vert.glsl" );
    QOpenGLShader fShadowShader( QOpenGLShader::Fragment );   fShadowShader.compileSourceFile( ":/shaders/shadow_map_frag.glsl" );
    m_shadow_shader.addShader( &vShadowShader );
    m_shadow_shader.addShader( &fShadowShader );
    m_shadow_shader.link();

    // Shadow Vertex Shader Input
    m_attribute_shadow_vertex =     m_shadow_shader.attributeLocation( "vertex" );
    m_attribute_shadow_tex_coord =  m_shadow_shader.attributeLocation( "texture_coordinates" );
    m_uniform_shadow_matrix =       m_shadow_shader.uniformLocation(   "u_matrix" );

    // Shadow Fragment Shader Input
    m_uniform_shadow_texture =      m_shadow_shader.uniformLocation(   "u_texture" );
    m_uniform_shadow_resolution =   m_shadow_shader.uniformLocation(   "u_resolution" );
    m_uniform_shadow_ray_count =    m_shadow_shader.uniformLocation(   "u_ray_count" );
    m_uniform_shadow_depth =        m_shadow_shader.uniformLocation(   "u_depth" );


    // ***** Initialize our 2D Light Shader
    QOpenGLShader vLightShader( QOpenGLShader::Vertex );     vLightShader.compileSourceFile( ":/shaders/default_vert.glsl" );
    QOpenGLShader fLightShader( QOpenGLShader::Fragment );   fLightShader.compileSourceFile( ":/shaders/2d_light_frag.glsl" );
    m_light_shader.addShader( &vLightShader );
    m_light_shader.addShader( &fLightShader );
    m_light_shader.link();

    // 2D Light Vertex Shader Input
    m_attribute_light_vertex =      m_light_shader.attributeLocation( "vertex" );
    m_attribute_light_tex_coord =   m_light_shader.attributeLocation( "texture_coordinates" );
    m_uniform_light_matrix =        m_light_shader.uniformLocation(   "u_matrix" );

    // 2D Light Fragment Shader Input
    m_uniform_light_texture =       m_light_shader.uniformLocation(   "u_texture" );
    m_uniform_light_diameter =      m_light_shader.uniformLocation(   "u_light_diameter" );
    m_uniform_light_fitted =        m_light_shader.uniformLocation(   "u_light_fitted" );
    m_uniform_light_alpha =         m_light_shader.uniformLocation(   "u_alpha" );
    m_uniform_light_color =         m_light_shader.uniformLocation(   "u_color" );
    m_uniform_light_cone =          m_light_shader.uniformLocation(   "u_cone" );
    m_uniform_light_shadows =       m_light_shader.uniformLocation(   "u_light_shadows" );
    m_uniform_light_intensity =     m_light_shader.uniformLocation(   "u_intensity" );
    m_uniform_light_blur =          m_light_shader.uniformLocation(   "u_blur" );
    m_uniform_light_draw_shadows =  m_light_shader.uniformLocation(   "u_draw_shadows" );
    m_uniform_light_is_glow =       m_light_shader.uniformLocation(   "u_is_glow" );

}













