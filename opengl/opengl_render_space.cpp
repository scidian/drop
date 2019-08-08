//
//      Created by Stephens Nunnally on 6/9/2019, (c) 2019 Scidian Software, All Rights Reserved
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
#include "engine/engine_thing_water.h"
#include "engine/engine_thing_object.h"
#include "engine/engine_texture.h"
#include "engine/engine_world.h"
#include "engine/form_engine.h"
#include "enums_engine.h"
#include "helper.h"
#include "opengl/opengl.h"


//####################################################################################
//##        Update the view matrices before rendering
//####################################################################################
void DrOpenGL::updateViewMatrix(Render_Type render_type, bool use_offset) {
    //          Axis:
    //              -X left,        +X right
    //              -Y down,        +Y up
    //              -Z back,        +Z front (close to camera)
    float aspect_ratio = static_cast<float>(width()) / static_cast<float>(height());

    // Orthographic
    m_model_view.setToIdentity();
    m_projection.setToIdentity();
    if (render_type == Render_Type::Orthographic) {
        float cam_x =  (m_engine->getCurrentWorld()->getCameraPos().x()) * m_scale;
        float cam_y =  (m_engine->getCurrentWorld()->getCameraPos().y() + 100) * m_scale;
        float left =   cam_x - (width() *  devicePixelRatio() / 2.0f);
        float right =  cam_x + (width() *  devicePixelRatio() / 2.0f);
        float top =    cam_y + (height() * devicePixelRatio() / 2.0f);
        float bottom = cam_y - (height() * devicePixelRatio() / 2.0f);
        m_projection.ortho( left, right, bottom, top, c_near_plane * m_scale, c_far_plane * m_scale);
        m_model_view.scale( m_scale );

    // Perspective
    } else {
        // Set camera position
        QVector3D  perspective_offset = use_offset ? QVector3D(200.0f, 200.0f, 0.0f) : QVector3D(0.0f, 0.0f, 0.0f);
        QVector3D  eye(     m_engine->getCurrentWorld()->getCameraPos().x()        * m_scale + perspective_offset.x(),
                           (m_engine->getCurrentWorld()->getCameraPos().y() + 100) * m_scale + perspective_offset.y(),
                            m_engine->getCurrentWorld()->getCameraPos().z() );
        QVector3D  look_at( m_engine->getCurrentWorld()->getCameraPos().x()        * m_scale,
                           (m_engine->getCurrentWorld()->getCameraPos().y() + 100) * m_scale,
                            0.0f );
        QVector3D  up(      0.0f, 1.0f, 0.0f);

        m_projection.perspective( c_field_of_view, aspect_ratio, 1.0f, (c_far_plane - c_near_plane) );
        m_model_view.lookAt(eye, look_at, up);
        m_model_view.scale( m_scale );

        // Rotates the camera around the center of the sceen
        ///m_angle += 1.0f;
        ///if (m_angle > 360) m_angle = 0;
        ///m_model_view.rotate( m_angle, 0.0f, 1.0f, 0.0f );
    }
}

//####################################################################################
//##        Render, Paint the Scene
//####################################################################################
// Before rendering 3D objects, enable face culling for triangles facing away from view
void DrOpenGL::cullingOn() {    glEnable( GL_CULL_FACE );   glCullFace(  GL_BACK );     glFrontFace( GL_CCW ); }
// Turn off culling before drawing 2D quads, #NOTE: Must turn OFF culling for QPainter to work
void DrOpenGL::cullingOff() {   glDisable( GL_CULL_FACE ); }

void DrOpenGL::setUpSpaceShader(std::vector<float> &texture_coords) {
    // ***** Enable shader program
    if (!m_default_shader.bind()) return;

    // ***** Blend function
    glEnable(GL_BLEND);
    ///glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);           // Standard non-premultiplied alpha blend
    ///glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);                     // Additive blending
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);                    // Premultiplied alpha blend

    // Fancy Seperate RGB/Alpha Blend Functions
    ///glBlendFuncSeparate(GL_ONE, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);        // Premultiplied alpha blend

    // ***** Set Matrix for Shader, calculates current matrix
    m_default_shader.setUniformValue( u_default_matrix, (m_projection * m_model_view) );

    // ***** Set Texture Coordinates for Shader
    m_default_shader.setAttributeArray( a_default_texture_coord, texture_coords.data(), 2 );
    m_default_shader.enableAttributeArray( a_default_texture_coord );
}

// Renders All Scene Objects
void DrOpenGL::drawSpace() {

    // Keeps track of if we have rendered the lights yet
    bool has_rendered_glow_lights = false;

    // ***** This variable was put in so that multiple Water things drawn next to each other will use the same copy of the render fbo as it currently was,
    //       this saves lots of blit calls, and stops some vertical fragments from appearing as they would try to refract each other
    DrThingType last_thing = DrThingType::Object;

    // ***** Initialize shader settings
    std::vector<float> texture_coordinates;
    setWholeTextureCoordinates(texture_coordinates);
    setUpSpaceShader(texture_coordinates);

    // ********** Render 2D Objects
    for (auto thing : m_engine->getCurrentWorld()->getThings()) {

        // ***** When we have gone past glow z_order, draw the lights to the scene
        if (!has_rendered_glow_lights && (thing->z_order > m_engine->getCurrentWorld()->getGlowZOrder())) {
            m_default_shader.disableAttributeArray( a_default_texture_coord );
            m_default_shader.release();
            has_rendered_glow_lights = drawGlowBuffer();
            setUpSpaceShader(texture_coordinates);
        }

        // ***** If Light, draw with seperate Light Shader, then move to next Thing
        if (thing->getThingType() == DrThingType::Light) {
            DrEngineLight *light = dynamic_cast<DrEngineLight*>(thing);
            if (light) {
                if (!light->isInView()) continue;

                if (light->light_type == Light_Type::Opaque) {
                    m_default_shader.disableAttributeArray( a_default_texture_coord );
                    m_default_shader.release();
                    draw2DLight(light);
                    setUpSpaceShader(texture_coordinates);
                    last_thing = DrThingType::Light;
                    continue;

                } else if (light->light_type == Light_Type::Glow) {

                }
            }
            continue;
        }

        // ***** If Water, draw with seperate Water Shader, then move to next Thing
        if (thing->getThingType() == DrThingType::Water) {
            DrEngineWater *water = dynamic_cast<DrEngineWater*>(thing);
            if (water) {
                m_default_shader.disableAttributeArray( a_default_texture_coord );
                m_default_shader.release();

                if (last_thing != DrThingType::Water) {
                    releaseOffscreenBuffer();
                    QOpenGLFramebufferObject::blitFramebuffer(m_texture_fbo, m_render_fbo);
                    bindOffscreenBuffer(false);
                }
                glDisable(GL_BLEND);
                drawFrameBufferUsingWaterShader(m_texture_fbo, water);
                last_thing = DrThingType::Water;

                setUpSpaceShader(texture_coordinates);
                continue;
            }
        }




        // ***** Convert Thing to Object, Continue with Render
        if (thing->getThingType() != DrThingType::Object) continue;
        DrEngineObject *object = dynamic_cast<DrEngineObject*>(thing);

        // ***** Don't draw Segments (lines)
        bool skip_object = false;
        for (auto shape : object->shapes) {
            if (object->shape_type[shape] == Shape_Type::Segment)
                skip_object = true;
        }
        if (skip_object) continue;

        // ***** Get texture to render with, set texture coordinates
        DrEngineTexture *texture = m_engine->getTexture(object->getTextureNumber());
        if (!texture->texture()->isBound()) {
            texture->texture()->bind();
        }
        float texture_width =  texture->width();
        float texture_height = texture->height();

        // ***** Get object position data
        QPointF center = object->getPosition();
        float   x, y, z;
        float   half_width, half_height;
        x = static_cast<float>(center.x());
        y = static_cast<float>(center.y());
        z = static_cast<float>(object->z_order);
        half_width =  texture_width *  object->getScaleX() / 2.0f;
        half_height = texture_height * object->getScaleY() / 2.0f;


        // ***** Create rotation matrix, apply rotation to object
        QMatrix4x4 matrix;
        matrix.rotate( static_cast<float>(object->getAngle()), 0.0, 0.0, 1.0 );
        QVector3D top_right = matrix * QVector3D( half_width,  half_height, 0);
        QVector3D top_left =  matrix * QVector3D(-half_width,  half_height, 0);
        QVector3D bot_right = matrix * QVector3D( half_width, -half_height, 0);
        QVector3D bot_left =  matrix * QVector3D(-half_width, -half_height, 0);

        // ***** Load vertices for this object
        QVector<GLfloat> vertices;
        vertices.clear();
        vertices.resize( 12 );                      // in sets of x, y, z
        vertices[ 0] = top_right.x() + x;       vertices[ 1] = top_right.y() + y;       vertices[ 2] = z;       // Top Right
        vertices[ 3] = top_left.x()  + x;       vertices[ 4] = top_left.y()  + y;       vertices[ 5] = z;       // Top Left
        vertices[ 6] = bot_right.x() + x;       vertices[ 7] = bot_right.y() + y;       vertices[ 8] = z;       // Bottom Right
        vertices[ 9] = bot_left.x()  + x;       vertices[10] = bot_left.y()  +  y;      vertices[11] = z;       // Bottom Left
        m_default_shader.setAttributeArray( a_default_vertex, vertices.data(), 3 );
        m_default_shader.enableAttributeArray( a_default_vertex );

        // ***** Set Shader Variables
        m_default_shader.setUniformValue( u_default_texture, 0 );                           // Use texture unit 0

        // Fade away dying object
        float alpha = object->getOpacity();                                                 // Start with object alpha
        if (!object->isAlive() && object->getFadeOnDeath()) {
            double fade_percent = 1.0 - (static_cast<double>(Dr::MillisecondsElapsed(object->getFadeTimer())) / static_cast<double>(object->getFadeDelay()));
            alpha *= static_cast<float>(fade_percent);
        }
        m_default_shader.setUniformValue( u_default_alpha,          alpha );
        m_default_shader.setUniformValue( u_default_tint,           0.0f, 0.0f, 0.0f );
        m_default_shader.setUniformValue( u_default_width,          texture_width );
        m_default_shader.setUniformValue( u_default_height,         texture_height );
        m_default_shader.setUniformValue( u_default_time,           static_cast<float>(QTime::currentTime().msecsSinceStartOfDay() / 1000.0) );
        m_default_shader.setUniformValue( u_default_pre,            true );

        m_default_shader.setUniformValue( u_default_pixel_x,        object->pixel_x );
        m_default_shader.setUniformValue( u_default_pixel_y,        object->pixel_y );
        m_default_shader.setUniformValue( u_default_pixel_offset,   0.0f, 0.0f );
        m_default_shader.setUniformValue( u_default_negative,       object->negative );
        m_default_shader.setUniformValue( u_default_grayscale,      object->grayscale );
        m_default_shader.setUniformValue( u_default_hue,            object->hue );
        m_default_shader.setUniformValue( u_default_saturation,     object->saturation );
        m_default_shader.setUniformValue( u_default_contrast,       object->contrast );
        m_default_shader.setUniformValue( u_default_brightness,     object->brightness );

        m_default_shader.setUniformValue( u_default_bitrate,        16.0f );
        m_default_shader.setUniformValue( u_default_cartoon,        false );
        m_default_shader.setUniformValue( u_default_wavy,           false );
        m_default_shader.setUniformValue( u_default_fisheye,        false );

        // ***** Draw triangles using shader program
        glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

        // Release bound items
        m_default_shader.disableAttributeArray( a_default_vertex );
        last_thing = DrThingType::Object;
    }

    // ***** Disable shader program
    m_default_shader.disableAttributeArray( a_default_texture_coord );
    m_default_shader.release();

    // ***** If we didn't draw Glow Lights yet, do it now
    if (!has_rendered_glow_lights) drawGlowBuffer();
}


//####################################################################################
//##        Draws Glow Buffer onto screen buffer
//####################################################################################
bool DrOpenGL::drawGlowBuffer() {

    Blend_Mode mode = m_engine->getCurrentWorld()->getGlowBlendMode();

    // If no lights and ambient light is 50% (normal) exit now
    double ambient_light = m_engine->getCurrentWorld()->getAmbientLight() / 100.0;
    if (m_glow_lights.count() <= 0 && Dr::IsCloseTo(0.5, ambient_light, .001) && mode == Blend_Mode::Standard) return true;

    // Blend functions, in order of best
    if (mode == Blend_Mode::Standard) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_DST_COLOR, GL_SRC_COLOR);                        // Best Light blend function, a little but of oversaturation, but looks nice
        ///glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);           // Standard blend function
        ///glBlendFunc(GL_DST_COLOR, GL_ZERO);                          // "Screen" (slembcke) light blend function. Good, but doesnt get brighter than texture colors
        ///glBlendFunc(GL_ONE, GL_ONE);                                 // Supposed to add lights together, not really great for lighting though
        drawFrameBufferUsingKernelShader(m_glow_fbo);

    } else {
        // Copy fbo to a GL_TEXTURE_2D (non multi-sampled) FBO for use as a texture during Screen Shader, Draw Glow Light buffer using Screen Shader
        releaseOffscreenBuffer();
        QOpenGLFramebufferObject::blitFramebuffer(m_texture_fbo, m_render_fbo);
        bindOffscreenBuffer();
        glDisable(GL_BLEND);
        drawFrameBufferUsingScreenShader(m_glow_fbo, m_texture_fbo, mode);
    }

    return true;
}












