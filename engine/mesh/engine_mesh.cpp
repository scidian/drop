//
//      Created by Stephens Nunnally on 1/24/2020, (c) 2020 Scidian Studios, All Rights Reserved
//
//  File:
//
//
//
#include "engine/mesh/engine_mesh.h"


//####################################################################################
//##    Builds a Vertex
//####################################################################################
Vertex Vertex::createVertex(DrVec3 pos, DrVec3 norm, DrVec3 uv, DrVec3 bary) {
    Vertex v;
    v.position =        pos;
    v.normal =          norm;
    v.texture_coords =  uv;
    v.barycentric =     bary;
    return v;
}


