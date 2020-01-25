//
//      Created by Stephens Nunnally on 1/24/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "engine/mesh/engine_mesh.h"
#include "engine/opengl/opengl.h"
#include "engine/things/engine_thing_object.h"


//####################################################################################
//##    Adds 1 Triangle to Soft Body Mesh
//####################################################################################
void addSoftTriangle(DrEngineObject *object, Vertex v1, Vertex v2, Vertex v3) {
    object->m_soft_vertices.push_back(v1.position.x);   object->m_soft_vertices.push_back(v1.position.y);   object->m_soft_vertices.push_back(v1.position.z);
    object->m_soft_vertices.push_back(v2.position.x);   object->m_soft_vertices.push_back(v2.position.y);   object->m_soft_vertices.push_back(v2.position.z);
    object->m_soft_vertices.push_back(v3.position.x);   object->m_soft_vertices.push_back(v3.position.y);   object->m_soft_vertices.push_back(v3.position.z);

    object->m_soft_texture_coordinates.push_back(v1.texture_coords.x);  object->m_soft_texture_coordinates.push_back(v1.texture_coords.y);
    object->m_soft_texture_coordinates.push_back(v2.texture_coords.x);  object->m_soft_texture_coordinates.push_back(v2.texture_coords.y);
    object->m_soft_texture_coordinates.push_back(v3.texture_coords.x);  object->m_soft_texture_coordinates.push_back(v3.texture_coords.y);

    object->m_soft_barycentric.push_back(1.0); object->m_soft_barycentric.push_back(0.0); object->m_soft_barycentric.push_back(0.0);
    object->m_soft_barycentric.push_back(0.0); object->m_soft_barycentric.push_back(1.0); object->m_soft_barycentric.push_back(0.0);
    object->m_soft_barycentric.push_back(0.0); object->m_soft_barycentric.push_back(0.0); object->m_soft_barycentric.push_back(1.0);

    object->m_soft_triangles++;
}


//####################################################################################
//##    Grabs and Vertex index from Array avoiding index out of bounds
//####################################################################################
Vertex& getVertex(std::vector<Vertex> &vertices, int get_at) {
    if (get_at < 0) {
        return vertices[ vertices.size() + get_at ];
    } else if (get_at > static_cast<int>(vertices.size()-1)) {
        return vertices[ get_at - vertices.size() ];
    } else {
        return vertices[get_at];
    }
}


//####################################################################################
//##    Recalculates Soft Body Mesh
//##        RETURNS: true if ready to render, false if there was an error
//####################################################################################
bool DrOpenGL::calculateSoftBodyMesh(DrEngineObject *object) {
    if (object == nullptr) return false;
    if (object->soft_balls.size() < 3) return false;

    // Calculate Object Angle
    DrEngineObject *first_ball = object->soft_balls[0];
    if (first_ball == nullptr) return false;
    object->setAngle(Dr::CalcRotationAngleInDegrees(object->getPosition(), first_ball->getPosition()) - 90);

    // Calculate Current Points
    std::vector<Vertex> vertices;
    for (size_t i = 0; i < object->soft_balls.size(); ++i) {
        DrEngineObject *next_ball = object->soft_balls[i];
        if (next_ball == nullptr) return false;
        DrPointF unrotated = Dr::RotatePointAroundOrigin(next_ball->getPosition(), object->getPosition(), -object->getAngle());
                 unrotated = unrotated - object->getPosition();
                 unrotated = unrotated / object->soft_diameter;             // Equalize mesh from -0.5 to +0.5
                 unrotated = unrotated * object->soft_scale;                // Scale soft balls to outer radius
                 unrotated.y = unrotated.y / object->height_width_ratio;
        Vertex v = Vertex::createVertex(DrVec3(unrotated.x,unrotated.y,0.0), c_up_vector_z, DrVec3(object->soft_uv[i].x,object->soft_uv[i].y,0.0), DrVec3(0,0,0));
        vertices.push_back(v);
    }

    // Smooth Points
    std::vector<DrVec3> smoothed_points;
    for (int i = 0; i < static_cast<int>(vertices.size()); ++i) {
        DrVec3 p1 = getVertex(vertices, i-2).position;
        DrVec3 p2 = getVertex(vertices, i-1).position;
        DrVec3 p3 = getVertex(vertices, i  ).position;
        DrVec3 p4 = getVertex(vertices, i+1).position;
        DrVec3 p5 = getVertex(vertices, i+2).position;
        DrVec3 average = (p1 + p2 + p3 + p4 + p5) / 5.0;
        smoothed_points.push_back( average );
    }
    for (size_t i = 0; i < vertices.size(); ++i) {
        vertices[i].position = smoothed_points[i];
    }

    // Center Point
    Vertex center_v = Vertex::createVertex(DrVec3(0.0,0.0,0.0), c_up_vector_z, DrVec3(0.5,0.5,0.0), DrVec3(0,0,0));

    // Build Mesh
    object->m_soft_vertices.clear();
    object->m_soft_texture_coordinates.clear();
    object->m_soft_barycentric.clear();
    object->m_soft_triangles = 0;

    for (size_t i = 0; i < vertices.size() - 1; ++i) {
        addSoftTriangle(object, vertices[i], vertices[i+1], center_v);
    }
    addSoftTriangle(object, vertices[0], vertices[vertices.size()-1], center_v);

    return true;
}























