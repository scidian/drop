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
    if (!m_shader.bind()) return;

    // ***** Standard blend function
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // ***** Set Matrix for Shader, calculates current matrix
    m_shader.setUniformValue( m_uniform_matrix, (m_projection * m_model_view) );

    // ***** Set Texture Coordinates for Shader
    m_shader.setAttributeArray( m_attribute_tex_coord, texture_coords.data(), 2 );
    m_shader.enableAttributeArray( m_attribute_tex_coord );
}

// Renders All Scene Objects
void DrOpenGL::drawSpace() {

    bool has_rendered_glow_lights = false;

    // Initialize shader settings
    std::vector<float> texture_coordinates;
    setWholeTextureCoordinates(texture_coordinates);
    setUpSpaceShader(texture_coordinates);

    // ********** Render 2D Objects
    for (auto thing : m_engine->getCurrentWorld()->getThings()) {

        // ***** When we have gone past glow z_order, draw the lights to the scene
        if (!has_rendered_glow_lights && (thing->z_order > m_engine->getCurrentWorld()->getGlowZOrder())) {
            m_shader.disableAttributeArray( m_attribute_tex_coord );
            m_shader.release();
            has_rendered_glow_lights = drawGlowBuffer();
            setUpSpaceShader(texture_coordinates);
        }

        // ***** If light, draw with seperate shader then move to next Thing
        float texture_width = 0, texture_height = 0;
        if (thing->getThingType() == DrThingType::Light) {
            DrEngineLight *light = dynamic_cast<DrEngineLight*>(thing);
            if (light) {
                if (!light->isInView()) continue;

                if (light->light_type == Light_Type::Opaque) {
                    m_shader.disableAttributeArray( m_attribute_tex_coord );
                    m_shader.release();
                    draw2DLight(light);
                    setUpSpaceShader(texture_coordinates);
                    continue;

                } else if (light->light_type == Light_Type::Glow) {


                }
            }
            continue;
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
        if (!texture->texture()->isBound())
            texture->texture()->bind();
        texture_width =  texture->width();
        texture_height = texture->height();

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
        m_shader.setAttributeArray( m_attribute_vertex, vertices.data(), 3 );
        m_shader.enableAttributeArray( m_attribute_vertex );

        // ***** Set Shader Variables
        m_shader.setUniformValue( m_uniform_texture, 0 );                           // Use texture unit 0

        // Fade away dying object
        float alpha = object->getOpacity();                                     // Start with object alpha
        if (!object->isAlive() && object->getFadeOnDeath()) {
            double fade_percent = 1.0 - (static_cast<double>(Dr::MillisecondsElapsed(object->getFadeTimer())) / static_cast<double>(object->getFadeDelay()));
            alpha *= static_cast<float>(fade_percent);
        }
        m_shader.setUniformValue( m_uniform_alpha,      alpha );
        m_shader.setUniformValue( m_uniform_width,      texture_width );
        m_shader.setUniformValue( m_uniform_height,     texture_height );
        m_shader.setUniformValue( m_uniform_bitrate,    16.0f );
        m_shader.setUniformValue( m_uniform_pixel_x,    object->pixel_x );
        m_shader.setUniformValue( m_uniform_pixel_y,    object->pixel_y );
        m_shader.setUniformValue( m_uniform_negative,   object->negative );
        m_shader.setUniformValue( m_uniform_grayscale,  object->grayscale );
        m_shader.setUniformValue( m_uniform_hue,        object->hue );
        m_shader.setUniformValue( m_uniform_saturation, object->saturation );
        m_shader.setUniformValue( m_uniform_contrast,   object->contrast );
        m_shader.setUniformValue( m_uniform_brightness, object->brightness );
        m_shader.setUniformValue( m_uniform_tint,       0.0f, 0.0f, 0.0f );
        m_shader.setUniformValue( m_uniform_kernel,     false );
        ///m_shader.setUniformValue( m_uniform_kernel,     true );


        // ***** Draw triangles using shader program
        glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

        // Release bound items
        m_shader.disableAttributeArray( m_attribute_vertex );
    }

    // ***** Disable shader program
    m_shader.disableAttributeArray( m_attribute_tex_coord );
    m_shader.release();

    // ***** If we didn't draw Glow Lights yet, do it now
    if (!has_rendered_glow_lights) drawGlowBuffer();
}


//####################################################################################
//##        Draws Glow Buffer onto screen buffer
//####################################################################################
enum class Blend_Mode {
    Standard,
    Hard_Light,
};

bool DrOpenGL::drawGlowBuffer() {

    Blend_Mode mode = Blend_Mode::Standard;

    // Check that we need to draw glow buffer because of lights or ambient light
    double ambient_light = m_engine->getCurrentWorld()->getAmbientLight() / 100.0;
    if (m_glow_lights.count() <= 0 && Dr::IsCloseTo(1.0, ambient_light, .001)) return true;

    // Blend functions, in order of best
    if (mode == Blend_Mode::Standard) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_DST_COLOR, GL_SRC_COLOR);                     // Best Light blend function, a little but of oversaturation, but looks nice
        ///glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);           // Standard blend function
        ///glBlendFunc(GL_DST_COLOR, GL_ZERO);                          // "Screen" (slembcke) light blend function. Good, but doesnt get brighter than texture colors
        ///glBlendFunc(GL_ONE, GL_ONE);                                 // To Add Lights Together, not really great for lighting though
        drawFrameBufferToScreenBufferDefaultShader(m_glow_fbo, true);

    } else if (mode == Blend_Mode::Hard_Light) {
        // Copy fbo to a GL_TEXTURE_2D (non multi-sampled) FBO for use as a texture during Screen Shader, Draw Glow Light buffer using Screen Shader
        releaseOffscreenBuffer();
        QOpenGLFramebufferObject::blitFramebuffer(m_texture_fbo, m_render_fbo);
        bindOffscreenBuffer();
        glDisable(GL_BLEND);
        ///drawFrameBufferToScreenBufferScreenShader(m_glow_fbo, m_texture_fbo);            // Normal
        drawFrameBufferToScreenBufferScreenShader(m_texture_fbo, m_glow_fbo);               // Reversed, cool metal look
    }

    return true;
}












