//
//      Created by Stephens Nunnally on 8/29/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QtMath>

#include "engine_vertex_data.h"

const float c_extrude_depth = 20.0f;

//####################################################################################
//##        Constructor
//####################################################################################
DrEngineVertexData::DrEngineVertexData() : m_count(0) {
    m_data.resize(100 * c_vertex_length);

    // EXAMPLE: Adding Triangles
    const GLfloat x1 = +20.f;
    const GLfloat y1 = +20.f;
    const GLfloat x2 = -20.f;
    const GLfloat y2 = +20.f;
    const GLfloat x3 = +20.f;
    const GLfloat y3 = -20.f;
    const GLfloat x4 = -20.f;
    const GLfloat y4 = -20.f;

    const GLfloat tx1 = 1.0;
    const GLfloat ty1 = 1.0;
    const GLfloat tx2 = 0.0;
    const GLfloat ty2 = 1.0;
    const GLfloat tx3 = 1.0;
    const GLfloat ty3 = 0.0;
    const GLfloat tx4 = 0.0;
    const GLfloat ty4 = 0.0;

    quad( x1,  y1,  x2,  y2,  x3,  y3,  x4,  y4,
         tx1, ty1, tx2, ty2, tx3, ty3, tx4, ty4);

    extrude( x1,  y1,  x2,  y2,
            tx1, ty1, tx2, ty2);
    extrude( x2,  y2,  x3,  y3,
            tx2, ty2, tx3, ty3);
    extrude( x3,  y3,  x4,  y4,
            tx3, ty3, tx4, ty4);
    extrude( x4,  y4,  x1,  y1,
            tx4, ty4, tx1, ty1);
}


//####################################################################################
//##        Adds a Vertex, including a Normal
//####################################################################################
void DrEngineVertexData::add(const QVector3D &v, const QVector3D &n, const QVector2D &t) {
    if (m_count + c_vertex_length > m_data.count()) m_data.resize(m_data.count() + (100 * c_vertex_length));
    GLfloat *p = m_data.data() + m_count;
    *p++ = v.x();
    *p++ = v.y();
    *p++ = v.z();
    *p++ = n.x();
    *p++ = n.y();
    *p++ = n.z();
    *p++ = t.x();
    *p++ = t.y();
    m_count += c_vertex_length;
}

//####################################################################################
//##        Adds a Quad, front and back
//####################################################################################
void DrEngineVertexData::quad(GLfloat x1, GLfloat y1, GLfloat tx1, GLfloat ty1,
                              GLfloat x2, GLfloat y2, GLfloat tx2, GLfloat ty2,
                              GLfloat x3, GLfloat y3, GLfloat tx3, GLfloat ty3,
                              GLfloat x4, GLfloat y4, GLfloat tx4, GLfloat ty4) {
    QVector3D n = QVector3D::normal(QVector3D(x4 - x1, y4 - y1, 0.0f), QVector3D(x2 - x1, y2 - y1, 0.0f));

    add(QVector3D(x1, y1, c_extrude_depth), n, QVector2D(tx1, ty1));
    add(QVector3D(x4, y4, c_extrude_depth), n, QVector2D(tx4, ty4));
    add(QVector3D(x2, y2, c_extrude_depth), n, QVector2D(tx2, ty2));

    add(QVector3D(x3, y3, c_extrude_depth), n, QVector2D(tx3, ty3));
    add(QVector3D(x2, y2, c_extrude_depth), n, QVector2D(tx2, ty2));
    add(QVector3D(x4, y4, c_extrude_depth), n, QVector2D(tx4, ty4));

    n = QVector3D::normal(QVector3D(x1 - x4, y1 - y4, 0.0f), QVector3D(x2 - x4, y2 - y4, 0.0f));

    add(QVector3D(x4, y4, -c_extrude_depth), n, QVector2D(tx4, ty4));
    add(QVector3D(x1, y1, -c_extrude_depth), n, QVector2D(tx1, ty1));
    add(QVector3D(x2, y2, -c_extrude_depth), n, QVector2D(tx2, ty2));

    add(QVector3D(x2, y2, -c_extrude_depth), n, QVector2D(tx2, ty2));
    add(QVector3D(x3, y3, -c_extrude_depth), n, QVector2D(tx3, ty3));
    add(QVector3D(x4, y4, -c_extrude_depth), n, QVector2D(tx4, ty4));
}

//####################################################################################
//##        Adds a Triangle, front and back
//####################################################################################
void DrEngineVertexData::triangle(GLfloat x1, GLfloat y1, GLfloat tx1, GLfloat ty1,
                                  GLfloat x2, GLfloat y2, GLfloat tx2, GLfloat ty2,
                                  GLfloat x3, GLfloat y3, GLfloat tx3, GLfloat ty3) {
    QVector3D n = QVector3D::normal(QVector3D(x3 - x1, y3 - y1, 0.0f), QVector3D(x2 - x1, y2 - y1, 0.0f));

    add(QVector3D(x1, y1, -c_extrude_depth), n, QVector2D(tx1, ty1));
    add(QVector3D(x3, y3, -c_extrude_depth), n, QVector2D(tx3, ty3));
    add(QVector3D(x2, y2, -c_extrude_depth), n, QVector2D(tx2, ty2));

    n = QVector3D::normal(QVector3D(x1 - x3, y1 - y3, 0.0f), QVector3D(x2 - x3, y2 - y3, 0.0f));

    add(QVector3D(x1, y1, c_extrude_depth), n, QVector2D(tx1, ty1));
    add(QVector3D(x2, y2, c_extrude_depth), n, QVector2D(tx2, ty2));
    add(QVector3D(x3, y3, c_extrude_depth), n, QVector2D(tx3, ty3));
}

//####################################################################################
//##        Adds a Quad extruded from an Edge
//####################################################################################
void DrEngineVertexData::extrude(GLfloat x1, GLfloat y1, GLfloat tx1, GLfloat ty1,
                                 GLfloat x2, GLfloat y2, GLfloat tx2, GLfloat ty2) {
    QVector3D n = QVector3D::normal(QVector3D(0.0f, 0.0f, -c_extrude_depth), QVector3D(x2 - x1, y2 - y1, 0.0f));

    add(QVector3D(x1, y1, +c_extrude_depth), n, QVector2D(tx1, ty1));
    add(QVector3D(x1, y1, -c_extrude_depth), n, QVector2D(tx1, ty1));
    add(QVector3D(x2, y2, +c_extrude_depth), n, QVector2D(tx2, ty2));

    add(QVector3D(x2, y2, -c_extrude_depth), n, QVector2D(tx2, ty2));
    add(QVector3D(x2, y2, +c_extrude_depth), n, QVector2D(tx2, ty2));
    add(QVector3D(x1, y1, -c_extrude_depth), n, QVector2D(tx1, ty1));
}














