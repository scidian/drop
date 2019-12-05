//
//      Created by Stephens Nunnally on 8/29/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef ENGINE_MESH_H
#define ENGINE_MESH_H

#include "engine/globals_engine.h"

struct Vertex {
    Vec3 position;
    Vec3 normal;
    Vec3 texture_coords;        // Vec2, Only uses x and y
    Vec3 barycentric;
};

struct Triangle {
    std::vector<Vertex>     points;
};

struct Mesh {
    std::vector<Triangle>   m_triangles;
    size_t                  triangle_count() const { return m_triangles.size(); }
};



#endif // ENGINE_MESH_H











