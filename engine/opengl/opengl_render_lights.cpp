//
//      Created by Stephens Nunnally on 6/17/2019, (c) 2019 Scidian Studios, All Rights Reserved
//
//  File:
//
//
//
#include <QOpenGLFramebufferObject>
#include "editor/helper_library.h"

#include "engine/engine.h"
#include "engine/engine_texture.h"
#include "engine/form_engine.h"
#include "engine/opengl/opengl.h"
#include "engine/thing/components_effects/thing_comp_light.h"
#include "engine/thing/engine_thing.h"
#include "engine/world/engine_world.h"


//####################################################################################
//##    Main Shadow Map / Occluder / Render Routine
//####################################################################################
void DrOpenGL::process2DLights() {

    // ***** Check for lights, update buffers
    if (m_engine->getCurrentWorld()->light_count > 0) {
        checkLightBuffers();                                                    // Clear list of which lights exist
        m_engine->getCurrentWorld()->light_count = findNeededShadowMaps();      // Check for lights with shadows, if none we don't need to draw occluder map
    }

    // ***** Process shadow casting lights if there are some
    if (m_shadow_lights.size() > 0) {
        // Render all Space Objects to a singular off-screen Frame Buffer Object Occluder Map
        bindOccluderMapBuffer();
        bool shader_initialized = false;
        for (auto thing : m_engine->getCurrentWorld()->getThings()) {
            if (!shader_initialized) {
                if (drawObjectOccluder(thing, true)) shader_initialized = true;
            } else {
                drawObjectOccluder(thing, false);
            }
        }
        m_occluder_fbo->release();

        // Code to have the Occluder Map fbo pop up so we can take a look
        /**
        static int count = 0;
        count++;
        if (count % 500 == 0) {
            Dr::ShowMessageBox("fbo", QPixmap::fromImage( m_occluder_fbo->toImage() ).scaled(512, 512) );
            count = 0;
        }
        */

        // Calculate Light 1D Shadow Maps
        drawShadowMaps();
    }

    // ***** Draw Glow Lights
    drawGlowLights();
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
//##    Check for lights with shadows, if there are none we don't need to draw occluder map
//####################################################################################
int DrOpenGL::findNeededShadowMaps() {
    int light_count = 0;
    m_shadow_lights.clear();
    m_glow_lights.clear();
    for (auto thing : m_engine->getCurrentWorld()->getThings()) {
        if (thing == nullptr) continue;

        // Get Light component of DrEngingThing
        DrThingComponent *component = thing->component(Comps::Thing_Settings_Light);        if (component == nullptr) continue;
        ThingCompLight   *light_settings = dynamic_cast<ThingCompLight*>(component);        if (light_settings == nullptr) continue;
        if (light_settings->light_type == Light_Type::Glow) m_glow_lights.push_back(thing);
        light_count++;

        // Calculate size of light texture (fbo)
        light_settings->setLightDiameter( abs(static_cast<int>(light_settings->light_size)) );
        light_settings->setLightDiameterFitted( (light_settings->getLightDiameter() > g_max_light_fbo_size) ? g_max_light_fbo_size : light_settings->getLightDiameter() );

        // In perspective mode we still draw the light the same size as z-order 0 even if its far away,
        // this allows for cool large lights (like a big sun) far away. This calculates the size difference for rendering later
        double light_radius = (light_settings->getLightDiameterFitted() / 2.0);
        if (m_engine->getCurrentWorld()->render_type == Render_Type::Perspective && m_occluder_fbo) {
            QMatrix4x4 view_matrix, proj_matrix;
            occluderMatrix(Render_Type::Perspective, view_matrix, proj_matrix);

            glm::vec3 point_0_a = glm::vec3(static_cast<float>(thing->getPosition().x), 0.0f, 0.0f);
            glm::vec3 point_0_b = glm::vec3(static_cast<float>(thing->getPosition().x + (light_settings->getLightDiameterFitted()/2.0)), 0.0f, 0.0f);

            float z_pos = static_cast<float>(thing->getZOrder());
            glm::vec3 point_z_a = glm::vec3(static_cast<float>(thing->getPosition().x), 0.0f, z_pos);
            glm::vec3 point_z_b = glm::vec3(static_cast<float>(thing->getPosition().x + (light_settings->getLightDiameterFitted()/2.0)), 0.0f, z_pos);

            DrPointF map_0_a = mapToFBO( point_0_a, m_occluder_fbo, view_matrix, proj_matrix );
            DrPointF map_0_b = mapToFBO( point_0_b, m_occluder_fbo, view_matrix, proj_matrix );
            DrPointF map_z_a = mapToFBO( point_z_a, m_occluder_fbo, view_matrix, proj_matrix );
            DrPointF map_z_b = mapToFBO( point_z_b, m_occluder_fbo, view_matrix, proj_matrix );

            double map_0 = (map_0_a.x > map_0_b.x) ? (map_0_a.x - map_0_b.x) : (map_0_b.x - map_0_a.x);
            double map_z = (map_z_a.x > map_z_b.x) ? (map_z_a.x - map_z_b.x) : (map_z_b.x - map_z_a.x);
            light_settings->setPerspectiveScale( float(map_z / map_0) );
            light_radius /= static_cast<double>(light_settings->getPerspectiveScale() + 0.0001f);
        }

        // Check if light is in view to be rendered
        DrPoint top_left =  mapToScreen(thing->getPosition().x - light_radius, thing->getPosition().y + light_radius, thing->getZOrder() ).toPoint();
        DrPoint top_right = mapToScreen(thing->getPosition().x + light_radius, thing->getPosition().y + light_radius, thing->getZOrder() ).toPoint();
        DrPoint bot_left =  mapToScreen(thing->getPosition().x - light_radius, thing->getPosition().y - light_radius, thing->getZOrder() ).toPoint();
        DrPoint bot_right = mapToScreen(thing->getPosition().x + light_radius, thing->getPosition().y - light_radius, thing->getZOrder() ).toPoint();
        QPolygon light_box;
        light_box << Dr::ToQPoint(top_left) << Dr::ToQPoint(top_right) << Dr::ToQPoint(bot_left) << Dr::ToQPoint(bot_right);
        QRect in_view = QRect(0, 0, width()*devicePixelRatio(), height()*devicePixelRatio());
        light_settings->setIsInView( light_box.boundingRect().intersects(in_view) ||
                                     light_box.boundingRect().contains(in_view)   ||
                                     in_view.contains(light_box.boundingRect()) );

        // If light needs shadows and is visible, add to shadow processing list
        if (light_settings->draw_shadows && light_settings->isInView())
            m_shadow_lights.push_back(thing);
    }
    return light_count;
}


//####################################################################################
//##    Calculate Light 1D Shadow Maps
//####################################################################################
void DrOpenGL::drawShadowMaps() {
    for (auto light : m_shadow_lights) {
        // Get Light component of DrEngingThing
        DrThingComponent *component = light->component(Comps::Thing_Settings_Light);        if (component == nullptr) continue;
        ThingCompLight   *light_settings = dynamic_cast<ThingCompLight*>(component);        if (light_settings == nullptr) continue;

        // Calculate light position on Occluder Map
        QMatrix4x4 view_matrix, proj_matrix;
        occluderMatrix(m_engine->getCurrentWorld()->render_type, view_matrix, proj_matrix);
        glm::vec3 light_position = glm::vec3(static_cast<float>(light->getPosition().x), static_cast<float>(light->getPosition().y), static_cast<float>(light->getZOrder()));
        light_settings->setScreenPos( mapToFBO( light_position, m_occluder_fbo, view_matrix, proj_matrix) );
        double middle = m_texture_fbo->height() / 2.0;
        double y_diff = middle - light_settings->getScreenPos().y;

        // Adjust scale for Perspective Mode lights
        double o_scale;
        if (m_engine->getCurrentWorld()->render_type == Render_Type::Orthographic) {
            o_scale = static_cast<double>(c_occluder_scale_ortho * combinedZoomScale());
        } else {
            float screen_scale;
            screen_scale = (m_occluder_fbo->height() / (800.0f * devicePixelRatio())) * 1.00f;          // field of view = 52.5 (in use)
            ///screen_scale = (m_occluder_fbo->height() / (800.0f * devicePixelRatio())) * 0.45f;       // field of view = 95
            ///screen_scale = (m_occluder_fbo->height() / (800.0f * devicePixelRatio())) * 0.70f;       // field of view = 70
            ///screen_scale = (m_occluder_fbo->height() / (800.0f * devicePixelRatio())) * 1.00f;       // field of view = 52.5
            ///screen_scale = (m_occluder_fbo->height() / (800.0f * devicePixelRatio())) * 1.20f;       // field of view = 45
            o_scale = static_cast<double>(c_occluder_scale_proj * combinedZoomScale() * screen_scale);
        }

        // Blit the area of the occluder map the light will take up to the Light Occluder FBO
        bindLightOcculderBuffer(light);
        QOpenGLFramebufferObject *light_fbo = m_occluders[light->getKey()];
        QOpenGLFramebufferObject::blitFramebuffer(
                    light_fbo, QRect(0, 0, light_fbo->width(), light_fbo->height()),
                    m_occluder_fbo, QRect(
                        static_cast<int>( floor(light_settings->getScreenPos().x - ((light_fbo->width() / 2.0) * o_scale)) ),
                        static_cast<int>( floor(light_settings->getScreenPos().y - ((light_fbo->height()/ 2.0) * o_scale) + (y_diff * 2.0)) ),
                        static_cast<int>( light_fbo->width()  * o_scale ),
                        static_cast<int>( light_fbo->height() * o_scale )),
                    GL_COLOR_BUFFER_BIT, GL_NEAREST);
        m_occluders[light->getKey()]->release();

        // Draw the 1D Shadow Map from the Occluder Map
        bindLightShadowBuffer(light);
        draw1DShadowMap(light);
        m_shadows[light->getKey()]->release();
    }
}

//####################################################################################
//##    Renders the 1D Shadow Map based on the Occluder Map (For Each Light)
//####################################################################################
void DrOpenGL::draw1DShadowMap(DrEngineThing *light) {
    // Get Light component of DrEngingThing
    DrThingComponent *component = light->component(Comps::Thing_Settings_Light);        if (component == nullptr) return;
    ThingCompLight   *light_settings = dynamic_cast<ThingCompLight*>(component);        if (light_settings == nullptr) return;

    // Bind Shadow Shader
    if (!m_shadow_shader.bind()) return;

    // Bind Light Occluder as a texture
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_occluders[light->getKey()]->texture());

    // Disable blending
    glDisable(GL_BLEND);

    // Reset our projection matrix to the FBO size
    m_shadow_shader.setUniformValue( u_shadow_matrix, orthoMatrix(m_shadows[light->getKey()]->width(), m_shadows[light->getKey()]->height()) );

    // Set Texture Coordinates for Shader
    m_shadow_shader.setAttributeArray(    a_shadow_texture_coord, m_quad_texture_coordinates.data(), 2 );
    m_shadow_shader.enableAttributeArray( a_shadow_texture_coord );

    // Load vertices for this object
    std::vector<GLfloat> vertices;
    setQuadVertices(vertices, m_shadows[light->getKey()]->width(), m_shadows[light->getKey()]->height(), DrPointF(0, 0), 0.0f);
    m_shadow_shader.setAttributeArray(    a_shadow_vertex, vertices.data(), 3 );
    m_shadow_shader.enableAttributeArray( a_shadow_vertex );

    // Use texture unit 0
    m_shadow_shader.setUniformValue( u_shadow_texture, 0 );

    // ***** Give the shader our Ray Count and Scaled Light Radius
    float screen_scale = width()*devicePixelRatio() / abs(light_settings->light_size);
    if (screen_scale > 1.0f) screen_scale = 1.0f;
    m_shadow_shader.setUniformValue( u_shadow_resolution, light_settings->getLightDiameter(), light_settings->getLightDiameter() * screen_scale);
    m_shadow_shader.setUniformValue( u_shadow_ray_count,  static_cast<float>(m_shadows[light->getKey()]->width()) );
    m_shadow_shader.setUniformValue( u_shadow_depth,      static_cast<float>(light->getZOrder()) );
    m_shadow_shader.setUniformValue( u_shadow_near_plane, c_near_plane );

    // Draw triangles using shader program
    glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

    // Disable arrays
    m_shadow_shader.disableAttributeArray( a_shadow_vertex );
    m_shadow_shader.disableAttributeArray( a_shadow_texture_coord );

    // Release Shader
    m_shadow_shader.release();
}


//####################################################################################
//##    Renders Glow Lights on Glow fbo
//####################################################################################
void DrOpenGL::drawGlowLights() {
    Blend_Mode mode = m_engine->getCurrentWorld()->getGlowBlendMode();

    // If no lights and ambient light is 50% (normal) exit now, otherwise bind Glow Light Buffer
    double ambient_light = m_engine->getCurrentWorld()->getAmbientLight() / 100.0;
    if (m_glow_lights.size() <= 0 && Dr::IsCloseTo(0.5, ambient_light, .001) && mode == Blend_Mode::Standard) return;
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
//##    Renders the light using the Shadow Map
//##        - Returns true if rendered, false if not
//####################################################################################
bool DrOpenGL::draw2DLight(DrEngineThing *light) {
    // Get Light component of DrEngingThing
    DrThingComponent *component = light->component(Comps::Thing_Settings_Light);        if (component == nullptr) return false;
    ThingCompLight   *light_settings = dynamic_cast<ThingCompLight*>(component);        if (light_settings == nullptr) return false;

    // Bind Light Shader
    if (!m_light_shader.bind()) return false;

    // If drawing Shadows, Bind Shadow Map as a texture
    if (light_settings->draw_shadows) {
        if (m_shadows[light->getKey()] == nullptr) return false;
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, m_shadows[light->getKey()]->texture());
    }

    // Set Matrix for to draw the Light
    QMatrix4x4 matrix = m_projection * m_view;
    m_light_shader.setUniformValue( u_light_matrix, matrix );


    // Set Texture Coordinates for Shader
    m_light_shader.setAttributeArray(    a_light_texture_coord, m_quad_texture_coordinates.data(), 2 );
    m_light_shader.enableAttributeArray( a_light_texture_coord );

    // Load vertices for this object
    float perspective_scale = (m_engine->getCurrentWorld()->render_type == Render_Type::Orthographic) ? 1.0 : (light_settings->getPerspectiveScale() + 0.0001f);

    float quad_width =  (light_settings->getLightDiameterFitted()/perspective_scale);
    float quad_height = (light_settings->getLightDiameterFitted()/perspective_scale);
    std::vector<GLfloat> vertices;
    setQuadVertices(vertices, quad_width, quad_height, DrPointF(light->getPosition().x, light->getPosition().y), static_cast<float>(light->getZOrder()));
    m_light_shader.setAttributeArray(    a_light_vertex, vertices.data(), 3 );
    m_light_shader.enableAttributeArray( a_light_vertex );

    // Use texture unit 0
    m_light_shader.setUniformValue( u_light_texture, 0 );

    // Give the shader our Ray Count (Shadow Map width)
    float shadow_width = (light_settings->draw_shadows) ? m_shadows[light->getKey()]->width() : 0.0f;
    m_light_shader.setUniformValue( u_light_ray_count,      static_cast<float>(shadow_width) );

    // Give shader the light_size diameter, fitted diameter
    m_light_shader.setUniformValue( u_light_diameter,       static_cast<float>(light_settings->getLightDiameter()) );
    m_light_shader.setUniformValue( u_light_fitted,         static_cast<float>(light_settings->getLightDiameterFitted()) );

    // Give shader the opacity, light color
    m_light_shader.setUniformValue( u_light_alpha,          light->getOpacity() );
    m_light_shader.setUniformValue( u_light_color,          static_cast<float>(light_settings->color.redF()),
                                                            static_cast<float>(light_settings->color.greenF()),
                                                            static_cast<float>(light_settings->color.blueF()) );

    // Give shader the light cone start and end angles
    float cone_1 = Dr::DegreesToRadians(static_cast<float>(light_settings->getRotatedCone().x));
    float cone_2 = Dr::DegreesToRadians(static_cast<float>(light_settings->getRotatedCone().y));
    if (cone_1 < 0.0f) cone_1 += (2.0f * 3.141592f);
    if (cone_2 < 0.0f) cone_2 += (2.0f * 3.141592f);
    m_light_shader.setUniformValue( u_light_cone, cone_1, cone_2 );

    // Give shader shadow visibility, reduced fade, blur values, whether or not to draw shadows
    m_light_shader.setUniformValue( u_light_intensity,      light_settings->intensity );
    m_light_shader.setUniformValue( u_light_shadows,        light_settings->shadows );
    m_light_shader.setUniformValue( u_light_blur,           light_settings->blur );
    m_light_shader.setUniformValue( u_light_draw_shadows,   light_settings->draw_shadows );
    m_light_shader.setUniformValue( u_light_is_glow,       (light_settings->light_type == Light_Type::Glow));

    // Draw triangles using shader program
    glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

    // Disable arrays
    m_light_shader.disableAttributeArray( a_light_vertex );
    m_light_shader.disableAttributeArray( a_light_texture_coord );

    // Release Shader
    m_light_shader.release();
    return true;
}
























