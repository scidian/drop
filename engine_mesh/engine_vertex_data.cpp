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
#include <QMatrix4x4>

#include "engine/engine_texture.h"
#include "engine_mesh/engine_vertex_data.h"
#include "helper.h"
#include "imaging/imaging.h"


//####################################################################################
//##    Constructor - Texture Cube
//####################################################################################
DrEngineVertexData::DrEngineVertexData() : m_count(0) { }


//####################################################################################
//##    Builds a Textured Quad
//####################################################################################
void DrEngineVertexData::initializeTextureQuad() {
    m_data.resize(8 * c_vertex_length);

    int   width =  1;
    int   height = 1;
    float w2 = width  / 2.f;
    float h2 = height / 2.f;

    // EXAMPLE: Adding Triangles
    GLfloat x1 = +w2, y1 = +h2;         // Top Right
    GLfloat x2 = -w2, y2 = +h2;         // Top Left
    GLfloat x3 = +w2, y3 = -h2;         // Bottom Right
    GLfloat x4 = -w2, y4 = -h2;         // Bottom Left

    GLfloat tx1 = 1.0, ty1 = 1.0;
    GLfloat tx2 = 0.0, ty2 = 1.0;
    GLfloat tx3 = 1.0, ty3 = 0.0;
    GLfloat tx4 = 0.0, ty4 = 0.0;

    QVector3D n = QVector3D::normal(QVector3D(x4 - x1, y4 - y1, 0.0f), QVector3D(x2 - x1, y2 - y1, 0.0f));
    add(QVector3D(x1, y1, 0.f), n, QVector2D(tx1, ty1), Triangle::Point1);
    add(QVector3D(x2, y2, 0.f), n, QVector2D(tx2, ty2), Triangle::Point2);
    add(QVector3D(x3, y3, 0.f), n, QVector2D(tx3, ty3), Triangle::Point3);
    add(QVector3D(x2, y2, 0.f), n, QVector2D(tx2, ty2), Triangle::Point1);
    add(QVector3D(x4, y4, 0.f), n, QVector2D(tx4, ty4), Triangle::Point2);
    add(QVector3D(x3, y3, 0.f), n, QVector2D(tx3, ty3), Triangle::Point3);
}


//####################################################################################
//##    Builds a Textured Cube
//####################################################################################
void DrEngineVertexData::initializeTextureCube() {
    m_data.resize(40 * c_vertex_length);

    int   width =  1;
    int   height = 1;
    float w2 = width  / 2.f;
    float h2 = height / 2.f;

    // EXAMPLE: Adding Triangles
    GLfloat x1 = +w2, y1 = +h2;         // Top Right
    GLfloat x2 = -w2, y2 = +h2;         // Top Left
    GLfloat x3 = +w2, y3 = -h2;         // Bottom Right
    GLfloat x4 = -w2, y4 = -h2;         // Bottom Left

    GLfloat tx1 = 1.0, ty1 = 1.0;
    GLfloat tx2 = 0.0, ty2 = 1.0;
    GLfloat tx3 = 1.0, ty3 = 0.0;
    GLfloat tx4 = 0.0, ty4 = 0.0;

    cube( x1,  y1,  tx1, ty1,
          x2,  y2,  tx2, ty2,
          x3,  y3,  tx3, ty3,
          x4,  y4,  tx4, ty4);
}

//####################################################################################
//##    Adds a Vertex, including a Normal
//####################################################################################
void DrEngineVertexData::add(const QVector3D &vertex, const QVector3D &normal, const QVector2D &text_coord, Triangle point_number) {
    if (m_count + c_vertex_length > m_data.count()) m_data.resize(m_data.count() + (100 * c_vertex_length));
    GLfloat *p = m_data.data() + m_count;
    *p++ = vertex.x();                  // 0 - x
    *p++ = vertex.y();                  // 1 - y
    *p++ = vertex.z();                  // 2 - z
    *p++ = normal.x();                  // 3 - normal x
    *p++ = normal.y();                  // 4 - normal y
    *p++ = normal.z();                  // 5 - normal z
    *p++ = text_coord.x();              // 6 - texture x
    *p++ = text_coord.y();              // 7 - texture y
    switch(point_number) {
        case Triangle::Point1:  *p++ = 1;   *p++ = 0;   *p++ = 0;   break;
        case Triangle::Point2:  *p++ = 0;   *p++ = 1;   *p++ = 0;   break;
        case Triangle::Point3:  *p++ = 0;   *p++ = 0;   *p++ = 1;   break;
    }
    m_count += c_vertex_length;
}



//####################################################################################
//##    Adds a Cube, as 3 pairs of front and back
//####################################################################################
void DrEngineVertexData::cube(GLfloat x1, GLfloat y1, GLfloat tx1, GLfloat ty1,
                              GLfloat x2, GLfloat y2, GLfloat tx2, GLfloat ty2,
                              GLfloat x3, GLfloat y3, GLfloat tx3, GLfloat ty3,
                              GLfloat x4, GLfloat y4, GLfloat tx4, GLfloat ty4) {
    QMatrix4x4 rotate;  rotate.setToIdentity();
    QVector3D  nf, nb;
    QVector3D  p1f, p2f, p3f, p4f;
    QVector3D  p1b, p2b, p3b, p4b;

    for (int i = 0; i <= 2; ++i) {
        nf = QVector3D::normal(QVector3D(x4 - x1, y4 - y1, 0.0f), QVector3D(x2 - x1, y2 - y1, 0.0f));
        nb = QVector3D::normal(QVector3D(x1 - x4, y1 - y4, 0.0f), QVector3D(x2 - x4, y2 - y4, 0.0f));
        p1f = QVector3D(x1, y1, +c_extrude_depth);
        p2f = QVector3D(x2, y2, +c_extrude_depth);
        p3f = QVector3D(x3, y3, +c_extrude_depth);
        p4f = QVector3D(x4, y4, +c_extrude_depth);
        p1b = QVector3D(x1, y1, -c_extrude_depth);
        p2b = QVector3D(x2, y2, -c_extrude_depth);
        p3b = QVector3D(x3, y3, -c_extrude_depth);
        p4b = QVector3D(x4, y4, -c_extrude_depth);

        if (i == 1)
            rotate.rotate( 90.f, 0.0, 1.0, 0.0);
        else if (i == 2)
            rotate.rotate( 90.f, 1.0, 0.0, 0.0);
        nf =  rotate * nf;
        nb =  rotate * nb;
        p1f = rotate * p1f;
        p2f = rotate * p2f;
        p3f = rotate * p3f;
        p4f = rotate * p4f;
        p1b = rotate * p1b;
        p2b = rotate * p2b;
        p3b = rotate * p3b;
        p4b = rotate * p4b;

        add(p1f, nf, QVector2D(tx1, ty1), Triangle::Point1);
        add(p2f, nf, QVector2D(tx2, ty2), Triangle::Point2);
        add(p3f, nf, QVector2D(tx3, ty3), Triangle::Point3);
        add(p2f, nf, QVector2D(tx2, ty2), Triangle::Point1);
        add(p4f, nf, QVector2D(tx4, ty4), Triangle::Point2);
        add(p3f, nf, QVector2D(tx3, ty3), Triangle::Point3);

        add(p1b, nb, QVector2D(tx1, ty1), Triangle::Point1);
        add(p3b, nb, QVector2D(tx3, ty3), Triangle::Point2);
        add(p2b, nb, QVector2D(tx2, ty2), Triangle::Point3);
        add(p2b, nb, QVector2D(tx2, ty2), Triangle::Point1);
        add(p3b, nb, QVector2D(tx3, ty3), Triangle::Point2);
        add(p4b, nb, QVector2D(tx4, ty4), Triangle::Point3);
    }
}


//####################################################################################
//##    Adds a Quad, front and back
//####################################################################################
void DrEngineVertexData::quad(GLfloat x1, GLfloat y1, GLfloat tx1, GLfloat ty1,
                              GLfloat x2, GLfloat y2, GLfloat tx2, GLfloat ty2,
                              GLfloat x3, GLfloat y3, GLfloat tx3, GLfloat ty3,
                              GLfloat x4, GLfloat y4, GLfloat tx4, GLfloat ty4) {
    QVector3D n;
    n = QVector3D::normal(QVector3D(x4 - x1, y4 - y1, 0.0f), QVector3D(x2 - x1, y2 - y1, 0.0f));

    add(QVector3D(x1, y1, +c_extrude_depth), n, QVector2D(tx1, ty1), Triangle::Point1);
    add(QVector3D(x2, y2, +c_extrude_depth), n, QVector2D(tx2, ty2), Triangle::Point2);
    add(QVector3D(x3, y3, +c_extrude_depth), n, QVector2D(tx3, ty3), Triangle::Point3);

    add(QVector3D(x2, y2, +c_extrude_depth), n, QVector2D(tx2, ty2), Triangle::Point1);
    add(QVector3D(x4, y4, +c_extrude_depth), n, QVector2D(tx4, ty4), Triangle::Point2);
    add(QVector3D(x3, y3, +c_extrude_depth), n, QVector2D(tx3, ty3), Triangle::Point3);

    n = QVector3D::normal(QVector3D(x1 - x4, y1 - y4, 0.0f), QVector3D(x2 - x4, y2 - y4, 0.0f));

    add(QVector3D(x1, y1, -c_extrude_depth), n, QVector2D(tx1, ty1), Triangle::Point1);
    add(QVector3D(x3, y3, -c_extrude_depth), n, QVector2D(tx3, ty3), Triangle::Point2);
    add(QVector3D(x2, y2, -c_extrude_depth), n, QVector2D(tx2, ty2), Triangle::Point3);

    add(QVector3D(x2, y2, -c_extrude_depth), n, QVector2D(tx2, ty2), Triangle::Point1);
    add(QVector3D(x3, y3, -c_extrude_depth), n, QVector2D(tx3, ty3), Triangle::Point2);
    add(QVector3D(x4, y4, -c_extrude_depth), n, QVector2D(tx4, ty4), Triangle::Point3);
}

//####################################################################################
//##    Adds a Triangle, front and back
//####################################################################################
void DrEngineVertexData::triangle(GLfloat x1, GLfloat y1, GLfloat tx1, GLfloat ty1,
                                  GLfloat x2, GLfloat y2, GLfloat tx2, GLfloat ty2,
                                  GLfloat x3, GLfloat y3, GLfloat tx3, GLfloat ty3) {
    QVector3D n = QVector3D::normal(QVector3D(x3 - x1, y3 - y1, 0.0f), QVector3D(x2 - x1, y2 - y1, 0.0f));

    add(QVector3D(x1, y1, +c_extrude_depth), n, QVector2D(tx1, ty1), Triangle::Point1);
    add(QVector3D(x2, y2, +c_extrude_depth), n, QVector2D(tx2, ty2), Triangle::Point2);
    add(QVector3D(x3, y3, +c_extrude_depth), n, QVector2D(tx3, ty3), Triangle::Point3);

    n = QVector3D::normal(QVector3D(x1 - x3, y1 - y3, 0.0f), QVector3D(x2 - x3, y2 - y3, 0.0f));

    add(QVector3D(x1, y1, -c_extrude_depth), n, QVector2D(tx1, ty1), Triangle::Point1);
    add(QVector3D(x3, y3, -c_extrude_depth), n, QVector2D(tx3, ty3), Triangle::Point2);
    add(QVector3D(x2, y2, -c_extrude_depth), n, QVector2D(tx2, ty2), Triangle::Point3);
}

//####################################################################################
//##    Adds a Quad extruded from an Edge
//####################################################################################
void DrEngineVertexData::extrude(GLfloat x1, GLfloat y1, GLfloat tx1, GLfloat ty1,
                                 GLfloat x2, GLfloat y2, GLfloat tx2, GLfloat ty2, int steps) {
    // Original normal formula
    ///QVector3D n = QVector3D::normal(QVector3D(0.0f, 0.0f, -c_extrude_depth), QVector3D(x2 - x1, y2 - y1, 0.0f));
    ///QVector3D n = QVector3D::normal(QVector3D(0.0f, 0.0f, -c_extrude_depth), QVector3D(x1 - x2, y1 - y2, 0.0f));

    float step = (c_extrude_depth * 2.0f) / static_cast<float>(steps);
    float front = c_extrude_depth;
    float back =  c_extrude_depth - step;

    for (int i = 0; i < steps; i++) {
        QVector3D n;
        n = QVector3D::normal( QVector3D(x1, y1, front),
                               QVector3D(x2, y2, front),
                               QVector3D(x1, y1, back));

        add(QVector3D(x1, y1, front), n, QVector2D(tx1, ty1), Triangle::Point1);
        add(QVector3D(x1, y1, back),  n, QVector2D(tx1, ty1), Triangle::Point2);
        add(QVector3D(x2, y2, front), n, QVector2D(tx2, ty2), Triangle::Point3);


        n = QVector3D::normal( QVector3D(x2, y2, front),
                               QVector3D(x2, y2, back),
                               QVector3D(x1, y1, back));

        add(QVector3D(x2, y2, front), n, QVector2D(tx2, ty2), Triangle::Point1);
        add(QVector3D(x1, y1, back),  n, QVector2D(tx1, ty1), Triangle::Point2);
        add(QVector3D(x2, y2, back),  n, QVector2D(tx2, ty2), Triangle::Point3);

        front -= step;
        back  -= step;
    }

}














