//
//      Created by Stephens Nunnally on 8/29/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QtMath>
#include <QDebug>
#include <QPainter>
#include <QPixmap>

#include "engine_texture.h"
#include "engine_vertex_data.h"
#include "helper.h"
#include "image_filter.h"

const float c_extrude_depth = 40.0f;

#define CELLSIZE 16

#define PAR_RGB  3
#define PAR_RGBA 4

//####################################################################################
//##        Constructor
//####################################################################################
DrEngineVertexData::DrEngineVertexData(QPixmap &pixmap) : m_count(0) {
    m_data.resize(1000 * c_vertex_length);

    int width =  pixmap.width();
    int height = pixmap.height();
    float w2 = width  / 2.f;
    float h2 = height / 2.f;





    // EXAMPLE: Adding Triangles
    const GLfloat x1 = +w2;     // Top Right
    const GLfloat y1 = +h2;
    const GLfloat x2 = -w2;     // Top Left
    const GLfloat y2 = +h2;
    const GLfloat x3 = +w2;     // Bottom Right
    const GLfloat y3 = -h2;
    const GLfloat x4 = -w2;     // Bottom Left
    const GLfloat y4 = -h2;

    const GLfloat tx1 = 1.0;
    const GLfloat ty1 = 1.0;
    const GLfloat tx2 = 0.0;
    const GLfloat ty2 = 1.0;
    const GLfloat tx3 = 1.0;
    const GLfloat ty3 = 0.0;
    const GLfloat tx4 = 0.0;
    const GLfloat ty4 = 0.0;

    quad( x1,  y1,  tx1, ty1,
          x2,  y2,  tx2, ty2,
          x3,  y3,  tx3, ty3,
          x4,  y4,  tx4, ty4);

    extrude( x1,  y1,  tx1, ty1,
             x2,  y2,  tx2, ty2);
    extrude( x2,  y2,  tx2, ty2,
             x4,  y4,  tx4, ty4);
    extrude( x4,  y4,  tx4, ty4,
             x3,  y3,  tx3, ty3);
    extrude( x3,  y3,  tx3, ty3,
             x1,  y1,  tx1, ty1);
}


//####################################################################################
//##        Adds a Vertex, including a Normal
//####################################################################################
void DrEngineVertexData::add(const QVector3D &v, const QVector3D &n, const QVector2D &t) {
    if (m_count + c_vertex_length > m_data.count()) m_data.resize(m_data.count() + (100 * c_vertex_length));
    GLfloat *p = m_data.data() + m_count;
    *p++ = v.x();       // 0 - x
    *p++ = v.y();       // 1 - y
    *p++ = v.z();       // 2 - z
    *p++ = n.x();       // 3 - normal x
    *p++ = n.y();       // 4 - normal y
    *p++ = n.z();       // 5 - normal z
    *p++ = t.x();       // 6 - texture x
    *p++ = t.y();       // 7 - texture y
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

    add(QVector3D(x1, y1, +c_extrude_depth), n, QVector2D(tx1, ty1));
    add(QVector3D(x2, y2, +c_extrude_depth), n, QVector2D(tx2, ty2));
    add(QVector3D(x3, y3, +c_extrude_depth), n, QVector2D(tx3, ty3));

    add(QVector3D(x2, y2, +c_extrude_depth), n, QVector2D(tx2, ty2));
    add(QVector3D(x4, y4, +c_extrude_depth), n, QVector2D(tx4, ty4));
    add(QVector3D(x3, y3, +c_extrude_depth), n, QVector2D(tx3, ty3));

    n = QVector3D::normal(QVector3D(x1 - x4, y1 - y4, 0.0f), QVector3D(x2 - x4, y2 - y4, 0.0f));

    add(QVector3D(x1, y1, -c_extrude_depth), n, QVector2D(tx1, ty1));
    add(QVector3D(x3, y3, -c_extrude_depth), n, QVector2D(tx3, ty3));
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

    add(QVector3D(x1, y1, +c_extrude_depth), n, QVector2D(tx1, ty1));
    add(QVector3D(x2, y2, +c_extrude_depth), n, QVector2D(tx2, ty2));
    add(QVector3D(x3, y3, +c_extrude_depth), n, QVector2D(tx3, ty3));

    n = QVector3D::normal(QVector3D(x1 - x3, y1 - y3, 0.0f), QVector3D(x2 - x3, y2 - y3, 0.0f));

    add(QVector3D(x1, y1, -c_extrude_depth), n, QVector2D(tx1, ty1));
    add(QVector3D(x3, y3, -c_extrude_depth), n, QVector2D(tx3, ty3));
    add(QVector3D(x2, y2, -c_extrude_depth), n, QVector2D(tx2, ty2));
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

    add(QVector3D(x2, y2, +c_extrude_depth), n, QVector2D(tx2, ty2));
    add(QVector3D(x1, y1, -c_extrude_depth), n, QVector2D(tx1, ty1));
    add(QVector3D(x2, y2, -c_extrude_depth), n, QVector2D(tx2, ty2));
}














