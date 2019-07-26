//
//      Created by Stephens Nunnally on 7/25/2019, (c) 2019 Scidian Software, All Rights Reserved
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
#include "engine/engine_thing_light.h"
#include "engine/engine_thing_object.h"
#include "engine/engine_thing_water.h"
#include "engine/engine_texture.h"
#include "engine/engine_world.h"
#include "engine/form_engine.h"
#include "helper.h"
#include "opengl/opengl.h"


//####################################################################################
//##        Renders Frame Buffer Object to screen buffer as a textured quad
//##            Uses Water Shader to draw reflective / refractive / textured water
//####################################################################################
void DrOpenGL::drawFrameBufferUsingWaterShader(QOpenGLFramebufferObject *fbo, DrEngineWater *water) {

    // Calculate sides of Water in shader coordinates
    QPointF position_top =    mapToScreen(water->getPosition().x(), water->getPosition().y() + (water->water_size.y() / 2.0), water->z_order);
    QPointF position_bottom = mapToScreen(water->getPosition().x(), water->getPosition().y() - (water->water_size.y() / 2.0), water->z_order);
    QPointF position_left =   mapToScreen(water->getPosition().x() - (water->water_size.x() / 2.0), water->getPosition().y(), water->z_order);
    QPointF position_right =  mapToScreen(water->getPosition().x() + (water->water_size.x() / 2.0), water->getPosition().y(), water->z_order);
    float water_top =    static_cast<float>((fbo->height() - position_top.y()) / fbo->height());
    float water_bottom = static_cast<float>((fbo->height() - position_bottom.y()) / fbo->height());
    float water_left =   static_cast<float>(position_left.x()  / fbo->width());
    float water_right =  static_cast<float>(position_right.x() / fbo->width());

    // Check water is in view
    if (water_top < 0.0f || water_bottom > 1.0f || water_right < 0.0f || water_left > 1.0f) return;

    // If we are to render water, bind the shader
    if (!m_water_shader.bind()) return;
    if (!fbo) return;

    // Bind offscreen frame buffer objects as textures
    glEnable(GL_TEXTURE_2D);
    GLint texture =       glGetUniformLocation(m_water_shader.programId(), "u_texture");
    GLint water_normal =  glGetUniformLocation(m_water_shader.programId(), "u_texture_displacement");
    GLint water_texture = glGetUniformLocation(m_water_shader.programId(), "u_texture_water");
    glUseProgram(m_water_shader.programId());
    glUniform1i(texture,       0);
    glUniform1i(water_normal,  1);
    glUniform1i(water_texture, 2);

    // Bind textures - !!!!! #NOTE: Must be called in descending order and end on 0
    glActiveTexture(GL_TEXTURE2);                           // Texture unit 2
    glBindTexture(GL_TEXTURE_2D, m_engine->getTexture(Asset_Textures::Water_Texture_1)->texture()->textureId());
    m_engine->getTexture(Asset_Textures::Water_Texture_1)->texture()->setWrapMode(QOpenGLTexture::WrapMode::MirroredRepeat);

    glActiveTexture(GL_TEXTURE1);                           // Texture unit 1
    glBindTexture(GL_TEXTURE_2D, m_engine->getTexture(Asset_Textures::Water_Normal_1)->texture()->textureId());
    m_engine->getTexture(Asset_Textures::Water_Normal_1)->texture()->setWrapMode(QOpenGLTexture::WrapMode::MirroredRepeat);

    glActiveTexture(GL_TEXTURE0);                           // Texture unit 0
    glBindTexture(GL_TEXTURE_2D, fbo->texture());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);      // Better for water ripples
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);      // Better for reflection


    // Set Matrix for Shader, apply Orthographic Matrix to fill the viewport
    float left =   0.0f - (fbo->width()  / 2.0f);
    float right =  0.0f + (fbo->width()  / 2.0f);
    float top =    0.0f + (fbo->height() / 2.0f);
    float bottom = 0.0f - (fbo->height() / 2.0f);
    QMatrix4x4 m_matrix;
    m_matrix.ortho( left, right, bottom, top, c_near_plane, c_far_plane);
    m_water_shader.setUniformValue( u_water_matrix, m_matrix );

    // Set Texture Coordinates for Shader
    std::vector<float> texture_coordinates;
    setWholeTextureCoordinates(texture_coordinates);
    m_water_shader.setAttributeArray(    a_water_texture_coord, texture_coordinates.data(), 2 );
    m_water_shader.enableAttributeArray( a_water_texture_coord );

    // Load vertices for this object
    QVector<GLfloat> vertices;
    setVertexFromSides(vertices, left, right, top, bottom, 0.0f);
    m_water_shader.setAttributeArray(    a_water_vertex, vertices.data(), 3 );
    m_water_shader.enableAttributeArray( a_water_vertex );

    // Set water variables
    m_water_shader.setUniformValue( u_water_top,        water_top );
    m_water_shader.setUniformValue( u_water_bottom,     water_bottom );
    m_water_shader.setUniformValue( u_water_left,       water_left );
    m_water_shader.setUniformValue( u_water_right,      water_right );
    m_water_shader.setUniformValue( u_water_color,
                                    static_cast<float>(water->water_color.redF()),
                                    static_cast<float>(water->water_color.greenF()),
                                    static_cast<float>(water->water_color.blueF()) );

    m_water_shader.setUniformValue( u_water_color_tint,     water->water_tint );
    m_water_shader.setUniformValue( u_water_reflection,     water->reflection_opacity );
    m_water_shader.setUniformValue( u_water_ripple_length,  water->ripple_length );
    m_water_shader.setUniformValue( u_water_ripple_speed,   water->ripple_speed );

    // Set variables for shader
    m_water_shader.setUniformValue( u_water_alpha,      water->getOpacity() );
    m_water_shader.setUniformValue( u_water_zoom,       m_scale );
    m_water_shader.setUniformValue( u_water_pos,        m_engine->getCurrentWorld()->getCameraPos().x(), m_engine->getCurrentWorld()->getCameraPos().y(), 0.0f );
    m_water_shader.setUniformValue( u_water_width,      static_cast<float>(fbo->width()) );
    m_water_shader.setUniformValue( u_water_height,     static_cast<float>(fbo->height()) );
    m_water_shader.setUniformValue( u_water_time,       static_cast<float>(QTime::currentTime().msecsSinceStartOfDay() / 1000.0) );

    // Draw triangles using shader program
    glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

    // Disable arrays
    m_water_shader.disableAttributeArray( a_water_vertex );
    m_water_shader.disableAttributeArray( a_water_texture_coord );

    // Release Shader
    m_water_shader.release();
}







