//
//      Created by Stephens Nunnally on 8/29/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "3rd_party/glm/gtx/transform.hpp"
#include "3rd_party/glm/mat4x4.hpp"
#include "core/types/dr_vec2.h"
#include "core/types/dr_vec3.h"
#include "engine/engine_texture.h"
#include "engine/mesh/engine_vertex_data.h"


//####################################################################################
//##    Constructor - Texture Cube
//####################################################################################
DrEngineVertexData::DrEngineVertexData() : m_count(0) { }


//####################################################################################
//##    Builds a Textured Quad
//####################################################################################
void DrEngineVertexData::initializeTextureQuad() {
    m_data.resize(6 * c_vertex_length);

    int   width =  1;
    int   height = 1;
    float w2 = width  / 2.f;
    float h2 = height / 2.f;

    // EXAMPLE: Adding Triangles
    float x1 = +w2, y1 = +h2;                   // Top Right
    float x2 = -w2, y2 = +h2;                   // Top Left
    float x3 = +w2, y3 = -h2;                   // Bottom Right
    float x4 = -w2, y4 = -h2;                   // Bottom Left

    float tx1 = 1.0, ty1 = 1.0;
    float tx2 = 0.0, ty2 = 1.0;
    float tx3 = 1.0, ty3 = 0.0;
    float tx4 = 0.0, ty4 = 0.0;

    DrVec3 n = DrVec3::triangleNormal(DrVec3(x1, y1, 0.f), DrVec3(x3, y3, 0.f), DrVec3(x2, y2, 0.f));

    add(DrVec3(x1, y1, 0.f), n, DrVec2(tx1, ty1), Triangle_Point::Point1);
    add(DrVec3(x2, y2, 0.f), n, DrVec2(tx2, ty2), Triangle_Point::Point2);
    add(DrVec3(x3, y3, 0.f), n, DrVec2(tx3, ty3), Triangle_Point::Point3);
    add(DrVec3(x2, y2, 0.f), n, DrVec2(tx2, ty2), Triangle_Point::Point1);
    add(DrVec3(x4, y4, 0.f), n, DrVec2(tx4, ty4), Triangle_Point::Point2);
    add(DrVec3(x3, y3, 0.f), n, DrVec2(tx3, ty3), Triangle_Point::Point3);
}


//####################################################################################
//##    Builds a Textured Cube
//####################################################################################
void DrEngineVertexData::initializeTextureCube() {
    m_data.resize(36 * c_vertex_length);

    int   width =  1;
    int   height = 1;
    float w2 = width  / 2.f;
    float h2 = height / 2.f;

    // EXAMPLE: Adding Triangles
    float x1 = +w2, y1 = +h2;                   // Top Right
    float x2 = -w2, y2 = +h2;                   // Top Left
    float x3 = +w2, y3 = -h2;                   // Bottom Right
    float x4 = -w2, y4 = -h2;                   // Bottom Left

    float tx1 = 1.0, ty1 = 1.0;
    float tx2 = 0.0, ty2 = 1.0;
    float tx3 = 1.0, ty3 = 0.0;
    float tx4 = 0.0, ty4 = 0.0;

    cube( x1,  y1,  tx1, ty1,
          x2,  y2,  tx2, ty2,
          x3,  y3,  tx3, ty3,
          x4,  y4,  tx4, ty4);
}


//####################################################################################
//##    Builds a Textured Spike (eventually cone)
//####################################################################################
void DrEngineVertexData::initializeTextureCone() {
    m_data.resize(16 * c_vertex_length);

    int   width =  1;
    int   height = 1;
    float w2 = width  / 2.f;
    float h2 = height / 2.f;

    // EXAMPLE: Adding Triangles
    float x1,   y1,  x2,  y2,  x3,  y3,  x4,  y4;
    float tx1, ty1, tx2, ty2, tx3, ty3, tx4, ty4;

    x1 =    0;  y1 = +h2;                       // Top
    x2 =  -w2;  y2 = -h2;                       // Bottom Left
    x3 =  +w2;  y3 = -h2;                       // Bottom Right
    tx1 = 0.5; ty1 = 0.5;
    tx2 = 0.0; ty2 = 0.0;
    tx3 = 1.0; ty3 = 0.0;

    DrVec3 n = DrVec3::triangleNormal(DrVec3(x1, y1, 0.f), DrVec3(x3, y3, 0.f), DrVec3(x2, y2, 0.f));
    DrVec3 point_t( x1, y1,                0);
    DrVec3 point_bl(x2, y2, +c_extrude_depth);
    DrVec3 point_br(x3, y3, +c_extrude_depth);

    glm::mat4 rotate = glm::mat4(1.0);

    for (int i = 0; i <= 4; ++i) {
        add(point_t , n, DrVec2(tx1, ty1), Triangle_Point::Point1);
        add(point_bl, n, DrVec2(tx2, ty2), Triangle_Point::Point2);
        add(point_br, n, DrVec2(tx3, ty3), Triangle_Point::Point3);

        rotate = glm::rotate(rotate, Dr::DegreesToRadians(90.f), glm::vec3(0.0, 1.0, 0.0));

        point_t =  rotate * point_t;
        point_bl = rotate * point_bl;
        point_br = rotate * point_br;
        n =        rotate * n;
    }


    // Bottom Square
    x1 =  +w2;  y1 = +h2;                       // Top Right
    x2 =  -w2;  y2 = +h2;                       // Top Left
    x3 =  +w2;  y3 = -h2;                       // Bottom Right
    x4 =  -w2;  y4 = -h2;                       // Bottom Left
    tx1 = 1.0; ty1 = 1.0;
    tx2 = 0.0; ty2 = 1.0;
    tx3 = 1.0; ty3 = 0.0;
    tx4 = 0.0; ty4 = 0.0;

    rotate = glm::mat4(1.0);                    // Set to identity matrix
    DrVec3  nf;                                 // Normal Front
    DrVec3  p1f, p2f, p3f, p4f;                 // Point 1 Front, etc

    nf = DrVec3::triangleNormal(DrVec3(x1, y1, 0.f), DrVec3(x3, y3, 0.f), DrVec3(x2, y2, 0.f));
    p1f = DrVec3(x1, y1, +c_extrude_depth);
    p2f = DrVec3(x2, y2, +c_extrude_depth);
    p3f = DrVec3(x3, y3, +c_extrude_depth);
    p4f = DrVec3(x4, y4, +c_extrude_depth);

    rotate = glm::rotate(rotate, Dr::DegreesToRadians(90.f), glm::vec3(1.0, 0.0, 0.0));

    nf =    rotate * nf;
    p1f =   rotate * p1f;
    p2f =   rotate * p2f;
    p3f =   rotate * p3f;
    p4f =   rotate * p4f;

    add(p1f, nf, DrVec2(tx1, ty1), Triangle_Point::Point1);
    add(p2f, nf, DrVec2(tx2, ty2), Triangle_Point::Point2);
    add(p3f, nf, DrVec2(tx3, ty3), Triangle_Point::Point3);
    add(p2f, nf, DrVec2(tx2, ty2), Triangle_Point::Point1);
    add(p4f, nf, DrVec2(tx4, ty4), Triangle_Point::Point2);
    add(p3f, nf, DrVec2(tx3, ty3), Triangle_Point::Point3);
}




//####################################################################################
//##    Adds a Vertex, including:
//##        Vec3 Position
//##        Vec3 Normal
//##        Vec2 UV Texture Coordinates
//##        Vec3 Barycentric Coordinates (gives shader a number between 0.0 and 1.0 to lerp to)
//####################################################################################
void DrEngineVertexData::add(const DrVec3 &vertex, const DrVec3 &normal, const DrVec2 &text_coord, Triangle_Point point_number) {
    if (static_cast<size_t>(m_count + c_vertex_length) > m_data.size()) m_data.resize(m_data.size() + (100 * c_vertex_length));
    float *p = m_data.data() + m_count;
    *p++ = vertex.x;                    // 0 - x
    *p++ = vertex.y;                    // 1 - y
    *p++ = vertex.z;                    // 2 - z
    *p++ = normal.x;                    // 3 - normal x
    *p++ = normal.y;                    // 4 - normal y
    *p++ = normal.z;                    // 5 - normal z
    *p++ = text_coord.x;                // 6 - texture x
    *p++ = text_coord.y;                // 7 - texture y
    switch (point_number) {
        case Triangle_Point::Point1:    *p++ = 1;   *p++ = 0;   *p++ = 0;   break;
        case Triangle_Point::Point2:    *p++ = 0;   *p++ = 1;   *p++ = 0;   break;
        case Triangle_Point::Point3:    *p++ = 0;   *p++ = 0;   *p++ = 1;   break;
    }
    m_count += c_vertex_length;
}


//####################################################################################
//##    Adds a Cube, as 3 pairs (six sides) of front and back
//####################################################################################
void DrEngineVertexData::cube(float x1, float y1, float tx1, float ty1,
                              float x2, float y2, float tx2, float ty2,
                              float x3, float y3, float tx3, float ty3,
                              float x4, float y4, float tx4, float ty4) {
    glm::mat4 rotate = glm::mat4(1.0);
    DrVec3 nf, nb;                                   // Normal Front, Normal Back
    DrVec3 p1f, p2f, p3f, p4f;                       // Point 1 Front, etc
    DrVec3 p1b, p2b, p3b, p4b;                       // Point 1 Back, etc

    for (int i = 0; i <= 2; ++i) {
        nf = DrVec3::triangleNormal(DrVec3(x1, y1, 0.f), DrVec3(x3, y3, 0.f), DrVec3(x2, y2, 0.f));
        nb = DrVec3::triangleNormal(DrVec3(x1, y1, 0.f), DrVec3(x2, y2, 0.f), DrVec3(x3, y3, 0.f));
        p1f = DrVec3(x1, y1, +c_extrude_depth);
        p2f = DrVec3(x2, y2, +c_extrude_depth);
        p3f = DrVec3(x3, y3, +c_extrude_depth);
        p4f = DrVec3(x4, y4, +c_extrude_depth);
        p1b = DrVec3(x1, y1, -c_extrude_depth);
        p2b = DrVec3(x2, y2, -c_extrude_depth);
        p3b = DrVec3(x3, y3, -c_extrude_depth);
        p4b = DrVec3(x4, y4, -c_extrude_depth);

        if (i == 1)
            rotate = glm::rotate(rotate, Dr::DegreesToRadians(90.f), glm::vec3(0.0, 1.0, 0.0));
        else if (i == 2)
            rotate = glm::rotate(rotate, Dr::DegreesToRadians(90.f), glm::vec3(1.0, 0.0, 0.0));

        nf =    rotate * nf;
        p1f =   rotate * p1f;
        p2f =   rotate * p2f;
        p3f =   rotate * p3f;
        p4f =   rotate * p4f;

        nb =    rotate * nb;
        p1b =   rotate * p1b;
        p2b =   rotate * p2b;
        p3b =   rotate * p3b;
        p4b =   rotate * p4b;

        add(p1f, nf, DrVec2(tx1, ty1), Triangle_Point::Point1);
        add(p2f, nf, DrVec2(tx2, ty2), Triangle_Point::Point2);
        add(p3f, nf, DrVec2(tx3, ty3), Triangle_Point::Point3);
        add(p2f, nf, DrVec2(tx2, ty2), Triangle_Point::Point1);
        add(p4f, nf, DrVec2(tx4, ty4), Triangle_Point::Point2);
        add(p3f, nf, DrVec2(tx3, ty3), Triangle_Point::Point3);

        add(p1b, nb, DrVec2(tx1, ty1), Triangle_Point::Point1);
        add(p3b, nb, DrVec2(tx3, ty3), Triangle_Point::Point2);
        add(p2b, nb, DrVec2(tx2, ty2), Triangle_Point::Point3);
        add(p2b, nb, DrVec2(tx2, ty2), Triangle_Point::Point1);
        add(p3b, nb, DrVec2(tx3, ty3), Triangle_Point::Point2);
        add(p4b, nb, DrVec2(tx4, ty4), Triangle_Point::Point3);
    }
}


//####################################################################################
//##    Adds a Quad, front and back
//####################################################################################
void DrEngineVertexData::quad(float x1, float y1, float tx1, float ty1,
                              float x2, float y2, float tx2, float ty2,
                              float x3, float y3, float tx3, float ty3,
                              float x4, float y4, float tx4, float ty4) {
    DrVec3 n;
    n = DrVec3::triangleNormal(DrVec3(x1, y1, 0.f), DrVec3(x3, y3, 0.f), DrVec3(x2, y2, 0.f));

    add(DrVec3(x1, y1, +c_extrude_depth), n, DrVec2(tx1, ty1), Triangle_Point::Point1);
    add(DrVec3(x2, y2, +c_extrude_depth), n, DrVec2(tx2, ty2), Triangle_Point::Point2);
    add(DrVec3(x3, y3, +c_extrude_depth), n, DrVec2(tx3, ty3), Triangle_Point::Point3);

    add(DrVec3(x2, y2, +c_extrude_depth), n, DrVec2(tx2, ty2), Triangle_Point::Point1);
    add(DrVec3(x4, y4, +c_extrude_depth), n, DrVec2(tx4, ty4), Triangle_Point::Point2);
    add(DrVec3(x3, y3, +c_extrude_depth), n, DrVec2(tx3, ty3), Triangle_Point::Point3);

    n = DrVec3::triangleNormal(DrVec3(x1, y1, 0.f), DrVec3(x2, y2, 0.f), DrVec3(x3, y3, 0.f));

    add(DrVec3(x1, y1, -c_extrude_depth), n, DrVec2(tx1, ty1), Triangle_Point::Point1);
    add(DrVec3(x3, y3, -c_extrude_depth), n, DrVec2(tx3, ty3), Triangle_Point::Point2);
    add(DrVec3(x2, y2, -c_extrude_depth), n, DrVec2(tx2, ty2), Triangle_Point::Point3);

    add(DrVec3(x2, y2, -c_extrude_depth), n, DrVec2(tx2, ty2), Triangle_Point::Point1);
    add(DrVec3(x3, y3, -c_extrude_depth), n, DrVec2(tx3, ty3), Triangle_Point::Point2);
    add(DrVec3(x4, y4, -c_extrude_depth), n, DrVec2(tx4, ty4), Triangle_Point::Point3);
}

//####################################################################################
//##    Adds a Triangle, front and back
//####################################################################################
void DrEngineVertexData::triangle(float x1, float y1, float tx1, float ty1,
                                  float x2, float y2, float tx2, float ty2,
                                  float x3, float y3, float tx3, float ty3) {
    DrVec3 n;
    n = DrVec3::triangleNormal(DrVec3(x1, y1, 0.f), DrVec3(x3, y3, 0.f), DrVec3(x2, y2, 0.f));

    add(DrVec3(x1, y1, +c_extrude_depth), n, DrVec2(tx1, ty1), Triangle_Point::Point1);
    add(DrVec3(x2, y2, +c_extrude_depth), n, DrVec2(tx2, ty2), Triangle_Point::Point2);
    add(DrVec3(x3, y3, +c_extrude_depth), n, DrVec2(tx3, ty3), Triangle_Point::Point3);

    n = DrVec3::triangleNormal(DrVec3(x1, y1, 0.f), DrVec3(x2, y2, 0.f), DrVec3(x3, y3, 0.f));

    add(DrVec3(x1, y1, -c_extrude_depth), n, DrVec2(tx1, ty1), Triangle_Point::Point1);
    add(DrVec3(x3, y3, -c_extrude_depth), n, DrVec2(tx3, ty3), Triangle_Point::Point2);
    add(DrVec3(x2, y2, -c_extrude_depth), n, DrVec2(tx2, ty2), Triangle_Point::Point3);
}

//####################################################################################
//##    Adds a Quad extruded from an Edge
//####################################################################################
void DrEngineVertexData::extrude(float x1, float y1, float tx1, float ty1,
                                 float x2, float y2, float tx2, float ty2, int steps) {
    float step = (c_extrude_depth * 2.0f) / static_cast<float>(steps);
    float front = c_extrude_depth;
    float back =  c_extrude_depth - step;

    for (int i = 0; i < steps; i++) {
        DrVec3 n;
        n = DrVec3::triangleNormal(DrVec3(x1, y1, front), DrVec3(x2, y2, front), DrVec3(x1, y1, back));

        add(DrVec3(x1, y1, front), n, DrVec2(tx1, ty1), Triangle_Point::Point1);
        add(DrVec3(x1, y1, back),  n, DrVec2(tx1, ty1), Triangle_Point::Point2);
        add(DrVec3(x2, y2, front), n, DrVec2(tx2, ty2), Triangle_Point::Point3);

        n = DrVec3::triangleNormal(DrVec3(x2, y2, front), DrVec3(x2, y2, back), DrVec3(x1, y1, back));

        add(DrVec3(x2, y2, front), n, DrVec2(tx2, ty2), Triangle_Point::Point1);
        add(DrVec3(x1, y1, back),  n, DrVec2(tx1, ty1), Triangle_Point::Point2);
        add(DrVec3(x2, y2, back),  n, DrVec2(tx2, ty2), Triangle_Point::Point3);

        front -= step;
        back  -= step;
    }

}














