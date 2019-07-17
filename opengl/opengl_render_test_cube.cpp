//
//      Created by Stephens Nunnally on 5/7/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QtMath>
#include <QPainter>

#include "engine/engine.h"
#include "engine/engine_thing_object.h"
#include "engine/engine_texture.h"
#include "engine/engine_world.h"
#include "opengl/opengl.h"


//####################################################################################
//##        Draws a 3D rotating cube for demo purposes
//####################################################################################
void DrOpenGL::drawCube(QVector3D center) {
    // ***** Turn culling on for 3d drawing
    cullingOn();

    // ***** Enable shader program
    if (!m_default_shader.bind()) return;

    // ***** Blend function
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);                    // Premultiplied alpha blend

    // ***** Set Matrix for Shader, calculates current matrix
    QMatrix4x4 m_matrix = m_projection * m_model_view;
    m_default_shader.setUniformValue( u_default_matrix, m_matrix );

    // ***** Set Texture Coordinates for Shader
    std::vector<float> texture_coordinates;
    setWholeTextureCoordinates(texture_coordinates);
    m_default_shader.setAttributeArray( a_default_texture_coord, texture_coordinates.data(), 2 );
    m_default_shader.enableAttributeArray( a_default_texture_coord );

    static float cube_angle;
    cube_angle += .1f;
    if (cube_angle > 360) cube_angle = 0;

    DrEngineTexture *texture = m_engine->getTexture( Asset_Textures::Block );
    texture->texture()->bind();

    // ***** Draw the six sides of the cube
    for (int loop = 0; loop < 6; loop++) {
        // Get object position data
        float x, y, z;
        float half_width, half_height;
        float half_no_border;
        float multi =  3;   // multiplier to make this bigger
        float width =  texture->width() *  multi;
        float height = texture->height() * multi;

        x = static_cast<float>(center.x());
        y = static_cast<float>(center.y());
        z = static_cast<float>(center.z());
        half_width =  width  / 2.0f;
        half_height = height / 2.0f;
        half_no_border =  width / 2.0f;

        // ***** Create rotation matrix, #NOTE: rotations happend in reverse order
        QMatrix4x4 matrix;
        matrix.rotate( 45.0f, 1.0, 0.0, 1.0 );                                              // Kick at a cool angle

        // Top of Cube
        if (loop == 4) {
            matrix.rotate( cube_angle + static_cast<float>(90*loop), 0.0, 1.0, 0.0 );       // Spins around y axis, Twirl
            matrix.rotate( -90.0f, 1.0, 0.0, 0.0 );                                         // Spins around x axis, Flip up to top
        } else if (loop == 5) {
            matrix.rotate( cube_angle + static_cast<float>(90*loop), 0.0, 1.0, 0.0 );       // Spins around y axis, Twirl
            matrix.rotate( 90.0f, 1.0, 0.0, 0.0 );                                          // Spins around x axis, Flip down to bottom
        } else {
            //matrix.rotate( cube_angle + static_cast<float>(90*loop), 1.0, 0.0, 0.0 );       // Spins around x axis
            matrix.rotate( cube_angle + static_cast<float>(90*loop), 0.0, 1.0, 0.0 );       // Spins around y axis
            //matrix.rotate( cube_angle + static_cast<float>(90*loop), 0.0, 0.0, 1.0 );       // Spins around z axis
        }

        // ***** Load vertices for this object
        QVector3D top_right, top_left, bot_right, bot_left;
        top_right = matrix * QVector3D( half_width,  half_height, half_no_border);
        top_left =  matrix * QVector3D(-half_width,  half_height, half_no_border);
        bot_right = matrix * QVector3D( half_width, -half_height, half_no_border);
        bot_left =  matrix * QVector3D(-half_width, -half_height, half_no_border);

        QVector<float> vertices;
        vertices.clear();
        vertices.resize( 12 );  // in sets of x, y, z
        vertices[ 0] = top_right.x() + x;       vertices[ 1] = top_right.y() + y;       vertices[ 2] = z;           // Top Right
        vertices[ 3] = top_left.x()  + x;       vertices[ 4] = top_left.y()  + y;       vertices[ 5] = z;           // Top Left
        vertices[ 6] = bot_right.x() + x;       vertices[ 7] = bot_right.y() + y;       vertices[ 8] = z;           // Bottom Right
        vertices[ 9] = bot_left.x() + x;        vertices[10] = bot_left.y() + y;        vertices[11] = z;           // Bottom Left

        m_default_shader.setAttributeArray( a_default_vertex, vertices.data(), 3 );
        m_default_shader.enableAttributeArray( a_default_vertex );

        // Set shader variables
        setShaderDefaultValues(static_cast<float>(texture->width()), static_cast<float>(texture->height()));

        // ***** Draw triangles using shader program
        glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );

        // ***** Disable arrays
        m_default_shader.disableAttributeArray( a_default_vertex );
    }   // For i

    // Release Texture and Shader
    texture->texture()->release();
    m_default_shader.disableAttributeArray( a_default_texture_coord );
    m_default_shader.release();

    // ***** Turn culling off now that we're done with 3d drawing
    cullingOff();
}














