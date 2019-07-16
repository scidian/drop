//
//      Created by Stephens Nunnally on 7/12/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//

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
        cam_x = (int(cam_x) / 5) * 5;
        cam_y = (int(cam_y) / 5) * 5;
        float left =   cam_x - (m_occluder_fbo->width() /  2.0f);
        float right =  cam_x + (m_occluder_fbo->width() /  2.0f);
        float top =    cam_y + (m_occluder_fbo->height() / 2.0f);
        float bottom = cam_y - (m_occluder_fbo->height() / 2.0f);
        matrix.ortho( left, right, bottom, top, c_near_plane * m_scale * c_occluder_scale_ortho, c_far_plane * m_scale * c_occluder_scale_ortho);
        matrix.scale( m_scale * c_occluder_scale_ortho );

    // Perspective
    } else {
        // Set camera position
        QVector3D  perspective_offset = use_offset? QVector3D( 200.0f, 200.0f, 0.0f) : QVector3D( 0.0f, 0.0f, 0.0f);
        float cam_x =  (m_engine->getCurrentWorld()->getCameraPos().x()) * m_scale * c_occluder_scale_proj;
        float cam_y =  (m_engine->getCurrentWorld()->getCameraPos().y() + 100) * m_scale * c_occluder_scale_proj;
        cam_x = (int(cam_x) / 5) * 5;
        cam_y = (int(cam_y) / 5) * 5;
        QVector3D  eye(     cam_x + perspective_offset.x(), cam_y + perspective_offset.y(), m_engine->getCurrentWorld()->getCameraPos().z());
        QVector3D  look_at( cam_x, cam_y, 0.0f );
        QVector3D  up(      0.0f,   1.0f, 0.0f );
        matrix.perspective( c_field_of_view, aspect_ratio, 1.0f, (c_far_plane - c_near_plane) );
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

    // ***** Standard blend function
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
        if (!object->cast_shadows) continue;

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
        half_width =  (static_cast<float>(texture->width()) *  object->getScaleX() * 1.00f) / 2.0f;
        half_height = (static_cast<float>(texture->height()) * object->getScaleY() * 1.00f) / 2.0f;

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
        vertices[ 0] = top_right.x() + x;       vertices[ 1] = top_right.y() + y;       vertices[ 2] = z;           // Top Right
        vertices[ 3] = top_left.x()  + x;       vertices[ 4] = top_left.y()  + y;       vertices[ 5] = z;           // Top Left
        vertices[ 6] = bot_right.x() + x;       vertices[ 7] = bot_right.y() + y;       vertices[ 8] = z;           // Bottom Right
        vertices[ 9] = bot_left.x()  + x;       vertices[10] = bot_left.y()  + y;       vertices[11] = z;           // Bottom Left
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
        m_occluder_shader.setUniformValue( m_uniform_occluder_near_plane, c_near_plane );
        m_occluder_shader.setUniformValue( m_uniform_occluder_far_plane,  c_far_plane );

        // ***** Draw triangles using shader program
        glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

        // Release bound items
        m_occluder_shader.disableAttributeArray( m_attribute_occluder_vertex );
    }

    // ***** Disable shader program
    m_occluder_shader.disableAttributeArray( m_attribute_occluder_tex_coord );
    m_occluder_shader.release();
}






