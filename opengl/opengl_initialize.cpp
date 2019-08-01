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

    // ***** Turn on mouse tracking (for mouseMoveEvent)
    setMouseTracking( true );

    // Load some global OpenGL Info
    glGetIntegerv ( GL_MAX_TEXTURE_SIZE, &g_max_texture_size );
    g_max_rays =              (g_max_texture_size < c_desired_max_rays) ?          g_max_texture_size : c_desired_max_rays;
    g_max_occluder_fbo_size = (g_max_texture_size < c_desired_occluder_fbo_size) ? g_max_texture_size : c_desired_occluder_fbo_size;
    g_max_light_fbo_size =    (g_max_texture_size < c_desired_light_fbo_size) ?    g_max_texture_size : c_desired_light_fbo_size;

    // Load test resources
    m_engine->addTexture(Asset_Textures::Numbers,           ":/assets/engine/numbers.png");
    m_engine->addTexture(Asset_Textures::Water_Normal_1,    ":/assets/textures/water_normal.jpg");
    m_engine->addTexture(Asset_Textures::Water_Texture_1,   ":/assets/textures/water_texture.png");
    m_engine->addTexture(Asset_Textures::Water_Texture_2,   ":/assets/textures/water_texture_2.png");
    m_engine->addTexture(Asset_Textures::Lava_Texture_1,    ":/assets/textures/lava_texture.jpg");
    m_engine->addTexture(Asset_Textures::Ball,              ":/assets/test_images/ball_1.png");
    m_engine->addTexture(Asset_Textures::Block,             ":/assets/test_images/metal_block.png");
    m_engine->addTexture(Asset_Textures::Plant,             ":/assets/test_images/moon_plant_6.png");
    m_engine->addTexture(Asset_Textures::Rover,             ":/assets/test_images/rover_body.png");
    m_engine->addTexture(Asset_Textures::Wheel,             ":/assets/test_images/rover_wheel.png");
    m_engine->addTexture(Asset_Textures::Spare,             ":/assets/test_images/spare_wheel.png");


    // ***** Load resources from project
    for (auto asset_pair : m_engine->getProject()->getAssetMap() ) {
        DrAsset *asset = asset_pair.second;

        if (asset->getAssetType() == DrAssetType::Object) {
            QPixmap pixmap = asset->getComponentProperty(Components::Asset_Animation, Properties::Asset_Animation_Default)->getValue().value<QPixmap>();
            m_engine->addTexture( asset->getKey(), pixmap);
        }
    }



    // ******************** Shaders ********************

    // ***** Initialize our basic shader, shaders have 2 parts, a Vertex shader followed by a Fragment shader
    QOpenGLShader v_default_shader( QOpenGLShader::Vertex );        v_default_shader.compileSourceFile( ":/shaders/default_vert.glsl" );
    QOpenGLShader f_default_shader( QOpenGLShader::Fragment );      f_default_shader.compileSourceFile( ":/shaders/default_frag.glsl" );
    m_default_shader.addShader( &v_default_shader );
    m_default_shader.addShader( &f_default_shader );
    m_default_shader.link();

    // Vertex Shader Input
    a_default_vertex =          m_default_shader.attributeLocation( "vertex" );
    a_default_texture_coord =   m_default_shader.attributeLocation( "texture_coordinates" );
    u_default_matrix =          m_default_shader.uniformLocation(   "u_matrix" );

    // Fragment Shader Input
    u_default_texture =         m_default_shader.uniformLocation(   "u_texture" );
    u_default_alpha =           m_default_shader.uniformLocation(   "u_alpha" );
    u_default_tint =            m_default_shader.uniformLocation(   "u_tint" );

    u_default_width =           m_default_shader.uniformLocation(   "u_width" );
    u_default_height =          m_default_shader.uniformLocation(   "u_height" );
    u_default_time =            m_default_shader.uniformLocation(   "u_time" );
    u_default_pre =             m_default_shader.uniformLocation(   "u_premultiplied");

    u_default_pixel_x =         m_default_shader.uniformLocation(   "u_pixel_x" );
    u_default_pixel_y =         m_default_shader.uniformLocation(   "u_pixel_y" );
    u_default_negative =        m_default_shader.uniformLocation(   "u_negative" );
    u_default_grayscale =       m_default_shader.uniformLocation(   "u_grayscale" );
    u_default_hue =             m_default_shader.uniformLocation(   "u_hue" );
    u_default_saturation =      m_default_shader.uniformLocation(   "u_saturation" );
    u_default_contrast =        m_default_shader.uniformLocation(   "u_contrast" );
    u_default_brightness =      m_default_shader.uniformLocation(   "u_brightness" );

    u_default_bitrate =         m_default_shader.uniformLocation(   "u_bitrate" );
    u_default_cartoon =         m_default_shader.uniformLocation(   "u_cartoon" );
    u_default_wavy =            m_default_shader.uniformLocation(   "u_wavy" );
    u_default_fisheye =         m_default_shader.uniformLocation(   "u_fisheye" );


    // ***** Initialize our Occluder Map Shader
    QOpenGLShader v_occluder_shader( QOpenGLShader::Vertex );       v_occluder_shader.compileSourceFile( ":/shaders/default_vert.glsl" );
    QOpenGLShader f_occluder_shader( QOpenGLShader::Fragment );     f_occluder_shader.compileSourceFile( ":/shaders/occluder_frag.glsl" );
    m_occluder_shader.addShader( &v_occluder_shader );
    m_occluder_shader.addShader( &f_occluder_shader );
    m_occluder_shader.link();

    // Shadow Vertex Shader Input
    a_occluder_vertex =         m_occluder_shader.attributeLocation( "vertex" );
    a_occluder_texture_coord =  m_occluder_shader.attributeLocation( "texture_coordinates" );
    u_occluder_matrix =         m_occluder_shader.uniformLocation(   "u_matrix" );

    // Shadow Fragment Shader Input
    u_occluder_texture =        m_occluder_shader.uniformLocation(   "u_texture" );
    u_occluder_alpha =          m_occluder_shader.uniformLocation(   "u_alpha" );
    u_occluder_depth =          m_occluder_shader.uniformLocation(   "u_depth" );
    u_occluder_near_plane =     m_occluder_shader.uniformLocation(   "u_near_plane" );
    u_occluder_far_plane =      m_occluder_shader.uniformLocation(   "u_far_plane" );


    // ***** Initialize our Shadow Map Shader
    QOpenGLShader v_shadow_shader( QOpenGLShader::Vertex );         v_shadow_shader.compileSourceFile( ":/shaders/default_vert.glsl" );
    QOpenGLShader f_shadow_shader( QOpenGLShader::Fragment );       f_shadow_shader.compileSourceFile( ":/shaders/shadow_map_frag.glsl" );
    m_shadow_shader.addShader( &v_shadow_shader );
    m_shadow_shader.addShader( &f_shadow_shader );
    m_shadow_shader.link();

    // Shadow Vertex Shader Input
    a_shadow_vertex =           m_shadow_shader.attributeLocation(  "vertex" );
    a_shadow_texture_coord =    m_shadow_shader.attributeLocation(  "texture_coordinates" );
    u_shadow_matrix =           m_shadow_shader.uniformLocation(    "u_matrix" );

    // Shadow Fragment Shader Input
    u_shadow_texture =          m_shadow_shader.uniformLocation(    "u_texture" );
    u_shadow_resolution =       m_shadow_shader.uniformLocation(    "u_resolution" );
    u_shadow_ray_count =        m_shadow_shader.uniformLocation(    "u_ray_count" );
    u_shadow_depth =            m_shadow_shader.uniformLocation(    "u_depth" );
    u_shadow_near_plane =       m_shadow_shader.uniformLocation(    "u_near_plane" );


    // ***** Initialize our 2D Light Shader
    QOpenGLShader v_light_shader( QOpenGLShader::Vertex );          v_light_shader.compileSourceFile( ":/shaders/default_vert.glsl" );
    QOpenGLShader f_light_shader( QOpenGLShader::Fragment );        f_light_shader.compileSourceFile( ":/shaders/2d_light_frag.glsl" );
    m_light_shader.addShader( &v_light_shader );
    m_light_shader.addShader( &f_light_shader );
    m_light_shader.link();

    // 2D Light Vertex Shader Input
    a_light_vertex =          m_light_shader.attributeLocation(     "vertex" );
    a_light_texture_coord =       m_light_shader.attributeLocation( "texture_coordinates" );
    u_light_matrix =            m_light_shader.uniformLocation(     "u_matrix" );

    // 2D Light Fragment Shader Input
    u_light_texture =           m_light_shader.uniformLocation(     "u_texture" );
    u_light_ray_count =         m_light_shader.uniformLocation(     "u_ray_count" );
    u_light_diameter =          m_light_shader.uniformLocation(     "u_light_diameter" );
    u_light_fitted =            m_light_shader.uniformLocation(     "u_light_fitted" );
    u_light_alpha =             m_light_shader.uniformLocation(     "u_alpha" );
    u_light_color =             m_light_shader.uniformLocation(     "u_color" );
    u_light_cone =              m_light_shader.uniformLocation(     "u_cone" );
    u_light_shadows =           m_light_shader.uniformLocation(     "u_light_shadows" );
    u_light_intensity =         m_light_shader.uniformLocation(     "u_intensity" );
    u_light_blur =              m_light_shader.uniformLocation(     "u_blur" );
    u_light_draw_shadows =      m_light_shader.uniformLocation(     "u_draw_shadows" );
    u_light_is_glow =           m_light_shader.uniformLocation(     "u_is_glow" );


    // ***** Initialize our Screen Shader
    QOpenGLShader v_screen_shader( QOpenGLShader::Vertex );         v_screen_shader.compileSourceFile( ":/shaders/default_vert.glsl" );
    QOpenGLShader f_screen_shader( QOpenGLShader::Fragment );       f_screen_shader.compileSourceFile( ":/shaders/screen_frag.glsl" );
    m_screen_shader.addShader( &v_screen_shader );
    m_screen_shader.addShader( &f_screen_shader );
    m_screen_shader.link();

    // 2D Light Vertex Shader Input
    a_screen_vertex =           m_screen_shader.attributeLocation(  "vertex" );
    a_screen_texture_coord =    m_screen_shader.attributeLocation(  "texture_coordinates" );
    u_screen_matrix =           m_screen_shader.uniformLocation(    "u_matrix" );

    // 2D Light Fragment Shader Input
    u_screen_upper =            m_screen_shader.uniformLocation(    "u_upper" );
    u_screen_lower =            m_screen_shader.uniformLocation(    "u_lower" );
    u_screen_width =            m_screen_shader.uniformLocation(    "u_width" );
    u_screen_height =           m_screen_shader.uniformLocation(    "u_height" );


    // ***** Initialize our Kernel Shader
    QOpenGLShader v_kernel_shader( QOpenGLShader::Vertex );         v_kernel_shader.compileSourceFile( ":/shaders/default_vert.glsl" );
    QOpenGLShader f_kernel_shader( QOpenGLShader::Fragment );       f_kernel_shader.compileSourceFile( ":/shaders/kernel_frag.glsl" );
    m_kernel_shader.addShader( &v_kernel_shader );
    m_kernel_shader.addShader( &f_kernel_shader );
    m_kernel_shader.link();

    // Kernel Vertex Shader Input
    a_kernel_vertex =           m_kernel_shader.attributeLocation(  "vertex" );
    a_kernel_texture_coord =    m_kernel_shader.attributeLocation(  "texture_coordinates" );
    u_kernel_matrix =           m_kernel_shader.uniformLocation(    "u_matrix" );

    // Kernel Fragment Shader Input
    u_kernel_texture =          m_kernel_shader.uniformLocation(    "u_texture" );
    u_kernel_width =            m_kernel_shader.uniformLocation(    "u_width" );
    u_kernel_height =           m_kernel_shader.uniformLocation(    "u_height" );
    u_kernel_alpha =            m_kernel_shader.uniformLocation(    "u_alpha" );


    // ***** Initialize our Water Shader
    QOpenGLShader v_water_shader( QOpenGLShader::Vertex );          v_water_shader.compileSourceFile( ":/shaders/default_vert.glsl" );
    QOpenGLShader f_water_shader( QOpenGLShader::Fragment );        f_water_shader.compileSourceFile( ":/shaders/water_frag.glsl" );
    m_water_shader.addShader( &v_water_shader );
    m_water_shader.addShader( &f_water_shader );
    m_water_shader.link();

    // Vertex Shader Input
    a_water_vertex =            m_water_shader.attributeLocation(   "vertex" );
    a_water_texture_coord =     m_water_shader.attributeLocation(   "texture_coordinates" );
    u_water_matrix =            m_water_shader.uniformLocation(     "u_matrix" );

    // Fragment Shader Input
    u_water_alpha =             m_water_shader.uniformLocation(     "u_alpha" );
    u_water_zoom =              m_water_shader.uniformLocation(     "u_zoom" );
    u_water_pos =               m_water_shader.uniformLocation(     "u_position" );
    u_water_width =             m_water_shader.uniformLocation(     "u_width" );
    u_water_height =            m_water_shader.uniformLocation(     "u_height" );
    u_water_time =              m_water_shader.uniformLocation(     "u_time" );

    u_water_top =               m_water_shader.uniformLocation(     "u_water_top" );
    u_water_bottom =            m_water_shader.uniformLocation(     "u_water_bottom" );
    u_water_left =              m_water_shader.uniformLocation(     "u_water_left" );
    u_water_right =             m_water_shader.uniformLocation(     "u_water_right" );

    u_start_color =             m_water_shader.uniformLocation(     "u_start_color" );
    u_end_color =               m_water_shader.uniformLocation(     "u_end_color" );
    u_water_color_tint =        m_water_shader.uniformLocation(     "u_color_tint" );
    u_water_reflection =        m_water_shader.uniformLocation(     "u_reflection" );
    u_water_ripple_frequency =  m_water_shader.uniformLocation(     "u_ripple_frequency" );
    u_water_ripple_speed =      m_water_shader.uniformLocation(     "u_ripple_speed" );
    u_water_ripple_amplitude =  m_water_shader.uniformLocation(     "u_ripple_amplitude" );
    u_water_ripple_stretch =    m_water_shader.uniformLocation(     "u_ripple_stretch" );
    u_water_wave_frequency =    m_water_shader.uniformLocation(     "u_wave_frequency" );
    u_water_wave_speed =        m_water_shader.uniformLocation(     "u_wave_speed" );
    u_water_wave_amplitude =    m_water_shader.uniformLocation(     "u_wave_amplitude" );
    u_water_surface_color =     m_water_shader.uniformLocation(     "u_surface_color" );
    u_water_surface_tint =      m_water_shader.uniformLocation(     "u_surface_tint" );
    u_water_surface_height =    m_water_shader.uniformLocation(     "u_surface_height" );
    u_refract_reflection =      m_water_shader.uniformLocation(     "u_refract_reflection" );
    u_refract_underwater =      m_water_shader.uniformLocation(     "u_refract_underwater" );
    u_refract_texture =         m_water_shader.uniformLocation(     "u_refract_texture" );
    u_refract_foam =            m_water_shader.uniformLocation(     "u_refract_foam" );
    u_movement_speed =          m_water_shader.uniformLocation(     "u_movement_speed" );


}














