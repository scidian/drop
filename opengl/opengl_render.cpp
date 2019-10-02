//
//      Created by Stephens Nunnally on 5/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QtMath>
#include <QOpenGLFramebufferObject>
#include <QOpenGLPaintDevice>
#include <QPainter>
#include <vector>

#include "engine/engine.h"
#include "engine/engine_camera.h"
#include "engine/engine_texture.h"
#include "engine/engine_world.h"
#include "engine_things/engine_thing_light.h"
#include "engine_things/engine_thing_object.h"
#include "engine_things/engine_thing_water.h"
#include "engine/form_engine.h"
#include "helper.h"
#include "opengl/opengl.h"


//####################################################################################
//##    Render, Paint the Scene (called by update())
//####################################################################################
void DrOpenGL::paintGL() {    
    // ***** Find OpenGL Version supported on this system
    ///auto ver = glGetString(GL_VERSION);
    ///m_engine->info = QString::fromUtf8(reinterpret_cast<const char*>(ver));

    // ***** Update Camera / View Matrix
    updateViewMatrix(m_engine->getCurrentWorld()->render_type, c_use_cam_offset);

    // ***** Make sure Things vector is sorted by depth
    EngineThings &things = m_engine->getCurrentWorld()->getThings();
    std::sort(things.begin(), things.end(), [] (const DrEngineThing *a, const DrEngineThing *b) { return a->z_order < b->z_order; });

    // ***** If there are Lights, Render Occluder Map, Calculate Shadow Maps, Draw Glow Lights
    process2DLights();

    // ***** Render Onto Frame Buffer Object
    bindOffscreenBuffer();                                                          // Create / Bind Offscreen Frame Buffer Object
    drawSpace();                                                                    // Render cpSpace Objects
    releaseOffscreenBuffer();                                                       // Release Frame Buffer Object
    QOpenGLFramebufferObject::blitFramebuffer(m_texture_fbo, m_render_fbo);         // Copy fbo to a GL_TEXTURE_2D (non multi-sampled) Frame Buffer Object

    // ***** Bind default Qt FrameBuffer, Clear and Render FBO to screen buffer as a textured quad, with post processing available
    QOpenGLFramebufferObject::bindDefault();
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glDisable(GL_BLEND);
    drawFrameBufferUsingDefaultShader(m_texture_fbo);

    // ***** Draws Debug Shapes / Text Onto Frame Buffer Object
    QOpenGLPaintDevice paint_gl(width() * devicePixelRatio(), height() * devicePixelRatio());
    QPainter painter (&paint_gl);
    drawDebug(painter);
    painter.end();

    // ***** Update FPS
    ++m_form_engine->fps_count_render;
}


//####################################################################################
//##    Returns list of vertices at z plane from size passed in
//####################################################################################
void DrOpenGL::setQuadVertices(QVector<GLfloat> &vertices, float width, float height, QPointF center, float z) {
    float left =   static_cast<float>(center.x()) - (width  / 2.0f);
    float right =  static_cast<float>(center.x()) + (width  / 2.0f);
    float top =    static_cast<float>(center.y()) + (height / 2.0f);
    float bottom = static_cast<float>(center.y()) - (height / 2.0f);
    QVector3D top_right = QVector3D( right, top, 0);
    QVector3D top_left =  QVector3D( left,  top, 0);
    QVector3D bot_right = QVector3D( right, bottom, 0);
    QVector3D bot_left =  QVector3D( left,  bottom, 0);

    vertices.clear();
    vertices.resize( 12 );              // in sets of x, y, z
    vertices[ 0] = top_right.x();       vertices[ 1] = top_right.y();       vertices[ 2] = z;           // Top Right
    vertices[ 3] = top_left.x();        vertices[ 4] = top_left.y();        vertices[ 5] = z;           // Top Left
    vertices[ 6] = bot_right.x();       vertices[ 7] = bot_right.y();       vertices[ 8] = z;           // Bottom Right
    vertices[ 9] = bot_left.x();        vertices[10] = bot_left.y();        vertices[11] = z;           // Bottom Left
}

//####################################################################################
//##    Returns list of vertices at z plane from 4 corners
//####################################################################################
void DrOpenGL::setQuadRotatedVertices(QVector<GLfloat> &vertices,
                                      QVector3D &top_right, QVector3D &top_left,
                                      QVector3D &bot_left,  QVector3D &bot_right,
                                      QVector3D position) {
    vertices.clear();
    vertices.resize( 12 );              // in sets of x, y, z
    vertices[ 0] = top_right.x() + position.x();    vertices[ 1] = top_right.y() + position.y();    vertices[ 2] = position.z();    // Top Right
    vertices[ 3] = top_left.x()  + position.x();    vertices[ 4] = top_left.y()  + position.y();    vertices[ 5] = position.z();    // Top Left
    vertices[ 6] = bot_right.x() + position.x();    vertices[ 7] = bot_right.y() + position.y();    vertices[ 8] = position.z();    // Bottom Right
    vertices[ 9] = bot_left.x()  + position.x();    vertices[10] = bot_left.y()  + position.y();    vertices[11] = position.z();    // Bottom Left
}


//####################################################################################
//##    Calculates rotated, z-ordered vertices for a DrEngineThing
//####################################################################################
void DrOpenGL::getThingVertices(QVector<GLfloat> &vertices, DrEngineThing *thing) {
    // ***** Get object position data
    DrPointF center = thing->getPosition();
    float   x, y, z;
    float   half_width, half_height;
    x = static_cast<float>(center.x);
    y = static_cast<float>(center.y);
    z = static_cast<float>(thing->z_order);
    half_width =  static_cast<float>(thing->getSize().x) * thing->getScaleX() / 2.0f;
    half_height = static_cast<float>(thing->getSize().y) * thing->getScaleY() / 2.0f;

    // ***** Create rotation matrix, apply rotation to object
    float now = static_cast<float>(Dr::MillisecondsSinceStartOfDay() / 10.0);
    QMatrix4x4 matrix;
    if (qFuzzyCompare(thing->getAngleX(), 0.0) == false) matrix.rotate(now * static_cast<float>(thing->getAngleX()), 1.f, 0.f, 0.f);
    if (qFuzzyCompare(thing->getAngleY(), 0.0) == false) matrix.rotate(now * static_cast<float>(thing->getAngleY()), 0.f, 1.f, 0.f);
    matrix.rotate( static_cast<float>(thing->getAngle()), 0.0, 0.0, 1.0 );
    QVector3D top_right = matrix * QVector3D( half_width,  half_height, 0);
    QVector3D top_left =  matrix * QVector3D(-half_width,  half_height, 0);
    QVector3D bot_right = matrix * QVector3D( half_width, -half_height, 0);
    QVector3D bot_left =  matrix * QVector3D(-half_width, -half_height, 0);

    // ***** Load vertices for this object
    setQuadRotatedVertices(vertices, top_right, top_left, bot_left, bot_right, QVector3D(x, y, z));
}


//####################################################################################
//##        Sets shader variables to default normal render
//####################################################################################
void DrOpenGL::setShaderDefaultValues(float texture_width, float texture_height) {
    m_default_shader.setUniformValue( u_default_texture,        0 );                            // Use texture unit "0"
    m_default_shader.setUniformValue( u_default_alpha,          1.0f );
    m_default_shader.setUniformValue( u_default_tint,           0.0f, 0.0f, 0.0f );             // Add 0 to red, green, and blue
    m_default_shader.setUniformValue( u_default_average_color,  0.0f, 0.0f, 0.0f );
    m_default_shader.setUniformValue( u_default_zoom,           m_scale );
    m_default_shader.setUniformValue( u_default_width,          texture_width  );
    m_default_shader.setUniformValue( u_default_height,         texture_height );
    m_default_shader.setUniformValue( u_default_time,           static_cast<float>(Dr::MillisecondsSinceStartOfDay() / 1000.0) );
    m_default_shader.setUniformValue( u_default_pre,            false );
    m_default_shader.setUniformValue( u_default_bitrate,        256.0f );
    m_default_shader.setUniformValue( u_default_pixel_x,        1.0f );
    m_default_shader.setUniformValue( u_default_pixel_y,        1.0f );
    m_default_shader.setUniformValue( u_default_pixel_offset,   0.5f, 0.5f );
    m_default_shader.setUniformValue( u_default_negative,       false );
    m_default_shader.setUniformValue( u_default_grayscale,      false );
    m_default_shader.setUniformValue( u_default_hue,            0.0f );
    m_default_shader.setUniformValue( u_default_saturation,     0.0f );
    m_default_shader.setUniformValue( u_default_contrast,       0.0f );
    m_default_shader.setUniformValue( u_default_brightness,     0.0f );
    m_default_shader.setUniformValue( u_default_shade_away,     false );
    m_default_shader.setUniformValue( u_default_cartoon,        false );
    m_default_shader.setUniformValue( u_default_wavy,           false );
    m_default_shader.setUniformValue( u_default_wireframe,      false );
}




