//
//      Created by Stephens Nunnally on 5/6/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef ENGINE_H
#define ENGINE_H

#include <QOpenGLTexture>
#include <QTime>
#include <QVector3D>
#include <map>

#include "chipmunk/chipmunk.h"

enum class Render_Type {
    Perspective,
    Orthographic,
};

enum class Demo_Player {
    Spawn,
    Car,
    Jump,
};

enum class Demo_Space {
    Lines1,
    Lines2,
    Blocks,
    Project,
};

enum class Pedal {
    Brake,
    None,
    Clockwise,
    CounterClockwise,
};

enum class Jump_State {
    Need_To_Jump,
    Jumped,
};

enum class Body_Type {
    Dynamic,
    Kinematic,
    Static,
};

enum class Shape_Type {
    Circle,                                     // cpCircleShapeNew
    Box,                                        // cpBoxShapeNew
    Segment,    // "Line"                       // cpSegmentShapeNew
    Polygon,                                    // cpPolyShapeNew
};

enum CollisionTypes {
    COLLISION_TYPE_ONE_WAY = 1,
};

enum Test_Textures {
    Ball =  -1,
    Block = -2,
    Plant = -3,
    Rover = -4,
    Wheel = -5,
    Spare = -6
};

// Forward declarations
class DrEngineCamera;
class DrEngineTexture;
class DrEngineWorld;
class DrProject;
class DrStage;

// Type definitions
typedef std::map<long, DrEngineCamera*>  EngineCameraMap;
typedef std::map<long, DrEngineTexture*> EngineTextureMap;
typedef std::map<cpShape*, Shape_Type> ShapeMap;

// Constants for calling engine addObject calls
constexpr QVector3D c_no_camera {0, 0, 800};    // Default camera position if there is no active camera
constexpr QPointF   c_center   {0, 0};          // Default offset in no offset
constexpr QPointF   c_scale1x1 {1, 1};          // Default scale of 1x1
constexpr double    c_norotate {0};             // Default rotation amount of zero
constexpr double    c_opaque   {1};             // Default transparency of fully opaque

// Global Variables - defined in engine_update_player.cpp
extern int      g_keyboard_x;                   // Used to pass keyboard x button state to static callback functions
extern int      g_keyboard_y;                   // Used to pass keyboard y button state to static callback functions
extern bool     g_jump_button;                  // Used to pass jump button state to static callback functions
extern bool     g_rotate_cw;                    // Used to pass rotate clockwise button state
extern bool     g_rotate_ccw;                   // Used to pass rotate counter clockwise button state
extern cpVect   g_gravity_normal;               // Stores a gravity as a normalized vector for use in static callback functions
extern QString  g_info;

// Global Forward Declaratopns for static Chipmunk callbacks
extern void playerUpdateVelocity(cpBody *body, cpVect gravity, cpFloat damping, cpFloat dt);            // defined in engine_udate_player.cpp


//####################################################################################
//##    SceneObject
//##        Holds on object for use in a cpSpace
//############################
struct SceneObject {
    // ***** Object Pieces
    cpBody             *body;               // Physical Body of object
    Body_Type           body_type;          // Body_Type

    QVector<cpShape*>   shapes;             // Collision Shapes of object
    ShapeMap            shape_type;         // Shape Types of Shapes of Object

    // ***** Object Properties
    bool        does_collide = true;        // Set to false to have this object not collide with anything

    long        texture_number;             // Reference to which texture to use from Engine.EngineTexture map
    float       scale_x = 1.0f;             // Scale of object in world
    float       scale_y = 1.0f;             // Scale of object in world
    float       alpha;                      // Transparency of object
    double      z_order;                    // Used for layering

    long        active_camera = 0;          // Set to ID of last camera that followed this object, 0 == no camera

    bool        one_way = false;            // Set to true if we're using this object as a one way platform
    cpVect      one_way_direction {0, 1};   // Direction of Normal for one way platforms

    bool        custom_friction = false;    // Set to true if we don't want this item affected by global m_friction
    bool        custom_bounce = false;      // Set to true if we don't want this item affected by global m_bounce

    double      rotate_speed = 0;           // Speed at which object should spin when Motor Rotate (gas pedal) is pressed

    // ***** Object interaction             // These properties are used by objects that have been attached to playerUpdateVelocity
    bool        lost_control = false;       // Set to true when players should not have button control
                                            //      (players are cpBody* that have been assigned the cpBodyUpdateVelocityFunc playerUpdateVelocity callback)

    double      max_speed_x =  1000.0;      // Maximum speed x of object
    double      max_speed_y =  1000.0;      // Maximum speed y of object

    double      move_speed_x =  400.0;      // Movement speed x
    double      move_speed_y =  400.0;      // Movement speed y

    double      jump_force_x =    0.0;      // Jump force x
    double      jump_force_y =  250.0;      // Jump force y
    double      jump_timeout =  800.0;      // Milliseconds to allow for jump to continue to receive a boost when jump button is held down
    int         jump_count =        0;      // How many jumps this player is allowed, -1 = unlimited, 0 = cannot jump, 1 = 1, 2 = 2, etc

    bool        can_rotate = true;          // Set during object creation, moment of inertia is set to infinity to stop rotation

    double      air_drag =       0.50;      // Affects acceleration in air (0 to 1+)
    double      ground_drag =    0.25;      // Affects acceleration on the ground (0 to 1+)

    bool        air_jump = true;            // Can this player jump while in the air (even if only has 1 jump, ex: fell off platform)
    bool        wall_jump = false;          // Can this player jump off of walls?


    // ***** Updated by Engine:
    bool        should_process = true;      // True while object exists in Space
    bool        has_been_processed = false; // Set to true after an initial updateSpace call has been ran once while the object was in the Space

    int         remaining_jumps = 0;                // How many jumps player has left before it must hit ground before it can jump again
    double      remaining_boost = 0;                // Used by Engine Update to process Jump Timeout boost
    double      remaining_wall_time = 0;            // Used by Engine Update to allow some time for a wall jump to occur
    bool        grounded = false;                   // Used by Engine Update to keep track of if this object is on the ground
    bool        on_wall = false;                    // Used by Engine Update to keep track of if this object is on a wall
    Jump_State  jump_state = Jump_State::Jumped;    // Used by Engine Update to keep track of if the current jump button press has been processed

    double      angle = 0;                  // Current object angle
    QPointF     velocity;                   // Current object velocity
    QPointF     position;                   // Current object posiiton
    QPointF     last_position;              // Previous frame position, for whatever may need it

};


//####################################################################################
//##    DrEngine
//##        Holds a project ready for use in an OpenGLWidget
//############################
class DrEngine
{
private:
    // Locals
    DrProject          *m_project;                  // Pointer to Project to load into Engine

    EngineCameraMap     m_cameras;                  // Map of Cameras used for this Engine
    EngineTextureMap    m_textures;                 // Map of Textures used for this Engine

    // Camera Variables
    long            m_active_camera = 0;            // Key to active camera in the Engine
    long            m_camera_keys = 1;              // ID Generator for cameras, camera IDs start at 1, 0 == no camera
    bool            m_switching_cameras = false;    // True when we want to start tweening towards a new camera
    QVector3D       m_temp_position;                // Used for tweening between cameras

    // Chipmunk Physics Space
    cpSpace        *m_space;                        // Current physics space shown on screen
                                                    //
    const int       m_iterations = 10;              // Times physics are processed each update, 10 is default and should be good enough for most games
    const double    m_time_step = 1 / 60.0;         // Speed at which want to try to update the Space, 1 / 60 = 60 times per second to up
                                                    //      It is *highly* recommended to use a fixed size time step (calling Update at a fixed interval)
    cpFloat         m_time_warp = 1.0;              // Speeds up or slows down physics time, 1 = 100% = Normal Time, Lower than 1 = slower, Higher = faster
                                                    //
    cpVect          m_gravity;                      // Current global gravity applied to current space. Defaults to cpvzero. Can be overridden on
                                                    //      a per body basis by writing custom integration functions. Changing the gravity will
                                                    //      activate all sleeping bodies in the space.
    cpFloat         m_damping;                      // A value of 0.9 means that each body will lose 10% of its velocity per second. Defaults to 1.
                                                    //      Like gravity, it can be overridden on a per body basis.
                                                    //      Minimum value 0.0, max unknown, values > 1 objects will gain velocity
    cpFloat         m_friction;                     // Default object Friction, can be overridden on a per body basis
                                                    //      0 = frictionless, unknown limit
    cpFloat         m_bounce;                       // Default object Elasticity, can be overridden on a per body basis
                                                    //      0 = no bounce, 1.0 will give a “perfect” bounce.
                                                    //      Due to inaccuracies in the simulation using 1.0 or greater is not recommended

    // Scene Variables
    double          m_delete_threshold_x = 5000;    // X distance away from camera an object can be before it's removed from the scene
    double          m_delete_threshold_y = 5000;    // Y distance away from camera an object can be before it's removed from the scene

    QColor          m_background_color {0,0,0};     // Background color to use to clear screen during render
    long            m_current_world;                // Key of current world shown from Project
    double          m_game_direction = 0;           // Direction to load new levels, 0 = to the right, 90 = up, 180 = to the left, etc
    QPointF         m_game_start {0, 0};            // Origin point start stage loaded at
    double          m_game_distance = 0;            // Keeps track of distance traveled linearly from the start
    double          m_load_buffer = 3000;           // Distance further from m_game_distance we should already have preloaded
    double          m_loaded_to = 0;                // Last distance we loaded a new stage



public:
    QVector<SceneObject*>   objects;

    bool            has_scene = false;
    Render_Type     render_type = Render_Type::Orthographic;          // Should render perspective or orthographic?

    int             keyboard_x;                             // Set to -1 for left, 1 for right
    int             keyboard_y;                             // Set to -1 for down, 1 for up
    bool            jump_button;                            // Set to 0 for not pressed, 1 for pressed
    Pedal           gas_pedal = Pedal::None;                // Pedal enumeration for rotate key state

    bool            debug_shapes =      false;
    bool            debug_collisions =  false;

    QTime           fps_timer;
    int             fps, last_fps;

    Demo_Space      demo_space =  Demo_Space::Project;
    Demo_Player     demo_player = Demo_Player::Car;
    SceneObject    *demo_jumper_1;
    SceneObject    *demo_jumper_2;



public:
    DrEngine(DrProject *project);

    SceneObject*    addLine(  Body_Type body_type,  QPointF p1, QPointF p2, double friction, double bounce, double mass);

    SceneObject*    addCircle(Body_Type body_type,  long texture_number, double x, double y, double z, double angle, QPointF scale, double opacity,
                              double shape_radius, QPointF shape_offset, double friction, double bounce, double mass, QPointF velocity,
                              bool should_collide = true, bool can_rotate = true);

    SceneObject*    addBlock( Body_Type body_type, long texture_number, double x, double y, double z, double angle, QPointF scale, double opacity,
                              double friction, double bounce, double mass, QPointF velocity,
                              bool should_collide = true, bool can_rotate = true);

    SceneObject*    addPolygon(Body_Type body_type, long texture_number, double x, double y, double z, double angle, QPointF scale, double opacity,
                               QVector<QPointF> points, double friction, double bounce, double mass, QPointF velocity,
                               bool should_collide = true, bool can_rotate = true);

    void        addPlayer(Demo_Player new_player_type);
    void        assignPlayerControls(SceneObject *object, bool has_controls_now, bool add_camera, bool set_active_camera);
    void        buildSpace(Demo_Space new_space_type);
    void        checkObjectCustomFrictionBounce(SceneObject *object, double &friction, double &bounce);
    void        clearSpace();
    void        loadStageToSpace(DrStage *stage, double offset_x, double offset_y);
    void        oneWayPlatform(SceneObject *object, cpVect direction);
    void        removeObject(SceneObject *object);
    void        updateSpace(double time_passed);
    void        updateSpaceHelper();

    void        deleteResources();


    // Cameras
    long                addCamera(SceneObject* object_to_follow = nullptr, float x = 0, float y = 0, float z = 800);
    void                clearCameras();
    const long&         getActiveCamera()       { return m_active_camera; }
    void                setActiveCamera(long new_camera) { m_active_camera = new_camera; }
    DrEngineCamera*     getCamera(long camera_id) { return m_cameras[camera_id]; }
    EngineCameraMap&    getCameraMap() { return m_cameras; }
    QVector3D           getCameraPos();
    void                moveCameras(double milliseconds);
    void                switchCameras(long new_camera);
    void                updateCameras();

    // Textures
    DrEngineTexture*    addTexture(long texture_id, QString from_asset_string);
    DrEngineTexture*    addTexture(long texture_id, QPixmap &pixmap);
    DrEngineTexture*    getTexture(long texture_id) { return m_textures[texture_id]; }
    EngineTextureMap&   getTextureMap() { return m_textures; }


    // Getter and Setters    
    DrProject*          getProject()            { return m_project; }
    cpSpace*            getSpace()              { return m_space; }

    const cpFloat&      getTimeStep()           { return m_time_step; }
    const cpFloat&      getTimeWarp()           { return m_time_warp; }
    const cpVect&       getGravity()            { return m_gravity; }
    const cpFloat&      getDamping()            { return m_damping; }
    const cpFloat&      getFriction()           { return m_friction; }
    const cpFloat&      getBounce()             { return m_bounce; }

    const double&       getDeleteThresholdX()   { return m_delete_threshold_x; }
    const double&       getDeleteThresholdY()   { return m_delete_threshold_y; }
    const QColor&       getBackgroundColor()    { return m_background_color; }

    void                setTimeWarp(double new_time_warp) { m_time_warp = new_time_warp; }
    void                setGravity(cpVect new_gravity) { m_gravity = new_gravity; }
    void                setDamping(cpFloat new_damping) { m_damping = new_damping; }
    void                setFriction(cpFloat new_friction) { m_friction = new_friction; }
    void                setBounce(cpFloat new_bounce) { m_bounce = new_bounce; }

    void                setDeleteThresholdX(double new_x)   { m_delete_threshold_x = new_x; }
    void                setDeleteThresholdY(double new_y)   { m_delete_threshold_y = new_y; }
    void                setBackgroundColor(QColor new_color) { m_background_color = new_color; }


};



#endif // ENGINE_H











