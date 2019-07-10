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
#include "engine/engine_thing_light.h"
#include "engine/engine_thing_object.h"
#include "engine/engine_texture.h"
#include "engine/engine_world.h"
#include "engine/form_engine.h"
#include "helper.h"
#include "opengl/opengl.h"


//####################################################################################
//##        Main Shadow Map / Occluder / Render Routine
//####################################################################################
void DrOpenGL::drawShadowMaps() {
    if (m_engine->getCurrentWorld()->light_count <= 0) return;

    // ***** Clear list of which lights exist
    checkLightBuffers();

    // ***** Check for lights with shadows, if there are none we don't need to draw occluder map
    int light_count = 0;
    m_shadow_lights.clear();
    m_glow_lights.clear();
    for (auto thing : m_engine->getCurrentWorld()->getThings()) {
        if (thing == nullptr) continue;
        if (thing->getThingType() != DrThingType::Light) continue;

        DrEngineLight *light = dynamic_cast<DrEngineLight*>(thing);
        if (light->light_type == Light_Type::Glow) m_glow_lights.append(light);
        light_count++;

        // Calculate size of light texture (fbo)
        light->setLightDiameter( static_cast<int>(light->light_size) );
        light->setLightDiameterFitted( (light->getLightDiameter() > g_max_light_fbo_size) ? g_max_light_fbo_size : light->getLightDiameter() );

        // In perspective mode we still draw the light the same size as z-order 0 even if its far away, this allows for cool large lights (like a big sun)
        //      far away. This calculates the size difference for rendering later
        double light_radius = (light->getLightDiameterFitted() / 2.0);
        if (m_engine->getCurrentWorld()->render_type == Render_Type::Perspective && m_occluder_fbo) {
            QPointF map_0_a = mapToFBO( QVector3D(static_cast<float>(light->getPosition().x()), 0.0f, 0.0f),
                                          m_occluder_fbo, occluderMatrix(Render_Type::Perspective, false) );
            QPointF map_0_b = mapToFBO( QVector3D(static_cast<float>(light->getPosition().x() + (light->getLightDiameterFitted()/2.0)), 0.0f, 0.0f),
                                          m_occluder_fbo, occluderMatrix(Render_Type::Perspective, false) );
            QPointF map_z_a = mapToFBO( QVector3D(static_cast<float>(light->getPosition().x()), 0.0f, static_cast<float>(light->z_order)),
                                          m_occluder_fbo, occluderMatrix(Render_Type::Perspective, false) );
            QPointF map_z_b = mapToFBO( QVector3D(static_cast<float>(light->getPosition().x() + (light->getLightDiameterFitted()/2.0)), 0.0f,
                                                  static_cast<float>(light->z_order)),
                                          m_occluder_fbo, occluderMatrix(Render_Type::Perspective, false) );
            double map_0 = (map_0_a.x() > map_0_b.x()) ? (map_0_a.x() - map_0_b.x()) : (map_0_b.x() - map_0_a.x());
            double map_z = (map_z_a.x() > map_z_b.x()) ? (map_z_a.x() - map_z_b.x()) : (map_z_b.x() - map_z_a.x());
            light->setPerspectiveScale( float(map_z / map_0) );
            light_radius /= static_cast<double>(light->getPerspectiveScale() + 0.0001f);
        }

        // Check if light is in view to be rendered
        QPoint top_left =  mapToScreen(light->getPosition().x() - light_radius, light->getPosition().y() + light_radius, light->z_order ).toPoint();
        QPoint top_right = mapToScreen(light->getPosition().x() + light_radius, light->getPosition().y() + light_radius, light->z_order ).toPoint();
        QPoint bot_left =  mapToScreen(light->getPosition().x() - light_radius, light->getPosition().y() - light_radius, light->z_order ).toPoint();
        QPoint bot_right = mapToScreen(light->getPosition().x() + light_radius, light->getPosition().y() - light_radius, light->z_order ).toPoint();
        QPolygon light_box; light_box << top_left << top_right << bot_left << bot_right;
        QRect in_view = QRect(0, 0, width()*devicePixelRatio(), height()*devicePixelRatio());
        light->setIsInView( light_box.boundingRect().intersects(in_view) || light_box.boundingRect().contains(in_view) ||
                            in_view.contains(light_box.boundingRect()) );

        // If light needs shadows and is visible, add to shadow processing list
        if (light->draw_shadows && light->isInView())
            m_shadow_lights.append(light);
    }
    m_engine->getCurrentWorld()->light_count = light_count;
    if (m_shadow_lights.count() <= 0) return;

    // ***** Render all Space Objects to an off-screen Frame Buffer Object Occluder Map
    bindOccluderMapBuffer();
    glViewport(0, 0, m_occluder_fbo->width(), m_occluder_fbo->height());
    drawSpaceOccluder();
    m_occluder_fbo->release();
    glViewport(0, 0, width()*devicePixelRatio(), height()*devicePixelRatio());

    // ***** Code to have the Occluder Map fbo pop up so we can take a look
    ///static int count = 0;
    ///count++;
    ///if (count % 500 == 0) {
    ///    Dr::ShowMessageBox("fbo", QPixmap::fromImage( m_occluder_fbo->toImage() ).scaled(512, 512) );
    ///    count = 0;
    ///}

    // ***** Calculate Light 1D Shadow Maps
    for (auto light : m_shadow_lights) {
        // Calculate light position on Occluder Map
        light->setScreenPos( mapToFBO( QVector3D(static_cast<float>(light->getPosition().x()), static_cast<float>(light->getPosition().y()),
                                                 static_cast<float>(light->z_order)),
                                       m_occluder_fbo, occluderMatrix(m_engine->getCurrentWorld()->render_type, false)) );
        double middle = m_texture_fbo->height() / 2.0;
        double y_diff = middle - light->getScreenPos().y();

        // Adjust scale for Perspective Mode lights
        double o_scale;
        if (m_engine->getCurrentWorld()->render_type == Render_Type::Orthographic) {
            o_scale = static_cast<double>(c_occluder_scale_ortho * m_scale);
        } else {
            float screen_scale = 1.0f;
            screen_scale = (m_occluder_fbo->height() / 800.0f) * 1.00f;         // field of view = 52.5 (in use)
            ///screen_scale = (m_occluder_fbo->height() / 800.0f) * 0.45f;      // field of view = 95
            ///screen_scale = (m_occluder_fbo->height() / 800.0f) * 0.70f;      // field of view = 70
            ///screen_scale = (m_occluder_fbo->height() / 800.0f) * 1.00f;      // field of view = 52.5
            ///screen_scale = (m_occluder_fbo->height() / 800.0f) * 1.20f;      // field of view = 45
            o_scale = static_cast<double>(c_occluder_scale_proj * m_scale * screen_scale);
        }

        // Blit the area of the occluder map the light will take up to the Light Occluder FBO
        bindLightOcculderBuffer(light);
        QOpenGLFramebufferObject *light_fbo = m_occluders[light->getKey()];
        QOpenGLFramebufferObject::blitFramebuffer(
                    light_fbo, QRect(0, 0, light_fbo->width(), light_fbo->height()),
                    m_occluder_fbo, QRect(
                        static_cast<int>( floor(light->getScreenPos().x() - ((light_fbo->width() / 2.0) * o_scale)) ),
                        static_cast<int>( floor(light->getScreenPos().y() - ((light_fbo->height()/ 2.0) * o_scale) + (y_diff * 2.0)) ),
                        static_cast<int>( light_fbo->width()  * o_scale ),
                        static_cast<int>( light_fbo->height() * o_scale )),
                    GL_COLOR_BUFFER_BIT, GL_NEAREST);
        m_occluders[light->getKey()]->release();

        // Draw the 1D Shadow Map from the Occluder Map
        bindLightShadowBuffer(light);
        draw1DShadowMap(light);
        m_shadows[light->getKey()]->release();
    } // End For
}


//####################################################################################
//##        Renders Glow Lights on Glow fbo
//####################################################################################
void DrOpenGL::drawGlowLights() {
    // Check that we should draw Glow Light Buffer, if so bind it
    double ambient_light = m_engine->getCurrentWorld()->getAmbientLight() / 100.0;
    if (m_glow_lights.count() <= 0 && Dr::IsCloseTo(1.0, ambient_light, .001)) return;
    bindGlowLightsBuffer(static_cast<float>(ambient_light));

    // Standard blend function
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Draw Glow Lights onto Glow FBO
    for (auto light : m_glow_lights) {
        draw2DLight(light);
    }
    m_glow_fbo->release();
}


//####################################################################################
//##    Check that lights still exist, if not delete buffers
//####################################################################################
void DrOpenGL::checkLightBuffers() {
    // Go through deleted keys
    for (auto key : m_engine->getCurrentWorld()->mark_light_as_deleted) {
        // Delete occluder fbo
        for (auto it = m_occluders.begin(); it != m_occluders.end(); ) {
            if ((*it).first == key) {
                delete (*it).second;
                it = m_occluders.erase(it);
                continue;
            }
            it++;
        }
        // Delete shadow fbo
        for (auto it = m_shadows.begin(); it != m_shadows.end(); ) {
            if ((*it).first == key) {
                delete (*it).second;
                it = m_shadows.erase(it);
                continue;
            }
            it++;
        }
    }
    m_engine->getCurrentWorld()->mark_light_as_deleted.clear();
}

//####################################################################################
//##        Allocate Glow Light FBO - To Render All Glow Lights at Once
//####################################################################################
void DrOpenGL::bindGlowLightsBuffer(float ambient_light) {
    // Check that off screen buffers are initialized
    if (!m_glow_fbo || (m_glow_fbo->width() != width()*devicePixelRatio() || m_glow_fbo->height() != height()*devicePixelRatio())) {
        delete m_glow_fbo;
        QOpenGLFramebufferObjectFormat format2;
        format2.setAttachment(QOpenGLFramebufferObject::Attachment::NoAttachment);
        m_glow_fbo = new QOpenGLFramebufferObject(width() * devicePixelRatio(), height() * devicePixelRatio(), format2);
    }
    m_glow_fbo->bind();

    // Clear the buffer
    glClearColor(ambient_light, ambient_light, ambient_light, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

//####################################################################################
//##        Allocate Occluder Map (For Whole Scene)
//####################################################################################
void DrOpenGL::bindOccluderMapBuffer() {
    int desired_x, desired_y;

    // Orthographic we can use a larger buffer size
    if (m_engine->getCurrentWorld()->render_type == Render_Type::Orthographic) {
        desired_x = g_max_occluder_fbo_size;
        desired_y = g_max_occluder_fbo_size;

    // Perspective uses same size as off screen buffer to match view angles
    } else {
        desired_x = width()* devicePixelRatio();
        desired_y = height()*devicePixelRatio();
    }

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
//##        Allocate Light Occluder Frame Buffer Object (For Each Light)
//####################################################################################
void DrOpenGL::bindLightOcculderBuffer(DrEngineLight *light) {
    // Check Frame Buffer Object is initialized
    bool need_to_create_new = false;
    if (!(m_occluders[light->getKey()]))
        need_to_create_new = true;
    else if (m_occluders[light->getKey()]->width() != light->getLightDiameterFitted() || m_occluders[light->getKey()]->height() != light->getLightDiameterFitted())
        need_to_create_new = true;

    // Initialize light fbo
    if (need_to_create_new) {
        delete m_occluders[light->getKey()];
        m_occluders[light->getKey()] = new QOpenGLFramebufferObject(light->getLightDiameterFitted(), light->getLightDiameterFitted());
    }

    // Bind and clear buffer
    m_occluders[light->getKey()]->bind();
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}


//####################################################################################
//##        Allocate Shadow Frame Buffer Object
//####################################################################################
void DrOpenGL::bindLightShadowBuffer(DrEngineLight *light) {
    // Shadow map size is the smallest of c_angles, light_radius_fitted, and width()
    int shadow_size = (light->getLightDiameterFitted() < g_max_rays) ? light->getLightDiameterFitted() : g_max_rays;
        shadow_size = (width()*devicePixelRatio() < shadow_size) ? width()*devicePixelRatio() : shadow_size;

    // Check Frame Buffer Object is initialized
    bool need_to_create_new = false;
    if (m_shadows[light->getKey()] == nullptr)
        need_to_create_new = true;
    else if (m_shadows[light->getKey()]->width() != shadow_size)
        need_to_create_new = true;

    // Initialize shadow fbo
    if (need_to_create_new) {
        delete m_shadows[light->getKey()];
        m_shadows[light->getKey()] = new QOpenGLFramebufferObject(shadow_size, 1);
    }

    // Bind and clear buffer
    m_shadows[light->getKey()]->bind();
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}


//####################################################################################
//##        Renders the 1D Shadow Map based on the Occluder Map (For Each Light)
//####################################################################################
void DrOpenGL::draw1DShadowMap(DrEngineLight *light) {
    // Bind Shadow Shader
    if (!m_shadow_shader.bind()) return;

    // Bind Light Occluder as a texture
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_occluders[light->getKey()]->texture());

    // Disable blending
    glDisable(GL_BLEND);

    // Reset our projection matrix to the FBO size
    float left =   0.0f - ((m_shadows[light->getKey()]->width() )  / 2.0f);
    float right =  0.0f + ((m_shadows[light->getKey()]->width() )  / 2.0f);
    float top =    0.0f + ((m_shadows[light->getKey()]->height() ) / 2.0f);
    float bottom = 0.0f - ((m_shadows[light->getKey()]->height() ) / 2.0f);
    QMatrix4x4 m_matrix;
    m_matrix.ortho( left, right, bottom, top, -5000.0f, 5000.0f);
    m_shadow_shader.setUniformValue( m_uniform_shadow_matrix, m_matrix );

    // Set Texture Coordinates for Shader
    std::vector<float> texture_coordinates;
    setWholeTextureCoordinates(texture_coordinates);
    m_shadow_shader.setAttributeArray(    m_attribute_shadow_tex_coord, texture_coordinates.data(), 2 );
    m_shadow_shader.enableAttributeArray( m_attribute_shadow_tex_coord );

    // Load vertices for this object
    QVector<GLfloat> vertices;
    setVertexFromSides(vertices, left, right, top, bottom, 0.0f);
    m_shadow_shader.setAttributeArray(    m_attribute_shadow_vertex, vertices.data(), 3 );
    m_shadow_shader.enableAttributeArray( m_attribute_shadow_vertex );

    // Use texture unit 0
    m_shadow_shader.setUniformValue( m_uniform_shadow_texture, 0 );

    // ***** Give the shader our Ray Count and Scaled Light Radius
    m_shadow_shader.setUniformValue( m_uniform_shadow_ray_count,  static_cast<float>(m_shadows[light->getKey()]->width()) );

    float screen_scale = width()*devicePixelRatio() / light->light_size;
    m_shadow_shader.setUniformValue( m_uniform_shadow_resolution, light->getLightDiameter(), light->getLightDiameter() * screen_scale );
    m_shadow_shader.setUniformValue( m_uniform_shadow_depth,      static_cast<float>(light->z_order) );

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
void DrOpenGL::draw2DLight(DrEngineLight *light) {
    // Bind Light Shader
    if (!m_light_shader.bind()) return;

    // If drawing Shadows, Bind Shadow Map as a texture
    if (light->draw_shadows) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, m_shadows[light->getKey()]->texture());
    }

    // Set Matrix for to draw the Light, turn off perspective offset before drawing light
    updateViewMatrix(m_engine->getCurrentWorld()->render_type, false);
    QMatrix4x4 m_matrix = m_projection * m_model_view;
    m_light_shader.setUniformValue( m_uniform_light_matrix, m_matrix );
    updateViewMatrix(m_engine->getCurrentWorld()->render_type, c_use_cam_offset);

    // Set Texture Coordinates for Shader
    std::vector<float> texture_coordinates;
    setWholeTextureCoordinates(texture_coordinates);
    m_light_shader.setAttributeArray(    m_attribute_light_tex_coord, texture_coordinates.data(), 2 );
    m_light_shader.enableAttributeArray( m_attribute_light_tex_coord );

    // Load vertices for this object
    float perspective_scale = (m_engine->getCurrentWorld()->render_type == Render_Type::Orthographic) ? 1.0 : (light->getPerspectiveScale() + 0.0001f);
    float left =   static_cast<float>(light->getPosition().x()) - (light->getLightDiameterFitted()/perspective_scale / 2.0f);
    float right =  static_cast<float>(light->getPosition().x()) + (light->getLightDiameterFitted()/perspective_scale / 2.0f);
    float top =    static_cast<float>(light->getPosition().y()) + (light->getLightDiameterFitted()/perspective_scale / 2.0f);
    float bottom = static_cast<float>(light->getPosition().y()) - (light->getLightDiameterFitted()/perspective_scale / 2.0f);
    QVector<GLfloat> vertices;

    setVertexFromSides(vertices, left, right, top, bottom, static_cast<float>(light->z_order));
    m_light_shader.setAttributeArray(    m_attribute_light_vertex, vertices.data(), 3 );
    m_light_shader.enableAttributeArray( m_attribute_light_vertex );

    // Use texture unit 0
    m_light_shader.setUniformValue( m_uniform_light_texture, 0 );

    // Give the shader our Ray Count (Shadow Map width)
    m_light_shader.setUniformValue( m_uniform_light_ray_count,      static_cast<float>(m_shadows[light->getKey()]->width()) );

    // Give shader the light_size diameter, fitted diameter
    m_light_shader.setUniformValue( m_uniform_light_diameter,       static_cast<float>(light->getLightDiameter()) );
    m_light_shader.setUniformValue( m_uniform_light_fitted,         static_cast<float>(light->getLightDiameterFitted()) );

    // Give shader the opacity, light color
    m_light_shader.setUniformValue( m_uniform_light_alpha,          light->getOpacity() );
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

    // Give shader shadow visibility, reduced fade, blur values, whether or not to draw shadows
    m_light_shader.setUniformValue( m_uniform_light_intensity,      light->intensity );
    m_light_shader.setUniformValue( m_uniform_light_shadows,        light->shadows );
    m_light_shader.setUniformValue( m_uniform_light_blur,           light->blur );
    m_light_shader.setUniformValue( m_uniform_light_draw_shadows,   light->draw_shadows );
    m_light_shader.setUniformValue( m_uniform_light_is_glow,        (light->light_type == Light_Type::Glow));

    // Draw triangles using shader program
    glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

    // Disable arrays
    m_light_shader.disableAttributeArray( m_attribute_light_vertex );
    m_light_shader.disableAttributeArray( m_attribute_light_tex_coord );

    // Release Shader
    m_light_shader.release();
}
























