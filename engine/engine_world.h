//
//      Created by Stephens Nunnally on 6/12/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef ENGINE_WORLD_H
#define ENGINE_WORLD_H

#include <QColor>
#include <QVector>
#include <QVector3D>
#include <map>

#include "chipmunk/chipmunk.h"
#include "enums_engine.h"

// Local Enumerations
enum class Render_Type {
    Perspective,
    Orthographic,
};

// Forward declarations
class DrEngine;
class DrEngineCamera;
class DrEngineObject;
class DrProject;
class DrStage;

// Type Definitions
typedef std::map<long, DrEngineCamera*>  EngineCameraMap;
typedef QVector<DrEngineObject*>         EngineObjects;

// Global Forward Declaratopns for static Chipmunk callbacks
extern cpBool   BeginFuncWildcard(cpArbiter *arb, cpSpace *, void *);                                       // defined in engine_space_collision.cpp
extern cpBool   PreSolveFuncWildcard(cpArbiter *arb, cpSpace *, void *);                                    // defined in engine_space_collision.cpp
extern void     SeperateFuncWildcard(cpArbiter *arb, cpSpace *, void *);                                    // defined in engine_space_collision.cpp
extern void     PlayerUpdateVelocity(cpBody *body, cpVect gravity, cpFloat damping, cpFloat dt);            // defined in engine_space_update_player.cpp


//####################################################################################
//##    DrEngineWorld
//##        Holds one Physics World for use in Engine
//############################
class DrEngineWorld
{

private:
    // External Borrowed Objects
    DrEngine           *m_engine;                   // Pointer to Parent Engine
    DrProject          *m_project;                  // Pointer to Project loaded into Engine

    // Local Variables
    long                m_world;                    // Key of world from Project represented in this instance of DrEngineWorld
    EngineCameraMap     m_cameras;                  // Map of Cameras used for this Engine

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
    double          m_game_direction = 0.0;         // Direction to load new levels, 0 = to the right, 90 = up, 180 = to the left, etc
    QPointF         m_game_start {0, 0};            // Origin point start stage loaded at
    double          m_game_distance = 0.0;          // Keeps track of distance traveled linearly from the start
    double          m_load_buffer = 3000;           // Distance further from m_game_distance we should already have preloaded
    double          m_loaded_to = 0.0;              // Last distance we loaded a new stage


// ***** Public Variables not yet implemented into function calls / getters / setters
public:
    // Local Variables
    EngineObjects   objects;                                    // Holds all objects shown in cpSpace
    bool            has_scene = false;                          // True after a scene has been loaded into cpSpace
    Render_Type     render_type = Render_Type::Orthographic;    // Should render perspective or orthographic?

    // Image Post Processing Variables
    float           bitrate = 16.0;                             // Bitrate          1 to 16
    float           pixel_x = 1.0;                              // Pixelation X     1.0+
    float           pixel_y = 1.0;                              // Pixelation Y     1.0+
    bool            negative = false;                           // Negative         True / False
    bool            grayscale = false;                          // Grayscale        True / False
    float           hue = 0.0f;                                 // Hue              Editor:    0 to 360     Shader:  0.0 to 1.0
    float           saturation = 0.0f;                          // Saturation       Editor: -255 to 255     Shader: -1.0 to 1.0
    float           contrast = 0.0f;                            // Contrast         Editor: -255 to 255     Shader: -1.0 to 1.0
    float           brightness = 0.0f;                          // Brightness       Editor: -255 to 255     Shader: -1.0 to 1.0



public:
    // Constructor / Destrcutor / Cleanup
    DrEngineWorld(DrEngine *engine, DrProject *project, long world_key);
    ~DrEngineWorld();

    // Space Construction / Handling
    DrEngineObject* addLine(  Body_Type body_type,  QPointF p1, QPointF p2, double friction, double bounce, double mass);

    DrEngineObject* addCircle(Body_Type body_type,  long texture_number, double x, double y, double z, double angle, QPointF scale, double opacity,
                              double shape_radius, QPointF shape_offset, double friction, double bounce, QPointF velocity,
                              bool should_collide = true, bool can_rotate = true);

    DrEngineObject* addBlock( Body_Type body_type, long texture_number, double x, double y, double z, double angle, QPointF scale, double opacity,
                              double friction, double bounce, QPointF velocity,
                              bool should_collide = true, bool can_rotate = true);

    DrEngineObject* addPolygon(Body_Type body_type, long texture_number, double x, double y, double z, double angle, QPointF scale, double opacity,
                               QVector<QPointF> points, double friction, double bounce, QPointF velocity,
                               bool should_collide = true, bool can_rotate = true);

    void            addPlayer(Demo_Player new_player_type);
    void            assignPlayerControls(DrEngineObject *object, bool has_controls_now, bool add_camera, bool set_active_camera);
    void            buildSpace(Demo_Space new_space_type);
    void            clearSpace();
    void            loadStageToSpace(DrStage *stage, double offset_x, double offset_y);
    void            removeObject(DrEngineObject *object);
    void            updateSpace(double time_passed);
    void            updateSpaceHelper();
    void            wakeAllBodies();

    // Cameras
    long                addCamera(DrEngineObject* object_to_follow = nullptr, float x = 0, float y = 0, float z = 800);
    void                clearCameras();
    const long&         getActiveCamera()       { return m_active_camera; }
    void                setActiveCamera(long new_camera) { m_active_camera = new_camera; }
    DrEngineCamera*     getCamera(long camera_id) { return m_cameras[camera_id]; }
    EngineCameraMap&    getCameraMap() { return m_cameras; }
    QVector3D           getCameraPos();
    double              getCameraPosXD();
    double              getCameraPosYD();
    double              getCameraPosZD();
    void                moveCameras(double milliseconds);
    void                switchCameras(long new_camera);
    void                updateCameras();

    // Getter and Setters
    DrEngine*           getEngine()                 { return m_engine; }
    DrProject*          getProject()                { return m_project; }
    cpSpace*            getSpace()                  { return m_space; }

    cpFloat             getTimeStepAsMilliseconds() { return (1000.0 * m_time_step); }
    const cpFloat&      getTimeStep()               { return m_time_step; }
    const cpFloat&      getTimeWarp()               { return m_time_warp; }
    const cpVect&       getGravity()                { return m_gravity; }
    const cpFloat&      getDamping()                { return m_damping; }
    const cpFloat&      getFriction()               { return m_friction; }
    const cpFloat&      getBounce()                 { return m_bounce; }

    const double&       getDeleteThresholdX()       { return m_delete_threshold_x; }
    const double&       getDeleteThresholdY()       { return m_delete_threshold_y; }
    const QColor&       getBackgroundColor()        { return m_background_color; }

    void                setTimeWarp(double new_time_warp) { m_time_warp = new_time_warp; }
    void                setGravity(cpVect new_gravity) { m_gravity = new_gravity; }
    void                setDamping(cpFloat new_damping) { m_damping = new_damping; }
    void                setFriction(cpFloat new_friction) { m_friction = new_friction; }
    void                setBounce(cpFloat new_bounce) { m_bounce = new_bounce; }

    void                setDeleteThresholdX(double new_x)   { m_delete_threshold_x = new_x; }
    void                setDeleteThresholdY(double new_y)   { m_delete_threshold_y = new_y; }
    void                setBackgroundColor(QColor new_color) { m_background_color = new_color; }

};




#endif // ENGINE_WORLD_H



















