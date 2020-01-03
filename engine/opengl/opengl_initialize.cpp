//
//      Created by Stephens Nunnally on 5/5/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QApplication>
#include <QFile>
#include <QPixmap>
#include <QTemporaryDir>
#include <QTemporaryFile>
#include "editor/helper_library.h"

#include <algorithm>
#include <set>

#include "core/dr_debug.h"
#include "engine/engine.h"
#include "engine/engine_texture.h"
#include "engine/mesh/engine_vertex_data.h"
#include "engine/opengl/opengl.h"
#include "engine/things/engine_thing_object.h"
#include "project/dr_project.h"
#include "project/entities/dr_animation.h"
#include "project/entities/dr_asset.h"
#include "project/entities/dr_image.h"
#include "project/settings/settings.h"
#include "project/settings/settings_component_property.h"


// OpenGL Constant Definitions
int g_max_texture_size =        2048;
int g_max_rays =                2048;
int g_max_occluder_fbo_size =   2048;
int g_max_light_fbo_size =      2048;


//####################################################################################
//##    Initialize OpenGL Resources
//####################################################################################
void DrOpenGL::initializeGL() {

    // Set up the rendering context, load shaders and other resources, etc.:
    initializeOpenGLFunctions();
    glClearColor(0.0, 0.0, 0.0, 0.0f);

    // Turn on mouse tracking (for mouseMoveEvent)
    setMouseTracking( true );

    // Load some global OpenGL Info
    glGetIntegerv ( GL_MAX_TEXTURE_SIZE, &g_max_texture_size );
    g_max_rays =              (g_max_texture_size < c_desired_max_rays) ?          g_max_texture_size : c_desired_max_rays;
    g_max_occluder_fbo_size = (g_max_texture_size < c_desired_occluder_fbo_size) ? g_max_texture_size : c_desired_occluder_fbo_size;
    g_max_light_fbo_size =    (g_max_texture_size < c_desired_light_fbo_size) ?    g_max_texture_size : c_desired_light_fbo_size;

    // Load Textures / Models / Shaders
    loadBuiltInTextures();
    loadProjectTextures();
    loadBuiltInModels();
    loadShaders();
    loadFonts();
}


//####################################################################################
//##    Adds texture to Engine and creates 3D Extruded VBO for texture
//####################################################################################
void DrOpenGL::importTexture(long texture_id, std::string from_asset_string) {
    QPixmap pix = QPixmap(QString::fromStdString(from_asset_string));
    importTexture(texture_id, pix);
}

void DrOpenGL::importTexture(long texture_id, QPixmap &pixmap) {
    m_engine->addTexture(texture_id, pixmap);

    // Dont process Noise / Water / Fire / Built In Textures
    if (texture_id < 1 && texture_id > -100) return;

    // ***** 3D Extruded Textures
    // Create mesh
    m_texture_data[texture_id] = new DrEngineVertexData();
    m_texture_data[texture_id]->initializeExtrudedBitmap( Dr::FromQPixmap(pixmap), false );     // Run with 'false' to reduce number of triangles in scene
                                                                                                // Run with 'true' for better looking models in wireframe

    // Allocate mesh into vbo for use with OpenGL (could delete m_texture_data after this)
    m_texture_vbos[texture_id] = new QOpenGLBuffer();
    m_texture_vbos[texture_id]->create();
    m_texture_vbos[texture_id]->bind();
    m_texture_vbos[texture_id]->allocate(m_texture_data[texture_id]->constData(),
                                         m_texture_data[texture_id]->count() * static_cast<int>(sizeof(GLfloat)));
    m_texture_vbos[texture_id]->release();
}


//####################################################################################
//##    Built in temp textures and shader textures
//####################################################################################
void DrOpenGL::loadBuiltInTextures() {
    importTexture(Asset_Textures::Numbers,              ":/assets/engine/numbers.png");

    importTexture(Asset_Textures::Fire_Noise,           ":/assets/textures/fire_noise.png");
    importTexture(Asset_Textures::Fire_Flame_None,      ":/assets/textures/fire_flame_none.png");
    importTexture(Asset_Textures::Fire_Flame_Torch,     ":/assets/textures/fire_flame_torch.png");
    importTexture(Asset_Textures::Fire_Flame_Candle,    ":/assets/textures/fire_flame_candle.png");
    importTexture(Asset_Textures::Fire_Flame_Square,    ":/assets/textures/fire_flame_square.png");
    importTexture(Asset_Textures::Fire_Flame_Triangle,  ":/assets/textures/fire_flame_triangle.png");

    importTexture(Asset_Textures::Mirror_Noise_1,       ":/assets/textures/mirror_noise_1.png");

    importTexture(Asset_Textures::Water_Normal_1,       ":/assets/textures/water_normal.jpg");
    importTexture(Asset_Textures::Water_Texture_1,      ":/assets/textures/water_texture_1.jpg");
    importTexture(Asset_Textures::Water_Texture_2,      ":/assets/textures/water_texture_2.jpg");
    importTexture(Asset_Textures::Water_Texture_3,      ":/assets/textures/water_texture_3.jpg");
    importTexture(Asset_Textures::Water_Texture_4,      ":/assets/textures/water_texture_4.jpg");

    importTexture(Asset_Textures::Ball,                 ":/assets/test_images/ball_1.png");
    importTexture(Asset_Textures::Block,                ":/assets/test_images/metal_block.png");
    importTexture(Asset_Textures::Plant,                ":/assets/test_images/moon_plant_6.png");
    importTexture(Asset_Textures::Rover,                ":/assets/test_images/rover_body.png");
    importTexture(Asset_Textures::Wheel,                ":/assets/test_images/rover_wheel.png");
    importTexture(Asset_Textures::Spare,                ":/assets/test_images/spare_wheel.png");

}


//####################################################################################
//##    Load resources from project
//####################################################################################
void DrOpenGL::loadProjectTextures() {
    // Build list of DrImage keys used by Project, using std::set to ensure no duplicates
    std::set<long> image_keys_used;
    image_keys_used.insert(c_key_image_empty);
    for (auto &animation_pair : m_engine->getProject()->getAnimationMap()) {
        for (auto &frame : animation_pair.second->getFrames()) {
            long image_key = frame->getKey();
            if (image_keys_used.find(image_key) == image_keys_used.end()) {
                image_keys_used.insert(image_key);
            }
        }
    }

    // Go through Project Images and load Images being used by Project
    for (auto &image_key : image_keys_used) {
        DrImage *image = m_engine->getProject()->findImageFromKey(image_key);
        if (image != nullptr) {
            QPixmap pixmap = Dr::ToQPixmap(image->getBitmap());
            importTexture(image->getKey(), pixmap);
        }
    }
}

//####################################################################################
//##    Built in 3D Models
//####################################################################################
void DrOpenGL::loadBuiltInModels() {
    // ***** Quad Vertex Array and UV Texture Array
    m_quad_texture_coordinates.clear();
    m_quad_texture_coordinates.resize( 8 );
    m_quad_texture_coordinates[0] = 1;    m_quad_texture_coordinates[1] = 1;
    m_quad_texture_coordinates[2] = 0;    m_quad_texture_coordinates[3] = 1;
    m_quad_texture_coordinates[4] = 1;    m_quad_texture_coordinates[5] = 0;
    m_quad_texture_coordinates[6] = 0;    m_quad_texture_coordinates[7] = 0;

    m_quad_vertices.clear();
    m_quad_vertices.resize( 12 );               // in sets of x, y, z
    m_quad_vertices[ 0] =  0.5;    m_quad_vertices[ 1] =  0.5;    m_quad_vertices[ 2] = 0.0;        // Top Right
    m_quad_vertices[ 3] = -0.5;    m_quad_vertices[ 4] =  0.5;    m_quad_vertices[ 5] = 0.0;        // Top Left
    m_quad_vertices[ 6] =  0.5;    m_quad_vertices[ 7] = -0.5;    m_quad_vertices[ 8] = 0.0;        // Bottom Right
    m_quad_vertices[ 9] = -0.5;    m_quad_vertices[10] = -0.5;    m_quad_vertices[11] = 0.0;        // Bottom Left

    m_quad_barycentric.clear();
    m_quad_barycentric.resize( 12 );            // in sets of x, y, z
    m_quad_barycentric[ 0] = 1.0;   m_quad_barycentric[ 1] = 0.0;  m_quad_barycentric[ 2] = 0.0;    // Top Right
    m_quad_barycentric[ 3] = 0.0;   m_quad_barycentric[ 4] = 1.0;  m_quad_barycentric[ 5] = 0.0;    // Top Left
    m_quad_barycentric[ 6] = 0.0;   m_quad_barycentric[ 7] = 0.0;  m_quad_barycentric[ 8] = 1.0;    // Bottom Right
    m_quad_barycentric[ 9] = 1.0;   m_quad_barycentric[10] = 0.0;  m_quad_barycentric[11] = 0.0;    // Bottom Left

    // ***** Cone to use to turn textures into cones
    DrEngineVertexData *cone = new DrEngineVertexData();
    cone->initializeTextureCone();
        m_cone_vbo = new QOpenGLBuffer();
        m_cone_vbo->create();
        m_cone_vbo->bind();
        m_cone_vbo->allocate(cone->constData(), cone->count() * static_cast<int>(sizeof(GLfloat)));
        m_cone_vbo->release();
    delete cone;

    // ***** Cube to use to turn textures into cubes
    DrEngineVertexData *cube = new DrEngineVertexData();
    cube->initializeTextureCube();
        m_cube_vbo = new QOpenGLBuffer();
        m_cube_vbo->create();
        m_cube_vbo->bind();
        m_cube_vbo->allocate(cube->constData(), cube->count() * static_cast<int>(sizeof(GLfloat)));
        m_cube_vbo->release();
    delete cube;

    // ***** Quad to use to render textures
    DrEngineVertexData *quad = new DrEngineVertexData();
    quad->initializeTextureQuad();
        m_quad_vbo = new QOpenGLBuffer();
        m_quad_vbo->create();
        m_quad_vbo->bind();
        m_quad_vbo->allocate(quad->constData(), quad->count() * static_cast<int>(sizeof(GLfloat)));
        m_quad_vbo->release();
    delete quad;
}


//####################################################################################
//##    Debug Font
//####################################################################################
void DrOpenGL::loadFonts() {
    fs = glfonsCreate(512, 512, FONS_ZERO_TOPLEFT);
    if (fs == nullptr) Dr::PrintDebug("Could not create font stash");

    // Attempt to load font from file
    font_normal = fonsAddFont(fs, "sans", "Aileron-Regular.otf");

    // If could not laod font from file, attempt to copy from Qt Resource file using temporary directory
    if (font_normal == FONS_INVALID) {
        QTemporaryDir temp_directory;
        if (temp_directory.isValid()) {
            const QString temp_file = temp_directory.path() + "/Aileron-Regular.otf";
            qDebug() << "Loading font Aileron from temporary directory: " << temp_file;
            if (QFile::copy(":/assets/fonts/Aileron-Regular.otf", temp_file)) {
                font_normal = fonsAddFont(fs, "sans", temp_file.toStdString().c_str());
            }
        }
    }

    // If could not laod font from file, attempt to copy from Qt Resource file using temporary file
    if (font_normal == FONS_INVALID) {
        QTemporaryFile temp_file(qApp);
        temp_file.setFileTemplate("XXXXXX.otf");
        if (temp_file.open()) {
            qDebug() << "Loading font Aileron from temporary file: " << temp_file.fileName();
            QFile file(":/assets/fonts/Aileron-Regular.otf");
            if (file.open(QIODevice::ReadOnly)) {
                temp_file.write(file.readAll());
            }
            temp_file.close();
            font_normal = fonsAddFont(fs, "sans", temp_file.fileName().toStdString().c_str());
        }
    }

    if (font_normal == FONS_INVALID) {
        Dr::PrintDebug("Could not add font Aileron to font stash!");
    } else {
        unsigned int white = glfonsRGBA(255, 255, 255, 255);
        ///unsigned int brown = glfonsRGBA(192, 128,   0, 128);
        fonsClearState(fs);
        fonsSetFont( fs, font_normal);
        fonsSetSize( fs, 14.0f);
        fonsSetColor(fs, white);
    }
}


//####################################################################################
//##    Shaders
//####################################################################################
void DrOpenGL::loadShaders() {

    // ***** Initialize our basic shader, shaders have 2 parts, a Vertex shader followed by a Fragment shader
    QOpenGLShader v_default_shader( QOpenGLShader::Vertex );        v_default_shader.compileSourceFile( ":/shaders/default_vert.glsl" );
    QOpenGLShader f_default_shader( QOpenGLShader::Fragment );      f_default_shader.compileSourceFile( ":/shaders/default_frag.glsl" );
    m_default_shader.addShader( &v_default_shader );
    m_default_shader.addShader( &f_default_shader );
    m_default_shader.bindAttributeLocation("vertex",                PROGRAM_VERTEX_ATTRIBUTE);
    m_default_shader.bindAttributeLocation("normal",                PROGRAM_NORMAL_ATTRIBUTE);
    m_default_shader.bindAttributeLocation("texture_coordinates",   PROGRAM_TEXCOORD_ATTRIBUTE);
    m_default_shader.bindAttributeLocation("barycentric",           PROGRAM_BARYCENTRIC_ATTRIBUTE);
    m_default_shader.link();

    // Vertex Shader Input
    a_default_vertex =              m_default_shader.attributeLocation( "vertex" );
    a_default_normal =              m_default_shader.attributeLocation( "normal" );
    a_default_texture_coord =       m_default_shader.attributeLocation( "texture_coordinates" );
    a_default_barycentric =         m_default_shader.attributeLocation( "barycentric" );
    u_default_matrix =              m_default_shader.uniformLocation(   "u_matrix" );
    u_default_matrix_object =       m_default_shader.uniformLocation(   "u_matrix_object" );

    // Fragment Shader Input
    u_default_texture =             m_default_shader.uniformLocation(   "u_texture" );
    u_default_alpha =               m_default_shader.uniformLocation(   "u_alpha" );
    u_default_average_color =       m_default_shader.uniformLocation(   "u_average_color" );
    u_default_tint =                m_default_shader.uniformLocation(   "u_tint" );
    u_default_zoom =                m_default_shader.uniformLocation(   "u_zoom" );

    u_default_width =               m_default_shader.uniformLocation(   "u_width" );
    u_default_height =              m_default_shader.uniformLocation(   "u_height" );
    u_default_time =                m_default_shader.uniformLocation(   "u_time" );
    u_default_pre =                 m_default_shader.uniformLocation(   "u_premultiplied");

    u_default_pixel_x =             m_default_shader.uniformLocation(   "u_pixel_x" );
    u_default_pixel_y =             m_default_shader.uniformLocation(   "u_pixel_y" );
    u_default_pixel_offset =        m_default_shader.uniformLocation(   "u_pixel_offset" );
    u_default_negative =            m_default_shader.uniformLocation(   "u_negative" );
    u_default_grayscale =           m_default_shader.uniformLocation(   "u_grayscale" );
    u_default_hue =                 m_default_shader.uniformLocation(   "u_hue" );
    u_default_saturation =          m_default_shader.uniformLocation(   "u_saturation" );
    u_default_contrast =            m_default_shader.uniformLocation(   "u_contrast" );
    u_default_brightness =          m_default_shader.uniformLocation(   "u_brightness" );

    u_default_shade_away =          m_default_shader.uniformLocation(   "u_shade_away" );
    u_default_camera_pos =          m_default_shader.uniformLocation(   "u_camera_pos" );

    u_default_bitrate =             m_default_shader.uniformLocation(   "u_bitrate" );
    u_default_cartoon =             m_default_shader.uniformLocation(   "u_cartoon" );
    u_default_cartoon_width =       m_default_shader.uniformLocation(   "u_cartoon_width" );
    u_default_cross_hatch =         m_default_shader.uniformLocation(   "u_cross_hatch" );
    u_default_cross_hatch_width =   m_default_shader.uniformLocation(   "u_cross_hatch_width" );
    u_default_wavy =                m_default_shader.uniformLocation(   "u_wavy" );
    u_default_wireframe =           m_default_shader.uniformLocation(   "u_wireframe" );


    // ***** Initialize our Occluder Map Shader
    QOpenGLShader v_occluder_shader( QOpenGLShader::Vertex );       v_occluder_shader.compileSourceFile( ":/shaders/simple_vert.glsl" );
    QOpenGLShader f_occluder_shader( QOpenGLShader::Fragment );     f_occluder_shader.compileSourceFile( ":/shaders/frag_occluder.glsl" );
    m_occluder_shader.addShader( &v_occluder_shader );
    m_occluder_shader.addShader( &f_occluder_shader );
    m_occluder_shader.link();

    // Vertex Shader Input
    a_occluder_vertex =         m_occluder_shader.attributeLocation( "vertex" );
    a_occluder_texture_coord =  m_occluder_shader.attributeLocation( "texture_coordinates" );
    u_occluder_matrix =         m_occluder_shader.uniformLocation(   "u_matrix" );

    // Fragment Shader Input
    u_occluder_texture =        m_occluder_shader.uniformLocation(   "u_texture" );
    u_occluder_alpha =          m_occluder_shader.uniformLocation(   "u_alpha" );
    u_occluder_depth =          m_occluder_shader.uniformLocation(   "u_depth" );
    u_occluder_near_plane =     m_occluder_shader.uniformLocation(   "u_near_plane" );
    u_occluder_far_plane =      m_occluder_shader.uniformLocation(   "u_far_plane" );


    // ***** Initialize our Shadow Map Shader
    QOpenGLShader v_shadow_shader( QOpenGLShader::Vertex );         v_shadow_shader.compileSourceFile( ":/shaders/simple_vert.glsl" );
    QOpenGLShader f_shadow_shader( QOpenGLShader::Fragment );       f_shadow_shader.compileSourceFile( ":/shaders/frag_shadow_map.glsl" );
    m_shadow_shader.addShader( &v_shadow_shader );
    m_shadow_shader.addShader( &f_shadow_shader );
    m_shadow_shader.link();

    // Vertex Shader Input
    a_shadow_vertex =           m_shadow_shader.attributeLocation(  "vertex" );
    a_shadow_texture_coord =    m_shadow_shader.attributeLocation(  "texture_coordinates" );
    u_shadow_matrix =           m_shadow_shader.uniformLocation(    "u_matrix" );

    // Fragment Shader Input
    u_shadow_texture =          m_shadow_shader.uniformLocation(    "u_texture" );
    u_shadow_resolution =       m_shadow_shader.uniformLocation(    "u_resolution" );
    u_shadow_ray_count =        m_shadow_shader.uniformLocation(    "u_ray_count" );
    u_shadow_depth =            m_shadow_shader.uniformLocation(    "u_depth" );
    u_shadow_near_plane =       m_shadow_shader.uniformLocation(    "u_near_plane" );


    // ***** Initialize our 2D Light Shader
    QOpenGLShader v_light_shader( QOpenGLShader::Vertex );          v_light_shader.compileSourceFile( ":/shaders/simple_vert.glsl" );
    QOpenGLShader f_light_shader( QOpenGLShader::Fragment );        f_light_shader.compileSourceFile( ":/shaders/frag_2d_light.glsl" );
    m_light_shader.addShader( &v_light_shader );
    m_light_shader.addShader( &f_light_shader );
    m_light_shader.link();

    // Vertex Shader Input
    a_light_vertex =            m_light_shader.attributeLocation(   "vertex" );
    a_light_texture_coord =     m_light_shader.attributeLocation(   "texture_coordinates" );
    u_light_matrix =            m_light_shader.uniformLocation(     "u_matrix" );

    // Fragment Shader Input
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
    QOpenGLShader v_screen_shader( QOpenGLShader::Vertex );         v_screen_shader.compileSourceFile( ":/shaders/simple_vert.glsl" );
    QOpenGLShader f_screen_shader( QOpenGLShader::Fragment );       f_screen_shader.compileSourceFile( ":/shaders/frag_screen.glsl" );
    m_screen_shader.addShader( &v_screen_shader );
    m_screen_shader.addShader( &f_screen_shader );
    m_screen_shader.link();

    // Vertex Shader Input
    a_screen_vertex =           m_screen_shader.attributeLocation(  "vertex" );
    a_screen_texture_coord =    m_screen_shader.attributeLocation(  "texture_coordinates" );
    u_screen_matrix =           m_screen_shader.uniformLocation(    "u_matrix" );

    // Fragment Shader Input
    u_screen_upper =            m_screen_shader.uniformLocation(    "u_upper" );
    u_screen_lower =            m_screen_shader.uniformLocation(    "u_lower" );
    u_screen_blend =            m_screen_shader.uniformLocation(    "u_blend_mode" );
    u_screen_width =            m_screen_shader.uniformLocation(    "u_width" );
    u_screen_height =           m_screen_shader.uniformLocation(    "u_height" );


    // ***** Initialize our Simple Shader
    QOpenGLShader v_simple_shader( QOpenGLShader::Vertex );         v_simple_shader.compileSourceFile( ":/shaders/simple_vert.glsl" );
    QOpenGLShader f_simple_shader( QOpenGLShader::Fragment );       f_simple_shader.compileSourceFile( ":/shaders/simple_frag.glsl" );
    m_simple_shader.addShader( &v_simple_shader );
    m_simple_shader.addShader( &f_simple_shader );
    m_simple_shader.link();

    // Vertex Shader Input
    a_simple_vertex =           m_simple_shader.attributeLocation(  "vertex" );
    a_simple_texture_coord =    m_simple_shader.attributeLocation(  "texture_coordinates" );
    u_simple_matrix =           m_simple_shader.uniformLocation(    "u_matrix" );

    // Fragment Shader Input
    u_simple_texture =          m_simple_shader.uniformLocation(    "u_texture" );
    u_simple_alpha =            m_simple_shader.uniformLocation(    "u_alpha" );



    // ***** Initialize our Debug Shader
    QOpenGLShader v_debug_shader( QOpenGLShader::Vertex );         v_debug_shader.compileSourceFile( ":/shaders/debug_vert.glsl" );
    QOpenGLShader f_debug_shader( QOpenGLShader::Fragment );       f_debug_shader.compileSourceFile( ":/shaders/debug_frag.glsl" );
    m_debug_shader.addShader( &v_debug_shader );
    m_debug_shader.addShader( &f_debug_shader );
    m_debug_shader.link();

    // Vertex Shader Input
    a_debug_position =          m_debug_shader.attributeLocation(  "a_pos" );
    a_debug_texture_coord =     m_debug_shader.attributeLocation(  "a_uv" );
    a_debug_radius =            m_debug_shader.attributeLocation(  "a_radius" );
    a_debug_color_fill =        m_debug_shader.attributeLocation(  "a_color_fill" );
    a_debug_color_border =      m_debug_shader.attributeLocation(  "a_color_border" );

    u_debug_matrix =            m_debug_shader.uniformLocation(    "u_matrix" );


    // ***** Initialize our Kernel Shader
    QOpenGLShader v_kernel_shader( QOpenGLShader::Vertex );         v_kernel_shader.compileSourceFile( ":/shaders/simple_vert.glsl" );
    QOpenGLShader f_kernel_shader( QOpenGLShader::Fragment );       f_kernel_shader.compileSourceFile( ":/shaders/frag_kernel.glsl" );
    m_kernel_shader.addShader( &v_kernel_shader );
    m_kernel_shader.addShader( &f_kernel_shader );
    m_kernel_shader.link();

    // Vertex Shader Input
    a_kernel_vertex =           m_kernel_shader.attributeLocation(  "vertex" );
    a_kernel_texture_coord =    m_kernel_shader.attributeLocation(  "texture_coordinates" );
    u_kernel_matrix =           m_kernel_shader.uniformLocation(    "u_matrix" );

    // Fragment Shader Input
    u_kernel_texture =          m_kernel_shader.uniformLocation(    "u_texture" );
    u_kernel_width =            m_kernel_shader.uniformLocation(    "u_width" );
    u_kernel_height =           m_kernel_shader.uniformLocation(    "u_height" );
    u_kernel_alpha =            m_kernel_shader.uniformLocation(    "u_alpha" );


    // ***** Initialize our Water Shader
    QOpenGLShader v_water_shader( QOpenGLShader::Vertex );          v_water_shader.compileSourceFile( ":/shaders/simple_vert.glsl" );
    QOpenGLShader f_water_shader( QOpenGLShader::Fragment );        f_water_shader.compileSourceFile( ":/shaders/frag_water.glsl" );
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
    u_water_angle =             m_water_shader.uniformLocation(     "u_angle" );

    u_water_top =               m_water_shader.uniformLocation(     "u_water_top" );
    u_water_bottom =            m_water_shader.uniformLocation(     "u_water_bottom" );
    u_water_left =              m_water_shader.uniformLocation(     "u_water_left" );
    u_water_right =             m_water_shader.uniformLocation(     "u_water_right" );

    u_water_start_color =       m_water_shader.uniformLocation(     "u_start_color" );
    u_water_end_color =         m_water_shader.uniformLocation(     "u_end_color" );
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
    u_water_surface_flat =      m_water_shader.uniformLocation(     "u_surface_keep_flat" );
    u_refract_reflection =      m_water_shader.uniformLocation(     "u_refract_reflection" );
    u_refract_underwater =      m_water_shader.uniformLocation(     "u_refract_underwater" );
    u_refract_texture =         m_water_shader.uniformLocation(     "u_refract_texture" );
    u_refract_foam =            m_water_shader.uniformLocation(     "u_refract_foam" );
    u_water_movement_speed =    m_water_shader.uniformLocation(     "u_movement_speed" );

    u_water_pixel_x =           m_water_shader.uniformLocation(      "u_pixel_x" );
    u_water_pixel_y =           m_water_shader.uniformLocation(      "u_pixel_y" );
    u_water_bitrate =           m_water_shader.uniformLocation(      "u_bitrate" );


    // ***** Initialize our Fisheye Shader
    QOpenGLShader v_fisheye_shader( QOpenGLShader::Vertex );        v_fisheye_shader.compileSourceFile( ":/shaders/simple_vert.glsl" );
    QOpenGLShader f_fisheye_shader( QOpenGLShader::Fragment );      f_fisheye_shader.compileSourceFile( ":/shaders/frag_fisheye.glsl" );
    m_fisheye_shader.addShader( &v_fisheye_shader );
    m_fisheye_shader.addShader( &f_fisheye_shader );
    m_fisheye_shader.link();

    // Vertex Shader Input
    a_fisheye_vertex =          m_fisheye_shader.attributeLocation(   "vertex" );
    a_fisheye_texture_coord =   m_fisheye_shader.attributeLocation(   "texture_coordinates" );
    u_fisheye_matrix =          m_fisheye_shader.uniformLocation(     "u_matrix" );

    // Fragment Shader Input
    u_fisheye_alpha =           m_fisheye_shader.uniformLocation(     "u_alpha" );
    u_fisheye_zoom =            m_fisheye_shader.uniformLocation(     "u_zoom" );
    u_fisheye_pos =             m_fisheye_shader.uniformLocation(     "u_position" );
    u_fisheye_width =           m_fisheye_shader.uniformLocation(     "u_width" );
    u_fisheye_height =          m_fisheye_shader.uniformLocation(     "u_height" );
    u_fisheye_time =            m_fisheye_shader.uniformLocation(     "u_time" );
    u_fisheye_angle =           m_fisheye_shader.uniformLocation(     "u_angle" );

    u_fisheye_top =             m_fisheye_shader.uniformLocation(     "u_top" );
    u_fisheye_bottom =          m_fisheye_shader.uniformLocation(     "u_bottom" );
    u_fisheye_left =            m_fisheye_shader.uniformLocation(     "u_left" );
    u_fisheye_right =           m_fisheye_shader.uniformLocation(     "u_right" );

    u_fisheye_start_color =     m_fisheye_shader.uniformLocation(     "u_start_color" );
    u_fisheye_color_tint =      m_fisheye_shader.uniformLocation(     "u_color_tint" );
    u_fisheye_lens_zoom =       m_fisheye_shader.uniformLocation(     "u_lens_zoom" );

    u_fisheye_pixel_x =         m_fisheye_shader.uniformLocation(     "u_pixel_x" );
    u_fisheye_pixel_y =         m_fisheye_shader.uniformLocation(     "u_pixel_y" );
    u_fisheye_bitrate =         m_fisheye_shader.uniformLocation(     "u_bitrate" );

    // ***** Initialize our Fire Shader
    QOpenGLShader v_fire_shader( QOpenGLShader::Vertex );           v_fire_shader.compileSourceFile( ":/shaders/simple_vert.glsl" );
    QOpenGLShader f_fire_shader( QOpenGLShader::Fragment );         f_fire_shader.compileSourceFile( ":/shaders/frag_fire.glsl" );
    m_fire_shader.addShader( &v_fire_shader );
    m_fire_shader.addShader( &f_fire_shader );
    m_fire_shader.link();

    // Vertex Shader Input
    a_fire_vertex =             m_fire_shader.attributeLocation(    "vertex" );
    a_fire_texture_coord =      m_fire_shader.attributeLocation(    "texture_coordinates" );
    u_fire_matrix =             m_fire_shader.uniformLocation(      "u_matrix" );

    // Fragment Shader Input
    u_fire_alpha =              m_fire_shader.uniformLocation(      "u_alpha" );
    u_fire_time =               m_fire_shader.uniformLocation(      "u_time" );
    u_fire_position =           m_fire_shader.uniformLocation(      "u_pos" );
    u_fire_width =              m_fire_shader.uniformLocation(      "u_width" );
    u_fire_height =             m_fire_shader.uniformLocation(      "u_height" );

    u_fire_shape =              m_fire_shader.uniformLocation(      "u_shape" );
    u_fire_start_color =        m_fire_shader.uniformLocation(      "u_start_color" );
    u_fire_end_color =          m_fire_shader.uniformLocation(      "u_end_color" );
    u_fire_smoke_color =        m_fire_shader.uniformLocation(      "u_smoke_color" );
    u_fire_intensity =          m_fire_shader.uniformLocation(      "u_intensity" );
    u_fire_smoothness =         m_fire_shader.uniformLocation(      "u_smoothness" );
    u_fire_wavy =               m_fire_shader.uniformLocation(      "u_wavy" );
    u_fire_speed =              m_fire_shader.uniformLocation(      "u_speed" );

    u_fire_pixel_x =            m_fire_shader.uniformLocation(      "u_pixel_x" );
    u_fire_pixel_y =            m_fire_shader.uniformLocation(      "u_pixel_y" );
    u_fire_bitrate =            m_fire_shader.uniformLocation(      "u_bitrate" );


    // ***** Initialize our Mirror Shader
    QOpenGLShader v_mirror_shader( QOpenGLShader::Vertex );         v_mirror_shader.compileSourceFile( ":/shaders/simple_vert.glsl" );
    QOpenGLShader f_mirror_shader( QOpenGLShader::Fragment );       f_mirror_shader.compileSourceFile( ":/shaders/frag_mirror.glsl" );
    m_mirror_shader.addShader( &v_mirror_shader );
    m_mirror_shader.addShader( &f_mirror_shader );
    m_mirror_shader.link();

    // Vertex Shader Input
    a_mirror_vertex =           m_mirror_shader.attributeLocation(   "vertex" );
    a_mirror_texture_coord =    m_mirror_shader.attributeLocation(   "texture_coordinates" );
    u_mirror_matrix =           m_mirror_shader.uniformLocation(     "u_matrix" );

    // Fragment Shader Input
    u_mirror_alpha =            m_mirror_shader.uniformLocation(     "u_alpha" );
    u_mirror_zoom =             m_mirror_shader.uniformLocation(     "u_zoom" );
    u_mirror_pos =              m_mirror_shader.uniformLocation(     "u_position" );
    u_mirror_width =            m_mirror_shader.uniformLocation(     "u_width" );
    u_mirror_height =           m_mirror_shader.uniformLocation(     "u_height" );
    u_mirror_time =             m_mirror_shader.uniformLocation(     "u_time" );
    u_mirror_angle =            m_mirror_shader.uniformLocation(     "u_angle" );

    u_mirror_top =              m_mirror_shader.uniformLocation(     "u_top" );
    u_mirror_bottom =           m_mirror_shader.uniformLocation(     "u_bottom" );
    u_mirror_left =             m_mirror_shader.uniformLocation(     "u_left" );
    u_mirror_right =            m_mirror_shader.uniformLocation(     "u_right" );

    u_mirror_color_top =        m_mirror_shader.uniformLocation(     "u_color_top" );
    u_mirror_color_bottom =     m_mirror_shader.uniformLocation(     "u_color_bottom" );
    u_mirror_color_tint =       m_mirror_shader.uniformLocation(     "u_color_tint" );
    u_mirror_blur =             m_mirror_shader.uniformLocation(     "u_blur" );
    u_mirror_blur_stretch =     m_mirror_shader.uniformLocation(     "u_blur_stretch" );
    u_mirror_scale =            m_mirror_shader.uniformLocation(     "u_scale" );

    u_mirror_pixel_x =          m_mirror_shader.uniformLocation(     "u_pixel_x" );
    u_mirror_pixel_y =          m_mirror_shader.uniformLocation(     "u_pixel_y" );
    u_mirror_bitrate =          m_mirror_shader.uniformLocation(     "u_bitrate" );


    // ***** Initialize our Swirl Shader
    QOpenGLShader v_swirl_shader( QOpenGLShader::Vertex );          v_swirl_shader.compileSourceFile( ":/shaders/simple_vert.glsl" );
    QOpenGLShader f_swirl_shader( QOpenGLShader::Fragment );        f_swirl_shader.compileSourceFile( ":/shaders/frag_swirl.glsl" );
    m_swirl_shader.addShader( &v_swirl_shader );
    m_swirl_shader.addShader( &f_swirl_shader );
    m_swirl_shader.link();

    // Vertex Shader Input
    a_swirl_vertex =            m_swirl_shader.attributeLocation(   "vertex" );
    a_swirl_texture_coord =     m_swirl_shader.attributeLocation(   "texture_coordinates" );
    u_swirl_matrix =            m_swirl_shader.uniformLocation(     "u_matrix" );

    // Fragment Shader Input
    u_swirl_alpha =             m_swirl_shader.uniformLocation(     "u_alpha" );
    u_swirl_zoom =              m_swirl_shader.uniformLocation(     "u_zoom" );
    u_swirl_pos =               m_swirl_shader.uniformLocation(     "u_position" );
    u_swirl_width =             m_swirl_shader.uniformLocation(     "u_width" );
    u_swirl_height =            m_swirl_shader.uniformLocation(     "u_height" );
    u_swirl_time =              m_swirl_shader.uniformLocation(     "u_time" );
    u_swirl_angle =             m_swirl_shader.uniformLocation(     "u_angle" );

    u_swirl_top =               m_swirl_shader.uniformLocation(     "u_top" );
    u_swirl_bottom =            m_swirl_shader.uniformLocation(     "u_bottom" );
    u_swirl_left =              m_swirl_shader.uniformLocation(     "u_left" );
    u_swirl_right =             m_swirl_shader.uniformLocation(     "u_right" );

    u_swirl_start_color =       m_swirl_shader.uniformLocation(     "u_start_color" );
    u_swirl_color_tint =        m_swirl_shader.uniformLocation(     "u_color_tint" );
    u_swirl_rotation =          m_swirl_shader.uniformLocation(     "u_rotation" );
    u_swirl_radius =            m_swirl_shader.uniformLocation(     "u_radius" );

    u_swirl_pixel_x =           m_swirl_shader.uniformLocation(     "u_pixel_x" );
    u_swirl_pixel_y =           m_swirl_shader.uniformLocation(     "u_pixel_y" );
    u_swirl_bitrate =           m_swirl_shader.uniformLocation(     "u_bitrate" );
}














