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
#include "engine_object.h"


// !!!!! #TEMP: Temporary Demo Enums
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

enum Test_Textures {
    Ball =  -1,
    Block = -2,
    Plant = -3,
    Rover = -4,
    Wheel = -5,
    Spare = -6
};
// ########## !!!!! END

// Local Enumerations
enum class Render_Type {
    Perspective,
    Orthographic,
};

enum class Pedal {
    Brake,
    None,
    Clockwise,
    CounterClockwise,
};

// Forward declarations
class DrEngineCamera;
class DrEngineTexture;
class DrEngineWorld;
class DrProject;
class DrStage;
class FormEngine;

// Type definitions
typedef std::map<long, DrEngineCamera*>  EngineCameraMap;
typedef std::map<long, DrEngineTexture*> EngineTextureMap;

// Global Forward Declaratopns for static Chipmunk callbacks
extern void playerUpdateVelocity(cpBody *body, cpVect gravity, cpFloat damping, cpFloat dt);            // defined in engine_udate_player.cpp

//####################################################################################
//##    DrEngine
//##        Holds a project ready for use in an OpenGLWidget
//############################
class DrEngine
{
private:
    // Locals
    FormEngine         *m_form_engine;              // Pointer to Parent FormEngine
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
    const double    m_time_step = 1.0 / 60.0;       // Speed at which want to try to update the Space, 1 / 60 = 60 times per second to up
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
    double          m_game_direction = 0.0;         // Direction to load new levels, 0 = to the right, 90 = up, 180 = to the left, etc
    QPointF         m_game_start {0, 0};            // Origin point start stage loaded at
    double          m_game_distance = 0.0;          // Keeps track of distance traveled linearly from the start
    double          m_load_buffer = 3000;           // Distance further from m_game_distance we should already have preloaded
    double          m_loaded_to = 0.0;              // Last distance we loaded a new stage



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

    double          fps_render =  60.0;
    double          fps_physics = 60.0;

    Demo_Space      demo_space =  Demo_Space::Project;
    Demo_Player     demo_player = Demo_Player::Car;
    SceneObject    *demo_jumper_1;
    SceneObject    *demo_jumper_2;



public:
    DrEngine(FormEngine *form_engine, DrProject *project);

    SceneObject*    addLine(  Body_Type body_type,  QPointF p1, QPointF p2, double friction, double bounce, double mass);

    SceneObject*    addCircle(Body_Type body_type,  long texture_number, double x, double y, double z, double angle, QPointF scale, double opacity,
                              double shape_radius, QPointF shape_offset, double friction, double bounce, QPointF velocity,
                              bool should_collide = true, bool can_rotate = true);

    SceneObject*    addBlock( Body_Type body_type, long texture_number, double x, double y, double z, double angle, QPointF scale, double opacity,
                              double friction, double bounce, QPointF velocity,
                              bool should_collide = true, bool can_rotate = true);

    SceneObject*    addPolygon(Body_Type body_type, long texture_number, double x, double y, double z, double angle, QPointF scale, double opacity,
                               QVector<QPointF> points, double friction, double bounce, QPointF velocity,
                               bool should_collide = true, bool can_rotate = true);

    void        addPlayer(Demo_Player new_player_type);
    void        assignPlayerControls(SceneObject *object, bool has_controls_now, bool add_camera, bool set_active_camera);
    void        buildSpace(Demo_Space new_space_type);
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
    FormEngine*         getFormEngine()         { return m_form_engine; }
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











