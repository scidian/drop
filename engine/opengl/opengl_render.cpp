//
//      Created by Stephens Nunnally on 5/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QOpenGLFramebufferObject>

#include "core/dr_math.h"
#include "engine/engine.h"
#include "engine/engine_camera.h"
#include "engine/engine_texture.h"
#include "engine/form_engine.h"
#include "engine/opengl/opengl.h"
#include "engine/thing/engine_thing_object.h"
#include "engine/world/engine_world.h"


//####################################################################################
//##    Render, Paint the Scene (called by update())
//####################################################################################
void DrOpenGL::paintGL() {    
    // ***** Find OpenGL Version supported on this system
    ///auto ver = glGetString(GL_VERSION);
    ///g_info = std::string(reinterpret_cast<const char*>(ver));

    // ***** Make sure Things vector is sorted by depth
    EngineThings &things = m_engine->getCurrentWorld()->getThings();
    std::sort(things.begin(), things.end(), [] (DrEngineThing *a, DrEngineThing *b) {
        return a->getZOrder() < b->getZOrder();
    });

    // ***** Update Camera / View Matrix
    m_desired_fbo_width =  width()  * devicePixelRatio();
    m_desired_fbo_height = height() * devicePixelRatio();
    // For Pixelation using shrunken fbo's:
    ///m_desired_fbo_width =  static_cast<int>(static_cast<float>(width()  * devicePixelRatio()) / m_engine->getCurrentWorld()->pixel_x);
    ///m_desired_fbo_height = static_cast<int>(static_cast<float>(height() * devicePixelRatio()) / m_engine->getCurrentWorld()->pixel_y);
    m_fbo_scale_x =        static_cast<float>(m_desired_fbo_width)  / static_cast<float>(width()  * devicePixelRatio());
    m_fbo_scale_y =        static_cast<float>(m_desired_fbo_height) / static_cast<float>(height() * devicePixelRatio());
    updateViewMatrix(m_engine->getCurrentWorld()->render_type, m_desired_fbo_width, m_desired_fbo_height);

    // ***** If there are Lights, Render Occluder Map, Calculate Shadow Maps, Draw Glow Lights
    process2DLights();

    // ***** Render Onto Frame Buffer Object
    bindOffscreenBuffer();                                                          // Create / Bind Offscreen Frame Buffer Object
    drawSpace();                                                                    // Render cpSpace Objects
    releaseOffscreenBuffer();                                                       // Release Frame Buffer Object

    // ***** Copy fbo to a GL_TEXTURE_2D (non multi-sampled) Frame Buffer Object
    // #NO_QT: As original QOpenGlWidget calls
    ///QOpenGLFramebufferObject::blitFramebuffer(m_texture_fbo, m_render_fbo);
    //         As standard OpenGL Calls
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_render_fbo->handle());
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_texture_fbo->handle());
    glBlitFramebuffer(0, 0, m_render_fbo->width(), m_render_fbo->height(), 0, 0, m_render_fbo->width(), m_render_fbo->height(), GL_COLOR_BUFFER_BIT, GL_LINEAR);

    // ***** Bind default Qt FrameBuffer, Clear and Render FBO to screen buffer as a textured quad, with post processing available
    QOpenGLFramebufferObject::bindDefault();
    glViewport(0, 0, width() * devicePixelRatio(), height() * devicePixelRatio());
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glDisable(GL_BLEND);
    drawFrameBufferUsingDefaultShader(m_texture_fbo);
    drawDebug();

    // ***** Update FPS
    ++m_fps_count_render;
}


//####################################################################################
//##    Returns list of vertices at z plane from size passed in
//####################################################################################
void DrOpenGL::setQuadVertices(std::vector<float> &vertices, float width, float height, DrPointF center, float z) {
    float left =   static_cast<float>(center.x) - (width  / 2.0f);
    float right =  static_cast<float>(center.x) + (width  / 2.0f);
    float top =    static_cast<float>(center.y) + (height / 2.0f);
    float bottom = static_cast<float>(center.y) - (height / 2.0f);

    vertices.clear();
    vertices.resize( 12 );              // in sets of x, y, z
    vertices[ 0] = right;       vertices[ 1] = top;         vertices[ 2] = z;           // Top Right
    vertices[ 3] = left;        vertices[ 4] = top;         vertices[ 5] = z;           // Top Left
    vertices[ 6] = right;       vertices[ 7] = bottom;      vertices[ 8] = z;           // Bottom Right
    vertices[ 9] = left;        vertices[10] = bottom;      vertices[11] = z;           // Bottom Left
}

//####################################################################################
//##    Returns list of vertices at z plane from 4 corners
//####################################################################################
void DrOpenGL::setQuadRotatedVertices(std::vector<float> &vertices,
                                      DrVec3 &top_right, DrVec3 &top_left,
                                      DrVec3 &bot_left,  DrVec3 &bot_right,
                                      DrVec3 position) {
    vertices.clear();
    vertices.resize( 12 );              // in sets of x, y, z
    vertices[ 0] = top_right.x + position.x;    vertices[ 1] = top_right.y + position.y;    vertices[ 2] = position.z;      // Top Right
    vertices[ 3] = top_left.x  + position.x;    vertices[ 4] = top_left.y  + position.y;    vertices[ 5] = position.z;      // Top Left
    vertices[ 6] = bot_right.x + position.x;    vertices[ 7] = bot_right.y + position.y;    vertices[ 8] = position.z;      // Bottom Right
    vertices[ 9] = bot_left.x  + position.x;    vertices[10] = bot_left.y  + position.y;    vertices[11] = position.z;      // Bottom Left
}


//####################################################################################
//##    Calculates rotated, z-ordered vertices for a DrEngineThing
//####################################################################################
void DrOpenGL::getThingVertices(std::vector<GLfloat> &vertices, DrEngineThing *thing, double width, double height, float extra_scale_x, float extra_scale_y) {
    // ***** Get object position data
    DrPointF center = thing->getPosition();
    float x, y, z;
    float half_width, half_height;
    x = static_cast<float>(center.x);
    y = static_cast<float>(center.y);
    z = static_cast<float>(thing->getZOrder());
    half_width =  static_cast<float>(width) *  thing->getScaleX() * extra_scale_x / 2.0f;
    half_height = static_cast<float>(height) * thing->getScaleY() * extra_scale_y / 2.0f;

    // ***** Create rotation matrix, apply rotation to object
    double now = Dr::MillisecondsSinceStartOfDay() / 10.0;
    glm::mat4 matrix = Dr::IdentityMatrix();
    float rotate_x {0}, rotate_y {0}, rotate_z {0};
    if (thing->comp3D() != nullptr) {
        double comp_3d_angle_x =        thing->comp3D()->getAngleX();
        double comp_3d_angle_y =        thing->comp3D()->getAngleY();
        double comp_3d_rotate_x_speed = thing->comp3D()->getRotateSpeedX();
        double comp_3d_rotate_y_speed = thing->comp3D()->getRotateSpeedY();
        rotate_x = Dr::DegreesToRadians(static_cast<float>(comp_3d_angle_x + (now * comp_3d_rotate_x_speed)));
        rotate_y = Dr::DegreesToRadians(static_cast<float>(comp_3d_angle_y + (now * comp_3d_rotate_y_speed)));
    }
    rotate_z = Dr::DegreesToRadians(static_cast<float>(thing->getAngle()));
    matrix = glm::rotate(matrix, rotate_x, glm::vec3(1.0, 0.0, 0.0));
    matrix = glm::rotate(matrix, rotate_y, glm::vec3(0.0, 1.0, 0.0));
    matrix = glm::rotate(matrix, rotate_z, glm::vec3(0.0, 0.0, 1.0));

    DrVec3 top_right = matrix * DrVec3( half_width,  half_height, 0.f);
    DrVec3 top_left =  matrix * DrVec3(-half_width,  half_height, 0.f);
    DrVec3 bot_right = matrix * DrVec3( half_width, -half_height, 0.f);
    DrVec3 bot_left =  matrix * DrVec3(-half_width, -half_height, 0.f);

    // ***** Load vertices for this object
    setQuadRotatedVertices(vertices, top_right, top_left, bot_left, bot_right, DrVec3(x, y, z));
}


//####################################################################################
//##        Sets shader variables to default normal render
//####################################################################################
void DrOpenGL::setShaderDefaultValues(float texture_width, float texture_height) {
    m_default_shader.setUniformValue( u_default_texture,            0 );
    m_default_shader.setUniformValue( u_default_texture_pixel,      1 );
    m_default_shader.setUniformValue( u_default_alpha,              1.0f );
    m_default_shader.setUniformValue( u_default_tint,               0.0f, 0.0f, 0.0f );                 // Add 0 to red, green, and blue
    m_default_shader.setUniformValue( u_default_average_color,      0.0f, 0.0f, 0.0f );
    m_default_shader.setUniformValue( u_default_zoom,               combinedZoomScale() );
    m_default_shader.setUniformValue( u_default_width,              texture_width  );
    m_default_shader.setUniformValue( u_default_height,             texture_height );
    m_default_shader.setUniformValue( u_default_time,               static_cast<float>(Dr::MillisecondsSinceStartOfDay() / 1000.0) );
    m_default_shader.setUniformValue( u_default_pre,                false );
    m_default_shader.setUniformValue( u_default_bitrate,            256.0f );
    m_default_shader.setUniformValue( u_default_pixel_x,            1.0f );
    m_default_shader.setUniformValue( u_default_pixel_y,            1.0f );
    m_default_shader.setUniformValue( u_default_pixel_offset,       0.5f, 0.5f );
    m_default_shader.setUniformValue( u_default_pixel_type,         0.0f );
    m_default_shader.setUniformValue( u_default_negative,           false );
    m_default_shader.setUniformValue( u_default_grayscale,          false );
    m_default_shader.setUniformValue( u_default_hue,                0.0f );
    m_default_shader.setUniformValue( u_default_saturation,         0.0f );
    m_default_shader.setUniformValue( u_default_contrast,           0.0f );
    m_default_shader.setUniformValue( u_default_brightness,         0.0f );
    m_default_shader.setUniformValue( u_default_camera_type,        static_cast<float>(m_engine->getCurrentWorld()->render_type) );
    m_default_shader.setUniformValue( u_default_shade_away,         false );
    m_default_shader.setUniformValue( u_default_cartoon,            false );
    m_default_shader.setUniformValue( u_default_cartoon_width,      5.0f );
    m_default_shader.setUniformValue( u_default_cross_hatch,        false );
    m_default_shader.setUniformValue( u_default_cross_hatch_width,  5.0f );
    m_default_shader.setUniformValue( u_default_wavy,               false );
    m_default_shader.setUniformValue( u_default_wireframe,          false );
    m_default_shader.setUniformValue( u_default_wireframe_width,    1.0f );
}









