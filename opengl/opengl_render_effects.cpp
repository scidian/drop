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
#include "engine/engine_thing_fisheye.h"
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

    // Get Water angle for use in this function
    float angle = static_cast<float>(water->getAngle());

    // Get water position in screen coordinates
    QPointF position_top =    mapToScreen(water->getPosition().x(), water->getPosition().y() + (water->getSize().y() / 2.0), water->z_order);
    QPointF position_bottom = mapToScreen(water->getPosition().x(), water->getPosition().y() - (water->getSize().y() / 2.0), water->z_order);
    QPointF position_left =   mapToScreen(water->getPosition().x() - (water->getSize().x() / 2.0), water->getPosition().y(), water->z_order);
    QPointF position_right =  mapToScreen(water->getPosition().x() + (water->getSize().x() / 2.0), water->getPosition().y(), water->z_order);
    if (position_left.x()   > position_right.x())
        Dr::Swap(position_left,   position_right);
    if (position_bottom.y() < position_top.y()) {
        Dr::Swap(position_bottom, position_top);
        angle += 180.0f;
    }

    // Calculate corners of Water to check if it is within view
    QPointF center = QPointF( position_right.x() - ((position_right.x() - position_left.x())   / 2.0),
                              position_top.y() -   ((position_top.y() -   position_bottom.y()) / 2.0));
    QTransform t = QTransform().translate(center.x(), center.y()).rotate(static_cast<double>(angle)).translate(-center.x(), -center.y());
    QPointF top_left =  t.map(QPointF(position_left.x(),  position_top.y()));
    QPointF top_right = t.map(QPointF(position_right.x(), position_top.y()));
    QPointF bot_left =  t.map(QPointF(position_left.x(),  position_bottom.y()));
    QPointF bot_right = t.map(QPointF(position_right.x(), position_bottom.y()));
    QRect   in_view = QRect(0, 0, width()*devicePixelRatio(), height()*devicePixelRatio());
    QPolygonF water_box; water_box << top_left << top_right << bot_left << bot_right;
    QRect     water_rect = water_box.boundingRect().toRect();
    bool process_water = water_rect.intersects(in_view) || water_rect.contains(in_view) || in_view.contains(water_rect);
    if (!process_water) return;

    // Calculate sides of Water in shader coordinates
    double water_top =    (fbo->height() - position_top.y()) / fbo->height();
    double water_bottom = (fbo->height() - position_bottom.y()) / fbo->height();
    double water_left =   position_left.x()  / fbo->width();
    double water_right =  position_right.x() / fbo->width();

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
    m_water_shader.setUniformValue( u_water_top,        static_cast<float>(water_top) );
    m_water_shader.setUniformValue( u_water_bottom,     static_cast<float>(water_bottom) );
    m_water_shader.setUniformValue( u_water_left,       static_cast<float>(water_left) );
    m_water_shader.setUniformValue( u_water_right,      static_cast<float>(water_right) );
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

    // Draw triangles using shader program
    glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

    // Disable arrays
    m_water_shader.disableAttributeArray( a_water_vertex );
    m_water_shader.disableAttributeArray( a_water_texture_coord );

    // Release Shader
    m_water_shader.release();
}


//####################################################################################
//##        Renders Frame Buffer Object to screen buffer as a textured quad
//##            Uses Fisheye Shader to draw Fisheye Lens
//####################################################################################
void DrOpenGL::drawFrameBufferUsingFisheyeShader(QOpenGLFramebufferObject *fbo, DrEngineFisheye *lens) {

    // Get angle for use in this function
    float angle = static_cast<float>(lens->getAngle());

    // Get position in screen coordinates
    QPointF position_top =    mapToScreen(lens->getPosition().x(), lens->getPosition().y() + (lens->getSize().y() / 2.0), lens->z_order);
    QPointF position_bottom = mapToScreen(lens->getPosition().x(), lens->getPosition().y() - (lens->getSize().y() / 2.0), lens->z_order);
    QPointF position_left =   mapToScreen(lens->getPosition().x() - (lens->getSize().x() / 2.0), lens->getPosition().y(), lens->z_order);
    QPointF position_right =  mapToScreen(lens->getPosition().x() + (lens->getSize().x() / 2.0), lens->getPosition().y(), lens->z_order);
    if (position_left.x()   > position_right.x())
        Dr::Swap(position_left,   position_right);
    if (position_bottom.y() < position_top.y()) {
        Dr::Swap(position_bottom, position_top);
        angle += 180.0f;
    }

    // Calculate corners to check if it is within view
    QPointF center = QPointF( position_right.x() - ((position_right.x() - position_left.x())   / 2.0),
                              position_top.y() -   ((position_top.y() -   position_bottom.y()) / 2.0));
    QTransform t = QTransform().translate(center.x(), center.y()).rotate(static_cast<double>(angle)).translate(-center.x(), -center.y());
    QPointF top_left =  t.map(QPointF(position_left.x(),  position_top.y()));
    QPointF top_right = t.map(QPointF(position_right.x(), position_top.y()));
    QPointF bot_left =  t.map(QPointF(position_left.x(),  position_bottom.y()));
    QPointF bot_right = t.map(QPointF(position_right.x(), position_bottom.y()));
    QRect   in_view = QRect(0, 0, width()*devicePixelRatio(), height()*devicePixelRatio());
    QPolygonF lens_box; lens_box << top_left << top_right << bot_left << bot_right;
    QRect     lens_rect = lens_box.boundingRect().toRect();
    bool process_lens = lens_rect.intersects(in_view) || lens_rect.contains(in_view) || in_view.contains(lens_rect);
    if (!process_lens) return;

    // Calculate sides of lens in shader coordinates
    double top =    (fbo->height() - position_top.y()) / fbo->height();
    double bottom = (fbo->height() - position_bottom.y()) / fbo->height();
    double left =   position_left.x()  / fbo->width();
    double right =  position_right.x() / fbo->width();

    // If we are to render lens, bind the shader
    if (!m_fisheye_shader.bind()) return;
    if (!fbo) return;

    // Bind offscreen frame buffer objects as textures
    glEnable(GL_TEXTURE_2D);
    GLint texture =       glGetUniformLocation(m_fisheye_shader.programId(), "u_texture");
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

    // Draw triangles using shader program
    glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

    // Disable arrays
    m_fisheye_shader.disableAttributeArray( a_fisheye_vertex );
    m_fisheye_shader.disableAttributeArray( a_fisheye_texture_coord );

    // Release Shader
    m_fisheye_shader.release();
}







