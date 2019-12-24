//
//      Created by Stephens Nunnally on 12/23/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "core/dr_math.h"
#include "core/types/dr_color.h"
#include "engine/opengl/opengl.h"


void DrOpenGL::drawDebugShape(QMatrix4x4 mvp, DrColor fill, DrColor border, float *position, float *uv, float *radius, int triangles) {
    // Enable shader program
    if (!m_debug_shader.bind()) return;

    // ***** Blend function
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);              // Standard non-premultiplied alpha blend

    // ***** Set Shader Variables
    m_debug_shader.setUniformValue( u_debug_matrix,         mvp );
    m_debug_shader.setUniformValue( u_debug_color_fill,     static_cast<float>(fill.redF()),
                                                            static_cast<float>(fill.greenF()),
                                                            static_cast<float>(fill.blueF()),
                                                            static_cast<float>(fill.alphaF()));
    m_debug_shader.setUniformValue( u_debug_color_border,   static_cast<float>(border.redF()),
                                                            static_cast<float>(border.greenF()),
                                                            static_cast<float>(border.blueF()),
                                                            static_cast<float>(border.alphaF()));

    // ***** Draw triangles using shader program
    m_debug_shader.enableAttributeArray( a_debug_position );
    m_debug_shader.enableAttributeArray( a_debug_texture_coord );
    m_debug_shader.enableAttributeArray( a_debug_radius );
    m_debug_shader.setAttributeArray(    a_debug_position,          position,   2 );
    m_debug_shader.setAttributeArray(    a_debug_texture_coord,     uv,         2 );
    m_debug_shader.setAttributeArray(    a_debug_radius,            radius,     1 );
    glDrawArrays( GL_TRIANGLE_STRIP, 0, triangles + 2 );
    m_debug_shader.disableAttributeArray( a_debug_position );
    m_debug_shader.disableAttributeArray( a_debug_texture_coord );
    m_debug_shader.disableAttributeArray( a_debug_radius );
    addTriangles( triangles );

    m_debug_shader.release();
}

//####################################################################################
//##    Draws a Circle Shape
//####################################################################################
void DrOpenGL::drawDebugCircle(DrPointF pos, float radius, float angle, DrColor fill, DrColor border) {

    // ***** Set Matrix for Shader, calculates current matrix, adds in object location
    float x =   static_cast<float>(pos.x);
    float y =   static_cast<float>(pos.y);

    // ***** Set circle position / angle
    QMatrix4x4 model;
    model.translate(  x,  y, 0);
    model.rotate(angle, 0.f, 0.f, 1.f);
    model.translate( -x, -y, 0);

    std::vector<float> position;                     // in sets of x, y
    position.resize( 8 );
    position[0] = x;                position[1] = y;
    position[2] = x;                position[3] = y;
    position[4] = x;                position[5] = y;
    position[6] = x;                position[7] = y;

    std::vector<float> texture_coordinates;          // in sets of x, y
    texture_coordinates.resize( 8 );
    texture_coordinates[0] = -1;     texture_coordinates[1] = -1;
    texture_coordinates[2] = -1;     texture_coordinates[3] =  1;
    texture_coordinates[4] =  1;     texture_coordinates[5] = -1;
    texture_coordinates[6] =  1;     texture_coordinates[7] =  1;

    std::vector<float> radiuses;                       // in sets of r
    radiuses.resize( 4 );
    std::fill(radiuses.begin(), radiuses.end(), radius);

    drawDebugShape(m_projection * m_view * model, fill, border, position.data(), texture_coordinates.data(), radiuses.data(), 2 );
    drawDebugLine(cpv(pos.x, pos.y), cpvadd(cpv(pos.x, pos.y), cpvmult(cpvforangle(static_cast<cpFloat>(Dr::DegreesToRadians(angle+90.f))), 0.98*static_cast<double>(radius))),
                  1.5f/combinedZoomScale(), border, border);
}


//####################################################################################
//##    Draws a Segment (Line) Shape
//####################################################################################
void DrOpenGL::drawDebugLine(cpVect a, cpVect b, float radius, DrColor fill, DrColor border) {

    cpVect t = cpvnormalize(cpvsub(b, a));

    std::vector<float> position;                     // in sets of x, y
    position.resize( 16 );
    position[ 0] = static_cast<float>(a.x);      position[ 1] = static_cast<float>(a.y);
    position[ 2] = static_cast<float>(a.x);      position[ 3] = static_cast<float>(a.y);
    position[ 4] = static_cast<float>(a.x);      position[ 5] = static_cast<float>(a.y);
    position[ 6] = static_cast<float>(a.x);      position[ 7] = static_cast<float>(a.y);
    position[ 8] = static_cast<float>(b.x);      position[ 9] = static_cast<float>(b.y);
    position[10] = static_cast<float>(b.x);      position[11] = static_cast<float>(b.y);
    position[12] = static_cast<float>(b.x);      position[13] = static_cast<float>(b.y);
    position[14] = static_cast<float>(b.x);      position[15] = static_cast<float>(b.y);

    std::vector<float> texture_coordinates;          // in sets of x, y
    texture_coordinates.clear();
    texture_coordinates.resize( 16 );
    texture_coordinates[ 0] = static_cast<float>(-t.x + t.y);   texture_coordinates[ 1] = static_cast<float>(-t.x - t.y);
    texture_coordinates[ 2] = static_cast<float>(-t.x - t.y);   texture_coordinates[ 3] = static_cast<float>(+t.x - t.y);
    texture_coordinates[ 4] = static_cast<float>(-0.0 + t.y);   texture_coordinates[ 5] = static_cast<float>(-t.x + 0.0);
    texture_coordinates[ 6] = static_cast<float>(-0.0 - t.y);   texture_coordinates[ 7] = static_cast<float>(+t.x + 0.0);

    texture_coordinates[ 8] = static_cast<float>(+0.0 + t.y);   texture_coordinates[ 9] = static_cast<float>(-t.x - 0.0);
    texture_coordinates[10] = static_cast<float>(+0.0 - t.y);   texture_coordinates[11] = static_cast<float>(+t.x - 0.0);
    texture_coordinates[12] = static_cast<float>(+t.x + t.y);   texture_coordinates[13] = static_cast<float>(-t.x + t.y);
    texture_coordinates[14] = static_cast<float>(+t.x - t.y);   texture_coordinates[15] = static_cast<float>(+t.x + t.y);

    std::vector<float> radiuses;                       // in sets of r
    radiuses.resize( 8 );
    std::fill(radiuses.begin(), radiuses.end(), radius);

    drawDebugShape(m_projection * m_view, fill, border, position.data(), texture_coordinates.data(), radiuses.data(), 6 );
}










