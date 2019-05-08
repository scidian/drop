//
//      Created by Stephens Nunnally on 5/7/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QtMath>
#include <QPainter>

#include "chipmunk/chipmunk.h"
#include "engine/engine.h"
#include "engine/engine_texture.h"
#include "opengl/opengl.h"


//####################################################################################
//##        Draws a 3D rotating cube for demo purposes
//####################################################################################
void OpenGL::drawCube() {

    static float angle = 0;
    angle++;
    if (angle > 360) angle = 0;

    DrEngineTexture *texture = m_engine->getTexture( 1 );     // 1 is metal_block
    texture->texture()->bind();

    for (int i = 0; i < 5; i++) {
        std::vector<float> texCoords;
        texCoords.clear();
        texCoords.resize( 8 );
        float one_x =  1 / texture->width();
        float one_y =  1 / texture->height();
        texCoords[0] = 1 - one_x;    texCoords[1] = 1 - one_y;
        texCoords[2] =     one_x;    texCoords[3] = 1 - one_y;
        texCoords[4] = 1 - one_x;    texCoords[5] =     one_y;
        texCoords[6] =     one_x;    texCoords[7] =     one_y;
        m_program.setAttributeArray( m_texCoordAttr, texCoords.data(), 2 );
        m_program.enableAttributeArray( m_texCoordAttr );


        // ***** Get object position data
        QPointF center = QPointF(0, 300);
        float x, y, half_width, half_height;
        float half_no_border;

        float multi =  3;
        float width =  texture->width() *  multi;
        float height = texture->height() * multi;

        if (m_engine->render_type == Render_Type::Orthographic) {
            x = static_cast<float>(center.x()) * m_scale;
            y = static_cast<float>(center.y()) * m_scale;
            half_width =  width  * m_scale / 2.0f;
            half_height = height * m_scale / 2.0f;
            half_no_border = width * m_scale / 2.0f;
        } else {
            x = static_cast<float>(center.x());
            y = static_cast<float>(center.y());
            half_width =  width  / 2.0f;
            half_height = height / 2.0f;
            half_no_border = width / 2.0f;
        }


        // ***** Create rotation matrix, apply rotation to object
        QMatrix4x4 matrix;
        if (i == 4) {
            matrix.rotate( static_cast<float>( -angle  ), 0.0, 0.0, 1.0 );
            matrix.rotate( -90, 1.0, 0.0, 0.0 );
        } else {
            matrix.rotate( static_cast<float>( angle + (90 * i) ), 0.0, 1.0, 0.0 );
        }
        matrix.rotate( static_cast<float>( 45               ), 1.0, 0.0, 1.0 );


        QVector3D top_right, top_left, bot_right, bot_left;
        top_right = QVector3D( half_width,  half_height, -half_no_border) * matrix;
        top_left =  QVector3D(-half_width,  half_height, -half_no_border) * matrix;
        bot_right = QVector3D( half_width, -half_height, -half_no_border) * matrix;
        bot_left =  QVector3D(-half_width, -half_height, -half_no_border) * matrix;


        // ***** Load vertices for this object
        QVector<float> vertices;
        vertices.clear();
        vertices.resize( 12 );  // in sets of x, y, z
        // Top Right
        vertices[0] = top_right.x() + x;
        vertices[1] = top_right.y() + y;
        vertices[2] = 0.00f;
        // Top Left
        vertices[3] = top_left.x()  + x;
        vertices[4] = top_left.y()  + y;
        vertices[5] = 0.00f;
        // Bottom Right
        vertices[6] = bot_right.x() + x;
        vertices[7] = bot_right.y() + y;
        vertices[8] = 0.00f;
        // Bottom Left
        vertices[ 9] = bot_left.x() + x;
        vertices[10] = bot_left.y() + y;
        vertices[11] = 0.00f;

        m_program.setAttributeArray( m_vertexAttr, vertices.data(), 3 );
        m_program.setUniformValue( m_texUniform, 0 );
        m_program.enableAttributeArray( m_vertexAttr );

        // ***** Draw triangles using shader program
        glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );            // GL_TRIANGLES

        // ***** Disable arrays
        m_program.disableAttributeArray( m_vertexAttr );
        m_program.disableAttributeArray( m_texCoordAttr );
    }

    texture->texture()->release();

}



