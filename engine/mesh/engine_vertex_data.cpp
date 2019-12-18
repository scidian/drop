//
//      Created by Stephens Nunnally on 8/29/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "3rd_party/glm/gtx/normal.hpp"
#include "3rd_party/glm/gtx/transform.hpp"
#include "3rd_party/glm/matrix.hpp"
#include "editor/imaging/imaging.h"
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
    GLfloat x1 = +w2, y1 = +h2;         // Top Right
    GLfloat x2 = -w2, y2 = +h2;         // Top Left
    GLfloat x3 = +w2, y3 = -h2;         // Bottom Right
    GLfloat x4 = -w2, y4 = -h2;         // Bottom Left

    GLfloat tx1 = 1.0, ty1 = 1.0;
    GLfloat tx2 = 0.0, ty2 = 1.0;
    GLfloat tx3 = 1.0, ty3 = 0.0;
    GLfloat tx4 = 0.0, ty4 = 0.0;

    glm::vec3 n = glm::triangleNormal(glm::vec3(x1, y1, 0.f), glm::vec3(x3, y3, 0.f), glm::vec3(x2, y2, 0.f));

    add(glm::vec3(x1, y1, 0.f), n, glm::vec2(tx1, ty1), Triangle_Point::Point1);
    add(glm::vec3(x2, y2, 0.f), n, glm::vec2(tx2, ty2), Triangle_Point::Point2);
    add(glm::vec3(x3, y3, 0.f), n, glm::vec2(tx3, ty3), Triangle_Point::Point3);
    add(glm::vec3(x2, y2, 0.f), n, glm::vec2(tx2, ty2), Triangle_Point::Point1);
    add(glm::vec3(x4, y4, 0.f), n, glm::vec2(tx4, ty4), Triangle_Point::Point2);
    add(glm::vec3(x3, y3, 0.f), n, glm::vec2(tx3, ty3), Triangle_Point::Point3);
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
    GLfloat x1 = +w2, y1 = +h2;         // Top Right
    GLfloat x2 = -w2, y2 = +h2;         // Top Left
    GLfloat x3 = +w2, y3 = -h2;         // Bottom Right
    GLfloat x4 = -w2, y4 = -h2;         // Bottom Left

    GLfloat tx1 = 1.0, ty1 = 1.0;
    GLfloat tx2 = 0.0, ty2 = 1.0;
    GLfloat tx3 = 1.0, ty3 = 0.0;
    GLfloat tx4 = 0.0, ty4 = 0.0;

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
    GLfloat x1,   y1,  x2,  y2,  x3,  y3,  x4,  y4;
    GLfloat tx1, ty1, tx2, ty2, tx3, ty3, tx4, ty4;

    x1 =    0;  y1 = +h2;               // Top
    x2 =  -w2;  y2 = -h2;               // Bottom Left
    x3 =  +w2;  y3 = -h2;               // Bottom Right
    tx1 = 0.5; ty1 = 0.5;
    tx2 = 0.0; ty2 = 0.0;
    tx3 = 1.0; ty3 = 0.0;

    glm::vec3 n = glm::triangleNormal(glm::vec3(x1, y1, 0.f), glm::vec3(x3, y3, 0.f), glm::vec3(x2, y2, 0.f));
    glm::vec3 point_t( x1, y1,                0);
    glm::vec3 point_bl(x2, y2, +c_extrude_depth);
    glm::vec3 point_br(x3, y3, +c_extrude_depth);

    glm::mat4 rotate = glm::mat4(1.0);

    for (int i = 0; i <= 4; ++i) {
        add(point_t , n, glm::vec2(tx1, ty1), Triangle_Point::Point1);
        add(point_bl, n, glm::vec2(tx2, ty2), Triangle_Point::Point2);
        add(point_br, n, glm::vec2(tx3, ty3), Triangle_Point::Point3);

        rotate = glm::rotate(rotate, Dr::DegreesToRadians(90.f), glm::vec3(0.0, 1.0, 0.0));

        point_t =  glm::vec3(rotate * glm::vec4(point_t, 1.f));
        point_bl = glm::vec3(rotate * glm::vec4(point_bl, 1.f));
        point_br = glm::vec3(rotate * glm::vec4(point_br, 1.f));
        n =        glm::vec3(rotate * glm::vec4(n, 1.f));
    }


    // Bottom Square
    x1 =  +w2;  y1 = +h2;               // Top Right
    x2 =  -w2;  y2 = +h2;               // Top Left
    x3 =  +w2;  y3 = -h2;               // Bottom Right
    x4 =  -w2;  y4 = -h2;               // Bottom Left
    tx1 = 1.0; ty1 = 1.0;
    tx2 = 0.0; ty2 = 1.0;
    tx3 = 1.0; ty3 = 0.0;
    tx4 = 0.0; ty4 = 0.0;

    rotate = glm::mat4(1.0);                            // Set to identity matrix
    glm::vec3  nf;                                      // Normal Front
    glm::vec3  p1f, p2f, p3f, p4f;                      // Point 1 Front, etc

    nf = glm::triangleNormal(glm::vec3(x1, y1, 0.f), glm::vec3(x3, y3, 0.f), glm::vec3(x2, y2, 0.f));
    p1f = glm::vec3(x1, y1, +c_extrude_depth);
    p2f = glm::vec3(x2, y2, +c_extrude_depth);
    p3f = glm::vec3(x3, y3, +c_extrude_depth);
    p4f = glm::vec3(x4, y4, +c_extrude_depth);

    rotate = glm::rotate(rotate, Dr::DegreesToRadians(90.f), glm::vec3(1.0, 0.0, 0.0));

    nf =    glm::vec3(rotate * glm::vec4(nf, 1.f));
    p1f =   glm::vec3(rotate * glm::vec4(p1f, 1.f));
    p2f =   glm::vec3(rotate * glm::vec4(p2f, 1.f));
    p3f =   glm::vec3(rotate * glm::vec4(p3f, 1.f));
    p4f =   glm::vec3(rotate * glm::vec4(p4f, 1.f));

    add(p1f, nf, glm::vec2(tx1, ty1), Triangle_Point::Point1);
    add(p2f, nf, glm::vec2(tx2, ty2), Triangle_Point::Point2);
    add(p3f, nf, glm::vec2(tx3, ty3), Triangle_Point::Point3);
    add(p2f, nf, glm::vec2(tx2, ty2), Triangle_Point::Point1);
    add(p4f, nf, glm::vec2(tx4, ty4), Triangle_Point::Point2);
    add(p3f, nf, glm::vec2(tx3, ty3), Triangle_Point::Point3);
}




//####################################################################################
//##    Adds a Vertex, including:
//##        Vec3 Position
//##        Vec3 Normal
//##        Vec2 UV Texture Coordinates
//##        Vec3 Barycentric Coordinates (gives shader a number between 0.0 and 1.0 to lerp to)
//####################################################################################
void DrEngineVertexData::add(const glm::vec3 &vertex, const glm::vec3 &normal, const glm::vec2 &text_coord, Triangle_Point point_number) {
    if (static_cast<size_t>(m_count + c_vertex_length) > m_data.size()) m_data.resize(m_data.size() + (100 * c_vertex_length));
    GLfloat *p = m_data.data() + m_count;
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
void DrEngineVertexData::cube(GLfloat x1, GLfloat y1, GLfloat tx1, GLfloat ty1,
                              GLfloat x2, GLfloat y2, GLfloat tx2, GLfloat ty2,
                              GLfloat x3, GLfloat y3, GLfloat tx3, GLfloat ty3,
                              GLfloat x4, GLfloat y4, GLfloat tx4, GLfloat ty4) {
    glm::mat4 rotate = glm::mat4(1.0);
    glm::vec3 nf, nb;                                   // Normal Front, Normal Back
    glm::vec3 p1f, p2f, p3f, p4f;                       // Point 1 Front, etc
    glm::vec3 p1b, p2b, p3b, p4b;                       // Point 1 Back, etc

    for (int i = 0; i <= 2; ++i) {
        nf = glm::triangleNormal(glm::vec3(x1, y1, 0.f), glm::vec3(x3, y3, 0.f), glm::vec3(x2, y2, 0.f));
        nb = glm::triangleNormal(glm::vec3(x1, y1, 0.f), glm::vec3(x2, y2, 0.f), glm::vec3(x3, y3, 0.f));
        p1f = glm::vec3(x1, y1, +c_extrude_depth);
        p2f = glm::vec3(x2, y2, +c_extrude_depth);
        p3f = glm::vec3(x3, y3, +c_extrude_depth);
        p4f = glm::vec3(x4, y4, +c_extrude_depth);
        p1b = glm::vec3(x1, y1, -c_extrude_depth);
        p2b = glm::vec3(x2, y2, -c_extrude_depth);
        p3b = glm::vec3(x3, y3, -c_extrude_depth);
        p4b = glm::vec3(x4, y4, -c_extrude_depth);

        if (i == 1)
            rotate = glm::rotate(rotate, Dr::DegreesToRadians(90.f), glm::vec3(0.0, 1.0, 0.0));
        else if (i == 2)
            rotate = glm::rotate(rotate, Dr::DegreesToRadians(90.f), glm::vec3(1.0, 0.0, 0.0));

        nf =    glm::vec3(rotate * glm::vec4(nf, 1.f));
        p1f =   glm::vec3(rotate * glm::vec4(p1f, 1.f));
        p2f =   glm::vec3(rotate * glm::vec4(p2f, 1.f));
        p3f =   glm::vec3(rotate * glm::vec4(p3f, 1.f));
        p4f =   glm::vec3(rotate * glm::vec4(p4f, 1.f));

        nb =    glm::vec3(rotate * glm::vec4(nb, 1.f));
        p1b =   glm::vec3(rotate * glm::vec4(p1b, 1.f));
        p2b =   glm::vec3(rotate * glm::vec4(p2b, 1.f));
        p3b =   glm::vec3(rotate * glm::vec4(p3b, 1.f));
        p4b =   glm::vec3(rotate * glm::vec4(p4b, 1.f));

        add(p1f, nf, glm::vec2(tx1, ty1), Triangle_Point::Point1);
        add(p2f, nf, glm::vec2(tx2, ty2), Triangle_Point::Point2);
        add(p3f, nf, glm::vec2(tx3, ty3), Triangle_Point::Point3);
        add(p2f, nf, glm::vec2(tx2, ty2), Triangle_Point::Point1);
        add(p4f, nf, glm::vec2(tx4, ty4), Triangle_Point::Point2);
        add(p3f, nf, glm::vec2(tx3, ty3), Triangle_Point::Point3);

        add(p1b, nb, glm::vec2(tx1, ty1), Triangle_Point::Point1);
        add(p3b, nb, glm::vec2(tx3, ty3), Triangle_Point::Point2);
        add(p2b, nb, glm::vec2(tx2, ty2), Triangle_Point::Point3);
        add(p2b, nb, glm::vec2(tx2, ty2), Triangle_Point::Point1);
        add(p3b, nb, glm::vec2(tx3, ty3), Triangle_Point::Point2);
        add(p4b, nb, glm::vec2(tx4, ty4), Triangle_Point::Point3);
    }
}


//####################################################################################
//##    Adds a Quad, front and back
//####################################################################################
void DrEngineVertexData::quad(GLfloat x1, GLfloat y1, GLfloat tx1, GLfloat ty1,
                              GLfloat x2, GLfloat y2, GLfloat tx2, GLfloat ty2,
                              GLfloat x3, GLfloat y3, GLfloat tx3, GLfloat ty3,
                              GLfloat x4, GLfloat y4, GLfloat tx4, GLfloat ty4) {
    glm::vec3 n;
    n = glm::triangleNormal(glm::vec3(x1, y1, 0.f), glm::vec3(x3, y3, 0.f), glm::vec3(x2, y2, 0.f));

    add(glm::vec3(x1, y1, +c_extrude_depth), n, glm::vec2(tx1, ty1), Triangle_Point::Point1);
    add(glm::vec3(x2, y2, +c_extrude_depth), n, glm::vec2(tx2, ty2), Triangle_Point::Point2);
    add(glm::vec3(x3, y3, +c_extrude_depth), n, glm::vec2(tx3, ty3), Triangle_Point::Point3);

    add(glm::vec3(x2, y2, +c_extrude_depth), n, glm::vec2(tx2, ty2), Triangle_Point::Point1);
    add(glm::vec3(x4, y4, +c_extrude_depth), n, glm::vec2(tx4, ty4), Triangle_Point::Point2);
    add(glm::vec3(x3, y3, +c_extrude_depth), n, glm::vec2(tx3, ty3), Triangle_Point::Point3);

    n = glm::triangleNormal(glm::vec3(x1, y1, 0.f), glm::vec3(x2, y2, 0.f), glm::vec3(x3, y3, 0.f));

    add(glm::vec3(x1, y1, -c_extrude_depth), n, glm::vec2(tx1, ty1), Triangle_Point::Point1);
    add(glm::vec3(x3, y3, -c_extrude_depth), n, glm::vec2(tx3, ty3), Triangle_Point::Point2);
    add(glm::vec3(x2, y2, -c_extrude_depth), n, glm::vec2(tx2, ty2), Triangle_Point::Point3);

    add(glm::vec3(x2, y2, -c_extrude_depth), n, glm::vec2(tx2, ty2), Triangle_Point::Point1);
    add(glm::vec3(x3, y3, -c_extrude_depth), n, glm::vec2(tx3, ty3), Triangle_Point::Point2);
    add(glm::vec3(x4, y4, -c_extrude_depth), n, glm::vec2(tx4, ty4), Triangle_Point::Point3);
}

//####################################################################################
//##    Adds a Triangle, front and back
//####################################################################################
void DrEngineVertexData::triangle(GLfloat x1, GLfloat y1, GLfloat tx1, GLfloat ty1,
                                  GLfloat x2, GLfloat y2, GLfloat tx2, GLfloat ty2,
                                  GLfloat x3, GLfloat y3, GLfloat tx3, GLfloat ty3) {
    glm::vec3 n;
    n = glm::triangleNormal(glm::vec3(x1, y1, 0.f), glm::vec3(x3, y3, 0.f), glm::vec3(x2, y2, 0.f));

    add(glm::vec3(x1, y1, +c_extrude_depth), n, glm::vec2(tx1, ty1), Triangle_Point::Point1);
    add(glm::vec3(x2, y2, +c_extrude_depth), n, glm::vec2(tx2, ty2), Triangle_Point::Point2);
    add(glm::vec3(x3, y3, +c_extrude_depth), n, glm::vec2(tx3, ty3), Triangle_Point::Point3);

    n = glm::triangleNormal(glm::vec3(x1, y1, 0.f), glm::vec3(x2, y2, 0.f), glm::vec3(x3, y3, 0.f));

    add(glm::vec3(x1, y1, -c_extrude_depth), n, glm::vec2(tx1, ty1), Triangle_Point::Point1);
    add(glm::vec3(x3, y3, -c_extrude_depth), n, glm::vec2(tx3, ty3), Triangle_Point::Point2);
    add(glm::vec3(x2, y2, -c_extrude_depth), n, glm::vec2(tx2, ty2), Triangle_Point::Point3);
}

//####################################################################################
//##    Adds a Quad extruded from an Edge
//####################################################################################
void DrEngineVertexData::extrude(GLfloat x1, GLfloat y1, GLfloat tx1, GLfloat ty1,
                                 GLfloat x2, GLfloat y2, GLfloat tx2, GLfloat ty2, int steps) {
    float step = (c_extrude_depth * 2.0f) / static_cast<float>(steps);
    float front = c_extrude_depth;
    float back =  c_extrude_depth - step;

    for (int i = 0; i < steps; i++) {
        glm::vec3 n;
        n = glm::triangleNormal(glm::vec3(x1, y1, front), glm::vec3(x2, y2, front), glm::vec3(x1, y1, back));

        add(glm::vec3(x1, y1, front), n, glm::vec2(tx1, ty1), Triangle_Point::Point1);
        add(glm::vec3(x1, y1, back),  n, glm::vec2(tx1, ty1), Triangle_Point::Point2);
        add(glm::vec3(x2, y2, front), n, glm::vec2(tx2, ty2), Triangle_Point::Point3);

        n = glm::triangleNormal(glm::vec3(x2, y2, front), glm::vec3(x2, y2, back), glm::vec3(x1, y1, back));

        add(glm::vec3(x2, y2, front), n, glm::vec2(tx2, ty2), Triangle_Point::Point1);
        add(glm::vec3(x1, y1, back),  n, glm::vec2(tx1, ty1), Triangle_Point::Point2);
        add(glm::vec3(x2, y2, back),  n, glm::vec2(tx2, ty2), Triangle_Point::Point3);

        front -= step;
        back  -= step;
    }

}














