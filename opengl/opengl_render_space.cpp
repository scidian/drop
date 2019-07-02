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
        m_projection.ortho( left, right, bottom, top, -5000.0f, 5000.0f);
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

        m_projection.perspective( c_field_of_view, aspect_ratio, 1.0f, 10000.0f );
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

// Renders All Scene Objects
void DrOpenGL::drawSpace() {
    // ***** Enable shader program
    if (!m_shader.bind()) return;

    // ***** Standard blend function
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // ***** Set Matrix for Shader, calculates current matrix
    m_shader.setUniformValue( m_uniform_matrix, (m_projection * m_model_view) );

    // ***** Set Texture Coordinates for Shader
    std::vector<float> texture_coordinates;
    setWholeTextureCoordinates(texture_coordinates);
    m_shader.setAttributeArray( m_attribute_tex_coord, texture_coordinates.data(), 2 );
    m_shader.enableAttributeArray( m_attribute_tex_coord );

    // ********** Render 2D Objects
    for (auto thing : m_engine->getCurrentWorld()->getThings()) {

        // ***** If light, draw with seperate shader then move to next Thing
        float texture_width = 0, texture_height = 0;
        if (thing->getThingType() == DrThingType::Light) {
            DrEngineLight *light = dynamic_cast<DrEngineLight*>(thing);
            if (light) {
                // ***** Renders 2D Lights onto Light frame buffer
                if (!light->isInView()) continue;
                glBlendFunc(GL_SRC_ALPHA, GL_SRC_ALPHA);                            // Light blend function
                m_shader.disableAttributeArray( m_attribute_tex_coord );
                m_shader.release();
                draw2DLight(light);
                if (!m_shader.bind()) return;
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);                  // Standard blend function
                m_shader.setUniformValue( m_uniform_matrix, (m_projection * m_model_view) );
                m_shader.setAttributeArray( m_attribute_tex_coord, texture_coordinates.data(), 2 );
                m_shader.enableAttributeArray( m_attribute_tex_coord );
                continue;
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

        // ***** Draw triangles using shader program
        glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

        // Release bound items
        m_shader.disableAttributeArray( m_attribute_vertex );
    }

    // ***** Disable shader program
    m_shader.disableAttributeArray( m_attribute_tex_coord );
    m_shader.release();
}


//####################################################################################
//####################################################################################


//####################################################################################
//##        Occluder Map FBO Matrices
//####################################################################################
QMatrix4x4 DrOpenGL::occluderMatrix(Render_Type render_type, bool use_offset) {
    float aspect_ratio = static_cast<float>(m_occluder_fbo->width()) / static_cast<float>(m_occluder_fbo->height());

    // Orthographic
    QMatrix4x4 matrix, matrix2;
    matrix.setToIdentity();
    matrix2.setToIdentity();
    if (render_type == Render_Type::Orthographic) {
        float cam_x =  (m_engine->getCurrentWorld()->getCameraPos().x()) * m_scale * c_occluder_scale_ortho;
        float cam_y =  (m_engine->getCurrentWorld()->getCameraPos().y() + 100) * m_scale * c_occluder_scale_ortho;
        float left =   cam_x - (m_occluder_fbo->width() / 2.0f);
        float right =  cam_x + (m_occluder_fbo->width() / 2.0f);
        float top =    cam_y + (m_occluder_fbo->height() / 2.0f);
        float bottom = cam_y - (m_occluder_fbo->height() / 2.0f);
        matrix.ortho( left, right, bottom, top, -5000.0f, 5000.0f);
        matrix.scale( m_scale * c_occluder_scale_ortho );

    // Perspective
    } else {
        // Set camera position
        QVector3D  perspective_offset = use_offset? QVector3D( 200.0f, 200.0f, 0.0f) : QVector3D( 0.0f, 0.0f, 0.0f);
        QVector3D  eye(     m_engine->getCurrentWorld()->getCameraPos().x()        * m_scale * c_occluder_scale_proj + perspective_offset.x(),
                           (m_engine->getCurrentWorld()->getCameraPos().y() + 100) * m_scale * c_occluder_scale_proj + perspective_offset.y(),
                            m_engine->getCurrentWorld()->getCameraPos().z());
        QVector3D  look_at( m_engine->getCurrentWorld()->getCameraPos().x()        * m_scale * c_occluder_scale_proj,
                           (m_engine->getCurrentWorld()->getCameraPos().y() + 100) * m_scale * c_occluder_scale_proj,
                            0.0f );
        QVector3D  up(      0.0f, 1.0f, 0.0f);        
        matrix.perspective( c_field_of_view, aspect_ratio, 1.0f, 10000.0f );
        matrix2.lookAt(eye, look_at, up);
        matrix2.scale( m_scale * c_occluder_scale_proj);
        matrix *= matrix2;
    }
    return matrix;
}


//####################################################################################
//##        Renders the light to the using the Shadow Map
//####################################################################################
void DrOpenGL::drawSpaceOccluder() {
    // ***** Enable shader program
    if (!m_occluder_shader.bind()) return;

    // ***** Set Matrix for Shader
    m_occluder_shader.setUniformValue( m_uniform_occluder_matrix, occluderMatrix(m_engine->getCurrentWorld()->render_type, c_use_cam_offset) );

    // ***** Set Texture Coordinates for Shader
    std::vector<float> texture_coordinates;
    setWholeTextureCoordinates(texture_coordinates);
    m_occluder_shader.setAttributeArray( m_attribute_occluder_tex_coord, texture_coordinates.data(), 2 );
    m_occluder_shader.enableAttributeArray( m_attribute_occluder_tex_coord );

    // ********** Render 2D Objects
    for (auto thing : m_engine->getCurrentWorld()->getThings()) {
        if ( thing->getThingType() != DrThingType::Object) continue;
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

        // ***** Get object position data
        QPointF center = object->getPosition();

        float x, y, z, half_width, half_height;
        x = static_cast<float>(center.x());
        y = static_cast<float>(center.y());
        z = static_cast<float>(object->z_order);
        half_width =  (static_cast<float>(texture->width()) *  object->getScaleX() * 0.98f) / 2.0f;
        half_height = (static_cast<float>(texture->height()) * object->getScaleY() * 0.98f) / 2.0f;

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
        vertices.resize( 12 );                  // in sets of x, y, z
        vertices[ 0] = top_right.x() + x;       vertices[1] = top_right.y() + y;        vertices[ 2] = z;           // Top Right
        vertices[ 3] = top_left.x()  + x;       vertices[4] = top_left.y()  + y;        vertices[ 5] = z;           // Top Left
        vertices[ 6] = bot_right.x() + x;       vertices[7] = bot_right.y() + y;        vertices[ 8] = z;           // Bottom Right
        vertices[ 9] = bot_left.x()  + x;       vertices[10] = bot_left.y() + y;        vertices[11] = z;           // Bottom Left
        m_occluder_shader.setAttributeArray( m_attribute_occluder_vertex, vertices.data(), 3 );
        m_occluder_shader.enableAttributeArray( m_attribute_occluder_vertex );

        // ***** Set Shader Variables
        // Texture unit 0
        m_occluder_shader.setUniformValue( m_uniform_occluder_texture, 0 );

        // Fade away dying object, start with object alpha
        float alpha = object->getOpacity();
        if (!object->isAlive() && object->getFadeOnDeath()) {
            double fade_percent = 1.0 - (static_cast<double>(Dr::MillisecondsElapsed(object->getFadeTimer())) / static_cast<double>(object->getFadeDelay()));
            alpha *= static_cast<float>(fade_percent);
        }
        m_occluder_shader.setUniformValue( m_uniform_occluder_alpha,      alpha );
        m_occluder_shader.setUniformValue( m_uniform_occluder_depth,      static_cast<float>(object->z_order) );

        // ***** Draw triangles using shader program
        glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

        // Release bound items
        m_occluder_shader.disableAttributeArray( m_attribute_occluder_vertex );
    }

    // ***** Disable shader program
    m_occluder_shader.disableAttributeArray( m_attribute_occluder_tex_coord );
    m_occluder_shader.release();
}















