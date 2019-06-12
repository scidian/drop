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
#include "engine/engine_object.h"
#include "engine/engine_texture.h"
#include "engine/engine_world.h"
#include "forms/form_engine.h"
#include "helper.h"
#include "opengl/opengl.h"


//####################################################################################
//##        Render, Paint the Scene
//####################################################################################
// Before rendering 3D objects, enable face culling for triangles facing away from view
void OpenGL::cullingOn() {      glEnable( GL_CULL_FACE );   glCullFace(  GL_BACK );     glFrontFace( GL_CCW ); }
// Turn off culling before drawing 2D quads, #NOTE: Must turn OFF culling for QPainter to work
void OpenGL::cullingOff() {     glDisable( GL_CULL_FACE ); }

// Renders All Scene Objects
void OpenGL::drawSpace() {

    // ***** Enable shader program
    if (!m_shader.bind()) return;

    // ***** Set Matrix for Shader, calculates current matrix
    QMatrix4x4 m_matrix = m_projection * m_model_view;
    m_shader.setUniformValue( m_uniform_matrix, m_matrix );

    // ***** Set Texture Coordinates for Shader
    std::vector<float> texture_coordinates;
    setWholeTextureCoordinates(texture_coordinates);
    m_shader.setAttributeArray( m_attribute_tex_coord, texture_coordinates.data(), 2 );
    m_shader.enableAttributeArray( m_attribute_tex_coord );

    // ***** Create a vector of the scene objects (ignoring lines / segments) and sort it by depth
    std::vector<std::pair<int, double>> v;
    for (int i = 0; i < m_engine->getCurrentWorld()->objects.count(); i++) {
        bool skip_object = false;
        for (auto shape : m_engine->getCurrentWorld()->objects[i]->shapes) {
            if (m_engine->getCurrentWorld()->objects[i]->shape_type[shape] == Shape_Type::Segment)          // Don't draw Segments (lines)
                skip_object = true;
        }
        if (skip_object) continue;

        v.push_back(std::make_pair(i, m_engine->getCurrentWorld()->objects[i]->getZOrder()));
    }
    sort(v.begin(), v.end(), [] (std::pair<int, double>&i, std::pair<int, double>&j) { return i.second < j.second; });

    // ********** Render 2D Objects
    ///for (auto object : m_engine->objects) {
    for (ulong i = 0; i < static_cast<ulong>(v.size()); i++) {
        DrEngineObject *object = m_engine->getCurrentWorld()->objects[ v[i].first ];
        if (!object->hasBeenProcessed()) continue;

        // ***** Get texture to render with, set texture coordinates
        DrEngineTexture *texture = m_engine->getTexture(object->getTextureNumber());
        if (!texture->texture()->isBound())
            texture->texture()->bind();

        // ***** Get object position data
        QPointF center = object->getBodyPosition();
        center.setX( (object->getBodyPreviousPosition().x() * (1.0 - m_time_percent)) + (object->getBodyPosition().x() * m_time_percent));
        center.setY( (object->getBodyPreviousPosition().y() * (1.0 - m_time_percent)) + (object->getBodyPosition().y() * m_time_percent));

        float x, y, z, half_width, half_height;
        if (m_engine->getCurrentWorld()->render_type == Render_Type::Orthographic) {
            x = static_cast<float>(center.x()) * m_scale;
            y = static_cast<float>(center.y()) * m_scale;
            z = static_cast<float>(object->getZOrder()) * m_scale;
            half_width =  static_cast<float>(texture->width()) *  object->getScaleX() * m_scale / 2.0f;
            half_height = static_cast<float>(texture->height()) * object->getScaleY() * m_scale / 2.0f;
        } else {
            x = static_cast<float>(center.x());
            y = static_cast<float>(center.y());
            z = static_cast<float>(object->getZOrder());
            half_width =  static_cast<float>(texture->width()) *  object->getScaleX() / 2.0f + 1.0f;
            half_height = static_cast<float>(texture->height()) * object->getScaleY() / 2.0f + 1.0f;
        }

        // ***** Create rotation matrix, apply rotation to object
        QMatrix4x4 matrix;
        matrix.rotate( static_cast<float>(object->getBodyAngle()), 0.0, 0.0, 1.0 );
        QVector3D top_right = matrix * QVector3D( half_width,  half_height, 0);
        QVector3D top_left =  matrix * QVector3D(-half_width,  half_height, 0);
        QVector3D bot_right = matrix * QVector3D( half_width, -half_height, 0);
        QVector3D bot_left =  matrix * QVector3D(-half_width, -half_height, 0);

        // ***** Load vertices for this object
        QVector<GLfloat> vertices;
        vertices.clear();
        vertices.resize( 12 );              // in sets of x, y, z
        // Top Right
        vertices[0] = top_right.x() + x;
        vertices[1] = top_right.y() + y;
        vertices[2] = z;
        // Top Left
        vertices[3] = top_left.x()  + x;
        vertices[4] = top_left.y()  + y;
        vertices[5] = z;
        // Bottom Right
        vertices[6] = bot_right.x() + x;
        vertices[7] = bot_right.y() + y;
        vertices[8] = z;
        // Bottom Left
        vertices[ 9] = bot_left.x() + x;
        vertices[10] = bot_left.y() + y;
        vertices[11] = z;

        m_shader.setAttributeArray( m_attribute_vertex, vertices.data(), 3 );
        m_shader.enableAttributeArray( m_attribute_vertex );

        // ***** Set Shader Variables
        m_shader.setUniformValue( m_uniform_texture, 0 );                           // Use texture unit 0

        // Fade away dying object
        float alpha = object->getOpacity();                                         // Start with object alpha
        if (!object->isAlive() && object->getFadeOnDeath()) {
            double fade_percent = 1.0 - (static_cast<double>(Dr::MillisecondsElapsed(object->getFadeTimer())) / static_cast<double>(object->getFadeDelay()));
            alpha *= static_cast<float>(fade_percent);
        }
        m_shader.setUniformValue( m_uniform_alpha,      alpha );
        m_shader.setUniformValue( m_uniform_width,      static_cast<float>(texture->width()) );
        m_shader.setUniformValue( m_uniform_height,     static_cast<float>(texture->height()) );
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
        ///texture->texture()->release();
    }

    // ***** Disable shader program
    m_shader.disableAttributeArray( m_attribute_tex_coord );
    m_shader.release();
}

















