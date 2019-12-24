//
//      Created by Stephens Nunnally on 12/23/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "core/types/dr_color.h"
#include "engine/opengl/opengl.h"


//####################################################################################
//##    Draws a Circle Shape
//####################################################################################
void DrOpenGL::drawShapeCircle(DrPointF position, float radius, float angle, float alpha, DrColor color) {

    // Enable shader program
    if (!m_circle_shader.bind()) return;

    // ***** Blend function
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);              // Standard non-premultiplied alpha blend

    // ***** Set Matrix for Shader, calculates current matrix, adds in object location
    float x =   static_cast<float>(position.x);
    float y =   static_cast<float>(position.y);
    float z =   static_cast<float>(0);

    // ***** Set circle position / angle
    QMatrix4x4 model;
    model.translate(x, y, z);
    model.rotate(angle, 0.f, 0.f, 1.f);
    model.scale(radius * 2.f, radius * 2.f, 1.0f);

    // ***** Set Shader Variables
    m_circle_shader.setUniformValue( u_circle_matrix,   m_projection * m_view * model );
    m_circle_shader.setUniformValue( u_circle_color,    static_cast<float>(color.redF()),
                                                        static_cast<float>(color.greenF()),
                                                        static_cast<float>(color.blueF()) );
    m_circle_shader.setUniformValue( u_circle_alpha,    alpha );

    // ***** Draw triangles using shader program
    m_circle_shader.enableAttributeArray( a_circle_vertex );
    m_circle_shader.enableAttributeArray( a_circle_texture_coord );
    m_circle_shader.setAttributeArray(    a_circle_vertex,        m_quad_vertices.data(),             3 );
    m_circle_shader.setAttributeArray(    a_circle_texture_coord, m_quad_texture_coordinates.data(),  2 );
    glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
    m_circle_shader.disableAttributeArray( a_circle_vertex );
    m_circle_shader.disableAttributeArray( a_circle_texture_coord );
    addTriangles( 2 );

    m_circle_shader.release();
}














