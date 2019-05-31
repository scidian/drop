//
//      Created by Stephens Nunnally on 5/28/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef ENGINE_OBJECT_H
#define ENGINE_OBJECT_H

#include <QPointF>
#include <QTime>
#include <QVector>
#include <QVector3D>
#include <map>

#include <iostream>

#include "chipmunk/chipmunk.h"

// Local Enumerations
enum class Body_Type {
    Dynamic,
    Kinematic,
    Static,
};

enum class Shape_Type {
    Circle,                             // cpCircleShapeNew
    Box,                                // cpBoxShapeNew
    Segment,    // "Line"               // cpSegmentShapeNew
    Polygon,                            // cpPolyShapeNew
};

enum class Collision_Type {
    Damage_None =           1,
    Damage_Player =         2,
    Damage_Enemy =          3,
    Damage_All =            4,
};

enum class One_Way {                    // One Way Collide
    None,
    Pass_Through,                       // Objects can pass through going one_way_direction
    Weak_Spot,                          // Only takes damage from one_way_direction (bustable block, turtle enemy)
};

enum class Jump_State {
    Need_To_Jump,
    Jumped,
};

// Type Definitions
typedef std::map<cpShape*, Shape_Type> ShapeMap;

// Constants for calling engine addObject calls
constexpr QVector3D c_no_camera {0, 0, 800};    // Default camera position if there is no active camera
constexpr QPointF   c_center    {0, 0};         // Default offset in no offset
constexpr QPointF   c_scale1x1  {1, 1};         // Default scale of 1x1
constexpr double    c_norotate  {0};            // Default rotation amount of zero
constexpr double    c_opaque    {1};            // Default transparency of fully opaque
constexpr double    c_friction = -1;            // Flag for add**** call to use world friction setting
constexpr double    c_bounce =   -1;            // Flag for add**** call to use world bounce setting

// Global Variables - defined in engine_update_player.cpp
extern int          g_keyboard_x;               // Used to pass keyboard x button state to static callback functions
extern int          g_keyboard_y;               // Used to pass keyboard y button state to static callback functions
extern bool         g_jump_button;              // Used to pass jump button state to static callback functions
extern bool         g_rotate_cw;                // Used to pass rotate clockwise button state
extern bool         g_rotate_ccw;               // Used to pass rotate counter clockwise button state
extern cpVect       g_gravity_normal;           // Stores a gravity as a normalized vector for use in static callback functions
extern QString      g_info;


//####################################################################################
//##    DrEngineObject
//##        Holds on object for use in a cpSpace
//############################
struct DrEngineObject {
    // ***** Object Info
    cpBody             *body;                   // Physical Body of object
    Body_Type           body_type;              // Body_Type

    QVector<cpShape*>   shapes;                 // Collision Shapes of object
    ShapeMap            shape_type;             // Shape Types of Shapes of Object

    // ***** Object Properties
    bool        does_collide = true;            // Set to false to have this object not collide with anything

    long        texture_number;                 // Reference to which texture to use from Engine.EngineTexture map
    float       scale_x = 1.0f;                 // Scale of object in world
    float       scale_y = 1.0f;                 // Scale of object in world
    float       alpha;                          // Transparency of object (0.0 invisible, 1.0 opaque)
    double      z_order;                        // Used for layering

    long        active_camera = 0;              // Set to ID of last camera that followed this object, 0 == no camera

    Collision_Type  collision_type = Collision_Type::Damage_None;       // Specifies what other types of objects this object can damage
    long        health = 3;                                             // Object Health, -1 = infinite
    long        damage = 1;                                             // Damage caused to other objects of Type collision_type
    long        death_delay = 100;                                      // Time it takes for item to die (can't deal damage while dying), in milliseconds
    bool        fade_on_death = true;                                   // If true, object is slowly faded over death_delay time
    long        fade_delay = 750;                                       // Time it takes for item to be removed after death, in milliseconds (0 == remove immediately)
    double      damage_recoil = 150.0;                                  // How much opposite force to apply when receiving damage

    One_Way     one_way = One_Way::None;        // Set to true if we're using this object as a one way platform
    cpVect      one_way_direction {0, 1};       // Direction of Normal for one way, defaults to Up (i.e. character can pass upwards through the bottom of a block)

    double      custom_friction = c_friction;   // Defaults to c_friction (-1) if this item uses global m_friction, otherwise stores custom friction
    double      custom_bounce = c_bounce;       // Defaults to c_bounce (-1) if this item uses global m_bounce, otherwise stores custom bounce

    double      rotate_speed =  0;              // Speed at which object should spin when Motor Rotate (gas pedal) is pressed

    // ***** Object interaction                 // These properties are used by objects that have been attached to PlayerUpdateVelocity
    bool        lost_control = false;           // Set to true when players should not have button control
                                                //      (players are cpBody* that have been assigned the cpBodyUpdateVelocityFunc PlayerUpdateVelocity callback)

    double      max_speed_x =  1000.0;          // Maximum speed x of object
    double      max_speed_y =  1000.0;          // Maximum speed y of object

    double      forced_speed_x =  0.0;          // Forced move speed x of object
    double      forced_speed_y =  0.0;          // Forced move speed y of object

    double      move_speed_x =  400.0;          // Movement speed x
    double      move_speed_y =    0.0;          // Movement speed y

    double      jump_force_x =    0.0;          // Jump force x
    double      jump_force_y =  250.0;          // Jump force y
    double      jump_timeout =  800.0;          // Milliseconds to allow for jump to continue to receive a boost when jump button is held down
    int         jump_count =        0;          // How many jumps this player is allowed, -1 = unlimited, 0 = cannot jump, 1 = 1, 2 = 2, etc

    bool        can_rotate = true;              // To be set during object creation, moment of inertia is set to infinity to stop rotation
    bool        ignore_gravity = false;         // If turned to true, this object no longer is affected by gravity

    double      air_drag =       0.50;          // Affects acceleration and decceleration in air (0 to 1+)
    double      ground_drag =    0.25;          // Affects acceleration and decceleration on the ground (0 to 1+)
    double      rotate_drag =    0.25;          // Affects rotation acceleration and decceleration (0 to 1+)

    bool        air_jump = true;                // Can this player jump while in the air (even if only has 1 jump, ex: fell off platform)
    bool        wall_jump = false;              // Can this player jump off of walls?


    // ***** Updated by Engine:
    bool        should_process = true;          // True while object exists in Space
    bool        has_been_processed = false;     // Set to true after an initial updateSpace call has been ran once while the object was in the Space

    int         remaining_jumps = 0;                    // How many jumps player has left before it must hit ground before it can jump again
    double      remaining_boost = 0;                    // Used by Engine Update to process Jump Timeout boost
    double      remaining_wall_time = 0;                // Used by Engine Update to allow some time for a wall jump to occur
    bool        grounded = false;                       // Used by Engine Update to keep track of if this object is on the ground
    bool        on_wall = false;                        // Used by Engine Update to keep track of if this object is on a wall
    cpVect      last_touched_ground_normal = cpvzero;   // Normal Vector of the last touched surface
    double      last_touched_ground_dot = 1.0;          // Dot product of the last touched surface
    Jump_State  jump_state = Jump_State::Need_To_Jump;  // Used by Engine Update to keep track of if the current jump button press has been processed

    bool        dying = false;                  // When health turns to zero, dying becomes true for death_delay time, then alive becomes false
    QTime       death_timer;                    // Used to incorporate death_delay for object dying
    bool        alive = true;                   // After item has been dying for death_delay time, alive becomes false, then fades for fade_delay time
    QTime       fade_timer;                     // Used to incorporate fade_delay for object fade / removal

    double      angle = 0;                      // Current object angle
    QPointF     velocity;                       // Current object velocity
    QPointF     position;                       // Current object posiiton
    QPointF     previous_position;              // Previous frame position, for whatever may need it
};





#endif // ENGINE_OBJECT_H













