//
//      Created by Stephens Nunnally on 6/15/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef PLAYGROUND_TOY_H
#define PLAYGROUND_TOY_H

#include <QGraphicsItem>
#include <map>

#include "chipmunk/chipmunk.h"
#include "enums_engine.h"

// Local Constants
constexpr double    c_friction =  -1;                   // Flag for add**** call to use world friction setting
constexpr double    c_bounce =    -1;                   // Flag for add**** call to use world bounce setting

//####################################################################################
//##    DrToy
//##        Holds one object for use in a Playground cpSpace
//############################
class DrToy
{

public:
    // ***** Object Body and Shape
    cpBody             *body;               // Physical Body of object
    Body_Type           body_type;          // Body_Type

    cpShape            *shape;              // Collision Shape of object
    Shape_Type          shape_type;         // Shape_Type

    QGraphicsItem      *graphic;            // Pointer to the QGraphicsItem that represents this toy on the Playground QGraphicsScene


    // ********** Object Component Properties
    //      To be set for each object as desired
    //
    // ***** Object Basic Settings
    bool            m_does_collide = true;          // Set to false to have this object not collide with anything
    long            m_texture_number;               // Reference to which texture to use from Engine->EngineTexture map

    float           m_scale_x = 1.0f;               // Scale of object in world
    float           m_scale_y = 1.0f;               // Scale of object in world
    float           m_alpha = 1.0;                  // Transparency of object (0.0 invisible, 1.0 opaque)
    double          m_z_order = 0.0;                // Used for layering

    // ***** Object Properties - Bounce / Friction
    double          m_custom_friction = c_friction; // Defaults to c_friction (-1) if this item uses global m_friction, otherwise stores custom friction
    double          m_custom_bounce = c_bounce;     // Defaults to c_bounce (-1) if this item uses global m_bounce, otherwise stores custom bounce

    // ***** Object Properties - Movement
    double          m_velocity_x;                   // Original x velocity when loaded into scene, used for KinematicUpdateVelocity func
    double          m_velocity_y;                   // Original y velocity when loaded into scene, used for KinematicUpdateVelocity func
    double          m_spin_velocity;                // Original angular velocity when loaded into scene, !!!!! #NOTE: In radians !!!!!
    bool            m_use_angle_velocity = true;    // Should the angle of the object affect velocity? (only for Kinematic)

    // ***** Object Properties - Health / Damage
    Collision_Type  m_collision_type = Collision_Type::Damage_None; // Specifies which types of objects this object can damage
    bool            m_invincible = false;                           // When true this object takes no damage nor damage_recoil force, cannot be killed
    bool            m_death_touch = false;                          // When true kills everything on contact, even unlimited health...but not invincible objects
    double          m_max_health = 100.0;                           // Maximum object health, c_no_max_health (-1) = no maximum
    double          m_health = 3.0;                                 // Object Health, c_unlimited_health (-1) = infinite, otherwise should be > 0
    double          m_damage = 1.0;                                 // Damage caused to other objects of Type m_collision_type
    long            m_damage_delay = 500;                           // Minimum time in milliseconds that must pass between receiving damage
    double          m_auto_damage = 0.0;            // Take x damage per second (can be negative, i.e. add health)
    long            m_death_delay = 100;            // Time it takes for item to die (can't deal damage while dying), in milliseconds
    bool            m_fade_on_death = true;         // If true, object is slowly faded over death_delay time
    long            m_fade_delay = 750;             // Time it takes for item to be removed after death, in milliseconds (0 == remove immediately)
    double          m_damage_recoil = 200.0;        // How much opposite force to apply when receiving damage

    // ***** Object Movement - Rotation
    double          m_rotate_speed =  0.0;          // Speed at which object should spin when Motor Rotate (gas pedal) is pressed

    // ***** Object Movement - PlayerUpdateVelocity Callback Func
    bool            m_key_controls = false;         // Set to true when object is a "player" and should respond to key / button / mouse events
                                                    //      (players are cpBody* that have been assigned the cpBodyUpdateVelocityFunc PlayerUpdateVelocity callback)
    bool            m_lost_control = false;         // Set to true when players should not have button control but have been assigned key_controls

    double          m_max_speed_x =  1000.0;        // Maximum speed x of object
    double          m_max_speed_y =  1000.0;        // Maximum speed y of object

    double          m_forced_speed_x =  0.0;        // Forced move speed x of object
    double          m_forced_speed_y =  0.0;        // Forced move speed y of object

    double          m_move_speed_x =  400.0;        // Movement speed x
    double          m_move_speed_y =    0.0;        // Movement speed y

    double          m_jump_force_x =    0.0;        // Jump force x
    double          m_jump_force_y =  250.0;        // Jump force y
    long            m_jump_timeout =  800;          // Milliseconds to allow for jump to continue to receive a boost when jump button is held down
    int             m_jump_count =      0;          // How many jumps this player is allowed, -1 = c_unlimited_jump, 0 = cannot jump, 1 = 1, 2 = 2, etc

    double          m_air_drag =       0.50;        // Affects acceleration and decceleration in air (0 to 1+)
    double          m_ground_drag =    0.25;        // Affects acceleration and decceleration on the ground (0 to 1+)
    double          m_rotate_drag =    0.25;        // Affects rotation acceleration and decceleration (0 to 1+)

    bool            m_air_jump = true;              // Can this player jump while in the air (even if only has 1 jump, ex: fell off platform)
    bool            m_wall_jump = false;            // Can this player jump off of walls?

    bool            m_can_rotate = true;            // To be set during object creation, moment of inertia is set to infinity to stop rotation
    bool            m_ignore_gravity = false;       // If turned to true, this object no longer is affected by gravity

    // ********** Local Variables Updated by Engine
    //                NOT TO BE SET BY USER
    //
    bool        m_should_process = true;                    // True while object exists in Space
    bool        m_has_been_processed = false;               // Set to true after an initial updateSpace call has been ran once while the object was in the Space

    double      m_angle = 0.0;                              // Current object->body angle, updated every frame by updateSpaceHelper()
    QPointF     m_position;                                 // Current object->body posiiton, updated every frame by updateSpaceHelper()
    QPointF     m_previous_position;                        // Previous frame position, updated every frame by updateSpaceHelper()



public:
    DrToy() {}


};


#endif // PLAYGROUND_TOY_H















