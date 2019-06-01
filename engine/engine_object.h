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

#include "chipmunk/chipmunk.h"
#include "enums_engine.h"
#include "helper.h"

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
constexpr double    c_epsilon = 0.0000001;              // Floating point zero
constexpr QPointF   c_center    {0, 0};                 // Default offset in no offset
constexpr QPointF   c_scale1x1  {1, 1};                 // Default scale of 1x1
constexpr QVector3D c_default_camera_pos {0, 0, 800};   // Default camera position if there is no active camera
constexpr long      c_no_camera =  0;                   // Default camera position if there is no active camera
constexpr double    c_norotate =   0;                   // Default rotation amount of zero
constexpr double    c_opaque =     1;                   // Default transparency of fully opaque
constexpr double    c_friction =  -1;                   // Flag for add**** call to use world friction setting
constexpr double    c_bounce =    -1;                   // Flag for add**** call to use world bounce setting
constexpr int       c_unlimited = -1;                   // Flag for ranged items to be unlimited (health, jump_count)

// Global Variables - defined in engine_update_player.cpp
extern int          g_keyboard_x;                       // Used to pass keyboard x button state to static callback functions
extern int          g_keyboard_y;                       // Used to pass keyboard y button state to static callback functions
extern bool         g_jump_button;                      // Used to pass jump button state to static callback functions
extern bool         g_rotate_cw;                        // Used to pass rotate clockwise button state
extern bool         g_rotate_ccw;                       // Used to pass rotate counter clockwise button state
extern cpVect       g_gravity_normal;                   // Stores a gravity as a normalized vector for use in static callback functions
extern QString      g_info;


//####################################################################################
//##    DrEngineObject
//##        Holds on object for use in a cpSpace
//############################
class DrEngineObject
{
private:
    // ***** Object Properties - Basic
    bool        m_does_collide = true;              // Set to false to have this object not collide with anything
    long        m_texture_number;                   // Reference to which texture to use from Engine->EngineTexture map

    float       m_scale_x = 1.0f;                   // Scale of object in world
    float       m_scale_y = 1.0f;                   // Scale of object in world
    float       m_alpha = 1.0;                      // Transparency of object (0.0 invisible, 1.0 opaque)
    double      m_z_order;                          // Used for layering

    // ***** Object Properties - Camera
    long        m_active_camera = c_no_camera;      // Set to ID of last camera that followed this object, 0 == no camera

    // ***** Object Properties - Health / Damage
    Collision_Type m_collision_type = Collision_Type::Damage_None;      // Specifies what other types of objects this object can damage

public:
    // ***** Object Info
    cpBody             *body;                   // Physical Body of object
    Body_Type           body_type;              // Body_Type

    QVector<cpShape*>   shapes;                 // Collision Shapes of object
    ShapeMap            shape_type;             // Shape Types of Shapes of Object



    /// ***** Object Properties - Health / Damage
    double      max_health = 100.0;                                     // Maximum object health, c_unlimited (-1) = no maximum
    double      health = 3.0;                                           // Object Health, c_unlimited (-1) = infinite, otherwise should be > 0
    double      damage = 1.0;                                           // Damage caused to other objects of Type m_collision_type
    double      auto_damage = 0.0;                                      // Take x damage per second (can be negative, i.e. add health)
    long        death_delay = 100;                                      // Time it takes for item to die (can't deal damage while dying), in milliseconds
    bool        fade_on_death = true;                                   // If true, object is slowly faded over death_delay time
    long        fade_delay = 750;                                       // Time it takes for item to be removed after death, in milliseconds (0 == remove immediately)
    double      damage_recoil = 200.0;                                  // How much opposite force to apply when receiving damage

    One_Way     one_way = One_Way::None;        // Set to true if we're using this object as a one way platform
    cpVect      one_way_direction {0, 1};       // Direction of Normal for one way, defaults to Up (i.e. character can pass upwards through the bottom of a block)

    double      custom_friction = c_friction;   // Defaults to c_friction (-1) if this item uses global m_friction, otherwise stores custom friction
    double      custom_bounce = c_bounce;       // Defaults to c_bounce (-1) if this item uses global m_bounce, otherwise stores custom bounce

    double      rotate_speed =  0.0;            // Speed at which object should spin when Motor Rotate (gas pedal) is pressed

    // ***** Object interaction                 // These properties are used by objects that have been attached to PlayerUpdateVelocity
    bool        key_controls = false;           // Set to true when object is a "player" and should respond to key / button / mouse events
                                                //      (players are cpBody* that have been assigned the cpBodyUpdateVelocityFunc PlayerUpdateVelocity callback)
    bool        lost_control = false;           // Set to true when players should not have button control but have been assigned key_controls


    double      max_speed_x =  1000.0;          // Maximum speed x of object
    double      max_speed_y =  1000.0;          // Maximum speed y of object

    double      forced_speed_x =  0.0;          // Forced move speed x of object
    double      forced_speed_y =  0.0;          // Forced move speed y of object

    double      move_speed_x =  400.0;          // Movement speed x
    double      move_speed_y =    0.0;          // Movement speed y

    double      jump_force_x =    0.0;          // Jump force x
    double      jump_force_y =  250.0;          // Jump force y
    double      jump_timeout =  800.0;          // Milliseconds to allow for jump to continue to receive a boost when jump button is held down
    int         jump_count =        0;          // How many jumps this player is allowed, c_unlimited (-1) = unlimited, 0 = cannot jump, 1 = 1, 2 = 2, etc

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
    double      remaining_boost = 0.0;                  // Used by Engine Update to process Jump Timeout boost
    double      remaining_wall_time = 0.0;              // Used by Engine Update to allow some time for a wall jump to occur
    bool        grounded = false;                       // Used by Engine Update to keep track of if this object is on the ground
    bool        on_wall = false;                        // Used by Engine Update to keep track of if this object is on a wall
    cpVect      last_touched_ground_normal = cpvzero;   // Normal Vector of the last touched surface
    double      last_touched_ground_dot = 1.0;          // Dot product of the last touched surface
    Jump_State  jump_state = Jump_State::Need_To_Jump;  // Used by Engine Update to keep track of if the current jump button press has been processed

    bool        dying = false;                          // When health turns to zero, dying becomes true for death_delay time, then alive becomes false
    bool        alive = true;                           // After item has been dying for death_delay time, alive becomes false, then fades for fade_delay time
    DrTime      death_timer = Clock::now();             // Used to incorporate death_delay for object dying
    DrTime      fade_timer = Clock::now();              // Used to incorporate fade_delay for object fade / removal

    DrTime      update_timer = Clock::now();            // Used to keep track of time passed since last object update
    double      time_since_last_update = 0.0;           // Milliseconds since last update

    double      angle = 0.0;                        // Current object angle
    QPointF     position;                           // Current object posiiton
    QPointF     previous_position;                  // Previous frame position, for whatever may need it


public:
    DrEngineObject() {}


    // Object Properties - Basic
    const bool&     doesCollide() { return m_does_collide; }
    const long&     getTextureNumber() { return m_texture_number; }
    const float&    getScaleX() { return m_scale_x; }
    const float&    getScaleY() { return m_scale_y; }
    const float&    getOpacity() { return m_alpha; }
    const double&   getZOrder() { return m_z_order; }

    void            setDoesCollide(bool should_collide) { m_does_collide = should_collide; }
    void            setTextureNumber(long texture_number) { m_texture_number = texture_number; }
    void            setScaleX(float new_scale_x)  { m_scale_x = new_scale_x; }
    void            setScaleX(double new_scale_x) { m_scale_x = static_cast<float>(new_scale_x); }
    void            setScaleY(float new_scale_y)  { m_scale_y = new_scale_y; }
    void            setScaleY(double new_scale_y) { m_scale_y = static_cast<float>(new_scale_y); }
    void            setOpacity(float new_alphaf)  { m_alpha = new_alphaf; }
    void            setOpacity(double new_alphaf) { m_alpha = static_cast<float>(new_alphaf); }
    void            setZOrder(double new_z_order) { m_z_order = new_z_order; }

    // Object Properties - Camera
    const long&     getActiveCameraKey() { return m_active_camera; }
    bool            hasActiveCamera() { return (m_active_camera == c_no_camera) ? false : true; }
    void            setActiveCameraKey(const long& new_camera_key) { m_active_camera = new_camera_key; }

    // Object Properties - Health / Damage
    Collision_Type  getCollisionType() { return m_collision_type; }

    void            setCollisionType(Collision_Type what_should_collide);
    bool            shouldDamage(Collision_Type check_can_damage);



};



#endif // ENGINE_OBJECT_H























