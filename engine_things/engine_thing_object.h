//
//      Created by Stephens Nunnally on 5/28/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef ENGINE_OBJECT_H
#define ENGINE_OBJECT_H

#include "constants_engine.h"
#include "engine_thing.h"
#include "globals_engine.h"


//####################################################################################
//##    DrEngineObject
//##        Holds one object for use in a cpSpace
//############################
class DrEngineObject : public DrEngineThing
{
public:
    // Object Body and Shapes
    cpBody             *body = nullptr;         // Physical Body of object
    Body_Type           body_type;              // Body_Type

    QVector<cpShape*>   shapes;                 // Collision Shapes of object
    ShapeMap            shape_type;             // Shape Types of Shapes of Object

private:
    // ***** Object Component Properties
    //      To be set for each object as desired
    //
    // Object Basic Settings
    bool                m_does_collide = true;                      // Set to false to have this object not collide with anything
    Collision_Groups    m_collide_with = Collision_Groups::All;     // Types of other objects this should collide with (physcics and collision)
    long                m_texture_number;                           // Reference to which texture to use from Engine->EngineTexture map

    // Object Properties - Bounce / Friction
    double          m_custom_friction = c_friction; // Defaults to c_friction (-1) if this item uses global m_friction, otherwise stores custom friction
    double          m_custom_bounce = c_bounce;     // Defaults to c_bounce (-1) if this item uses global m_bounce, otherwise stores custom bounce

    // Object Properties - Movement
    double          m_velocity_x;                   // Original x velocity when loaded into scene, used for KinematicUpdateVelocity func
    double          m_velocity_y;                   // Original y velocity when loaded into scene, used for KinematicUpdateVelocity func
    double          m_spin_velocity;                // Original angular velocity when loaded into scene, !!!!! #NOTE: In radians !!!!!
    bool            m_use_angle_velocity = true;    // Should the angle of the object affect velocity? (only for Kinematic)
    bool            m_rotate_to_player = false;     // Should the angle of the object rotate toward active player? (only for Kinematic)

    // Object Properties - One Way / Collision
    One_Way         m_one_way = One_Way::None;      // Set one way collision type desired (None, Pass Through, Weak_Spot)
    cpVect          m_one_way_direction {0, 1};     // Direction for one way collision, defaults to Up (i.e. objects can pass upwards through the bottom of a block)
    double          m_gravity_multiplier = 1.0;     // Use to cancel gravity (0.0) on objects that collide (climbable ladders), or to reduce gravity (sticky wall)
    cpVect          m_surface_velocity {0, 0};      // Provides surface movement on contact, useful for conveyor belts, etc.

    // Object Properties - Health / Damage
    Collision_Type  m_collision_type = Collision_Type::Damage_None; // Specifies which types of objects this object can damage
    bool            m_invincible = false;                           // When true this object takes no damage nor damage_recoil force, cannot be killed
    bool            m_death_touch = false;                          // When true kills everything on contact, even unlimited health...but not invincible objects
    double          m_max_health = 10.0;                            // Maximum object health, c_no_max_health (-1) = no maximum
    double          m_health = 1.0;                                 // Object Health, c_unlimited_health (-1) = infinite, otherwise should be > 0
    double          m_damage = 1.0;                                 // Damage caused to other objects of Type m_collision_type
    long            m_damage_delay = 500;                           // Minimum time in milliseconds that must pass between receiving damage
    double          m_auto_damage = 0.0;            // Take x damage per second (can be negative, i.e. add health)
    long            m_death_delay = 100;            // Time it takes for item to die (can't deal damage while dying), in milliseconds

    Death_Animation m_death_animation = Death_Animation::Fade;      // If true, object is slowly faded over death_delay time
    long            m_death_duration = 750;                         // Duration (in milliseconds) of Death Animation (0 = remove immediately)

    double          m_damage_recoil = 200.0;        // How much opposite force to apply when receiving damage

    // Object Movement - Rotation
    double          m_rotate_speed =  0.0;          // Speed at which object should spin (on standard 2D Z axis) when Motor Rotate (gas pedal) is pressed

    // Object Movement - PlayerUpdateVelocity Callback Func
    bool            m_key_controls = false;         // Set to true when object is a "player" and should respond to key / button / mouse events
                                                    //      (players are cpBody* that have been assigned the cpBodyUpdateVelocityFunc PlayerUpdateVelocity callback)
    bool            m_lost_control = false;         // Set to true when players should not have button control but have been assigned key_controls

    double          m_max_speed_x =  1000.0;        // Maximum speed x of object
    double          m_max_speed_y =  1000.0;        // Maximum speed y of object

    double          m_forced_speed_x =  0.0;        // Forced move speed x of object
    double          m_forced_speed_y =  0.0;        // Forced move speed y of object

    double          m_move_speed_x =  400.0;        // Movement speed x
    double          m_move_speed_y =  400.0;        // Movement speed y
    bool            m_angle_movement = false;       // Should the objects angle affect the objects move speeds? (good for first person, etc)

    double          m_jump_force_x =    0.0;        // Jump force x
    double          m_jump_force_y =  250.0;        // Jump force y
    long            m_jump_timeout =  800;          // Milliseconds to allow for jump to continue to receive a boost when jump button is held down
    int             m_jump_count =      0;          // How many jumps this player is allowed, -1 = c_unlimited_jump, 0 = cannot jump, 1 = 1, 2 = 2, etc

    double          m_acceleration =  1.00;         // Affects move / switch speeds, 0.0 is instant, 1.0 is default, 5.0 is slower
    double          m_air_drag =       1.00;        // Affects acceleration and decceleration in air (0 to 1+)
    double          m_ground_drag =    1.00;        // Affects acceleration and decceleration on the ground (0 to 1+)
    double          m_rotate_drag =    0.25;        // Affects rotation acceleration and decceleration (0 to 1+)

    bool            m_air_jump = true;              // Can this player jump while in the air (even if only has 1 jump, ex: fell off platform)
    bool            m_wall_jump = false;            // Can this player jump off of walls?

    bool            m_can_rotate = true;            // To be set during object creation, moment of inertia is set to infinity to stop rotation
    bool            m_ignore_gravity = false;       // If turned to true, this object no longer is affected by gravity

    bool            m_flip_image_x = false;         // If turned to true, player flips left / right depending on velocity
    bool            m_flip_image_y = false;         // If turned to true, player flips up /   down depending on velocity
    bool            m_mouse_rotate = false;         // If turned to true, player rotates to mouse position


    // ***** Local Variables Updated by Engine
    //              NOT TO BE SET BY USER
    //
    int         m_remaining_jumps = 0;                      // How many jumps player has left before it must hit ground before it can jump again
    double      m_remaining_boost = 0.0;                    // Used by Engine Update to process Jump Timeout boost
    double      m_remaining_ground_time = 0.0;              // Used by Engine Update to allow some time for a ground jump to occur (helps with bumpiness)
    double      m_remaining_wall_time = 0.0;                // Used by Engine Update to allow some time for a wall jump to occur

    bool        m_grounded = false;                         // Used by Engine Update to keep track of if this object is on the ground
    bool        m_on_wall = false;                          // Used by Engine Update to keep track of if this object is on a wall
    double      m_temp_gravity_multiplier = 1.0;            // Set multipler (0.0, 0.5, etc) when colliding with a gravity multiplying object (ladder, wall, etc)

    cpVect      m_last_touched_ground_normal = cpvzero;     // Normal Vector of the last touched surface
    double      m_last_touched_ground_dot = 1.0;            // Dot product of the last touched surface
    Jump_State  m_jump_state = Jump_State::Need_To_Jump;    // Used by Engine Update to keep track of if the current jump button press has been processed

    bool        m_dying = false;                            // When health turns to zero, dying becomes true for death_delay time, then alive becomes false
    bool        m_alive = true;                             // After item has been dying for death_delay time, alive becomes false, then fades for fade_delay time

    DrTime      m_damage_timer = Clock::now();              // Used to track last time this object was damaged to implement m_damage_delay
    DrTime      m_death_timer =  Clock::now();              // Used to incorporate death_delay for object dying
    DrTime      m_fade_timer =   Clock::now();              // Used to incorporate fade_delay for object fade / removal

    Vec3        m_previous_position;                        // Previous frame position, updated every frame by update()
    bool        m_flipped_x = false;                        // True when image is flipped (going left) over the x axis
    bool        m_flipped_y = false;                        // True when image is flipped (going down) over the y axis




public:
    // ***** Image Post Processing Attributes
    bool        cast_shadows = true;                        // Will cast shadows when in front of a Light



public:
    // Constructor / Destructor
    DrEngineObject(DrEngineWorld *world, long unique_key, long original_key);
    DrEngineObject(DrEngineWorld *world, long unique_key, long original_key, Body_Type body_type, long texture_number = 0,
                   double x = 0, double y = 0, double z = 0, DrPointF scale = c_scale1x1, double friction = c_friction, double bounce = c_bounce,
                   bool should_collide = true, bool can_rotate = true, double angle = c_norotate, float opacity = c_opaque);
    virtual ~DrEngineObject() override;

    // Abstract Engine Thing Overrides
    virtual void        addToWorld() override;
    virtual DrThingType getThingType() override { return DrThingType::Object; }
    virtual bool        update(double time_passed, double time_warp, QRectF &area) override;

    // Shape Creation
    void                addShapeBox(double width, double height);
    void                addShapeBox(cpBB box);
    void                addShapeBoxFromTexture(long texture_number);
    void                addShapeCircle(double circle_radius, DrPointF shape_offset);
    void                addShapeCircleFromTexture(long texture_number);
    void                addShapeTriangleFromTexture(long texture_number);
    void                addShapePolygon(const QVector<DrPointF> &points);
    void                addShapeSegment(DrPointF p1, DrPointF p2, double padding = 2.0);
    void                applyShapeSettings(cpShape *shape, double area, Shape_Type shape_type);
    QVector<DrPointF>   createEllipseFromCircle(const DrPointF &center, const double &radius, const int &point_count);



    // Object Basic Settings
    const bool&         doesCollide()           { return m_does_collide; }
    Collision_Groups    getCollidesWith()       { return m_collide_with; }
    const long&         getTextureNumber()      { return m_texture_number; }
    const long&         getAssetKey()           { return m_texture_number; }

    DrPointF            mapPositionToScreen();
    void                setDoesCollide(bool should_collide) { m_does_collide = should_collide; }
    void                setCollidesWith(Collision_Groups groups) { m_collide_with = groups; }
    void                setTextureNumber(long texture_number) { m_texture_number = texture_number; }

    // Object Properties - Bounce / Friction
    const double&   getCustomFriction()         { return m_custom_friction; }
    const double&   getCustomBounce()           { return m_custom_bounce; }
    void            setCustomFriction(double new_friction) { m_custom_friction = new_friction; }
    void            setCustomBounce(double new_bounce) { m_custom_bounce = new_bounce; }

    // Object Properties - Movement
    const double&   getOriginalVelocityX()      { return m_velocity_x; }
    const double&   getOriginalVelocityY()      { return m_velocity_y; }
    const double&   getOriginalSpinVelocity()   { return m_spin_velocity; }                                                 // !!!!! In Radians
    const bool&     getUseAngleVelocity()       { return m_use_angle_velocity; }
    const bool&     getRotateToPlayer()         { return m_rotate_to_player; }
    void            setOriginalVelocityX(double vel_x)                  { m_velocity_x = vel_x; }
    void            setOriginalVelocityY(double vel_y)                  { m_velocity_y = vel_y; }
    void            setOriginalSpinVelocity(double radian_velocity)     { m_spin_velocity = radian_velocity; }              // !!!!! In Radians
    void            setUseAngleVelocity(bool angle_affects_velocity)    { m_use_angle_velocity = angle_affects_velocity; }
    void            setRotateToPlayer(bool angle_towards_player)        { m_rotate_to_player = angle_towards_player; }

    // Object Properties - One Way / Collision
    One_Way         getOneWay()                 { return m_one_way; }
    cpVect          getOneWayDirection()        { return m_one_way_direction; }
    const double&   getGravityMultiplier()      { return m_gravity_multiplier; }
    cpVect          getSurfaceVelocity()        { return m_surface_velocity; }

    void            setOneWay(One_Way one_way_type) { m_one_way = one_way_type; }
    void            setOneWayDirection(cpVect direction) { m_one_way_direction = direction; }
    void            setOneWayDirection(DrPointF direction) { m_one_way_direction = cpv(direction.x, direction.y); }
    void            setGravityMultiplier(double gravity_multiplier) { m_gravity_multiplier = gravity_multiplier; }
    void            setSurfaceVelocity(cpVect surface_vel);

    // Object Properties - Health / Damage
    Collision_Type  getCollisionType()          { return m_collision_type; }
    const bool&     isInvincible()              { return m_invincible; }
    const bool&     hasDeathTouch()             { return m_death_touch; }
    const double&   getMaxHealth()              { return m_max_health; }
    const double&   getHealth()                 { return m_health; }
    const double&   getDamage()                 { return m_damage; }
    const long&     getDamageDelay()            { return m_damage_delay; }
    const double&   getAutoDamage()             { return m_auto_damage; }
    const long&     getDeathDelay()             { return m_death_delay; }
    Death_Animation getDeathAnimation()         { return m_death_animation; }
    const long&     getDeathDuration()          { return m_death_duration; }
    const double&   getDamageRecoil()           { return m_damage_recoil; }

    void            setCollisionType(Collision_Type what_should_collide);
    void            setInvincible(bool invincible) { m_invincible = invincible; }
    void            setDeathTouch(bool should_have_death_touch) { m_death_touch = should_have_death_touch; }
    void            setMaxHealth(double new_max_health) { m_max_health = new_max_health; }
    void            setHealth(double new_health) { m_health = new_health; }
    void            setDamage(double new_damage) { m_damage = new_damage; }
    void            setDamageDelay(long new_damage_delay) { m_damage_delay = new_damage_delay; }
    void            setAutoDamage(double new_auto_damage) { m_auto_damage = new_auto_damage; }
    void            setDeathDelay(long new_death_delay_in_milliseconds) { m_death_delay = new_death_delay_in_milliseconds; }
    void            setDeathAnimation(Death_Animation death_animation) { m_death_animation = death_animation; }
    void            setDeathDuration(long new_duration_in_milliseconds) { m_death_duration = new_duration_in_milliseconds; }
    void            setDamageRecoil(double new_damage_recoil_force) { m_damage_recoil = new_damage_recoil_force; }

    bool            doesDamage();
    bool            shouldDamage(Collision_Type check_can_damage);
    bool            takeDamage(double damage_to_take, bool reset_damage_timer = true, bool death_touch = false);

    bool            shouldCollide(DrEngineObject *object);

    // Object Movement - Rotation (On Z Axis)
    const double&   getRotateSpeedZ()           { return m_rotate_speed; }
    void            setRotateSpeedZ(double new_rotate_speed) { m_rotate_speed = new_rotate_speed; }

    // Object Movemnt - PlayerUpdateVelocity Callback Func
    const bool&     hasKeyControls()            { return m_key_controls; }
    const bool&     hasLostControl()            { return m_lost_control; }
    const double&   getMaxSpeedX()              { return m_max_speed_x; }
    const double&   getMaxSpeedY()              { return m_max_speed_y; }
    const double&   getForcedSpeedX()           { return m_forced_speed_x; }
    const double&   getForcedSpeedY()           { return m_forced_speed_y; }
    const double&   getMoveSpeedX()             { return m_move_speed_x; }
    const double&   getMoveSpeedY()             { return m_move_speed_y; }
    const bool&     getAngleMovement()          { return m_angle_movement; }
    const double&   getJumpForceX()             { return m_jump_force_x; }
    const double&   getJumpForceY()             { return m_jump_force_y; }
    const long&     getJumpTimeout()            { return m_jump_timeout; }
    const int&      getJumpCount()              { return m_jump_count; }

    const double&   getAcceleration()           { return m_acceleration; }
    const double&   getAirDrag()                { return m_air_drag; }
    const double&   getGroundDrag()             { return m_ground_drag; }
    const double&   getRotateDrag()             { return m_rotate_drag; }
    const bool&     canAirJump()                { return m_air_jump; }
    const bool&     canWallJump()               { return m_wall_jump; }
    const bool&     canRotate()                 { return m_can_rotate; }
    bool            ignoreGravity()             { return m_ignore_gravity; }
    bool            shouldFlipImageX()          { return m_flip_image_x; }
    bool            shouldFlipImageY()          { return m_flip_image_y; }
    bool            shouldMouseRotate()         { return m_mouse_rotate; }

    void            setKeyControls(bool has_key_controls) { m_key_controls = has_key_controls; }
    void            setLostControl(bool lost_control) { m_lost_control = lost_control; }
    void            setMaxSpeedX(double new_max_speed_x) { m_max_speed_x = new_max_speed_x; }
    void            setMaxSpeedY(double new_max_speed_y) { m_max_speed_y = new_max_speed_y; }
    void            setForcedSpeedX(double new_forced_speed_x) { m_forced_speed_x = new_forced_speed_x; }
    void            setForcedSpeedY(double new_forced_speed_y) { m_forced_speed_y = new_forced_speed_y; }
    void            setMoveSpeedX(double new_move_speed_x) { m_move_speed_x = new_move_speed_x; }
    void            setMoveSpeedY(double new_move_speed_y) { m_move_speed_y = new_move_speed_y; }
    void            setAngleMovement(bool angle_should_affect_movement) { m_angle_movement = angle_should_affect_movement; }
    void            setJumpForceX(double new_jump_force_x) { m_jump_force_x = new_jump_force_x; }
    void            setJumpForceY(double new_jump_force_y) { m_jump_force_y = new_jump_force_y; }
    void            setJumpTimeout(long new_jump_timeout) { m_jump_timeout = new_jump_timeout; }
    void            setJumpCount(int new_jump_count) { m_jump_count = (new_jump_count < -1) ? c_unlimited_jump : new_jump_count; }

    void            setAcceleration(double new_acceleration) { m_acceleration = new_acceleration; }
    void            setAirDrag(double new_air_drag) { m_air_drag = new_air_drag; }
    void            setGroundDrag(double new_ground_drag) { m_ground_drag = new_ground_drag; }
    void            setRotateDrag(double new_rotate_drag) { m_rotate_drag = new_rotate_drag; }
    void            setCanAirJump(bool can_air_jump) { m_air_jump = can_air_jump; }
    void            setCanWallJump(bool can_wall_jump) { m_wall_jump = can_wall_jump; }
    void            setCanRotate(bool can_rotate) { m_can_rotate = can_rotate; }
    void            setIgnoreGravity(bool ignore_gravity) { m_ignore_gravity = ignore_gravity; }
    void            setFlipImageX(bool flip_x) { m_flip_image_x = flip_x; }
    void            setFlipImageY(bool flip_y) { m_flip_image_y = flip_y; }
    void            setMouseRotate(bool mouse_rotate) { m_mouse_rotate = mouse_rotate; }

    // ***** Local Variables - Updated By Engine
    const int&      getRemainingJumps()         { return m_remaining_jumps; }
    const double&   getRemainingBoost()         { return m_remaining_boost; }
    const double&   getRemainingGroundTime()    { return m_remaining_ground_time; }
    const double&   getRemainingWallTime()      { return m_remaining_wall_time; }
    const bool&     isOnGround()                { return m_grounded; }
    const bool&     isOnWall()                  { return m_on_wall; }
    const double&   getTempGravityMultiplier()  { return m_temp_gravity_multiplier; }
    const cpVect&   getLastTouchedGroundNormal() { return m_last_touched_ground_normal; }
    const double&   getLastTouchedGroundDot()   { return m_last_touched_ground_dot; }
    Jump_State      getJumpState()              { return m_jump_state; }
    void            setRemainingJumps(int new_remaining_jumps) { m_remaining_jumps = new_remaining_jumps; }
    void            setRemainingBoost(double boost_time) { m_remaining_boost = boost_time; }
    void            setRemainingGroundTime(double ground_time) { m_remaining_ground_time = ground_time; }
    void            setRemainingWallTime(double wall_time) { m_remaining_wall_time = wall_time; }
    void            setOnGround(bool on_ground) { m_grounded = on_ground; }
    void            setOnWall(bool on_wall) { m_on_wall = on_wall; }
    void            setTempGravityMultiplier(double gravity_multiplier) { m_temp_gravity_multiplier = gravity_multiplier; }
    void            setLastTouchedGroundNormal(cpVect last_touched_normal) { m_last_touched_ground_normal = last_touched_normal; }
    void            setLastTouchedGroundDot(double last_touched_dot) { m_last_touched_ground_dot = last_touched_dot; }
    void            setJumpState(Jump_State new_jump_state) { m_jump_state = new_jump_state; }

    const bool&     isDying()                   { return m_dying; }
    const bool&     isAlive()                   { return m_alive; }
    bool            isDead()                    { return (!m_alive); }
    void            setDying(bool is_dying) { m_dying = is_dying; }
    void            setAlive(bool is_alive) { m_alive = is_alive; }

    DrTime&         getDamageTimer()            { return m_damage_timer; }
    DrTime&         getDeathTimer()             { return m_death_timer; }
    DrTime&         getFadeTimer()              { return m_fade_timer; }

    bool            isFlippedX()                { return (m_flipped_x); }
    bool            isFlippedY()                { return (m_flipped_y); }
    void            setFlipX(bool flipped)  { m_flipped_x = flipped; }
    void            setFlipY(bool flipped)  { m_flipped_y = flipped; }

    // Object->Body Data - Updated every frame by updateWorld()
    Vec3            getPreviousPosition()       { return m_previous_position; }
    void            updateBodyPosition(DrPointF updated_position, bool update_previous_position_also = false);

};



#endif // ENGINE_OBJECT_H























