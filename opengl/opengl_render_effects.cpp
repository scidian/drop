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
#include "engine/engine_texture.h"
#include "engine/engine_world.h"
#include "engine/form_engine.h"
#include "engine_things/engine_thing_fisheye.h"
#include "engine_things/engine_thing_light.h"
#include "engine_things/engine_thing_mirror.h"
#include "engine_things/engine_thing_object.h"
#include "engine_things/engine_thing_swirl.h"
#include "engine_things/engine_thing_water.h"
#include "helper.h"
#include "opengl/opengl.h"


//####################################################################################
//##    Finds Effect in screen coordinates, checks if should be rendered.
//##        - Returns top, bottom, left, right in shader coordinates.
//####################################################################################
bool DrOpenGL::getEffectPosition(QOpenGLFramebufferObject *fbo, DrEngineThing *thing,
                       double &top, double &bottom, double &left, double &right, float &angle) {
    // Get Thing position in screen coordinates
    QPointF position_top =    mapToScreen(thing->getPosition().x(), thing->getPosition().y() + (thing->getSize().y() / 2.0), thing->z_order);
    QPointF position_bottom = mapToScreen(thing->getPosition().x(), thing->getPosition().y() - (thing->getSize().y() / 2.0), thing->z_order);
    QPointF position_left =   mapToScreen(thing->getPosition().x() - (thing->getSize().x() / 2.0), thing->getPosition().y(), thing->z_order);
    QPointF position_right =  mapToScreen(thing->getPosition().x() + (thing->getSize().x() / 2.0), thing->getPosition().y(), thing->z_order);
    if (position_left.x()   > position_right.x())
        Dr::Swap(position_left,   position_right);
    if (position_bottom.y() < position_top.y()) {
        Dr::Swap(position_bottom, position_top);
        angle += 180.0f;
    }

    // Calculate corners of Thing to check if it is within view
    QPointF center = QPointF( position_right.x() - ((position_right.x() - position_left.x())   / 2.0),
                              position_top.y() -   ((position_top.y() -   position_bottom.y()) / 2.0));
    QTransform t = QTransform().translate(center.x(), center.y()).rotate(static_cast<double>(angle)).translate(-center.x(), -center.y());
    QPointF top_left =  t.map(QPointF(position_left.x(),  position_top.y()));
    QPointF top_right = t.map(QPointF(position_right.x(), position_top.y()));
    QPointF bot_left =  t.map(QPointF(position_left.x(),  position_bottom.y()));
    QPointF bot_right = t.map(QPointF(position_right.x(), position_bottom.y()));
    QRect   in_view = QRect(0, 0, width()*devicePixelRatio(), height()*devicePixelRatio());
    QPolygonF thing_box; thing_box << top_left << top_right << bot_left << bot_right;
    QRect     thing_rect = thing_box.boundingRect().toRect();
    bool process = thing_rect.intersects(in_view) || thing_rect.contains(in_view) || in_view.contains(thing_rect);
    if (!process) return false;

    // Calculate sides of Thing in shader coordinates
    top =    (fbo->height() - position_top.y()) / fbo->height();
    bottom = (fbo->height() - position_bottom.y()) / fbo->height();
    left =   position_left.x()  / fbo->width();
    right =  position_right.x() / fbo->width();
    return true;
}


//####################################################################################
//##    Renders FBO to screen buffer as a textured quad using Fisheye Shader
//##        - Returns true if rendered, false if not
//####################################################################################
bool DrOpenGL::drawFrameBufferUsingFisheyeShader(QOpenGLFramebufferObject *fbo, DrEngineFisheye *lens) {

    // Check effect position and if we should render it
    double top, bottom, left, right;
    float  angle = static_cast<float>(lens->getAngle());
    if (getEffectPosition(fbo, lens, top, bottom, left, right, angle) == false) return false;

    // If we are to render, bind the shader
    if (!m_fisheye_shader.bind()) return false;
    if (!fbo) return false;

    // Bind offscreen frame buffer objects as textures
    glEnable(GL_TEXTURE_2D);
    GLint texture = glGetUniformLocation(m_fisheye_shader.programId(), "u_texture");
    glUseProgram(m_fisheye_shader.programId());
    glUniform1i(texture,       0);
    glActiveTexture(GL_TEXTURE0);                           // Texture unit 0
    glBindTexture(GL_TEXTURE_2D, fbo->texture());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);// GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);// GL_CLAMP_TO_EDGE);


    // Set Matrix for Shader, apply Orthographic Matrix to fill the viewport
    m_fisheye_shader.setUniformValue( u_fisheye_matrix, orthoMatrix(fbo->width(), fbo->height()) );

    // Set Texture Coordinates for Shader
    m_fisheye_shader.setAttributeArray(    a_fisheye_texture_coord, m_whole_texture_coordinates.data(), 2 );
    m_fisheye_shader.enableAttributeArray( a_fisheye_texture_coord );

    // Load vertices for this object
    QVector<GLfloat> vertices;
    setQuadVertices(vertices, fbo->width(), fbo->height(), QPointF(0, 0), 0.0f);
    m_fisheye_shader.setAttributeArray(    a_fisheye_vertex, vertices.data(), 3 );
    m_fisheye_shader.enableAttributeArray( a_fisheye_vertex );

    // Set fisheye variables
    m_fisheye_shader.setUniformValue( u_fisheye_top,        static_cast<float>(top) );
    m_fisheye_shader.setUniformValue( u_fisheye_bottom,     static_cast<float>(bottom) );
    m_fisheye_shader.setUniformValue( u_fisheye_left,       static_cast<float>(left) );
    m_fisheye_shader.setUniformValue( u_fisheye_right,      static_cast<float>(right) );
    m_fisheye_shader.setUniformValue( u_fisheye_start_color,
                                        static_cast<float>(lens->start_color.redF()),
                                        static_cast<float>(lens->start_color.greenF()),
                                        static_cast<float>(lens->start_color.blueF()) );
    m_fisheye_shader.setUniformValue( u_fisheye_color_tint,         lens->color_tint );
    m_fisheye_shader.setUniformValue( u_fisheye_lens_zoom,          lens->lens_zoom );

    // Set more variables for shader
    m_fisheye_shader.setUniformValue( u_fisheye_alpha,      lens->getOpacity() );
    m_fisheye_shader.setUniformValue( u_fisheye_zoom,       m_scale );
    m_fisheye_shader.setUniformValue( u_fisheye_pos,        m_engine->getCurrentWorld()->getCameraPos().x(), m_engine->getCurrentWorld()->getCameraPos().y(), 0.0f );
    m_fisheye_shader.setUniformValue( u_fisheye_width,      static_cast<float>(fbo->width()) );
    m_fisheye_shader.setUniformValue( u_fisheye_height,     static_cast<float>(fbo->height()) );
    m_fisheye_shader.setUniformValue( u_fisheye_time,       static_cast<float>(QTime::currentTime().msecsSinceStartOfDay() / 1000.0) );
    m_fisheye_shader.setUniformValue( u_fisheye_angle,      angle );

    m_fisheye_shader.setUniformValue( u_fisheye_pixel_x,    lens->pixel_x );
    m_fisheye_shader.setUniformValue( u_fisheye_pixel_y,    lens->pixel_y );
    m_fisheye_shader.setUniformValue( u_fisheye_bitrate,    lens->bitrate );

    // Draw triangles using shader program
    glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

    // Disable arrays
    m_fisheye_shader.disableAttributeArray( a_fisheye_vertex );
    m_fisheye_shader.disableAttributeArray( a_fisheye_texture_coord );

    // Release Shader
    m_fisheye_shader.release();
    return true;
}


//####################################################################################
//##    Renders FBO to screen buffer as a textured quad using Mirror Shader
//##        - Returns true if rendered, false if not
//####################################################################################
bool DrOpenGL::drawFrameBufferUsingMirrorShader(QOpenGLFramebufferObject *fbo, DrEngineMirror *mirror) {

    // Check effect position and if we should render it
    double top, bottom, left, right;
    float  angle = static_cast<float>(mirror->getAngle());
    if (getEffectPosition(fbo, mirror, top, bottom, left, right, angle) == false) return false;

    // If we are to render, bind the shader
    if (!m_mirror_shader.bind()) return false;
    if (!fbo) return false;

    // Bind offscreen frame buffer objects as textures
    glEnable(GL_TEXTURE_2D);
    GLint texture = glGetUniformLocation(m_mirror_shader.programId(), "u_texture");
    GLint noise =   glGetUniformLocation(m_mirror_shader.programId(), "u_texture_noise");
    glUseProgram(m_mirror_shader.programId());
    glUniform1i(texture,    0);
    glUniform1i(noise,      1);

    // Bind textures - !!!!! #NOTE: Must be called in descending order and end on 0
    glActiveTexture(GL_TEXTURE1);                           // Texture unit 1
    glBindTexture(GL_TEXTURE_2D, m_engine->getTexture(Asset_Textures::Mirror_Noise_1)->texture()->textureId());
    m_engine->getTexture(Asset_Textures::Mirror_Noise_1)->texture()->setWrapMode(QOpenGLTexture::WrapMode::MirroredRepeat);

    glActiveTexture(GL_TEXTURE0);                           // Texture unit 0
    glBindTexture(GL_TEXTURE_2D, fbo->texture());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); // GL_CLAMP_TO_EDGE  // GL_MIRRORED_REPEAT
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER); // GL_CLAMP_TO_EDGE  // GL_MIRRORED_REPEAT

    // Set Matrix for Shader, apply Orthographic Matrix to fill the viewport
    m_mirror_shader.setUniformValue( u_mirror_matrix, orthoMatrix(fbo->width(), fbo->height()) );

    // Set Texture Coordinates for Shader
    m_mirror_shader.setAttributeArray(    a_mirror_texture_coord, m_whole_texture_coordinates.data(), 2 );
    m_mirror_shader.enableAttributeArray( a_mirror_texture_coord );

    // Load vertices for this object
    QVector<GLfloat> vertices;
    setQuadVertices(vertices, fbo->width(), fbo->height(), QPointF(0, 0), 0.0f);
    m_mirror_shader.setAttributeArray(    a_mirror_vertex, vertices.data(), 3 );
    m_mirror_shader.enableAttributeArray( a_mirror_vertex );

    // Set mirror variables
    m_mirror_shader.setUniformValue( u_mirror_top,        static_cast<float>(top) );
    m_mirror_shader.setUniformValue( u_mirror_bottom,     static_cast<float>(bottom) );
    m_mirror_shader.setUniformValue( u_mirror_left,       static_cast<float>(left) );
    m_mirror_shader.setUniformValue( u_mirror_right,      static_cast<float>(right) );
    m_mirror_shader.setUniformValue( u_mirror_color_top,
                                        static_cast<float>(mirror->start_color.redF()),
                                        static_cast<float>(mirror->start_color.greenF()),
                                        static_cast<float>(mirror->start_color.blueF()) );
    m_mirror_shader.setUniformValue( u_mirror_color_bottom,
                                        static_cast<float>(mirror->end_color.redF()),
                                        static_cast<float>(mirror->end_color.greenF()),
                                        static_cast<float>(mirror->end_color.blueF()) );
    m_mirror_shader.setUniformValue( u_mirror_color_tint,   mirror->tint_percent );
    m_mirror_shader.setUniformValue( u_mirror_blur,         mirror->blur );
    m_mirror_shader.setUniformValue( u_mirror_blur_stretch, mirror->blur_stretch );
    m_mirror_shader.setUniformValue( u_mirror_scale,        mirror->scale );

    // Set more variables for shader
    m_mirror_shader.setUniformValue( u_mirror_alpha,      mirror->getOpacity() );
    m_mirror_shader.setUniformValue( u_mirror_zoom,       m_scale );
    m_mirror_shader.setUniformValue( u_mirror_pos,        m_engine->getCurrentWorld()->getCameraPos().x(), m_engine->getCurrentWorld()->getCameraPos().y(), 0.0f );
    m_mirror_shader.setUniformValue( u_mirror_width,      static_cast<float>(fbo->width()) );
    m_mirror_shader.setUniformValue( u_mirror_height,     static_cast<float>(fbo->height()) );
    m_mirror_shader.setUniformValue( u_mirror_time,       static_cast<float>(QTime::currentTime().msecsSinceStartOfDay() / 1000.0) );
    m_mirror_shader.setUniformValue( u_mirror_angle,      angle );

    m_mirror_shader.setUniformValue( u_mirror_pixel_x,    mirror->pixel_x );
    m_mirror_shader.setUniformValue( u_mirror_pixel_y,    mirror->pixel_y );
    m_mirror_shader.setUniformValue( u_mirror_bitrate,    mirror->bitrate );

    // Draw triangles using shader program
    glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

    // Disable arrays
    m_mirror_shader.disableAttributeArray( a_mirror_vertex );
    m_mirror_shader.disableAttributeArray( a_mirror_texture_coord );

    // Release Shader
    m_mirror_shader.release();
    return true;
}


//####################################################################################
//##    Renders FBO to screen buffer as a textured quad using Swirl Shader
//##        - Returns true if rendered, false if not
//####################################################################################
bool DrOpenGL::drawFrameBufferUsingSwirlShader(QOpenGLFramebufferObject *fbo, DrEngineSwirl *swirl) {

    // Check effect position and if we should render it
    double top, bottom, left, right;
    float  angle = static_cast<float>(swirl->getAngle());
    if (getEffectPosition(fbo, swirl, top, bottom, left, right, angle) == false) return false;

    // If we are to render, bind the shader
    if (!m_swirl_shader.bind()) return false;
    if (!fbo) return false;

    // Bind offscreen frame buffer objects as textures
    glEnable(GL_TEXTURE_2D);
    GLint texture = glGetUniformLocation(m_swirl_shader.programId(), "u_texture");
    glUseProgram(m_swirl_shader.programId());
    glUniform1i(texture,    0);

    glActiveTexture(GL_TEXTURE0);                           // Texture unit 0
    glBindTexture(GL_TEXTURE_2D, fbo->texture());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT); // GL_CLAMP_TO_BORDER); // GL_CLAMP_TO_EDGE
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT); // GL_CLAMP_TO_BORDER); // GL_CLAMP_TO_EDGE

    // Set Matrix for Shader, apply Orthographic Matrix to fill the viewport
    m_swirl_shader.setUniformValue( u_swirl_matrix, orthoMatrix(fbo->width(), fbo->height()) );

    // Set Texture Coordinates for Shader
    m_swirl_shader.setAttributeArray(    a_swirl_texture_coord, m_whole_texture_coordinates.data(), 2 );
    m_swirl_shader.enableAttributeArray( a_swirl_texture_coord );

    // Load vertices for this object
    QVector<GLfloat> vertices;
    setQuadVertices(vertices, fbo->width(), fbo->height(), QPointF(0, 0), 0.0f);
    m_swirl_shader.setAttributeArray(    a_swirl_vertex, vertices.data(), 3 );
    m_swirl_shader.enableAttributeArray( a_swirl_vertex );

    // Set swirl variables
    m_swirl_shader.setUniformValue( u_swirl_top,        static_cast<float>(top) );
    m_swirl_shader.setUniformValue( u_swirl_bottom,     static_cast<float>(bottom) );
    m_swirl_shader.setUniformValue( u_swirl_left,       static_cast<float>(left) );
    m_swirl_shader.setUniformValue( u_swirl_right,      static_cast<float>(right) );

    m_swirl_shader.setUniformValue( u_swirl_start_color,
                                        static_cast<float>(swirl->start_color.redF()),
                                        static_cast<float>(swirl->start_color.greenF()),
                                        static_cast<float>(swirl->start_color.blueF()) );
    m_swirl_shader.setUniformValue( u_swirl_color_tint,         swirl->color_tint );
    m_swirl_shader.setUniformValue( u_swirl_rotation,   swirl->rotation );
    m_swirl_shader.setUniformValue( u_swirl_radius,     static_cast<float>(swirl->getSize().x()) );

    // Set more variables for shader
    m_swirl_shader.setUniformValue( u_swirl_alpha,      swirl->getOpacity() );
    m_swirl_shader.setUniformValue( u_swirl_zoom,       m_scale );
    m_swirl_shader.setUniformValue( u_swirl_pos,        m_engine->getCurrentWorld()->getCameraPos().x(), m_engine->getCurrentWorld()->getCameraPos().y(), 0.0f );
    m_swirl_shader.setUniformValue( u_swirl_width,      static_cast<float>(fbo->width()) );
    m_swirl_shader.setUniformValue( u_swirl_height,     static_cast<float>(fbo->height()) );
    m_swirl_shader.setUniformValue( u_swirl_time,       static_cast<float>(QTime::currentTime().msecsSinceStartOfDay() / 1000.0) );
    m_swirl_shader.setUniformValue( u_swirl_angle,      angle );

    m_swirl_shader.setUniformValue( u_swirl_pixel_x,    swirl->pixel_x );
    m_swirl_shader.setUniformValue( u_swirl_pixel_y,    swirl->pixel_y );
    m_swirl_shader.setUniformValue( u_swirl_bitrate,    swirl->bitrate );

    // Draw triangles using shader program
    glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

    // Disable arrays
    m_swirl_shader.disableAttributeArray( a_swirl_vertex );
    m_swirl_shader.disableAttributeArray( a_swirl_texture_coord );

    // Release Shader
    m_swirl_shader.release();
    return true;
}


//####################################################################################
//##    Renders FBO to screen buffer as a textured quad using Water Shader to draw reflective / refractive / textured water
//##        - Returns true if rendered, false if not
//####################################################################################
bool DrOpenGL::drawFrameBufferUsingWaterShader(QOpenGLFramebufferObject *fbo, DrEngineWater *water) {

    // Check effect position and if we should render it
    double top, bottom, left, right;
    float  angle = static_cast<float>(water->getAngle());
    if (getEffectPosition(fbo, water, top, bottom, left, right, angle) == false) return false;

    // If we are to render, bind the shader
    if (!m_water_shader.bind()) return false;
    if (!fbo) return false;

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
    switch (water->water_texture) {
        case Water_Texture::None:

            break;
        case Water_Texture::Ripples:
            glBindTexture(GL_TEXTURE_2D, m_engine->getTexture(Asset_Textures::Water_Texture_1)->texture()->textureId());
            m_engine->getTexture(Asset_Textures::Water_Texture_1)->texture()->setWrapMode(QOpenGLTexture::WrapMode::MirroredRepeat);
            break;
        case Water_Texture::Cells:
            glBindTexture(GL_TEXTURE_2D, m_engine->getTexture(Asset_Textures::Water_Texture_2)->texture()->textureId());
            m_engine->getTexture(Asset_Textures::Water_Texture_2)->texture()->setWrapMode(QOpenGLTexture::WrapMode::MirroredRepeat);
            break;
        case Water_Texture::Caustic:
            glBindTexture(GL_TEXTURE_2D, m_engine->getTexture(Asset_Textures::Water_Texture_3)->texture()->textureId());
            m_engine->getTexture(Asset_Textures::Water_Texture_3)->texture()->setWrapMode(QOpenGLTexture::WrapMode::MirroredRepeat);
            break;
        case Water_Texture::Liquid:
            glBindTexture(GL_TEXTURE_2D, m_engine->getTexture(Asset_Textures::Water_Texture_4)->texture()->textureId());
            m_engine->getTexture(Asset_Textures::Water_Texture_4)->texture()->setWrapMode(QOpenGLTexture::WrapMode::MirroredRepeat);
            break;
    }

    glActiveTexture(GL_TEXTURE1);                           // Texture unit 1
    glBindTexture(GL_TEXTURE_2D, m_engine->getTexture(Asset_Textures::Water_Normal_1)->texture()->textureId());
    m_engine->getTexture(Asset_Textures::Water_Normal_1)->texture()->setWrapMode(QOpenGLTexture::WrapMode::MirroredRepeat);

    glActiveTexture(GL_TEXTURE0);                           // Texture unit 0
    glBindTexture(GL_TEXTURE_2D, fbo->texture());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);      // GL_CLAMP_TO_EDGE // <-- Was better for sides before rotation was added
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);      // Better for reflection


    // Set Matrix for Shader, apply Orthographic Matrix to fill the viewport
    m_water_shader.setUniformValue( u_water_matrix, orthoMatrix(fbo->width(), fbo->height()) );

    // Set Texture Coordinates for Shader
    m_water_shader.setAttributeArray(    a_water_texture_coord, m_whole_texture_coordinates.data(), 2 );
    m_water_shader.enableAttributeArray( a_water_texture_coord );

    // Load vertices for this object
    QVector<GLfloat> vertices;
    setQuadVertices(vertices, fbo->width(), fbo->height(), QPointF(0, 0), 0.0f);
    m_water_shader.setAttributeArray(    a_water_vertex, vertices.data(), 3 );
    m_water_shader.enableAttributeArray( a_water_vertex );

    // Set water variables
    m_water_shader.setUniformValue( u_water_top,        static_cast<float>(top) );
    m_water_shader.setUniformValue( u_water_bottom,     static_cast<float>(bottom) );
    m_water_shader.setUniformValue( u_water_left,       static_cast<float>(left) );
    m_water_shader.setUniformValue( u_water_right,      static_cast<float>(right) );
    m_water_shader.setUniformValue( u_water_start_color,
                                        static_cast<float>(water->start_color.redF()),
                                        static_cast<float>(water->start_color.greenF()),
                                        static_cast<float>(water->start_color.blueF()) );
    m_water_shader.setUniformValue( u_water_end_color,
                                        static_cast<float>(water->end_color.redF()),
                                        static_cast<float>(water->end_color.greenF()),
                                        static_cast<float>(water->end_color.blueF()) );

    m_water_shader.setUniformValue( u_water_color_tint,         water->water_tint );
    m_water_shader.setUniformValue( u_water_reflection,         water->reflection_opacity );
    m_water_shader.setUniformValue( u_water_ripple_frequency,   water->ripple_frequency );
    m_water_shader.setUniformValue( u_water_ripple_speed,       water->ripple_speed );
    m_water_shader.setUniformValue( u_water_ripple_amplitude,   water->ripple_amplitude );
    m_water_shader.setUniformValue( u_water_ripple_stretch,     water->ripple_stretch );
    m_water_shader.setUniformValue( u_water_wave_frequency,     water->wave_frequency );
    m_water_shader.setUniformValue( u_water_wave_speed,         water->wave_speed );
    m_water_shader.setUniformValue( u_water_wave_amplitude,     water->wave_amplitude );

    m_water_shader.setUniformValue( u_water_surface_color,
                                        static_cast<float>(water->surface_color.redF()),
                                        static_cast<float>(water->surface_color.greenF()),
                                        static_cast<float>(water->surface_color.blueF()) );
    m_water_shader.setUniformValue( u_water_surface_tint,       water->surface_tint );
    m_water_shader.setUniformValue( u_water_surface_height,     water->surface_height );
    m_water_shader.setUniformValue( u_water_surface_flat,       water->surface_keep_flat );

    m_water_shader.setUniformValue( u_refract_reflection,       water->refract_reflection );
    m_water_shader.setUniformValue( u_refract_underwater,       water->refract_underwater );
    m_water_shader.setUniformValue( u_refract_texture,          water->refract_texture );
    m_water_shader.setUniformValue( u_refract_foam,             water->refract_foam );
    m_water_shader.setUniformValue( u_water_movement_speed,     water->movement_speed );

    // Set variables for shader
    m_water_shader.setUniformValue( u_water_alpha,      water->getOpacity() );
    m_water_shader.setUniformValue( u_water_zoom,       m_scale );
    m_water_shader.setUniformValue( u_water_pos,        m_engine->getCurrentWorld()->getCameraPos().x(), m_engine->getCurrentWorld()->getCameraPos().y(), 0.0f );
    m_water_shader.setUniformValue( u_water_width,      static_cast<float>(fbo->width()) );
    m_water_shader.setUniformValue( u_water_height,     static_cast<float>(fbo->height()) );
    m_water_shader.setUniformValue( u_water_time,       static_cast<float>(QTime::currentTime().msecsSinceStartOfDay() / 1000.0) );
    m_water_shader.setUniformValue( u_water_angle,      angle );

    m_water_shader.setUniformValue( u_water_pixel_x,    water->pixel_x );
    m_water_shader.setUniformValue( u_water_pixel_y,    water->pixel_y );
    m_water_shader.setUniformValue( u_water_bitrate,    water->bitrate );

    // Draw triangles using shader program
    glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

    // Disable arrays
    m_water_shader.disableAttributeArray( a_water_vertex );
    m_water_shader.disableAttributeArray( a_water_texture_coord );

    // Release Shader
    m_water_shader.release();
    return true;
}






