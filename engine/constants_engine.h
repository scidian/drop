//
//      Created by Stephens Nunnally on 11/6/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef CONSTANTS_ENGINE_H
#define CONSTANTS_ENGINE_H

#include "3rd_party/glm/vec3.hpp"
#include "core/types/dr_pointf.h"


// Camera Constants
const       glm::vec3   c_up_vector_y { 0.0, 1.0, 0.0 };
const       glm::vec3   c_up_vector_z { 0.0, 0.0, 1.0 };
const       float       c_near_plane =     -10000.0;
const       float       c_far_plane =       10000.0;
const       float       c_field_of_view =   52.5f;              // Close to Orthographic size when using standard widescreen ratio
const       int         c_slop_buffer_size = 5;                 // Default number of past object speeds to average together for camera follow


// 2D Light Constants
const       float       c_occluder_scale_ortho =       1.00f;   // Scale to use for occlusion map (higher the number, less shaky the shadows), MUST be 1.0 for now
const       float       c_occluder_scale_proj =        1.00f;   // Scale to use for occlusion map (higher the number, less shaky the shadows), MUST be 1.0 for now
const       int         c_desired_max_rays =           4096;    // Desired max number of rays to send out during shadow map calculations
const       int         c_desired_occluder_fbo_size =  8192;    // Desired max width and height of offscreen fbo used for shadow map
const       int         c_desired_light_fbo_size =     4096;    // Desired max width and height of offscreen fbo used for lights (and max size of lights themselves)


// Constants for calling engine addObject calls
constexpr   double      c_epsilon = 0.000001;                   // Floating point zero
const       DrPointF    c_center             {0, 0};            // Default offset in no offset
const       DrPointF    c_scale1x1           {1, 1};            // Default scale of 1x1
const       double      c_default_camera_z =  801;              // Default camera z distance
const       glm::vec3   c_default_camera_pos {0, 0, 801};       // Default camera position if there is no active camera
const       glm::vec3   c_default_camera_rot {0, 0, 0};         // Default camera rotation if there is no active camera
constexpr   double      c_zero_rotate =         0;              // Default rotation amount of zero
constexpr   double      c_opaque =              1;              // Default transparency of fully opaque
constexpr   double      c_transparent =         0;              // Fully transparent
constexpr   double      c_friction =           -1;              // Flag for add**** call to use world friction setting
constexpr   double      c_bounce =             -1;              // Flag for add**** call to use world bounce setting
constexpr   int         c_no_max_health =      -1;              // Flag for no maximum health
constexpr   int         c_unlimited_health =   -1;              // Flag for unlimited health
constexpr   int         c_unlimited_jump =     -1;              // Flag for unlimited jump
const       bool        c_collide_true =     true;              // Constant for collision true
const       bool        c_collide_false =   false;              // Constant for collision false


// Constants for Object Body / Shape Creation
constexpr   double      c_extra_radius =    0.50;               // Radius added on to block and polygon shapes for better collisions
constexpr   double      c_mass_multiplier = 0.002;              // Shapes Area times this multiplier = shape mass





#endif // CONSTANTS_ENGINE_H








