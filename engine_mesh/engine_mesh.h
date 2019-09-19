//
//      Created by Stephens Nunnally on 8/29/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef ENGINE_MESH_H
#define ENGINE_MESH_H

#include <QOpenGLWidget>
#include <QVector>
#include <QVector2D>
#include <QVector3D>

#include "3rd_party/vec3.h"

struct Vertex {
    Vec3 position;
    Vec3 normal;
    Vec3 texture_coords;
    Vec3 barycentric;
};

struct Tri_Face {
    Vertex point1;
    Vertex point2;
    Vertex point3;
};

struct Mesh {
    std::vector<Vec3> m_vertices;
    std::vector<Vec3> m_normals;
    std::vector<Vec3> m_colors;
    std::vector<Tri_Face> m_triangles;
    unsigned long vertices_count() const { return m_vertices.size(); }
    unsigned long triangle_count() const { return m_triangles.size(); }
};



#endif // ENGINE_MESH_H











