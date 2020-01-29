//
//      Created by Stephens Nunnally on 1/24/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "3rd_party/delaunator.h"
#include "engine/engine.h"
#include "engine/mesh/engine_mesh.h"
#include "engine/opengl/opengl.h"
#include "engine/things/engine_thing_object.h"
#include "engine/world/engine_world.h"


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
//##    Grabs a Vertex at index from Array avoiding index out of bounds
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
//##    Grabs a DrEngineObject at index from Array avoiding index out of bounds
//####################################################################################
DrEngineObject* getEngineObject(std::vector<DrEngineObject*> &objects, int get_at) {
    if (get_at < 0) {
        return objects[ objects.size() + get_at ];
    } else if (get_at > static_cast<int>(objects.size()-1)) {
        return objects[ get_at - objects.size() ];
    } else {
        return objects[get_at];
    }
}


//####################################################################################
//##    Recalculates Soft Body Mesh
//##        RETURNS: true if ready to render, false if there was an error
//####################################################################################
bool DrOpenGL::calculateSoftBodyMesh(DrEngineObject *object, Body_Style body_style, bool delaunator) {
    if (object == nullptr) return false;
    if (object->soft_balls.size() < 3) return false;

    // ***** Calculate Object Angle
    if (object->canRotate()) {
//        DrEngineObject *first_ball = m_engine->getCurrentWorld()->findObjectByKey(object->soft_balls[0]);
//        if (first_ball == nullptr) return false;
//        double angle_adjust = object->soft_start_angle;/// - g_double;
//        ///g_info = "Angle Adjust: " + std::to_string(angle_adjust) + ", Global Double: " + std::to_string(g_double);
//        object->setAngle(Dr::CalcRotationAngleInDegrees(object->getPosition(), first_ball->getPosition()) - angle_adjust);
    }

    // ***** Calculate Current Points
    std::vector<DrEngineObject*> balls;
    std::vector<Vertex> vertices;
    for (size_t i = 0; i < object->soft_balls.size(); ++i) {
        DrEngineObject *next_ball = m_engine->getCurrentWorld()->findObjectByKey(object->soft_balls[i]);
        if (next_ball == nullptr) return false;
        balls.push_back(next_ball);
        DrPointF unrotated = Dr::RotatePointAroundOrigin(next_ball->getPosition(), object->getPosition(), -object->getAngle());
                 unrotated = unrotated - object->getPosition();
                 unrotated = unrotated / object->soft_size.x;             // Equalize mesh from -0.5 to +0.5
                 // Scale soft balls to outer radius
                 if (body_style == Body_Style::Square_Blob || body_style == Body_Style::Mesh_Blob) {
                     unrotated.x = unrotated.x * object->soft_scale.x;
                     unrotated.y = unrotated.y * object->soft_scale.y;
                 } else if (body_style == Body_Style::Circular_Blob) {
                     unrotated = unrotated * object->soft_scale.x;
                 }
                 unrotated.y = unrotated.y / object->height_width_ratio;
        vertices.push_back(Vertex::createVertex(DrVec3(unrotated.x,unrotated.y,0.0), c_up_vector_z, DrVec3(object->soft_uv[i].x,object->soft_uv[i].y,0.0), DrVec3(0,0,0)));
    }

    // ***** Smooth Points
    std::vector<DrVec3> smoothed_points;
    for (int i = 0; i < static_cast<int>(vertices.size()); ++i) {
        DrVec3 average (0.0, 0.0, 0.0);
        float  total_weight =   0.f;
        float  weight =         0.f;
        if (body_style == Body_Style::Circular_Blob) {
            weight = 1.0f;
            average += getVertex(vertices, i).position * weight;    total_weight += weight;
            weight = 1.00f;
            average += getVertex(vertices, i-1).position * weight;  total_weight += weight;
            average += getVertex(vertices, i+1).position * weight;  total_weight += weight;
            weight = 0.50f;
            average += getVertex(vertices, i-2).position * weight;  total_weight += weight;
            average += getVertex(vertices, i+2).position * weight;  total_weight += weight;
            average = (average) / total_weight;
        } else if (body_style == Body_Style::Square_Blob) {
            weight = 1.0f;
            average += getVertex(vertices, i).position * weight;            total_weight += weight;

            if (getEngineObject(balls, i-1)->soft_corner == false &&
                getEngineObject(balls, i  )->soft_corner == false &&
                getEngineObject(balls, i+1)->soft_corner == false) {
                weight = 0.75f;
                average += getVertex(vertices, i-1).position * weight;      total_weight += weight;
                average += getVertex(vertices, i+1).position * weight;      total_weight += weight;

                if (getEngineObject(balls, i-2)->soft_corner == false &&
                    getEngineObject(balls, i+2)->soft_corner == false) {
                    weight = 0.50f;
                    average += getVertex(vertices, i-2).position * weight;  total_weight += weight;
                    average += getVertex(vertices, i+2).position * weight;  total_weight += weight;
                }
            } else if (getEngineObject(balls, i)->soft_corner == false) {
                weight = 0.50f;
                average += getVertex(vertices, i-1).position * weight;      total_weight += weight;
                average += getVertex(vertices, i+1).position * weight;      total_weight += weight;
            } else {
                weight = 0.05f;
                average += getVertex(vertices, i-1).position * weight; total_weight += weight;
                average += getVertex(vertices, i+1).position * weight; total_weight += weight;
            }
            average = (average) / total_weight;
        } else if (body_style == Body_Style::Mesh_Blob) {
            weight = 1.0f;
            average += getVertex(vertices, i).position * weight;    total_weight += weight;
            average = (average) / total_weight;
        }
        smoothed_points.push_back( average);
    }
    // Updated smoothed points
    for (size_t i = 0; i < vertices.size(); ++i) {
        vertices[i].position = smoothed_points[i];
    }

    // ***** Clear Mesh
    object->m_soft_vertices.clear();
    object->m_soft_texture_coordinates.clear();
    object->m_soft_barycentric.clear();
    object->m_soft_triangles = 0;

    // Square Mesh
    if (body_style == Body_Style::Mesh_Blob) {

        for (long y = 1; y < object->soft_grid_size.y; y++) {
            for (long x = 1; x < object->soft_grid_size.x; x++) {
                long corner_tr = ((y  ) * object->soft_grid_size.x) + (x  );                // Top Right
                long corner_tl = ((y  ) * object->soft_grid_size.x) + (x-1);                // Top Left
                long corner_bl = ((y-1) * object->soft_grid_size.x) + (x-1);                // Bottom Left
                long corner_br = ((y-1) * object->soft_grid_size.x) + (x  );                // Bottom Right

                addSoftTriangle(object, vertices[corner_tr], vertices[corner_tl], vertices[corner_bl]);
                addSoftTriangle(object, vertices[corner_tr], vertices[corner_bl], vertices[corner_br]);
            }
        }

    // Delaunay Triangulation
    } else if (delaunator) {
        // Add Center Point
        ///vertices.push_back(Vertex::createVertex(DrVec3( 0.0, 0.0,0.0), c_up_vector_z, DrVec3(0.5,0.5,0.0), DrVec3(0,0,0)));

        // Corner Points
        ///vertices.push_back(Vertex::createVertex(DrVec3(-0.5, 0.5,0.0), c_up_vector_z, DrVec3(0.0,1.0,0.0), DrVec3(0,0,0)));
        ///vertices.push_back(Vertex::createVertex(DrVec3( 0.5, 0.5,0.0), c_up_vector_z, DrVec3(1.0,1.0,0.0), DrVec3(0,0,0)));
        ///vertices.push_back(Vertex::createVertex(DrVec3( 0.5,-0.5,0.0), c_up_vector_z, DrVec3(1.0,0.0,0.0), DrVec3(0,0,0)));
        ///vertices.push_back(Vertex::createVertex(DrVec3(-0.5,-0.5,0.0), c_up_vector_z, DrVec3(0.0,0.0,0.0), DrVec3(0,0,0)));

        // Get ready for Triangulation, coordinates are stored in x, y pairs
        std::vector<double> coordinates;
        for (std::size_t i = 0; i < vertices.size(); i++) {
            coordinates.push_back(static_cast<double>(vertices[i].position.x));
            coordinates.push_back(static_cast<double>(vertices[i].position.y));
        }
        if (coordinates.size() < 6) return false;                                           // We need at least 3 points!!

        // Run triangulation, add triangles to vertex data
        Delaunator d(coordinates);

        // Build Triangles
        for (size_t i = 0; i < d.triangles.size(); i += 3) {
            size_t triangle_index_1 = d.triangles[i + 0];
            size_t triangle_index_2 = d.triangles[i + 1];
            size_t triangle_index_3 = d.triangles[i + 2];
            addSoftTriangle(object, vertices[triangle_index_1], vertices[triangle_index_2], vertices[triangle_index_3]);
        }

    // Radial Triangulation
    } else {
        // Center Point
        Vertex center_v = Vertex::createVertex(DrVec3(0.0,0.0,0.0), c_up_vector_z, DrVec3(0.5,0.5,0.0), DrVec3(0,0,0));

        for (size_t i = 0; i < vertices.size() - 1; ++i) {
            addSoftTriangle(object, vertices[i], vertices[i+1], center_v);
        }
        addSoftTriangle(object, vertices[0], vertices[vertices.size()-1], center_v);
    }

    return true;
}

















