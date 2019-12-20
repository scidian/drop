//
//      Created by Stephens Nunnally on 8/29/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef ENGINE_VERTEX_DATA_H
#define ENGINE_VERTEX_DATA_H

#include <QOpenGLTexture>

#include "engine_mesh.h"

// Type Definitions
typedef std::map<DrVec3, std::vector<Vertex>> NeighborMap;

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
    std::vector<GLfloat>    m_data;
    int                     m_count;

public:
    // Constructor
    DrEngineVertexData();

    // Properties
    const GLfloat  *constData() const {     return m_data.data(); }
    int             count() const {         return m_count; }
    int             triangleCount() const { return vertexCount() / 3; }
    int             vertexCount() const {   return m_count / c_vertex_length; }

    // Creation Functions
    void    initializeExtrudedBitmap(const DrBitmap &bitmap, bool wireframe = true);

    void    initializeTextureCone();
    void    initializeTextureCube();
    void    initializeTextureQuad();

    // Helper Functions
    static  std::vector<DrPointF>   simplifyPoints(const std::vector<DrPointF> &outline_points, double tolerance, int test_count, bool average = false);
    static  std::vector<DrPointF>   smoothPoints(const std::vector<DrPointF> &outline_points, int neighbors, double neighbor_distance, double weight);

    Mesh                        getMesh(NeighborMap &neighbors);
    Vertex                      getVertex(int vertex_number);
    void                        setVertex(int vertex_number, Vertex v);
    void                        smoothVertices(float weight);


    // Extrusion Functions
    void    extrudeFacePolygon(const std::vector<DrPointF> &outline_points, int width, int height, int steps);
    void    triangulateFace(const std::vector<DrPointF> &outline_points, const std::vector<std::vector<DrPointF>> &hole_list,
                            const DrBitmap &black_and_white, bool wireframe, Trianglulation type);

    // Building Functions
    void    add(const DrVec3 &vertex, const DrVec3 &normal, const DrVec2 &text_coord, Triangle_Point point_number);
    void    extrude(float x1, float y1, float tx1, float ty1,
                    float x2, float y2, float tx2, float ty2, int steps = 1);
    void    cube(float x1, float y1, float tx1, float ty1,
                 float x2, float y2, float tx2, float ty2,
                 float x3, float y3, float tx3, float ty3,
                 float x4, float y4, float tx4, float ty4);
    void    quad(float x1, float y1, float tx1, float ty1,
                 float x2, float y2, float tx2, float ty2,
                 float x3, float y3, float tx3, float ty3,
                 float x4, float y4, float tx4, float ty4);
    void    triangle(float x1, float y1, float tx1, float ty1,
                     float x2, float y2, float tx2, float ty2,
                     float x3, float y3, float tx3, float ty3);
};


#endif // ENGINE_VERTEX_DATA_H











