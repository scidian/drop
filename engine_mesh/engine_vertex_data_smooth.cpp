//
//      Created by Stephens Nunnally on 9/19/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QDebug>

#include "engine/engine_texture.h"
#include "engine_mesh/engine_vertex_data.h"
#include "helper.h"
#include "imaging/imaging.h"

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
//    v.position.x = roundToDecimalPlace(v.position.x, 2);
//    v.position.y = roundToDecimalPlace(v.position.y, 2);
//    v.position.z = roundToDecimalPlace(v.position.z, 2);
    v.position.z *= 100.f;
    return v;
}

//####################################################################################
//##    Sets a singular Vertex into Data Array
//####################################################################################
void DrEngineVertexData::setVertex(int vertex_number, Vertex v) {
    GLfloat *p = m_data.data() + (vertex_number * c_vertex_length);
    *p++ = v.position.x;
    *p++ = v.position.y;
    *p++ = v.position.z / 100.f;
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
//##    Loads Data Array into Mesh (list of triangles)
//####################################################################################
Mesh DrEngineVertexData::getMesh(NeighborMap &neighbors) {
    Mesh mesh;
    for (int i = 0; i < vertexCount(); i += 3) {
        Vertex point0 = getVertex(i);
        Vertex point1 = getVertex(i+1);
        Vertex point2 = getVertex(i+2);

        std::string pos0 = Dr::RemoveTrailingDecimals(static_cast<double>(point0.position.x), 1).toStdString() + ":" +
                           Dr::RemoveTrailingDecimals(static_cast<double>(point0.position.y), 1).toStdString() + ":" +
                           Dr::RemoveTrailingDecimals(static_cast<double>(point0.position.z), 1).toStdString();
        std::string pos1 = Dr::RemoveTrailingDecimals(static_cast<double>(point1.position.x), 1).toStdString() + ":" +
                           Dr::RemoveTrailingDecimals(static_cast<double>(point1.position.y), 1).toStdString() + ":" +
                           Dr::RemoveTrailingDecimals(static_cast<double>(point1.position.z), 1).toStdString();
        std::string pos2 = Dr::RemoveTrailingDecimals(static_cast<double>(point2.position.x), 1).toStdString() + ":" +
                           Dr::RemoveTrailingDecimals(static_cast<double>(point2.position.y), 1).toStdString() + ":" +
                           Dr::RemoveTrailingDecimals(static_cast<double>(point2.position.z), 1).toStdString();
        neighbors[pos0].push_back(point1.position);
        neighbors[pos0].push_back(point2.position);
        neighbors[pos1].push_back(point0.position);
        neighbors[pos1].push_back(point2.position);
        neighbors[pos2].push_back(point0.position);
        neighbors[pos2].push_back(point1.position);

        Triangle tri;
        tri.points.push_back( point0 );
        tri.points.push_back( point1 );
        tri.points.push_back( point2 );
        mesh.m_triangles.push_back(tri);
    }
    return mesh;
}


//####################################################################################
//##    Smooths Vertices based on 'weight' of neighbors, recalculates normals
//####################################################################################
void DrEngineVertexData::smoothVertices(float weight) {
    // Get Data Array into Mesh, find neighbors
    NeighborMap neighbors;
    Mesh mesh = getMesh(neighbors);

    // Smooth points
    for (auto &triangle : mesh.m_triangles) {
        for (auto &point : triangle.points) {
            float total_used = 1.f;
            float x = point.position.x;
            float y = point.position.y;
            float z = point.position.z;

            std::string pos = Dr::RemoveTrailingDecimals(static_cast<double>(point.position.x), 1).toStdString() + ":" +
                              Dr::RemoveTrailingDecimals(static_cast<double>(point.position.y), 1).toStdString() + ":" +
                              Dr::RemoveTrailingDecimals(static_cast<double>(point.position.z), 1).toStdString();
            for (auto neighbor : neighbors[pos]) {
                x += (neighbor.x * weight);
                y += (neighbor.y * weight);
                z += (neighbor.z * weight);
                total_used += weight;
            }
            point.position.x = x / total_used;
            point.position.y = y / total_used;
            point.position.z = z / total_used;
        }
    }

    // Set Mesh Data back into Data Array
    for (int i = 0; i < vertexCount(); i += 3) {
        Triangle tri = mesh.m_triangles[static_cast<unsigned long>(i / 3)];

        // Recalculate normals
        QVector3D n = QVector3D::normal( QVector3D(tri.points[0].position.x, tri.points[0].position.y, tri.points[0].position.z),
                                         QVector3D(tri.points[2].position.x, tri.points[2].position.y, tri.points[2].position.z),
                                         QVector3D(tri.points[1].position.x, tri.points[1].position.y, tri.points[1].position.z));
        for (auto &point : tri.points) {
            point.normal.x = n.x();
            point.normal.y = n.y();
            point.normal.z = n.z();
        }

        setVertex(i,   tri.points[0]);
        setVertex(i+1, tri.points[1]);
        setVertex(i+2, tri.points[2]);
    }
}























