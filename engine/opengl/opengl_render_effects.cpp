//
//      Created by Stephens Nunnally on 7/25/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QOpenGLFramebufferObject>

#include <vector>

#include "engine/engine.h"
#include "engine/engine_camera.h"
#include "engine/engine_texture.h"
#include "engine/form_engine.h"
#include "engine/opengl/opengl.h"
#include "engine/thing/engine_thing.h"
#include "engine/thing_component_effects/thing_comp_fisheye.h"
#include "engine/thing_component_effects/thing_comp_mirror.h"
#include "engine/thing_component_effects/thing_comp_swirl.h"
#include "engine/thing_component_effects/thing_comp_water.h"
#include "engine/world/engine_world.h"


//####################################################################################
//##    Finds Effect in screen coordinates, checks if should be rendered.
//##        - Returns top, bottom, left, right in shader coordinates.
//####################################################################################
bool DrOpenGL::getEffectPosition(QOpenGLFramebufferObject *fbo, DrEngineThing *thing,
                       double &top, double &bottom, double &left, double &right, float &angle) {
    // Get Thing position in screen coordinates
    DrPointF position_top =    mapToScreen(thing->getPosition().x, thing->getPosition().y + (thing->getSize().y / 2.0), thing->getZOrder());
    DrPointF position_bottom = mapToScreen(thing->getPosition().x, thing->getPosition().y - (thing->getSize().y / 2.0), thing->getZOrder());
    DrPointF position_left =   mapToScreen(thing->getPosition().x - (thing->getSize().x / 2.0), thing->getPosition().y, thing->getZOrder());
    DrPointF position_right =  mapToScreen(thing->getPosition().x + (thing->getSize().x / 2.0), thing->getPosition().y, thing->getZOrder());
    if (position_left.x   > position_right.x)
        Dr::Swap(position_left,   position_right);
    if (position_bottom.y < position_top.y) {
        Dr::Swap(position_bottom, position_top);
        angle += 180.0f;
    }

    // Calculate corners of Thing to check if it is within view
    QPointF center = QPointF( position_right.x - ((position_right.x - position_left.x)   / 2.0),
                              position_top.y -   ((position_top.y -   position_bottom.y) / 2.0));
    QTransform t = QTransform().translate(center.x(), center.y()).rotate(static_cast<double>(angle)).translate(-center.x(), -center.y());
    QPointF top_left =  t.map(QPointF(position_left.x,  position_top.y));
    QPointF top_right = t.map(QPointF(position_right.x, position_top.y));
    QPointF bot_left =  t.map(QPointF(position_left.x,  position_bottom.y));
    QPointF bot_right = t.map(QPointF(position_right.x, position_bottom.y));
    QRect   in_view = QRect(0, 0, width()*devicePixelRatio(), height()*devicePixelRatio());
    QPolygonF thing_box; thing_box << top_left << top_right << bot_left << bot_right;
    QRect     thing_rect = thing_box.boundingRect().toRect();
    bool process = thing_rect.intersects(in_view) || thing_rect.contains(in_view) || in_view.contains(thing_rect);
    if (!process) return false;

    // Calculate sides of Thing in shader coordinates
    top =    (fbo->height() - position_top.y) / fbo->height();
    bottom = (fbo->height() - position_bottom.y) / fbo->height();
    left =   position_left.x  / fbo->width();
    right =  position_right.x / fbo->width();
    return true;
}


//####################################################################################
//##    Renders FBO to screen buffer as a textured quad using Fisheye Shader
//##        - Returns true if rendered, false if not
//####################################################################################
bool DrOpenGL::drawFrameBufferUsingFisheyeShader(QOpenGLFramebufferObject *fbo, DrEngineThing *fisheye) {
    // Get Fisheye component of DrEngingThing
    DrThingComponent *component = fisheye->component(Comps::Thing_Settings_Fisheye);    if (component == nullptr) return false;
    ThingCompFisheye *fisheye_settings = dynamic_cast<ThingCompFisheye*>(component);    if (fisheye_settings == nullptr) return false;

    // Check effect position and if we should render it
    double top, bottom, left, right;
    float  angle = static_cast<float>(fisheye->getAngle());
    if (getEffectPosition(fbo, fisheye, top, bottom, left, right, angle) == false) return false;

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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // GL_CLAMP_TO_BORDER  // GL_MIRRORED_REPEAT
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // GL_CLAMP_TO_BORDER  // GL_MIRRORED_REPEAT


    // Set Matrix for Shader, apply Orthographic Matrix to fill the viewport
    m_fisheye_shader.setUniformValue( u_fisheye_matrix, orthoMatrix(fbo->width(), fbo->height()) );

    // Set Texture Coordinates for Shader
    m_fisheye_shader.setAttributeArray(    a_fisheye_texture_coord, m_quad_texture_coordinates.data(), 2 );
    m_fisheye_shader.enableAttributeArray( a_fisheye_texture_coord );

    // Load vertices for this object
    std::vector<GLfloat> vertices;
    setQuadVertices(vertices, fbo->width(), fbo->height(), DrPointF(0, 0), 0.0f);
    m_fisheye_shader.setAttributeArray(    a_fisheye_vertex, vertices.data(), 3 );
    m_fisheye_shader.enableAttributeArray( a_fisheye_vertex );

    // Set fisheye variables
    m_fisheye_shader.setUniformValue( u_fisheye_top,            static_cast<float>(top) );
    m_fisheye_shader.setUniformValue( u_fisheye_bottom,         static_cast<float>(bottom) );
    m_fisheye_shader.setUniformValue( u_fisheye_left,           static_cast<float>(left) );
    m_fisheye_shader.setUniformValue( u_fisheye_right,          static_cast<float>(right) );
    m_fisheye_shader.setUniformValue( u_fisheye_start_color,    static_cast<float>(fisheye_settings->start_color.redF()),
                                                                static_cast<float>(fisheye_settings->start_color.greenF()),
                                                                static_cast<float>(fisheye_settings->start_color.blueF()) );
    m_fisheye_shader.setUniformValue( u_fisheye_color_tint,     fisheye_settings->color_tint );
    m_fisheye_shader.setUniformValue( u_fisheye_lens_zoom,      fisheye_settings->lens_zoom );

    // Set more variables for shader
    m_fisheye_shader.setUniformValue( u_fisheye_alpha,          fisheye->getOpacity() );
    m_fisheye_shader.setUniformValue( u_fisheye_zoom,           combinedZoomScale() );
    m_fisheye_shader.setUniformValue( u_fisheye_pos,            m_engine->getCurrentWorld()->getCameraPosition().x,
                                                                m_engine->getCurrentWorld()->getCameraPosition().y, 0.0f );
    m_fisheye_shader.setUniformValue( u_fisheye_width,          static_cast<float>(fbo->width()) );
    m_fisheye_shader.setUniformValue( u_fisheye_height,         static_cast<float>(fbo->height()) );
    m_fisheye_shader.setUniformValue( u_fisheye_time,           static_cast<float>(Dr::MillisecondsSinceStartOfDay() / 1000.0) );
    m_fisheye_shader.setUniformValue( u_fisheye_angle,          angle );

    m_fisheye_shader.setUniformValue( u_fisheye_pixel_x,        fisheye->pixel_x );
    m_fisheye_shader.setUniformValue( u_fisheye_pixel_y,        fisheye->pixel_y );
    m_fisheye_shader.setUniformValue( u_fisheye_bitrate,        fisheye->bitrate );

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
bool DrOpenGL::drawFrameBufferUsingMirrorShader(QOpenGLFramebufferObject *fbo, DrEngineThing *mirror) {
    // Get Mirror component of DrEngingThing
    DrThingComponent *component = mirror->component(Comps::Thing_Settings_Mirror);      if (component == nullptr) return false;
    ThingCompMirror  *mirror_settings = dynamic_cast<ThingCompMirror*>(component);      if (mirror_settings == nullptr) return false;

    // Check effect position and if we should render it
    double top, bottom, left, right;
    float  angle = static_cast<float>(mirror->getAngle());
    if (getEffectPosition(fbo, mirror, top, bottom, left, right, angle) == false) return false;

    // If we are to render, bind the shader
    if (!m_mirror_shader.bind()) return false;
    if (fbo == nullptr) return false;

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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // GL_CLAMP_TO_BORDER  // GL_MIRRORED_REPEAT
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // GL_CLAMP_TO_BORDER  // GL_MIRRORED_REPEAT

    // Set Matrix for Shader, apply Orthographic Matrix to fill the viewport
    m_mirror_shader.setUniformValue( u_mirror_matrix, orthoMatrix(fbo->width(), fbo->height()) );

    // Set Texture Coordinates for Shader
    m_mirror_shader.setAttributeArray(    a_mirror_texture_coord, m_quad_texture_coordinates.data(), 2 );
    m_mirror_shader.enableAttributeArray( a_mirror_texture_coord );

    // Load vertices for this object
    std::vector<GLfloat> vertices;
    setQuadVertices(vertices, fbo->width(), fbo->height(), DrPointF(0, 0), 0.0f);
    m_mirror_shader.setAttributeArray(    a_mirror_vertex, vertices.data(), 3 );
    m_mirror_shader.enableAttributeArray( a_mirror_vertex );

    // Set mirror variables
    m_mirror_shader.setUniformValue( u_mirror_top,          static_cast<float>(top) );
    m_mirror_shader.setUniformValue( u_mirror_bottom,       static_cast<float>(bottom) );
    m_mirror_shader.setUniformValue( u_mirror_left,         static_cast<float>(left) );
    m_mirror_shader.setUniformValue( u_mirror_right,        static_cast<float>(right) );
    m_mirror_shader.setUniformValue( u_mirror_color_top,    static_cast<float>(mirror_settings->start_color.redF()),
                                                            static_cast<float>(mirror_settings->start_color.greenF()),
                                                            static_cast<float>(mirror_settings->start_color.blueF()) );
    m_mirror_shader.setUniformValue( u_mirror_color_bottom, static_cast<float>(mirror_settings->end_color.redF()),
                                                            static_cast<float>(mirror_settings->end_color.greenF()),
                                                            static_cast<float>(mirror_settings->end_color.blueF()) );
    m_mirror_shader.setUniformValue( u_mirror_color_tint,   mirror_settings->tint_percent );
    m_mirror_shader.setUniformValue( u_mirror_blur,         mirror_settings->blur );
    m_mirror_shader.setUniformValue( u_mirror_blur_stretch, mirror_settings->blur_stretch );
    m_mirror_shader.setUniformValue( u_mirror_scale,        mirror_settings->scale );

    // Set more variables for shader
    m_mirror_shader.setUniformValue( u_mirror_alpha,        mirror->getOpacity() );
    m_mirror_shader.setUniformValue( u_mirror_zoom,         combinedZoomScale() );
    m_mirror_shader.setUniformValue( u_mirror_pos,          m_engine->getCurrentWorld()->getCameraPosition().x,
                                                            m_engine->getCurrentWorld()->getCameraPosition().y, 0.0f );
    m_mirror_shader.setUniformValue( u_mirror_width,        static_cast<float>(fbo->width()) );
    m_mirror_shader.setUniformValue( u_mirror_height,       static_cast<float>(fbo->height()) );
    m_mirror_shader.setUniformValue( u_mirror_time,         static_cast<float>(Dr::MillisecondsSinceStartOfDay() / 1000.0) );
    m_mirror_shader.setUniformValue( u_mirror_angle,        angle );

    m_mirror_shader.setUniformValue( u_mirror_pixel_x,      mirror->pixel_x );
    m_mirror_shader.setUniformValue( u_mirror_pixel_y,      mirror->pixel_y );
    m_mirror_shader.setUniformValue( u_mirror_bitrate,      mirror->bitrate );

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
bool DrOpenGL::drawFrameBufferUsingSwirlShader(QOpenGLFramebufferObject *fbo, DrEngineThing *swirl) {
    // Get Swirl component of DrEngingThing
    DrThingComponent *component = swirl->component(Comps::Thing_Settings_Swirl);        if (component == nullptr) return false;
    ThingCompSwirl   *swirl_settings = dynamic_cast<ThingCompSwirl*>(component);        if (swirl_settings == nullptr) return false;

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
    m_swirl_shader.setAttributeArray(    a_swirl_texture_coord, m_quad_texture_coordinates.data(), 2 );
    m_swirl_shader.enableAttributeArray( a_swirl_texture_coord );

    // Load vertices for this object
    std::vector<GLfloat> vertices;
    setQuadVertices(vertices, fbo->width(), fbo->height(), DrPointF(0, 0), 0.0f);
    m_swirl_shader.setAttributeArray(    a_swirl_vertex, vertices.data(), 3 );
    m_swirl_shader.enableAttributeArray( a_swirl_vertex );

    // Set swirl variables
    m_swirl_shader.setUniformValue( u_swirl_top,            static_cast<float>(top) );
    m_swirl_shader.setUniformValue( u_swirl_bottom,         static_cast<float>(bottom) );
    m_swirl_shader.setUniformValue( u_swirl_left,           static_cast<float>(left) );
    m_swirl_shader.setUniformValue( u_swirl_right,          static_cast<float>(right) );
    m_swirl_shader.setUniformValue( u_swirl_start_color,    static_cast<float>(swirl_settings->start_color.redF()),
                                                            static_cast<float>(swirl_settings->start_color.greenF()),
                                                            static_cast<float>(swirl_settings->start_color.blueF()) );
    m_swirl_shader.setUniformValue( u_swirl_color_tint,     swirl_settings->color_tint );
    m_swirl_shader.setUniformValue( u_swirl_rotation,       swirl_settings->rotation );
    m_swirl_shader.setUniformValue( u_swirl_radius,         static_cast<float>(swirl->getSize().x) );

    // Set more variables for shader
    m_swirl_shader.setUniformValue( u_swirl_alpha,          swirl->getOpacity() );
    m_swirl_shader.setUniformValue( u_swirl_zoom,           combinedZoomScale() );
    m_swirl_shader.setUniformValue( u_swirl_pos,            m_engine->getCurrentWorld()->getCameraPosition().x,
                                                            m_engine->getCurrentWorld()->getCameraPosition().y, 0.0f );
    m_swirl_shader.setUniformValue( u_swirl_width,          static_cast<float>(fbo->width()) );
    m_swirl_shader.setUniformValue( u_swirl_height,         static_cast<float>(fbo->height()) );
    m_swirl_shader.setUniformValue( u_swirl_time,           static_cast<float>(Dr::MillisecondsSinceStartOfDay() / 1000.0) );
    m_swirl_shader.setUniformValue( u_swirl_angle,          angle );

    m_swirl_shader.setUniformValue( u_swirl_pixel_x,        swirl->pixel_x );
    m_swirl_shader.setUniformValue( u_swirl_pixel_y,        swirl->pixel_y );
    m_swirl_shader.setUniformValue( u_swirl_bitrate,        swirl->bitrate );

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
bool DrOpenGL::drawFrameBufferUsingWaterShader(QOpenGLFramebufferObject *fbo, DrEngineThing *water) {
    // Get Water component of DrEngingThing
    DrThingComponent *component = water->component(Comps::Thing_Settings_Water);        if (component == nullptr) return false;
    ThingCompWater   *water_settings = dynamic_cast<ThingCompWater*>(component);        if (water_settings == nullptr) return false;

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
    switch (water_settings->water_texture) {
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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);    // GL_CLAMP_TO_BORDER  // GL_MIRRORED_REPEAT
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);    // GL_CLAMP_TO_BORDER  // GL_MIRRORED_REPEAT


    // Set Matrix for Shader, apply Orthographic Matrix to fill the viewport
    m_water_shader.setUniformValue( u_water_matrix, orthoMatrix(fbo->width(), fbo->height()) );

    // Set Texture Coordinates for Shader
    m_water_shader.setAttributeArray(    a_water_texture_coord, m_quad_texture_coordinates.data(), 2 );
    m_water_shader.enableAttributeArray( a_water_texture_coord );

    // Load vertices for this object
    std::vector<GLfloat> vertices;
    setQuadVertices(vertices, fbo->width(), fbo->height(), DrPointF(0, 0), 0.0f);
    m_water_shader.setAttributeArray(    a_water_vertex, vertices.data(), 3 );
    m_water_shader.enableAttributeArray( a_water_vertex );

    // Set water variables
    m_water_shader.setUniformValue( u_water_top,                static_cast<float>(top) );
    m_water_shader.setUniformValue( u_water_bottom,             static_cast<float>(bottom) );
    m_water_shader.setUniformValue( u_water_left,               static_cast<float>(left) );
    m_water_shader.setUniformValue( u_water_right,              static_cast<float>(right) );
    m_water_shader.setUniformValue( u_water_start_color,        static_cast<float>(water_settings->start_color.redF()),
                                                                static_cast<float>(water_settings->start_color.greenF()),
                                                                static_cast<float>(water_settings->start_color.blueF()) );
    m_water_shader.setUniformValue( u_water_end_color,          static_cast<float>(water_settings->end_color.redF()),
                                                                static_cast<float>(water_settings->end_color.greenF()),
                                                                static_cast<float>(water_settings->end_color.blueF()) );

    m_water_shader.setUniformValue( u_water_color_tint,         water_settings->water_tint );
    m_water_shader.setUniformValue( u_water_reflection,         water_settings->reflection_opacity );
    m_water_shader.setUniformValue( u_water_ripple_frequency,   water_settings->ripple_frequency );
    m_water_shader.setUniformValue( u_water_ripple_speed,       water_settings->ripple_speed );
    m_water_shader.setUniformValue( u_water_ripple_amplitude,   water_settings->ripple_amplitude );
    m_water_shader.setUniformValue( u_water_ripple_stretch,     water_settings->ripple_stretch );
    m_water_shader.setUniformValue( u_water_wave_frequency,     water_settings->wave_frequency );
    m_water_shader.setUniformValue( u_water_wave_speed,         water_settings->wave_speed );
    m_water_shader.setUniformValue( u_water_wave_amplitude,     water_settings->wave_amplitude );

    m_water_shader.setUniformValue( u_water_surface_color,      static_cast<float>(water_settings->surface_color.redF()),
                                                                static_cast<float>(water_settings->surface_color.greenF()),
                                                                static_cast<float>(water_settings->surface_color.blueF()) );
    m_water_shader.setUniformValue( u_water_surface_tint,       water_settings->surface_tint );
    m_water_shader.setUniformValue( u_water_surface_height,     water_settings->surface_height );
    m_water_shader.setUniformValue( u_water_surface_flat,       water_settings->surface_keep_flat );

    m_water_shader.setUniformValue( u_refract_reflection,       water_settings->refract_reflection );
    m_water_shader.setUniformValue( u_refract_underwater,       water_settings->refract_underwater );
    m_water_shader.setUniformValue( u_refract_texture,          water_settings->refract_texture );
    m_water_shader.setUniformValue( u_refract_foam,             water_settings->refract_foam );
    m_water_shader.setUniformValue( u_water_movement_speed,     water_settings->movement_speed );

    // Set variables for shader
    m_water_shader.setUniformValue( u_water_alpha,      water->getOpacity() );
    m_water_shader.setUniformValue( u_water_zoom,       combinedZoomScale() );
    m_water_shader.setUniformValue( u_water_pos,        m_engine->getCurrentWorld()->getCameraPosition().x,
                                                        m_engine->getCurrentWorld()->getCameraPosition().y, 0.0f );
    m_water_shader.setUniformValue( u_water_width,      static_cast<float>(fbo->width()) );
    m_water_shader.setUniformValue( u_water_height,     static_cast<float>(fbo->height()) );

    // Wave motion (ticking between 60 and 120 hz depending on monitor)
    static Clock::time_point time_last_frame = Clock::now();
    static long frame_count = 0;
    double milliseconds_past = Dr::MillisecondsElapsed(time_last_frame);
    if (milliseconds_past > 8.0) {
        while (milliseconds_past > 8.0) {
            frame_count++;
            milliseconds_past -= 8.0;
        }
        time_last_frame = Clock::now() - std::chrono::milliseconds(static_cast<long>(milliseconds_past));
    }
    m_water_shader.setUniformValue( u_water_time,       static_cast<float>(frame_count / 60.0) );
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






