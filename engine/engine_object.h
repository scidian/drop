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
constexpr int       c_no_max_health = -1;               // Flag for no maximum health
constexpr int       c_unlimited_health = -1;            // Flag for unlimited health
constexpr int       c_unlimited_jump =   -1;            // Flag for unlimited jump

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

public:
    // ***** Object Body and Shapes
    cpBody             *body;                   // Physical Body of object
    Body_Type           body_type;              // Body_Type

    QVector<cpShape*>   shapes;                 // Collision Shapes of object
    ShapeMap            shape_type;             // Shape Types of Shapes of Object


private:
    // ***** Object Basic Settings
    bool        m_does_collide = true;              // Set to false to have this object not collide with anything
    long        m_texture_number;                   // Reference to which texture to use from Engine->EngineTexture map

    float       m_scale_x = 1.0f;                   // Scale of object in world
    float       m_scale_y = 1.0f;                   // Scale of object in world
    float       m_alpha = 1.0;                      // Transparency of object (0.0 invisible, 1.0 opaque)
    double      m_z_order;                          // Used for layering

    // ***** Object Properties - Camera
    long        m_active_camera = c_no_camera;      // Set to ID of last camera that followed this object, 0 == no camera

    // ***** Object Properties - Bounce / Friction
    double      m_custom_friction = c_friction;     // Defaults to c_friction (-1) if this item uses global m_friction, otherwise stores custom friction
    double      m_custom_bounce = c_bounce;         // Defaults to c_bounce (-1) if this item uses global m_bounce, otherwise stores custom bounce

    // ***** Object Properties - One Way
    One_Way     m_one_way = One_Way::None;          // Set one way collision type desired (None, Pass Through, Weak_Spot)
    cpVect      m_one_way_direction {0, 1};         // Direction for one way collision, defaults to Up (i.e. objects can pass upwards through the bottom of a block)

    // ***** Object Properties - Health / Damage
    Collision_Type  m_collision_type = Collision_Type::Damage_None;     // Specifies which types of objects this object can damage
    bool            m_invincible = false;                               // When true this object takes no damage nor damage_recoil force, cannot be killed
    bool            m_death_touch = false;                              // When true kills everything on contact, even unlimited health...but not invincible objects
    double          m_max_health = 100.0;                               // Maximum object health, c_no_max_health (-1) = no maximum
    double          m_health = 3.0;                                     // Object Health, c_unlimited_health (-1) = infinite, otherwise should be > 0
    double          m_damage = 1.0;                                     // Damage caused to other objects of Type m_collision_type
    double          m_auto_damage = 0.0;            // Take x damage per second (can be negative, i.e. add health)
    long            m_death_delay = 100;            // Time it takes for item to die (can't deal damage while dying), in milliseconds
    bool            m_fade_on_death = true;         // If true, object is slowly faded over death_delay time
    long            m_fade_delay = 750;             // Time it takes for item to be removed after death, in milliseconds (0 == remove immediately)
    double          m_damage_recoil = 200.0;        // How much opposite force to apply when receiving damage

    // ***** Object Movement - Rotation
    double          m_rotate_speed =  0.0;          // Speed at which object should spin when Motor Rotate (gas pedal) is pressed



    // ***** Object Movemnt - PlayerUpdateVelocity Callback Func
    bool        m_key_controls = false;             // Set to true when object is a "player" and should respond to key / button / mouse events
                                                    //      (players are cpBody* that have been assigned the cpBodyUpdateVelocityFunc PlayerUpdateVelocity callback)
    bool        m_lost_control = false;             // Set to true when players should not have button control but have been assigned key_controls

    double      m_max_speed_x =  1000.0;            // Maximum speed x of object
    double      m_max_speed_y =  1000.0;            // Maximum speed y of object

    double      m_forced_speed_x =  0.0;            // Forced move speed x of object
    double      m_forced_speed_y =  0.0;            // Forced move speed y of object

    double      m_move_speed_x =  400.0;            // Movement speed x
    double      m_move_speed_y =    0.0;            // Movement speed y

    double      m_jump_force_x =    0.0;            // Jump force x
    double      m_jump_force_y =  250.0;            // Jump force y
    long        m_jump_timeout =  800.0;            // Milliseconds to allow for jump to continue to receive a boost when jump button is held down
    int         m_jump_count =        0;            // How many jumps this player is allowed, -1 = c_unlimited_jump, 0 = cannot jump, 1 = 1, 2 = 2, etc

    double      m_air_drag =       0.50;            // Affects acceleration and decceleration in air (0 to 1+)
    double      m_ground_drag =    0.25;            // Affects acceleration and decceleration on the ground (0 to 1+)
    double      m_rotate_drag =    0.25;            // Affects rotation acceleration and decceleration (0 to 1+)

    bool        m_air_jump = true;                  // Can this player jump while in the air (even if only has 1 jump, ex: fell off platform)
    bool        m_wall_jump = false;                // Can this player jump off of walls?

    bool        m_can_rotate = true;                // To be set during object creation, moment of inertia is set to infinity to stop rotation
    bool        m_ignore_gravity = false;           // If turned to true, this object no longer is affected by gravity






public:
    // ***** Updated by Engine:
    bool        should_process = true;              // True while object exists in Space
    bool        has_been_processed = false;         // Set to true after an initial updateSpace call has been ran once while the object was in the Space

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


    // Object Basic Settings
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

    // Object Properties - Bounce / Friction
    const double&   getCustomFriction() { return m_custom_friction; }
    const double&   getCustomBounce() { return m_custom_bounce; }
    void            setCustomFriction(double new_friction) { m_custom_friction = new_friction; }
    void            setCustomBounce(double new_bounce) { m_custom_bounce = new_bounce; }

    // Object Properties - One Way
    One_Way     getOneWay() { return m_one_way; }
    cpVect      getOneWayDirection() { return m_one_way_direction; }

    void        setOneWay(One_Way one_way_type) { m_one_way = one_way_type; }
    void        setOneWayDirection(cpVect direction) { m_one_way_direction = direction; }
    void        setOneWayDirection(QPointF direction) { m_one_way_direction = cpv(direction.x(), direction.y()); }

    // Object Properties - Health / Damage
    Collision_Type  getCollisionType() { return m_collision_type; }
    const bool&     isInvincible() { return m_invincible; }
    const bool&     hasDeathTouch() { return m_death_touch; }
    const double&   getMaxHealth() { return m_max_health; }
    const double&   getHealth() { return m_health; }
    const double&   getDamage() { return m_damage; }
    const double&   getAutoDamage() { return m_auto_damage; }
    const long&     getDeathDelay() { return m_death_delay; }
    const bool&     getFadeOnDeath() { return m_fade_on_death; }
    const long&     getFadeDelay() { return m_fade_delay; }
    const double&   getDamageRecoil() { return m_damage_recoil; }

    void            setCollisionType(Collision_Type what_should_collide);
    void            setInvincible(bool invincible) { m_invincible = invincible; }
    void            setDeathTouch(bool should_have_death_touch) { m_death_touch = should_have_death_touch; }
    void            setMaxHealth(double new_max_health) { m_max_health = new_max_health; }
    void            setHealth(double new_health) { m_health = new_health; }
    void            setDamage(double new_damage) { m_damage = new_damage; }
    void            setAutoDamage(double new_auto_damage) { m_auto_damage = new_auto_damage; }
    void            setDeathDelay(long new_death_delay_in_milliseconds) { m_death_delay = new_death_delay_in_milliseconds; }
    void            setFadeOnDeath(bool should_fade) { m_fade_on_death = should_fade; }
    void            setFadeDelay(long new_fade_dealy_in_milliseconds) { m_fade_delay = new_fade_dealy_in_milliseconds; }
    void            setDamageRecoil(double new_damage_recoil_force) { m_damage_recoil = new_damage_recoil_force; }

    bool            doesDamage();
    bool            shouldDamage(Collision_Type check_can_damage);
    bool            takeDamage(double damage_to_take, bool death_touch = false);

    // Object Movement - Rotation
    const double&   getRotateSpeed() { return m_rotate_speed; }
    void            setRotateSpeed(double new_rotate_speed) { m_rotate_speed = new_rotate_speed; }

    // Object Movemnt - PlayerUpdateVelocity Callback Func
    const bool&     hasKeyControls() { return m_key_controls; }
    const bool&     hasLostControl() { return m_lost_control; }
    const double&   getMaxSpeedX() { return m_max_speed_x; }
    const double&   getMaxSpeedY() { return m_max_speed_y; }
    const double&   getForcedSpeedX() { return m_forced_speed_x; }
    const double&   getForcedSpeedY() { return m_forced_speed_y; }
    const double&   getMoveSpeedX() { return m_move_speed_x; }
    const double&   getMoveSpeedY() { return m_move_speed_y; }
    const double&   getJumpForceX() { return m_jump_force_x; }
    const double&   getJumpForceY() { return m_jump_force_y; }
    const long&     getJumpTimeout() { return m_jump_timeout; }
    const int&      getJumpCount() { return m_jump_count; }

    const double&   getAirDrag() { return m_air_drag; }
    const double&   getGroundDrag() { return m_ground_drag; }
    const double&   getRotateDrag() { return m_rotate_drag; }
    const bool&     canAirJump() { return m_air_jump; }
    const bool&     canWallJump() { return m_wall_jump; }
    const bool&     canRotate() { return m_can_rotate; }
    const bool&     ignoreGravity() { return m_ignore_gravity; }

    void            setKeyControls(bool has_key_controls) { m_key_controls = has_key_controls; }
    void            setLostControl(bool lost_control) { m_lost_control = lost_control; }
    void            setMaxSpeedX(double new_max_speed_x) { m_max_speed_x = new_max_speed_x; }
    void            setMaxSpeedY(double new_max_speed_y) { m_max_speed_y = new_max_speed_y; }
    void            setForcedSpeedX(double new_forced_speed_x) { m_max_speed_x = new_forced_speed_x; }
    void            setForcedSpeedY(double new_forced_speed_y) { m_max_speed_y = new_forced_speed_y; }
    void            setMoveSpeedX(double new_move_speed_x) { m_max_speed_x = new_move_speed_x; }
    void            setMoveSpeedY(double new_move_speed_y) { m_max_speed_y = new_move_speed_y; }
    void            setJumpForceX(double new_jump_force_x) { m_jump_force_x = new_jump_force_x; }
    void            setJumpForceY(double new_jump_force_y) { m_jump_force_y = new_jump_force_y; }
    void            setJumpTimeout(long new_jump_timeout) { m_jump_timeout = new_jump_timeout; }
    void            setJumpCount(int new_jump_count) { m_jump_count = new_jump_count; }

    void            setAirDrag(double new_air_drag) { m_air_drag = new_air_drag; }
    void            setGroundDrag(double new_ground_drag) { m_ground_drag = new_ground_drag; }
    void            setRotateDrag(double new_rotate_drag) { m_rotate_drag = new_rotate_drag; }
    void            setCanAirJump(bool can_air_jump) { m_air_jump = can_air_jump; }
    void            setCanWallJump(bool can_wall_jump) { m_wall_jump = can_wall_jump; }
    void            setCanRotate(bool can_rotate) { m_can_rotate = can_rotate; }
    void            setIgnoreGravity(bool ignore_gravity) { m_ignore_gravity = ignore_gravity; }

};



#endif // ENGINE_OBJECT_H























