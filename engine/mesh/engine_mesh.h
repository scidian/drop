//
//      Created by Stephens Nunnally on 8/29/2019, (c) 2019 Scidian Studios, All Rights Reserved
//
//  File:
//
//
//
#ifndef ENGINE_MESH_H
#define ENGINE_MESH_H

#include "core/types/dr_vec3.h"
#include "engine/globals_engine.h"


//####################################################################################
//##    Vertex
//##        Basic vertex struct
//############################
struct Vertex {
    DrVec3      position        { 0, 0, 0 };
    DrVec3      normal          { 0, 0, 0 };
    DrVec3      texture_coords  { 0, 0, 0 };        // !!!!! #NOTE: Only uses x and y
    DrVec3      barycentric     { 0, 0, 0 };

    static      Vertex createVertex(DrVec3 pos, DrVec3 norm, DrVec3 uv, DrVec3 bary);
};


//####################################################################################
//##    Other Mesh Structs
//############################
struct Triangle {
    std::vector<Vertex>     points;
};

struct Mesh {
    std::vector<Triangle>   m_triangles;
    size_t                  triangle_count() const { return m_triangles.size(); }
};



#endif // ENGINE_MESH_H











