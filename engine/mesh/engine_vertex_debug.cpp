//
//      Created by Stephens Nunnally on 1/2/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "engine_vertex_debug.h"


//####################################################################################
//##    Constructor
//####################################################################################
DebugVertex::DebugVertex() {
    position.clear();
    texture_coordinates.clear();
    radiuses.clear();
    color_fill.clear();
    color_border.clear();
}


//####################################################################################
//##    Adds a Vertex
//####################################################################################
void DebugVertex::addVertex(double x, double y, float uv_x, float uv_y, float r, DrColor fill, DrColor border) {
    addVertex(static_cast<float>(x), static_cast<float>(y), uv_x, uv_y, r, fill, border);
}

void DebugVertex::addVertex(float x, float y, float uv_x, float uv_y, float r, DrColor fill, DrColor border) {
    position.push_back(x);
    position.push_back(y);
    texture_coordinates.push_back(uv_x);
    texture_coordinates.push_back(uv_y);
    radiuses.push_back(r);
    color_fill.push_back(static_cast<float>(fill.redF()));
    color_fill.push_back(static_cast<float>(fill.greenF()));
    color_fill.push_back(static_cast<float>(fill.blueF()));
    color_fill.push_back(static_cast<float>(fill.alphaF()));
    color_border.push_back(static_cast<float>(border.redF()));
    color_border.push_back(static_cast<float>(border.greenF()));
    color_border.push_back(static_cast<float>(border.blueF()));
    color_border.push_back(static_cast<float>(border.alphaF()));
}


