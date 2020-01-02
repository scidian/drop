//
//      Created by Stephens Nunnally on 1/2/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef ENGINE_VERTEX_DEBUG_H
#define ENGINE_VERTEX_DEBUG_H

#include "core/colors/colors.h"


//####################################################################################
//##    Vertex for Debug Shapes
//############################
struct DebugVertex {
    std::vector<float> position;                        // in sets of x, y
    std::vector<float> texture_coordinates;             // in sets of x, y
    std::vector<float> radiuses;                        // in sets of r
    std::vector<float> color_fill;                      // in sets of r, g, b, a
    std::vector<float> color_border;                    // in sets of r, g, b, a

    // Constructor
    DebugVertex();

    // Info
    int triangleCount() { return static_cast<int>(radiuses.size() / 3); }
    int vertexCount() { return static_cast<int>(radiuses.size()); }

    // Functions
    void addVertex(double x, double y, float uv_x, float uv_y, float r, DrColor fill, DrColor border);
    void addVertex(float x, float y, float uv_x, float uv_y, float r, DrColor fill, DrColor border);
};

#endif // ENGINE_VERTEX_DEBUG_H
