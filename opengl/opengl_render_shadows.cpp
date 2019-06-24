//
//      Created by Stephens Nunnally on 6/17/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QtMath>
#include <QOpenGLFramebufferObject>

#include "engine/engine.h"
#include "engine/engine_light.h"
#include "engine/engine_object.h"
#include "engine/engine_texture.h"
#include "engine/engine_world.h"
#include "engine/form_engine.h"
#include "helper.h"
#include "opengl/opengl.h"


//####################################################################################
//##        Main Shadow Map / Occluder / Render Routine
//####################################################################################
void OpenGL::drawShadowMaps() {
    if (m_engine->getCurrentWorld()->lights.count() <= 0) return;

    // ***** Check for lights with shadows, if there are non we don't need to draw occluder map
    bool has_shadows = false;
    for (auto light : m_engine->getCurrentWorld()->lights) {
        if (light == nullptr) continue;
        if (light->draw_shadows == true)  has_shadows = true;

        // Calculate size of light texture (fbo)
        light->setLightDiameter( static_cast<int>(light->light_size * viewScale()) );
        light->setLightDiameterFitted( (light->getLightDiameter() > g_max_light_fbo_size) ? g_max_light_fbo_size : light->getLightDiameter() );
    }
    if (!has_shadows) return;

    // ***** Render all Space Objects to an off-screen Frame Buffer Object Occluder Map
    bindOccluderMapBuffer();
    glViewport(0, 0, m_occluder_fbo->width(), m_occluder_fbo->height());
    drawSpaceOccluder();
    m_occluder_fbo->release();
    glViewport(0, 0, width()*devicePixelRatio(), height()*devicePixelRatio());

    // ***** Calculate Light 1D Shadow Maps
    for (auto light : m_engine->getCurrentWorld()->lights) {
        if (light == nullptr) continue;
        if (light->draw_shadows == false) continue;

        // Calculate light position on Occluder Map
        light->setScreenPos( mapToOccluder( QVector3D(static_cast<float>(light->getBodyPosition().x()),
                                                      static_cast<float>(light->getBodyPosition().y()), 0.0f)) );
        double middle = m_texture_fbo->height() / 2.0;
        double y_diff = middle - light->getScreenPos().y();

        // Blit the area of the occluder map the light will take up to the Light Occluder FBO
        bindLightOcculderBuffer(light);
        QOpenGLFramebufferObject::blitFramebuffer(
                    light->occluder_fbo, QRect(0, 0, light->occluder_fbo->width(), light->occluder_fbo->height()),
                    m_occluder_fbo, QRect(
                        static_cast<int>(floor(light->getScreenPos().x() - (light->occluder_fbo->width() / 2.0 * double(c_occluder_scale)))),
                        static_cast<int>(floor(light->getScreenPos().y() - (light->occluder_fbo->height()/ 2.0 * double(c_occluder_scale)) + (y_diff * 2.0))),
                        static_cast<int>(light->occluder_fbo->width()* c_occluder_scale),
                        static_cast<int>(light->occluder_fbo->height()*c_occluder_scale)),
                    GL_COLOR_BUFFER_BIT, GL_NEAREST);
        light->occluder_fbo->release();

        // Draw the 1D Shadow Map from the Occluder Map
        bindLightShadowBuffer(light);
        draw1DShadowMap(light);
        light->shadow_fbo->release();
    } // End For
}


//####################################################################################
//##        Allocate Occluder Map
//####################################################################################
void OpenGL::bindOccluderMapBuffer() {
    int desired_x = g_max_occluder_fbo_size;
    int desired_y = g_max_occluder_fbo_size;

    if (!m_occluder_fbo || (m_occluder_fbo->width() != desired_x || m_occluder_fbo->height() != desired_y)) {
        delete m_occluder_fbo;
        m_occluder_fbo =  new QOpenGLFramebufferObject(desired_x, desired_y);
    }
    m_occluder_fbo->bind();

    // Clear the buffers
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

//####################################################################################
//##        Allocate Light Occluder Frame Buffer Object
//####################################################################################
void OpenGL::bindLightOcculderBuffer(DrEngineLight *light) {
    // Check Frame Buffer Object is initialized
    bool need_to_create_new = false;
    if (light->occluder_fbo == nullptr)
        need_to_create_new = true;
    else if (light->occluder_fbo->width() != light->getLightDiameterFitted() || light->occluder_fbo->height() != light->getLightDiameterFitted())
        need_to_create_new = true;
    if (need_to_create_new) {
        delete light->occluder_fbo;
        light->occluder_fbo = new QOpenGLFramebufferObject(light->getLightDiameterFitted(), light->getLightDiameterFitted());
    }

    // Bind and clear buffer
    light->occluder_fbo->bind();
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}


//####################################################################################
//##        Allocate Shadow Frame Buffer Object
//####################################################################################
void OpenGL::bindLightShadowBuffer(DrEngineLight *light) {
    // Shadow map size is the smallest of c_angles, light_radius_fitted, and width()
    int shadow_size = (light->getLightDiameterFitted() < g_max_rays) ? light->getLightDiameterFitted() : g_max_rays;
        shadow_size = (width()*devicePixelRatio() < shadow_size) ? width()*devicePixelRatio() : shadow_size;

    // Check Frame Buffer Object is initialized
    bool need_to_create_new = false;
    if (light->shadow_fbo == nullptr)
        need_to_create_new = true;
    else if (light->shadow_fbo->width() != shadow_size)
        need_to_create_new = true;
    if (need_to_create_new) {
        delete light->shadow_fbo;
        light->shadow_fbo = new QOpenGLFramebufferObject(shadow_size, 1);
    }

    // Bind and clear buffer
    light->shadow_fbo->bind();
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}


//####################################################################################
//##        Renders the 1D Shadow Map based on the Occluder Map
//####################################################################################
void OpenGL::draw1DShadowMap(DrEngineLight *light) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, light->occluder_fbo->texture());

    if (!m_shadow_shader.bind()) return;

    // Reset our projection matrix to the FBO size
    float left =   0.0f - ((light->shadow_fbo->width() )  / 2.0f);
    float right =  0.0f + ((light->shadow_fbo->width() )  / 2.0f);
    float top =    0.0f + ((light->shadow_fbo->height() ) / 2.0f);
    float bottom = 0.0f - ((light->shadow_fbo->height() ) / 2.0f);
    QMatrix4x4 m_matrix;
    m_matrix.ortho( left, right, bottom, top,  -1000.0f, 1000.0f);
    m_shadow_shader.setUniformValue( m_uniform_shadow_matrix, m_matrix );

    // Set Texture Coordinates for Shader
    std::vector<float> texture_coordinates;
    setWholeTextureCoordinates(texture_coordinates);
    m_shadow_shader.setAttributeArray(    m_attribute_shadow_tex_coord, texture_coordinates.data(), 2 );
    m_shadow_shader.enableAttributeArray( m_attribute_shadow_tex_coord );

    // Load vertices for this object
    QVector<GLfloat> vertices;
    setVertexFromSides(vertices, left, right, top, bottom);
    m_shadow_shader.setAttributeArray(    m_attribute_shadow_vertex, vertices.data(), 3 );
    m_shadow_shader.enableAttributeArray( m_attribute_shadow_vertex );

    // Use texture unit 0
    m_shadow_shader.setUniformValue( m_uniform_shadow_texture, 0 );

    // ***** Give the shader our Ray Count and Scaled Light Radius
    m_shadow_shader.setUniformValue( m_uniform_shadow_ray_count,  static_cast<float>(light->shadow_fbo->width()) );

    float screen_scale = (width()*devicePixelRatio() / light->light_size);
    m_shadow_shader.setUniformValue( m_uniform_shadow_resolution, light->getLightDiameter(), (light->getLightDiameter() / viewScale()) * screen_scale );
    m_shadow_shader.setUniformValue( m_uniform_shadow_depth,      static_cast<float>(light->getZOrder()) );

    // Draw triangles using shader program
    glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

    // Disable arrays
    m_shadow_shader.disableAttributeArray( m_attribute_shadow_vertex );
    m_shadow_shader.disableAttributeArray( m_attribute_shadow_tex_coord );

    // Release Shader
    m_shadow_shader.release();
}


//####################################################################################
//##        Renders the light to the using the Shadow Map
//####################################################################################
void OpenGL::draw2DLight(DrEngineLight *light) {

    if (light->draw_shadows) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, light->shadow_fbo->texture());
    }

    if (!m_light_shader.bind()) return;

    // Set Matrix for Shader, apply Orthographic Matrix to fill the viewport
    QMatrix4x4 m_matrix = m_projection * m_model_view;
    m_light_shader.setUniformValue( m_uniform_light_matrix, m_matrix );

    // Set Texture Coordinates for Shader
    std::vector<float> texture_coordinates;
    setWholeTextureCoordinates(texture_coordinates);
    m_light_shader.setAttributeArray(    m_attribute_light_tex_coord, texture_coordinates.data(), 2 );
    m_light_shader.enableAttributeArray( m_attribute_light_tex_coord );

    // Load vertices for this object
    float left =   ( static_cast<float>(light->getBodyPosition().x()) * viewScale() ) - (light->getLightDiameterFitted() / 2.0f);
    float right =  ( static_cast<float>(light->getBodyPosition().x()) * viewScale() ) + (light->getLightDiameterFitted() / 2.0f);
    float top =    ( static_cast<float>(light->getBodyPosition().y()) * viewScale() ) + (light->getLightDiameterFitted() / 2.0f);
    float bottom = ( static_cast<float>(light->getBodyPosition().y()) * viewScale() ) - (light->getLightDiameterFitted() / 2.0f);
    QVector<GLfloat> vertices;
    setVertexFromSides(vertices, left, right, top, bottom);
    m_light_shader.setAttributeArray(    m_attribute_light_vertex, vertices.data(), 3 );
    m_light_shader.enableAttributeArray( m_attribute_light_vertex );

    // Use texture unit 0
    m_light_shader.setUniformValue( m_uniform_light_texture, 0 );

    // Find out if light texture has been reduced to fit in the screen, if so increase brightness of light as we get closer
    float shrink_multiplier = 1.0f;
    if (light->getLightDiameterFitted() < light->getLightDiameter()) {
        shrink_multiplier = static_cast<float>( qSqrt(double(light->getLightDiameter()) / double(light->getLightDiameterFitted())) );
    }

    // Give shader the light_size diameter, fitted diameter, color
    m_light_shader.setUniformValue( m_uniform_light_diameter, static_cast<float>(light->getLightDiameter()) );
    m_light_shader.setUniformValue( m_uniform_light_fitted,   static_cast<float>(light->getLightDiameterFitted()) );
    m_light_shader.setUniformValue( m_uniform_light_shrink,   shrink_multiplier );
    m_light_shader.setUniformValue( m_uniform_light_color,
                                    static_cast<float>(light->color.redF()),
                                    static_cast<float>(light->color.greenF()),
                                    static_cast<float>(light->color.blueF()) );

    // Give shader the light cone start and end angles
    float cone_1 = qDegreesToRadians(static_cast<float>(light->cone.x()));
    float cone_2 = qDegreesToRadians(static_cast<float>(light->cone.y()));
    if (cone_1 < 0.0f) cone_1 += (2.0f * 3.141592f);
    if (cone_2 < 0.0f) cone_2 += (2.0f * 3.141592f);
    m_light_shader.setUniformValue( m_uniform_light_cone, cone_1, cone_2 );

    // Give shader shadow visibility, light intensity, blur values, whether or not to draw shadows
    m_light_shader.setUniformValue( m_uniform_light_shadows,        light->shadows );
    m_light_shader.setUniformValue( m_uniform_light_intensity,      light->intensity );
    m_light_shader.setUniformValue( m_uniform_light_blur,           light->blur );
    m_light_shader.setUniformValue( m_uniform_light_draw_shadows,   light->draw_shadows );

    // Draw triangles using shader program
    glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

    // Disable arrays
    m_light_shader.disableAttributeArray( m_attribute_light_vertex );
    m_light_shader.disableAttributeArray( m_attribute_light_tex_coord );

    // Release Shader
    m_light_shader.release();
}
























