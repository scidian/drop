//
//      Created by Stephens Nunnally on 2/2/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef THING_COMP_PLAYER_H
#define THING_COMP_PLAYER_H

#include "engine/globals_engine.h"
#include "engine/thing/engine_thing_component.h"


//####################################################################################
//##    ThingCompPlayer
//##        Built-In Component for DrEngineThing dealing with Player Controls
//############################
class ThingCompPlayer : public DrThingComponent
{
public:
    // Constructor / Destructor
    ThingCompPlayer(DrEngineWorld *engine_world, DrEngineThing *parent_thing);
    virtual ~ThingCompPlayer();


    // #################### VARIABLES ####################
private:
    // Player Movement (if DrEngineObject has this component, its physics update callback function is PlayerUpdateVelocity)
    double          m_max_speed_x       { 2000.0 };         // Maximum speed x of object
    double          m_max_speed_y       { 2000.0 };         // Maximum speed y of object

    double          m_forced_speed_x    { 0.0 };            // Forced move speed x of object
    double          m_forced_speed_y    { 0.0 };            // Forced move speed y of object

    double          m_move_speed_x      { 400.0 };          // Movement speed x
    double          m_move_speed_y      { 400.0 };          // Movement speed y
    bool            m_angle_movement    { false };          // Should the objects angle affect the objects move speeds? (good for first person, etc)

    double          m_jump_force_x      { 0.0 };            // Jump force x
    double          m_jump_force_y      { 250.0 };          // Jump force y
    long            m_jump_timeout      { 800 };            // Milliseconds to allow for jump to continue to receive a boost when jump button is held down
    int             m_jump_count        { 0 };              // How many jumps this player is allowed, -1 = c_unlimited_jump, 0 = cannot jump, 1 = 1, 2 = 2, etc

    double          m_acceleration      { 1.00 };           // Affects move / switch speeds, 0.0 is instant, 1.0 is default, 5.0 is slower
    double          m_air_drag          { 1.00 };           // Affects acceleration and decceleration in air (0 to 1+)
    double          m_ground_drag       { 1.00 };           // Affects acceleration and decceleration on the ground (0 to 1+)
    double          m_rotate_drag       { 0.25 };           // Affects rotation acceleration and decceleration (0 to 1+)

    bool            m_air_jump          { true };           // Can this player jump while in the air (even if only has 1 jump, ex: fell off platform)
    bool            m_wall_jump         { false };          // Can this player jump off of walls?

    bool            m_mouse_rotate      { false };          // If turned to true, player rotates to mouse position


    // ***** Local Variables Updated by Engine
    //              NOT TO BE SET BY USER
    //
    int         m_remaining_jumps               { 0 };          // How many jumps player has left before it must hit ground before it can jump again
    double      m_remaining_boost               { 0.0 };        // Used by Engine Update to process Jump Timeout boost
    double      m_remaining_ground_time         { 0.0 };        // Used by Engine Update to allow some time for a ground jump to occur (helps with bumpiness)
    double      m_remaining_wall_time           { 0.0 };        // Used by Engine Update to allow some time for a wall jump to occur

    bool        m_grounded                      { false };      // Used by Engine Update to keep track of if this object is on the ground
    bool        m_on_wall                       { false };      // Used by Engine Update to keep track of if this object is on a wall
    double      m_temp_gravity_multiplier       { 1.0 };        // Set multipler (0.0, 0.5, etc) when colliding with a gravity multiplying object (ladder, wall, etc)

    cpVect      m_last_touched_ground_normal    { cpvzero };    // Normal Vector of the last touched surface
    double      m_last_touched_ground_dot       { 1.0 };        // Dot product of the last touched surface
    Jump_State  m_jump_state = Jump_State::Need_To_Jump;        // Used by Engine Update to keep track of if the current jump button press has been processed

    std::vector<cpShape*> m_ledge_grabbers      { };            // Stores shapes used to process ledge grabbing, if enabled


    // #################### FUNCTIONS TO BE EXPOSED TO API ####################
public:
    // Basic Component Events
    virtual void    init();                                                         // Called when component is first created
    virtual void    update(double time_passed, double time_warp);                   // Called during DrEngineWorld->updateWorld() step
    virtual void    destroy();                                                      // Called when component is destroyed


    // #################### INTERNAL FUNCTIONS ####################
public:
    // Getters / Setters
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
    bool            shouldMouseRotate()         { return m_mouse_rotate; }

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
    void            setMouseRotate(bool mouse_rotate) { m_mouse_rotate = mouse_rotate; }

    // ***** Local Variables - Updated By Engine
    const int&      getRemainingJumps()             { return m_remaining_jumps; }
    const double&   getRemainingBoost()             { return m_remaining_boost; }
    const double&   getRemainingGroundTime()        { return m_remaining_ground_time; }
    const double&   getRemainingWallTime()          { return m_remaining_wall_time; }
    const bool&     isOnGround()                    { return m_grounded; }
    const bool&     isOnWall()                      { return m_on_wall; }
    const double&   getTempGravityMultiplier()      { return m_temp_gravity_multiplier; }
    const cpVect&   getLastTouchedGroundNormal()    { return m_last_touched_ground_normal; }
    const double&   getLastTouchedGroundDot()       { return m_last_touched_ground_dot; }
    Jump_State      getJumpState()                  { return m_jump_state; }
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

    std::vector<cpShape*>&  getLedgeGrabbers()      { return m_ledge_grabbers; }


};

#endif // THING_COMP_PLAYER_H
















