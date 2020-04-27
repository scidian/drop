//
//      Created by Stephens Nunnally on 2/2/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "3rd_party/delaunator.h"
#include "core/dr_random.h"
#include "engine/engine.h"
#include "engine/engine_signal.h"
#include "engine/mesh/engine_mesh.h"
#include "engine/thing/components/thing_comp_physics.h"
#include "engine/thing/components/thing_comp_soft_body.h"
#include "engine/thing/engine_thing.h"
#include "engine/world/engine_world.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
ThingCompSoftBody::ThingCompSoftBody(DrEngineWorld *engine_world, DrEngineThing *parent_thing)
    : DrThingComponent(engine_world, parent_thing, Comps::Thing_Soft_Body) {

}

ThingCompSoftBody::~ThingCompSoftBody() {

}



//####################################################################################
//##    Basic Virtual Component Functions
//####################################################################################




//####################################################################################
//##    Adds 1 Triangle to Soft Body Mesh
//####################################################################################
void ThingCompSoftBody::addSoftTriangle(Vertex v1, Vertex v2, Vertex v3) {
    m_soft_vertices.push_back(v1.position.x);    m_soft_vertices.push_back(v1.position.y);    m_soft_vertices.push_back(v1.position.z);
    m_soft_vertices.push_back(v2.position.x);    m_soft_vertices.push_back(v2.position.y);    m_soft_vertices.push_back(v2.position.z);
    m_soft_vertices.push_back(v3.position.x);    m_soft_vertices.push_back(v3.position.y);    m_soft_vertices.push_back(v3.position.z);

    m_soft_texture_coordinates.push_back(v1.texture_coords.x);  m_soft_texture_coordinates.push_back(v1.texture_coords.y);
    m_soft_texture_coordinates.push_back(v2.texture_coords.x);  m_soft_texture_coordinates.push_back(v2.texture_coords.y);
    m_soft_texture_coordinates.push_back(v3.texture_coords.x);  m_soft_texture_coordinates.push_back(v3.texture_coords.y);

    m_soft_barycentric.push_back(1.0);  m_soft_barycentric.push_back(0.0);  m_soft_barycentric.push_back(0.0);
    m_soft_barycentric.push_back(0.0);  m_soft_barycentric.push_back(1.0);  m_soft_barycentric.push_back(0.0);
    m_soft_barycentric.push_back(0.0);  m_soft_barycentric.push_back(0.0);  m_soft_barycentric.push_back(1.0);

    m_soft_triangles++;
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
//##    Grabs a DrEngineThing at index from Array avoiding index out of bounds
//####################################################################################
DrEngineThing* getEngineThing(std::vector<DrEngineThing*> &objects, int get_at) {
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
bool ThingCompSoftBody::calculateSoftBodyMesh(Body_Style body_style, Soft_Mesh_Style mesh_style) {
    ThingCompPhysics *physics = thing()->compPhysics();
    if (physics == nullptr) return false;
    if (soft_balls.size() < 3) return false;

    // ***** Adjust shakiness of non-rotating soft bodies
    double angle_diff = -thing()->getAngle();
    if (physics->canRotate() == false) {
        ///if (body_style == Body_Style::Circular_Blob) {
            DrEngineThing *first_ball = world()->findPhysicsObjectByKey(soft_balls[0]);
            if (first_ball == nullptr) return false;
            double angle_start = soft_start_angle;
            double angle_now =   Dr::CalcRotationAngleInDegrees(thing()->getPosition(), first_ball->getPosition());
            angle_diff =  angle_start - angle_now;
            ///g_info = "Start Angle: " +   std::to_string(angle_start) + ", Angle Now: " + std::to_string(angle_now) + ", Diff: " +    std::to_string(angle_diff);
        ///}
    }

    // ***** Calculate Current Points
    std::vector<DrEngineThing*> balls;
    std::vector<Vertex> vertices;
    for (size_t i = 0; i < soft_balls.size(); ++i) {
        DrEngineThing *next_ball = world()->findPhysicsObjectByKey(soft_balls[i]);
        if (next_ball == nullptr) return false;
        ThingCompSoftBody *next_body = next_ball->compSoftBody();
        if (next_body == nullptr) return false;

        balls.push_back(next_ball);
        DrPointF unrotated = Dr::RotatePointAroundOrigin(next_ball->getPosition(), thing()->getPosition(), angle_diff);
                 unrotated = unrotated - thing()->getPosition();
                 // Scale soft balls to outer radius
                 if (body_style == Body_Style::Square_Blob || body_style == Body_Style::Mesh_Blob) {
                     unrotated.x = unrotated.x * soft_scale.x;
                     unrotated.y = unrotated.y * soft_scale.y;
                 } else if (body_style == Body_Style::Circular_Blob) {
                     unrotated = unrotated * soft_scale.x;
                 }
                 next_body->soft_position = unrotated;

                 unrotated.y = unrotated.y / height_width_ratio;
                 unrotated = unrotated / soft_size.x;               // Equalize mesh from -0.5 to +0.5 (due to how scale is handled during opengl_render_object.cpp)
        vertices.push_back(Vertex::createVertex(DrVec3(unrotated.x,unrotated.y,0.0), c_up_vector_z, DrVec3(soft_uv[i].x, soft_uv[i].y,0.0), DrVec3(0,0,0)));
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

            if (getEngineThing(balls, i-1)->compSoftBody()->soft_corner == false &&
                getEngineThing(balls, i  )->compSoftBody()->soft_corner == false &&
                getEngineThing(balls, i+1)->compSoftBody()->soft_corner == false) {
                weight = 0.75f;
                average += getVertex(vertices, i-1).position * weight;      total_weight += weight;
                average += getVertex(vertices, i+1).position * weight;      total_weight += weight;

                if (getEngineThing(balls, i-2)->compSoftBody()->soft_corner == false &&
                    getEngineThing(balls, i+2)->compSoftBody()->soft_corner == false) {
                    weight = 0.50f;
                    average += getVertex(vertices, i-2).position * weight;  total_weight += weight;
                    average += getVertex(vertices, i+2).position * weight;  total_weight += weight;
                }
            } else if (getEngineThing(balls, i)->compSoftBody()->soft_corner == false) {
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
    m_soft_vertices.clear();
    m_soft_texture_coordinates.clear();
    m_soft_barycentric.clear();
    m_soft_triangles = 0;

    // Square Mesh
    if (mesh_style == Soft_Mesh_Style::Grid_Square) {

        for (long y = 1; y < soft_grid_size.y; y++) {
            for (long x = 1; x < soft_grid_size.x; x++) {
                long corner_tr = ((y  ) * soft_grid_size.x) + (x  );                 // Top Right
                long corner_tl = ((y  ) * soft_grid_size.x) + (x-1);                 // Top Left
                long corner_bl = ((y-1) * soft_grid_size.x) + (x-1);                 // Bottom Left
                long corner_br = ((y-1) * soft_grid_size.x) + (x  );                 // Bottom Right

                addSoftTriangle(vertices[corner_tr], vertices[corner_tl], vertices[corner_bl]);
                addSoftTriangle(vertices[corner_tr], vertices[corner_bl], vertices[corner_br]);
            }
        }

    // Delaunay Triangulation
    } else if (mesh_style == Soft_Mesh_Style::Delaunay) {
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
            addSoftTriangle(vertices[triangle_index_1], vertices[triangle_index_2], vertices[triangle_index_3]);
        }

    // Radial Triangulation
    } else {
        // Center Point
        Vertex center_v = Vertex::createVertex(DrVec3(0.0,0.0,0.0), c_up_vector_z, DrVec3(0.5,0.5,0.0), DrVec3(0,0,0));

        for (size_t i = 0; i < vertices.size() - 1; ++i) {
            addSoftTriangle(vertices[i], vertices[i+1], center_v);
        }
        addSoftTriangle(vertices[0], center_v, vertices[vertices.size()-1]);
    }

    return true;
}
















