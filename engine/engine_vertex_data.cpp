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

#include "3rdparty/poly_partition.h"
#include "engine_texture.h"
#include "engine_vertex_data.h"
#include "helper.h"
#include "imaging/imaging.h"

const float c_extrude_depth = 0.5f;

#define CELLSIZE 16

#define PAR_RGB  3
#define PAR_RGBA 4


//####################################################################################
//##        Constructor - Texture Cube
//####################################################################################
DrEngineVertexData::DrEngineVertexData() : m_count(0) { }


//####################################################################################
//##        Builds a Textured Quad
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
    add(QVector3D(x1, y1, 0.f), n, QVector2D(tx1, ty1));
    add(QVector3D(x2, y2, 0.f), n, QVector2D(tx2, ty2));
    add(QVector3D(x3, y3, 0.f), n, QVector2D(tx3, ty3));
    add(QVector3D(x2, y2, 0.f), n, QVector2D(tx2, ty2));
    add(QVector3D(x4, y4, 0.f), n, QVector2D(tx4, ty4));
    add(QVector3D(x3, y3, 0.f), n, QVector2D(tx3, ty3));
}


//####################################################################################
//##        Builds a Textured Cube
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
//##        Builds an Extruded Pixmap
//####################################################################################
void DrEngineVertexData::initializeExtrudedPixmap(QPixmap &pixmap) {
    m_data.resize(1000 * c_vertex_length);

    int width =  pixmap.width();
    int height = pixmap.height();
    float   w2 = width  / 2.f;
    float   h2 = height / 2.f;
    double w2d = width  / 2.0;
    double h2d = height / 2.0;

    // EXAMPLE: Adding Triangles
    GLfloat x1 = +w2, y1 = +h2;         // Top Right
    GLfloat x2 = -w2, y2 = +h2;         // Top Left
    GLfloat x3 = +w2, y3 = -h2;         // Bottom Right

    GLfloat tx1 = 1.0, ty1 = 1.0;
    GLfloat tx2 = 0.0, ty2 = 1.0;
    GLfloat tx3 = 1.0, ty3 = 0.0;


    // ***** Find concave hull
    QVector<HullPoint> image_points = DrImaging::outlinePointList( pixmap.toImage(), 0.90 );
    QVector<HullPoint> concave_hull = HullFinder::FindConcaveHull( image_points,     5.00 );


    // ***** Simplify points:
    //       !!!!! If ((y2-y1) / (x2-x1)) == ((y3-y1)/(x3-x1)) then slope is same and is along same line
    QVector<HullPoint> final_points;
    int i = 0;
    while (i < concave_hull.count()) {
        final_points.push_back(concave_hull[i]);
        i++;
    }


    // ***** Triangulate concave hull
    // Copy HullPoints into TPPLPoly
    std::list<TPPLPoly> testpolys, result;
    TPPLPoly poly;
    poly.Init(final_points.count());
    for (int i = 0; i < final_points.count(); i++) {
        poly[i].x = final_points[i].x;
        poly[i].y = final_points[i].y;
    }
    testpolys.push_back( poly );

    // Run triangulation, add triangles to vertex data
    TPPLPartition pp;
    pp.Triangulate_EC(&(*testpolys.begin()), &result);
    for (auto poly : result) {
        x1 = static_cast<GLfloat>(         poly[0].x - w2d);
        y1 = static_cast<GLfloat>(height - poly[0].y - h2d);
        x2 = static_cast<GLfloat>(         poly[1].x - w2d);
        y2 = static_cast<GLfloat>(height - poly[1].y - h2d);
        x3 = static_cast<GLfloat>(         poly[2].x - w2d);
        y3 = static_cast<GLfloat>(height - poly[2].y - h2d);

        tx1 = static_cast<GLfloat>(      poly[0].x / width);
        ty1 = static_cast<GLfloat>(1.0 - poly[0].y / height);
        tx2 = static_cast<GLfloat>(      poly[1].x / width);
        ty2 = static_cast<GLfloat>(1.0 - poly[1].y / height);
        tx3 = static_cast<GLfloat>(      poly[2].x / width);
        ty3 = static_cast<GLfloat>(1.0 - poly[2].y / height);

        triangle( x1, y1, tx1, ty1,
                  x3, y3, tx3, ty3,
                  x2, y2, tx2, ty2);
    }


    // ***** Add extruded triangles
    for (int i = 0; i < final_points.count(); i++) {
        int point1, point2;
        if (i == final_points.count() - 1) {
            point1 = 0;         point2 = i;
        } else {
            point1 = i + 1;     point2 = i;
        }

        x1 = static_cast<GLfloat>(         final_points[point1].x);
        y1 = static_cast<GLfloat>(height - final_points[point1].y);
       tx1 = static_cast<GLfloat>(      final_points[point1].x / width);
       ty1 = static_cast<GLfloat>(1.0 - final_points[point1].y / height);

        x2 = static_cast<GLfloat>(         final_points[point2].x);
        y2 = static_cast<GLfloat>(height - final_points[point2].y);
       tx2 = static_cast<GLfloat>(      final_points[point2].x / width);
       ty2 = static_cast<GLfloat>(1.0 - final_points[point2].y / height);

       x1 -= static_cast<GLfloat>(w2d);
       x2 -= static_cast<GLfloat>(w2d);
       y1 -= static_cast<GLfloat>(h2d);
       y2 -= static_cast<GLfloat>(h2d);

       float pixel_w = (1.0f / width);
       float pixel_h = (1.0f / height);
       if (tx1 > 0.5f) x1 -= pixel_w; else x1 += pixel_w;
       if (tx2 > 0.5f) x2 -= pixel_w; else x2 += pixel_w;
       if (ty1 > 0.5f) y1 -= pixel_h; else y1 += pixel_h;
       if (ty2 > 0.5f) y2 -= pixel_h; else y2 += pixel_h;

       extrude( x1, y1, tx1, ty1,
                x2, y2, tx2, ty2);
    }
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
//##        Adds a Cube, as 3 pairs of front and back
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

        add(p1f, nf, QVector2D(tx1, ty1));
        add(p2f, nf, QVector2D(tx2, ty2));
        add(p3f, nf, QVector2D(tx3, ty3));
        add(p2f, nf, QVector2D(tx2, ty2));
        add(p4f, nf, QVector2D(tx4, ty4));
        add(p3f, nf, QVector2D(tx3, ty3));

        add(p1b, nb, QVector2D(tx1, ty1));
        add(p3b, nb, QVector2D(tx3, ty3));
        add(p2b, nb, QVector2D(tx2, ty2));
        add(p2b, nb, QVector2D(tx2, ty2));
        add(p3b, nb, QVector2D(tx3, ty3));
        add(p4b, nb, QVector2D(tx4, ty4));
    }
}


//####################################################################################
//##        Adds a Quad, front and back
//####################################################################################
void DrEngineVertexData::quad(GLfloat x1, GLfloat y1, GLfloat tx1, GLfloat ty1,
                              GLfloat x2, GLfloat y2, GLfloat tx2, GLfloat ty2,
                              GLfloat x3, GLfloat y3, GLfloat tx3, GLfloat ty3,
                              GLfloat x4, GLfloat y4, GLfloat tx4, GLfloat ty4) {
    QVector3D n;
    n = QVector3D::normal(QVector3D(x4 - x1, y4 - y1, 0.0f), QVector3D(x2 - x1, y2 - y1, 0.0f));

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
    //QVector3D n = QVector3D::normal(QVector3D(0.0f, 0.0f, -c_extrude_depth), QVector3D(x2 - x1, y2 - y1, 0.0f));
    //QVector3D n = QVector3D::normal(QVector3D(0.0f, 0.0f, -c_extrude_depth), QVector3D(x1 - x2, y1 - y2, 0.0f));
    QVector3D n;
    n = QVector3D::normal( QVector3D(x1, y1, +c_extrude_depth),
                           QVector3D(x2, y2, +c_extrude_depth),
                           QVector3D(x1, y1, -c_extrude_depth));

    add(QVector3D(x1, y1, +c_extrude_depth), n, QVector2D(tx1, ty1));
    add(QVector3D(x1, y1, -c_extrude_depth), n, QVector2D(tx1, ty1));
    add(QVector3D(x2, y2, +c_extrude_depth), n, QVector2D(tx2, ty2));


    n = QVector3D::normal( QVector3D(x2, y2, +c_extrude_depth),
                           QVector3D(x2, y2, -c_extrude_depth),
                           QVector3D(x1, y1, -c_extrude_depth));

    add(QVector3D(x2, y2, +c_extrude_depth), n, QVector2D(tx2, ty2));
    add(QVector3D(x1, y1, -c_extrude_depth), n, QVector2D(tx1, ty1));
    add(QVector3D(x2, y2, -c_extrude_depth), n, QVector2D(tx2, ty2));
}














