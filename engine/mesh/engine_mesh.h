//
//      Created by Stephens Nunnally on 8/29/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef ENGINE_MESH_H
#define ENGINE_MESH_H

#include "core/types/dr_vec3.h"
#include "engine/globals_engine.h"

struct Vertex {
    DrVec3      position;
    DrVec3      normal;
    DrVec3      texture_coords;             // Only using x and y
    DrVec3      barycentric;
};

struct Triangle {
    std::vector<Vertex>     points;
};

struct Mesh {
    std::vector<Triangle>   m_triangles;
    size_t                  triangle_count() const { return m_triangles.size(); }
};



#endif // ENGINE_MESH_H











