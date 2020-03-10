//
//      Created by Stephens Nunnally on 2/2/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef THING_COMP_SOFT_BODY_H
#define THING_COMP_SOFT_BODY_H

#include "engine/globals_engine.h"
#include "engine/thing/engine_thing_component.h"

// Forward Declarations
struct Vertex;


//####################################################################################
//##    ThingCompSoftBody
//##        Built-In Component for DrEngineThing dealing with Soft Body Physics
//############################
class ThingCompSoftBody : public DrThingComponent
{
public:
    // Constructor / Destructor
    ThingCompSoftBody(DrEngineWorld *engine_world, DrEngineThing *parent_thing);
    virtual ~ThingCompSoftBody();


    // #################### VARIABLES ####################
public:
    // Soft Body Variables
    double                  height_width_ratio  { 1.0 };            // Stores ratio for rectangular bodies
    DrPoint                 soft_grid_size      { 1, 1 };           // Stores width and height of soft ball array (for Body_Style::Mesh_Blob)
    DrPointF                soft_size           { 1.0, 1.0 };       // Stores total size of soft body object
    DrPointF                soft_scale          { 1.0, 1.0 };       // Stores scale difference between outside of soft body and ball location

    std::vector<long>       soft_balls;                             // Stores keys of children soft bodies
    std::vector<DrPointF>   soft_start;                             // Soft body starting positions
    std::vector<DrPointF>   soft_uv;                                // Soft body texture coordinates

    bool                    soft_corner         { false };          // Used to store corners of soft bodies to stop from average corners of mesh
    double                  soft_start_angle    { 0.0 };            // Tracks starting angle of 1st soft child

    std::vector<size_t>     soft_outline_indexes;                   // Stores array indexes of soft_balls that represent outline point list for debug drawing
    DrPointF                soft_position       { 0, 0 };           // Stores current relative shape coordinate, used in debug drawing

    std::vector<float>      m_soft_vertices;                        // Used to keep soft body textured quad coordinates
    std::vector<float>      m_soft_texture_coordinates;             // Used to keep the coordinates of rendering an entire texture as a soft body
    std::vector<float>      m_soft_barycentric;                     // Used to keep soft body textured quad barycentric coords
    int                     m_soft_triangles    { 0 };              // Stores number of triangles


    // #################### FUNCTIONS TO BE EXPOSED TO API ####################
public:
    // Basic Component Events
    virtual void    init();                                                         // Called when component is first created
    virtual void    update(double time_passed, double time_warp);                   // Called during DrEngineWorld->updateWorld() step
    virtual void    destroy();


    // #################### INTERNAL FUNCTIONS ####################
public:
    // Mesh Functions
    void            addSoftTriangle(Vertex v1, Vertex v2, Vertex v3);
    bool            calculateSoftBodyMesh(Body_Style body_style, Soft_Mesh_Style mesh_style);


};

#endif // THING_COMP_SOFT_BODY_H





















