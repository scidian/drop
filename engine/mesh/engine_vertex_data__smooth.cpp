//
//      Created by Stephens Nunnally on 9/19/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//  For some same code framework for Lapacian Smooting see:
//      http://rodolphe-vaillant.fr/?c=code
//      https://github.com/silenthell/harmonic_weights_triangle_mesh
//
#include <limits>

#include "3rd_party/glm/vec3.hpp"
#include "engine/engine_texture.h"
#include "engine/mesh/engine_vertex_data.h"
#include "library/imaging/imaging.h"
#include "library/types/dr_vec3.h"


//####################################################################################
//##    Rounds to two decimal places, this keeps neighbors
//####################################################################################
float roundToDecimalPlace(float number_to_round, int decimal_places) {
    // If rounding to decimal_places == 2:
    //      37.66666 * 100 = 3766.66
    //      3766.66 + .5 =   3767.16    for rounding off value
    //      then cast to int so value is 3767
    //      then divide by 100 so the value converted into 37.67
    float power = powf(10.f, decimal_places);
    float value = static_cast<int>(number_to_round * power + 0.5f);
    return (static_cast<float>(value) / power);
}

//####################################################################################
//##    Loads singular vertex from Data Array
//####################################################################################
Vertex DrEngineVertexData::getVertex(int vertex_number) {
    GLfloat *p = m_data.data() + (vertex_number * c_vertex_length);
    Vertex v;
    v.position.x =          *p++;
    v.position.y =          *p++;
    v.position.z =          *p++;
    v.normal.x =            *p++;
    v.normal.y =            *p++;
    v.normal.z =            *p++;
    v.texture_coords.x =    *p++;
    v.texture_coords.y =    *p++;
    v.barycentric.x =       *p++;
    v.barycentric.y =       *p++;
    v.barycentric.z =       *p++;

    v.position.z *= 10.f;
    v.position.x = roundToDecimalPlace(v.position.x, 1);
    v.position.y = roundToDecimalPlace(v.position.y, 1);
    v.position.z = roundToDecimalPlace(v.position.z, 1);
    return v;
}

//####################################################################################
//##    Sets a singular Vertex into Data Array
//####################################################################################
void DrEngineVertexData::setVertex(int vertex_number, Vertex v) {
    GLfloat *p = m_data.data() + (vertex_number * c_vertex_length);
    *p++ = v.position.x;
    *p++ = v.position.y;
    *p++ = v.position.z / 10.f;
    *p++ = v.normal.x;
    *p++ = v.normal.y;
    *p++ = v.normal.z;
    *p++ = v.texture_coords.x;
    *p++ = v.texture_coords.y;
    *p++ = v.barycentric.x;
    *p++ = v.barycentric.y;
    *p++ = v.barycentric.z;
}


//####################################################################################
//##    Adds point_to_add to neighbor_list if point is not already included
//####################################################################################
void addNeighbor(std::vector<Vertex> &neighbor_list, Vertex point_to_add) {
    for (auto &point : neighbor_list) {
        if (point.position == point_to_add.position)
            return;
    }
    neighbor_list.push_back(point_to_add);
}

//####################################################################################
//##    Loads Data Array into Mesh (list of triangles)
//####################################################################################
Mesh DrEngineVertexData::getMesh(NeighborMap &neighbors) {
    Mesh mesh;
    for (int i = 0; i < vertexCount(); i += 3) {
        Vertex point0 = getVertex(i);
        Vertex point1 = getVertex(i+1);
        Vertex point2 = getVertex(i+2);

        addNeighbor(neighbors[point0.position], point1);
        addNeighbor(neighbors[point0.position], point2);
        addNeighbor(neighbors[point1.position], point0);
        addNeighbor(neighbors[point1.position], point2);
        addNeighbor(neighbors[point2.position], point0);
        addNeighbor(neighbors[point2.position], point1);

        Triangle tri;
        tri.points.push_back( point0 );
        tri.points.push_back( point1 );
        tri.points.push_back( point2 );
        mesh.m_triangles.push_back(tri);
    }
    return mesh;
}


//####################################################################################
//##    Cotangent weights over a triangular mesh
//##        Source: http://rodolphe-vaillant.fr/?e=69
//####################################################################################
float LaplacianPositiveCotanWeight(Vertex vertex_i, Vertex edge_j, Vertex edge_j_previous, Vertex edge_j_next ) {
    DrVec3 pi(vertex_i.position);
    DrVec3 pj(edge_j.position);
    DrVec3 pj_prev(edge_j_previous.position);
    DrVec3 pj_next(edge_j_next.position);

    float e1 = (pi      - pj     ).norm();
    float e2 = (pi      - pj_prev).norm();
    float e3 = (pj_prev - pj     ).norm();
    // NOTE: cos(alpha) = (a^2.b^2  - c^2) / (2.a.b)
    //       with a, b, c the lengths of of the sides of the triangle and (a, b) forming the angle alpha
    float cos_alpha = fabs((e3*e3 + e2*e2 - e1*e1) / (2.0f*e3*e2));

    float e4 = (pi      - pj_next).norm();
    float e5 = (pj_next - pj     ).norm();
    float cos_beta = fabs((e4*e4 + e5*e5 - e1*e1) / (2.0f*e4*e5));

    // NOTE: cot(x) = cos(x)/sin(x)
    //       and recall cos(x)^2 + sin(x)^2 = 1
    //       then sin(x) = sqrt(1-cos(x))
    float cotan1 = cos_alpha / sqrt(1.0f - cos_alpha * cos_alpha);
    float cotan2 = cos_beta  / sqrt(1.0f - cos_beta  * cos_beta );

    // If the mesh is not a water-tight closed volume we must check for edges lying on the sides of wholes
//    if (mesh.is_vert_on_side(vertex_i) && mesh.is_vert_on_side(vertex_j)) {
//        if (vertex_j_next == vertex_j_prev) {
//            cotan2 = 0.0f;
//        } else {
//            if (mesh.is_vert_on_side(vertex_i) && mesh.is_vert_on_side(vertex_j_next))
//                cotan2 = 0.0;
//            else
//                cotan1 = 0.0f;
//        }
//    }

    // wij = (cot(alpha) + cot(beta))
    float wij = (cotan1 + cotan2) / 2.0f;
    if (std::isnan(wij)) wij = 0.0f;

    // Compute the cotangent value close to 0.0f, as cotan approaches infinity close to zero we clamp higher values
    const float eps = 1e-6f;
    const float cotan_max = cos( eps ) / sin( eps );
    if (wij >= cotan_max) wij = cotan_max;

    return wij;
}



//####################################################################################
//##    Smooths Vertices based on 'weight' of neighbors, recalculates normals
//####################################################################################
void DrEngineVertexData::smoothVertices(float weight) {

    // ***** Get Data Array into Mesh, find neighbors
    NeighborMap neighbors;
    Mesh mesh = getMesh(neighbors);

    // ***** Smooth points
    for (auto &triangle : mesh.m_triangles) {
        for (auto &point : triangle.points) {
            DrVec3  position(0.f);
            DrVec3  normals(0.f);
            DrVec3  texture(0.f);
            float   total_weight = 0.f;

            std::size_t prev = neighbors[point.position].size() - 1;
            std::size_t next = 1;
            for (std::size_t i = 0; i < neighbors[point.position].size(); i++) {
                Vertex neighbor = neighbors[point.position][i];
                ///Vertex neighbor_prev = neighbors[point.position][prev];
                ///Vertex neighbor_next = neighbors[point.position][next];

                // #NOTE: If we set neighbor_weight = 1.0f, the procedure operates a uniform smoothing,
                //        the initial distribution of the triangles won't be preserved
                float neighbor_weight = weight;
                ///float distance = QVector3D(point.position.x, point.position.y, point.position.z).distanceToPoint(
                ///                           QVector3D(neighbor.position.x, neighbor.position.y, neighbor.position.z));
                ///neighbor_weight *= Dr::Clamp(abs(1.0f / distance), 0.f, 1.f);
                ///neighbor_weight *= LaplacianPositiveCotanWeight(point, neighbor, neighbor_next, neighbor_prev);

                position += (neighbor_weight * neighbor.position);
                normals  += (neighbor_weight * neighbor.normal);
                texture  += (neighbor_weight * neighbor.texture_coords);
                total_weight += neighbor_weight;

                // Increment neighbors
                prev = i;
                next++;
                if (next >= neighbors[point.position].size()) next = 0;
            }

            // When using cotan weights smoothing may be unstable, in this case we need to set t < 1
            // sometimes you even need to get as low as t < 0.5
            float t = 0.9f;
            point.position =       ((position / total_weight) * t + point.position * (1.f - t)).toGlmVec3();
            point.texture_coords = ((texture /  total_weight) * t + point.texture_coords * (1.f - t)).toGlmVec3();
            point.normal =         ((normals /  total_weight) * t + point.normal * (1.f - t)).toGlmVec3();
        }
    }

    // ***** Set Mesh Data back into Data Array
    for (int i = 0; i < vertexCount(); i += 3) {
        Triangle tri = mesh.m_triangles[static_cast<std::size_t>(i / 3)];

        // Recalculate normals
        DrVec3 v1 = tri.points[2].position - tri.points[0].position;
        DrVec3 v2 = tri.points[1].position - tri.points[0].position;
        DrVec3 n = v1.cross( v2 );
        n.normalize();
        for (auto &point : tri.points) { point.normal = n; }

        setVertex(i,   tri.points[0]);
        setVertex(i+1, tri.points[1]);
        setVertex(i+2, tri.points[2]);
    }
}























