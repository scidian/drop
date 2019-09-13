//
//      Created by Stephens Nunnally on 8/29/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef ENGINE_VERTEX_DATA_H
#define ENGINE_VERTEX_DATA_H

#include <QOpenGLWidget>
#include <QVector>
#include <QVector2D>
#include <QVector3D>

#include "3rd_party/hullfinder.h"
#include "3rd_party/poly_partition.h"


// Local Constants
const int c_vertex_length = 8;          // 8 is (3) for xyz, (3) for normal, and (2) for texture coordinate


// Local Enums
enum class Trianglulation {
    Ear_Clipping,
    Monotone,
    Delaunay,
};


//####################################################################################
//##    DrEngineVertexData
//##        Stores a list of triangles for rendering
//############################
class DrEngineVertexData
{
private:
    QVector<GLfloat>    m_data;
    int                 m_count;

public:
    // Constructor
    DrEngineVertexData();

    // Properties
    const GLfloat  *constData() const {     return m_data.constData(); }
    int             count() const {         return m_count; }
    int             triangleCount() const { return vertexCount() / 3; }
    int             vertexCount() const {   return m_count / c_vertex_length; }

    // Creation Functions
    void    initializeExtrudedPixmap(QPixmap &pixmap);
    void    initializeTextureCube();
    void    initializeTextureQuad();

    // Helper Functions
    static  QVector<HullPoint>  simplifyPoints(const QVector<HullPoint> &from_points, double tolerance, int test_count, bool average = false);
    static  QVector<HullPoint>  smoothPoints(const QVector<HullPoint> &from_points, int neighbors, double neighbor_distance, double weight);

    // Extrusion Functions
    void    extrudeFacePolygon(const QVector<HullPoint> &from_points, int width, int height, int steps);
    void    triangulateFace(const QVector<HullPoint> &from_points, QImage &black_and_white, Trianglulation type);

    // Building Functions
    void    add(const QVector3D &v, const QVector3D &n, const QVector2D &t);
    void    extrude(GLfloat x1, GLfloat y1, GLfloat tx1, GLfloat ty1,
                    GLfloat x2, GLfloat y2, GLfloat tx2, GLfloat ty2, int steps = 1);
    void    cube(GLfloat x1, GLfloat y1, GLfloat tx1, GLfloat ty1,
                 GLfloat x2, GLfloat y2, GLfloat tx2, GLfloat ty2,
                 GLfloat x3, GLfloat y3, GLfloat tx3, GLfloat ty3,
                 GLfloat x4, GLfloat y4, GLfloat tx4, GLfloat ty4);
    void    quad(GLfloat x1, GLfloat y1, GLfloat tx1, GLfloat ty1,
                 GLfloat x2, GLfloat y2, GLfloat tx2, GLfloat ty2,
                 GLfloat x3, GLfloat y3, GLfloat tx3, GLfloat ty3,
                 GLfloat x4, GLfloat y4, GLfloat tx4, GLfloat ty4);
    void    triangle(GLfloat x1, GLfloat y1, GLfloat tx1, GLfloat ty1,
                     GLfloat x2, GLfloat y2, GLfloat tx2, GLfloat ty2,
                     GLfloat x3, GLfloat y3, GLfloat tx3, GLfloat ty3);
};


#endif // ENGINE_VERTEX_DATA_H











