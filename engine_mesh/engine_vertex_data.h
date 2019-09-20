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
#include <map>

#include "3rd_party/hullfinder.h"
#include "3rd_party/poly_partition.h"
#include "engine_mesh.h"

// Type Definitions
typedef std::map<Vec3, std::vector<Vertex>> NeighborMap;

// Defines
#define PAR_RGB  3
#define PAR_RGBA 4

// Constants
const int   c_vertex_length = 11;           // 11 is (3) for xyz + (3) for normal + (2) for texture coordinate + (3) for barycentric (for wireframe)
const float c_extrude_depth = 0.5f;

// Local Enums
enum class Trianglulation {
    Ear_Clipping,
    Monotone,
    Delaunay,
};

enum class Triangle_Point {
    Point1,
    Point2,
    Point3,
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
    static  QVector<HullPoint>  simplifyPoints(const QVector<HullPoint> &outline_points, double tolerance, int test_count, bool average = false);
    static  QVector<HullPoint>  smoothPoints(const QVector<HullPoint> &outline_points, int neighbors, double neighbor_distance, double weight);

    Mesh                        getMesh(NeighborMap &neighbors);
    Vertex                      getVertex(int vertex_number);
    void                        setVertex(int vertex_number, Vertex v);
    void                        smoothVertices(float weight);


    // Extrusion Functions
    void    extrudeFacePolygon(const QVector<HullPoint> &outline_points, int width, int height, int steps);
    void    triangulateFace(const QVector<HullPoint> &outline_points, QImage &black_and_white, Trianglulation type);

    // Building Functions
    void    add(const QVector3D &vertex, const QVector3D &normal, const QVector2D &text_coord, Triangle_Point point_number);
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











