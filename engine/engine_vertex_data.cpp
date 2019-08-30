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
#include "library/par_msquares.h"

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

//    int new_width =  width;
//    int new_height = height;
//    if (pixmap.width()  % CELLSIZE != 0) new_width +=  (CELLSIZE - (pixmap.width()  % CELLSIZE));
//    if (pixmap.height() % CELLSIZE != 0) new_height += (CELLSIZE - (pixmap.height() % CELLSIZE));

//    QPixmap new_pix(new_width, new_height);
//    if (new_pix.width() != pixmap.width() || new_pix.height() != pixmap.height()) {
//        new_pix.fill(Qt::transparent);
//        QPainter painter(&new_pix);
//        painter.drawPixmap( new_pix.rect(), pixmap, pixmap.rect() );
//        painter.end();
//    } else {
//        new_pix = pixmap.copy();
//    }
//    float x_ratio = static_cast<float>(new_width) /  static_cast<float>(width);
//    float y_ratio = static_cast<float>(new_height) / static_cast<float>(height);


//    // Run marching squares as Multi-Colorerd, must be 256 or less colors
//    //QImage image = new_pix.toImage();
//    //image = image.convertToFormat( QImage::Format_Indexed8, Qt::ThresholdDither | Qt::AutoColor );
//    //image = image.convertToFormat( QImage::Format_Mono, Qt::ImageConversionFlag::AvoidDither | Qt::AutoColor );
//    //image = image.convertToFormat( QImage::Format_ARGB32 );
//    //image.detach();
//    //unsigned char *pixels = image.bits();
//    //par_msquares_meshlist* mlist = par_msquares_color_multi(pixels, new_pix.width(), new_pix.height(), CELLSIZE, PAR_RGBA, PAR_MSQUARES_SIMPLIFY);

//    // Run marching squares as Color
//    //QColor mask_color = QColor(0, 0, 0, 0);
//    //QImage image = DrImaging::imageMask(new_pix.toImage(), mask_color, 1);
//    //unsigned char *pixels = image.bits();
//    //par_msquares_meshlist* mlist = par_msquares_color(pixels, new_pix.width(), new_pix.height(), CELLSIZE, 0x0, PAR_RGBA, PAR_MSQUARES_SIMPLIFY);

//    // Run marching squares as Grayscale
//    QImage image = new_pix.toImage();
//    float *grays = DrImaging::imageBitsAsFloat( image );
//    par_msquares_meshlist* mlist = par_msquares_grayscale(grays, new_pix.width(), new_pix.height(), CELLSIZE, 1000000.0, PAR_MSQUARES_SIMPLIFY);


//    // Load all points from all meshes into vector
//    QVector<QVector3D> points;
//    points.clear();
//    for (int m = 0; m < par_msquares_get_count(mlist); m++) {
//        par_msquares_mesh const* mesh = par_msquares_get_mesh(mlist, m);
//        float* pt = mesh->points;
//        for (int i = 0; i < mesh->npoints; i++) {
//            float z = mesh->dim > 2 ? pt[2] : 0;
//            points.push_back( QVector3D(pt[0] * x_ratio, pt[1] * y_ratio, z) );
//            pt += mesh->dim;
//        }
//    }

//    // Add all triangles to Vertex Data buffer
//    int offset = 0;
//    for (int m = 0; m < par_msquares_get_count(mlist); m++) {
//        par_msquares_mesh const* mesh = par_msquares_get_mesh(mlist, m);
//        uint16_t *index = mesh->triangles;
//        for (int i = 0; i < mesh->ntriangles; i++) {
//            int a = (offset + *index++);
//            int b = (offset + *index++);
//            int c = (offset + *index++);

//            triangle((points[a].x() * width) - w2, (points[a].y() * height) - h2, points[a].x(), points[a].y(),
//                     (points[b].x() * width) - w2, (points[b].y() * height) - h2, points[b].x(), points[b].y(),
//                     (points[c].x() * width) - w2, (points[c].y() * height) - h2, points[c].x(), points[c].y());
//        }
//        offset += mesh->npoints;
//    }
//    par_msquares_free(mlist);
//    return;



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














