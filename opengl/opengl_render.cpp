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
#include "engine/engine_object.h"
#include "engine/engine_texture.h"
#include "engine/engine_world.h"
#include "engine/form_engine.h"
#include "helper.h"
#include "opengl/opengl.h"


//####################################################################################
//##        Render, Paint the Scene (called by update())
//####################################################################################
void OpenGL::paintGL() {
    // ***** Find OpenGL Version supported on this system
    ///auto ver = glGetString(GL_VERSION);
    ///m_engine->info = QString::fromUtf8(reinterpret_cast<const char*>(ver));

    // ***** Make sure objects vector is sorted by depth
    EngineObjects objects = m_engine->getCurrentWorld()->objects;
    std::sort(objects.begin(), objects.end(), [] (const DrEngineObject *a, const DrEngineObject *b) { return a->z_order < b->z_order; });

    // ***** Update Camera / View Matrix
    updateViewMatrix();

    // ***** Render Onto Frame Buffer Object
    bindOffscreenBuffer();                                      // Create / Bind Offscreen Frame Buffer Object
    ///drawCube( QVector3D( 2000, 400, -300) );                    // Render Background 3D Objects
    drawSpace();                                                // Render cpSpace Objects
    ///drawCube( QVector3D(1600, 500, 600) );                      // Render Foreground 3D Objects

    // ***** Draws Debug Shapes / Text Onto Frame Buffer Object
    QOpenGLPaintDevice paint_gl(width() * devicePixelRatio(), height() * devicePixelRatio());
    QPainter painter (&paint_gl);
    drawDebug(painter);
    painter.end();

    // ***** Relase Frame Buffer Object and copy it onto a GL_TEXTURE_2D (non multi-sampled) Frame Buffer Object
    m_fbo->release();
    QOpenGLFramebufferObject::blitFramebuffer(m_texture_fbo, m_fbo);


    // ***** Render 2D Lights
    bindShadowBuffer();
    renderShadowMap();

    static long count = 0;
    count++;
    if (count == 700) {
        Dr::ShowMessageBox("Shadow", QPixmap::fromImage(m_shadow_fbo->toImage()));
    }

    // ***** Renders Frame Buffer Object to screen buffer as a textured quad, with post processing available
    m_fbo->bindDefault();
    drawFrameBufferToScreenBuffer();

    // ***** Update FPS
    ++m_form_engine->fps_count_render;
}


//####################################################################################
//##        Allocate additional FBO for rendering or resize it if widget size changed
//####################################################################################
void OpenGL::bindOffscreenBuffer() {
    bool create_fbo = false;
    // Check if exists first
    if (!m_fbo || !m_texture_fbo)
        create_fbo = true;
    // Or if it does exist, check the width and height
    else if (m_fbo->width() != width() || m_fbo->height() != height())
        create_fbo = true;

    if (create_fbo) {
        delete m_fbo;
        delete m_texture_fbo;
        QOpenGLFramebufferObjectFormat format;
        format.setAttachment(QOpenGLFramebufferObject::Attachment::CombinedDepthStencil);
        format.setSamples(4);
        ///format.setTextureTarget(GL_TEXTURE_2D);                      // This is set automatically, cannot be gl_texture_2d if multisampling is enabled
        ///format.setInternalTextureFormat(GL_RGBA32F_ARB);             // This is set automatically depending on the system
        ///format.setMipmap(true);                                      // Don't need
        m_fbo = new QOpenGLFramebufferObject(width() * devicePixelRatio(), height() * devicePixelRatio(), format);
        m_texture_fbo = new QOpenGLFramebufferObject(width() * devicePixelRatio(), height() * devicePixelRatio());
    }
    m_fbo->bind();

    // Clear the buffers
    float background_red =   static_cast<float>(m_engine->getCurrentWorld()->getBackgroundColor().redF());
    float background_green = static_cast<float>(m_engine->getCurrentWorld()->getBackgroundColor().greenF());
    float background_blue =  static_cast<float>(m_engine->getCurrentWorld()->getBackgroundColor().blueF());
    glClearColor(background_red, background_green, background_blue, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // Enable alpha channel
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);                  // Standard blend function

    // Enable anti aliasing if not on mobile
#if not defined(Q_OS_ANDROID) && not defined(Q_OS_IOS)
    glEnable( GL_MULTISAMPLE );
#endif

    // Enable depth / stencil test
    ///glEnable( GL_DEPTH_TEST  );
    ///glEnable( GL_STENCIL_TEST );

    // Alpha clamping
    ///glEnable(GL_ALPHA_TEST);
    ///glAlphaFunc(GL_GREATER,0);                                       // 0.0 (transparent) to 1.0 (opaque)
}


//####################################################################################
//##        Update the view matrices before rendering
//####################################################################################
void OpenGL::updateViewMatrix() {
    //          Axis:
    //              -X left,        +X right
    //              -Y down,        +Y up
    //              -Z back,        +Z front (close to camera)
    float aspect_ratio = static_cast<float>(width()) / static_cast<float>(height());

    // Set camera position
    QVector3D  perspective_offset ( 200.0f, 200.0f, 0.0f);
    QVector3D  eye(     m_engine->getCurrentWorld()->getCameraPos().x() * m_scale + perspective_offset.x(),
                        m_engine->getCurrentWorld()->getCameraPos().y() * m_scale + perspective_offset.y(),
                        m_engine->getCurrentWorld()->getCameraPos().z() );
    QVector3D  look_at( m_engine->getCurrentWorld()->getCameraPos().x() * m_scale,
                        m_engine->getCurrentWorld()->getCameraPos().y() * m_scale, 0.0f );
    QVector3D  up(      0.0f, 1.0f, 0.0f);

    // Create Matrices
    m_model_view.setToIdentity();
    m_projection.setToIdentity();
    if (m_engine->getCurrentWorld()->render_type == Render_Type::Orthographic) {
        float cam_x =  m_engine->getCurrentWorld()->getCameraPos().x() * m_scale;
        float cam_y =  m_engine->getCurrentWorld()->getCameraPos().y() * m_scale;
        float left =   cam_x - (width() *  devicePixelRatio() / 2.0f);
        float right =  cam_x + (width() *  devicePixelRatio() / 2.0f);
        float top =    cam_y + (height() * devicePixelRatio() / 2.0f);
        float bottom = cam_y - (height() * devicePixelRatio() / 2.0f);
        m_projection.ortho( left, right, bottom, top,  -1000.0f, 1000.0f);
    } else {
        m_projection.perspective( 70.0f, aspect_ratio, 1.0f, 5000.0f );
        m_model_view.lookAt(eye, look_at, up);
        m_model_view.scale( m_scale );

        // Rotates the camera around the center of the sceen
        ///m_angle += 1.0f;
        ///if (m_angle > 360) m_angle = 0;
        ///m_model_view.rotate( m_angle, 0.0f, 1.0f, 0.0f );
    }
}


//####################################################################################
//##        Applies coordinates that represents an entire texture
//####################################################################################
void OpenGL::setWholeTextureCoordinates(std::vector<float> &texture_coords) {
    texture_coords.clear();
    texture_coords.resize( 8 );
    texture_coords[0] = 1;    texture_coords[1] = 1;
    texture_coords[2] = 0;    texture_coords[3] = 1;
    texture_coords[4] = 1;    texture_coords[5] = 0;
    texture_coords[6] = 0;    texture_coords[7] = 0;
}


//####################################################################################
//##        Renders Frame Buffer Object to screen buffer as a textured quad
//##            Post processing available through the fragment shader
//####################################################################################
void OpenGL::drawFrameBufferToScreenBuffer() {

    // Clear the screen buffer
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_texture_fbo->texture());

    if (!m_shader.bind()) return;

    // Set Matrix for Shader, apply Orthographic Matrix to fill the viewport
    float left =   0.0f - (m_texture_fbo->width()  / 2.0f);
    float right =  0.0f + (m_texture_fbo->width()  / 2.0f);
    float top =    0.0f + (m_texture_fbo->height() / 2.0f);
    float bottom = 0.0f - (m_texture_fbo->height() / 2.0f);
    QMatrix4x4 m_matrix;
    m_matrix.ortho( left, right, bottom, top,  -1000.0f, 1000.0f);
    m_shader.setUniformValue( m_uniform_matrix, m_matrix );

    // Set Texture Coordinates for Shader
    std::vector<float> texture_coordinates;
    setWholeTextureCoordinates(texture_coordinates);
    m_shader.setAttributeArray( m_attribute_tex_coord, texture_coordinates.data(), 2 );
    m_shader.enableAttributeArray( m_attribute_tex_coord );

    // Load vertices for this object
    QVector3D top_right = QVector3D( right, top, 0);
    QVector3D top_left =  QVector3D( left,  top, 0);
    QVector3D bot_right = QVector3D( right, bottom, 0);
    QVector3D bot_left =  QVector3D( left,  bottom, 0);

    QVector<GLfloat> vertices;
    vertices.clear();
    vertices.resize( 12 );              // in sets of x, y, z
    vertices[ 0] = top_right.x();       vertices[ 1] = top_right.y();       vertices[ 2] = 0;           // Top Right
    vertices[ 3] = top_left.x();        vertices[ 4] = top_left.y();        vertices[ 5] = 0;           // Top Left
    vertices[ 6] = bot_right.x();       vertices[ 7] = bot_right.y();       vertices[ 8] = 0;           // Bottom Right
    vertices[ 9] = bot_left.x();        vertices[10] = bot_left.y();        vertices[11] = 0;           // Bottom Left
    m_shader.setAttributeArray( m_attribute_vertex, vertices.data(), 3 );
    m_shader.enableAttributeArray( m_attribute_vertex );

    // Set variables for shader
    setShaderDefaultValues( m_texture_fbo->width(), m_texture_fbo->height() );

    m_shader.setUniformValue( m_uniform_bitrate,    m_engine->getCurrentWorld()->bitrate );
    m_shader.setUniformValue( m_uniform_pixel_x,    m_engine->getCurrentWorld()->pixel_x );
    m_shader.setUniformValue( m_uniform_pixel_y,    m_engine->getCurrentWorld()->pixel_y );
    m_shader.setUniformValue( m_uniform_negative,   m_engine->getCurrentWorld()->negative );
    m_shader.setUniformValue( m_uniform_grayscale,  m_engine->getCurrentWorld()->grayscale );
    m_shader.setUniformValue( m_uniform_hue,        m_engine->getCurrentWorld()->hue );
    m_shader.setUniformValue( m_uniform_saturation, m_engine->getCurrentWorld()->saturation );
    m_shader.setUniformValue( m_uniform_contrast,   m_engine->getCurrentWorld()->contrast );
    m_shader.setUniformValue( m_uniform_brightness, m_engine->getCurrentWorld()->brightness );

    // Draw triangles using shader program
    glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

    // Disable arrays
    m_shader.disableAttributeArray( m_attribute_vertex );
    m_shader.disableAttributeArray( m_attribute_tex_coord );

    // Release Shader
    m_shader.release();
}


//####################################################################################
//##        Sets shader variables to default normal render
//####################################################################################
void OpenGL::setShaderDefaultValues(float texture_width, float texture_height) {
    m_shader.setUniformValue( m_uniform_texture,    0 );                    // Use texture unit "0"
    m_shader.setUniformValue( m_uniform_alpha,      1.0f );
    m_shader.setUniformValue( m_uniform_width,      texture_width  );
    m_shader.setUniformValue( m_uniform_height,     texture_height );
    m_shader.setUniformValue( m_uniform_bitrate,    16.0f );
    m_shader.setUniformValue( m_uniform_pixel_x,    1.0f );
    m_shader.setUniformValue( m_uniform_pixel_y,    1.0f );
    m_shader.setUniformValue( m_uniform_negative,   false );
    m_shader.setUniformValue( m_uniform_grayscale,  false );
    m_shader.setUniformValue( m_uniform_hue,        0.0f );
    m_shader.setUniformValue( m_uniform_saturation, 0.0f );
    m_shader.setUniformValue( m_uniform_contrast,   0.0f );
    m_shader.setUniformValue( m_uniform_brightness, 0.0f );
    m_shader.setUniformValue( m_uniform_tint,       0.0f, 0.0f, 0.0f );     // Add 0 to red, green, and blue
    m_shader.setUniformValue( m_uniform_kernel,     false );                // Currently overrides other effects if set to true
}











