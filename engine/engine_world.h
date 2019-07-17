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

// Forward declarations
class DrEngine;
class DrEngineLight;
class DrEngineCamera;
class DrEngineObject;
class DrEngineThing;
class DrEngineTexture;
class DrOpenGL;
class DrProject;
class DrStage;
class DrThing;

// Type Definitions
typedef std::map<long, DrEngineCamera*>  EngineCameraMap;
typedef std::map<long, DrEngineTexture*> EngineTextureMap;
typedef QVector<DrEngineLight*>          EngineLights;
typedef QVector<DrEngineThing*>          EngineThings;

// Global Forward Declaratopns for static Chipmunk callbacks
extern cpBool   BeginFuncWildcard(cpArbiter *arb, cpSpace *, void *);                                       // defined in engine_world_collision.cpp
extern cpBool   PreSolveFuncWildcard(cpArbiter *arb, cpSpace *, void *);                                    // defined in engine_world_collision.cpp
extern void     SeperateFuncWildcard(cpArbiter *arb, cpSpace *, void *);                                    // defined in engine_world_collision.cpp
extern void     PlayerUpdateVelocity(cpBody *body, cpVect gravity, cpFloat damping, cpFloat dt);            // defined in engine_world_update_player.cpp
extern void     KinematicUpdateVelocity(cpBody *body, cpVect gravity, cpFloat damping, cpFloat dt);         // defined in engine_world_update_object.cpp

//####################################################################################
//##    DrEngineWorld
//##        Holds one Physics World for use in Engine
//############################
class DrEngineWorld
{

private:
    // External Borrowed Pointers
    DrProject          *m_project;                  // Pointer to Project loaded into Engine
    EngineTextureMap   &m_textures;                 // Reference to map of Textures used for Rendering

    // Local Variables
    long                m_key_generator = 1;        // Variable to hand out unique id key's to all children items, keys start at 1
    long                m_world;                    // Key of world from Project represented in this instance of DrEngineWorld

    // **********
    //      Every Item In These Projects Maps:
    //          - Is assigned an unique key upon creation from DrEngineWorld::getNextKey()
    //
    EngineThings        m_things;                   // Holds all things being used in this World / cpSpace
    EngineCameraMap     m_cameras;                  // Map of Cameras used for this Engine


    // Chipmunk Physics Space
    cpSpace        *m_space = nullptr;              // Current physics space shown on screen
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

    // Camera Variables
    long            m_active_camera = 0;            // Key to active camera in the Engine, 0 == No Camera
    bool            m_switching_cameras = false;    // True when we want to start tweening towards a new camera
    QVector3D       m_temp_position;                // Used for tweening between cameras


    // Scene Variables
    double          m_delete_threshold_x = 5000;    // X distance away from camera an object can be before it's removed from the scene
    double          m_delete_threshold_y = 5000;    // Y distance away from camera an object can be before it's removed from the scene

    double          m_ambient_light = 50.0;         // Ambient light percentage to use for World, mostly for use with Glow Lights
    double          m_glow_light_z_order = 0.0;     // Z Ordering for Glow Lights

    QColor          m_background_color {0,0,0,1};   // Background color to use to clear screen during render
    double          m_game_direction = 0.0;         // Direction to load new levels, 0 = to the right, 90 = up, 180 = to the left, etc
    QPointF         m_game_start {0, 0};            // Origin point start stage loaded at
    double          m_game_distance = 0.0;          // Keeps track of distance traveled linearly from the start
    double          m_load_buffer = 3000;           // Distance further from m_game_distance we should already have preloaded
    double          m_loaded_to = 0.0;              // Last distance we loaded a new stage


// ***** Public Variables not yet implemented into function calls / getters / setters
public:
    bool            has_scene = false;                          // True after a scene has been loaded into cpSpace    
    Render_Type     render_type = Render_Type::Orthographic;    // Should render perspective or orthographic?
    long            light_count = 0;                            // Stores number of lights in scene
    QList<long>     mark_light_as_deleted;                      // Marks a light as removed from scene for use by other parts of engine (shadow fbos)

    // !!!!! #TEMP: Variables
    Demo_Space      demo_space =  Demo_Space::Project;
    Demo_Player     demo_player = Demo_Player::Jump;
    DrEngineObject *ball;
    DrEngineLight  *light1;
    DrEngineLight  *light2;

    // Image Post Processing Variables
    float           pixel_x = 1.0;                              // Pixelation X     1.0+
    float           pixel_y = 1.0;                              // Pixelation Y     1.0+
    bool            negative = false;                           // Negative         True / False
    bool            grayscale = false;                          // Grayscale        True / False
    float           hue = 0.0f;                                 // Hue              Editor:    0 to 360     Shader:  0.0 to 1.0
    float           saturation = 0.0f;                          // Saturation       Editor: -255 to 255     Shader: -1.0 to 1.0
    float           contrast = 0.0f;                            // Contrast         Editor: -255 to 255     Shader: -1.0 to 1.0
    float           brightness = 0.0f;                          // Brightness       Editor: -255 to 255     Shader: -1.0 to 1.0

    float           bitrate = 16.0;                             // Bitrate          1 to 16
    bool            cartoon = false;                            // Cartoon          True / False
    bool            wavy = false;                               // Wavy (Ripple)    True / False
    bool            fisheye = false;                            // Fisheye Lens     True / False


public:
    // Constructor / Destrcutor / Cleanup
    DrEngineWorld(DrProject *project, EngineTextureMap &textures, long world_key);
    ~DrEngineWorld();

    // Important Functions
    DrEngineThing*  findThingByKey(long key);
    long            getNextKey()            { return m_key_generator++; }


    // World Construction / Handling
    void            addPlayer(Demo_Player new_player_type);
    void            assignPlayerControls(DrEngineObject *object, bool has_controls_now, bool add_camera, bool set_active_camera);
    void            addThing(DrEngineThing *thing);
    void            addThings(QList<DrEngineThing*> things);
    void            buildWorld(Demo_Space new_space_type, long current_editor_world);
    void            clearWorld();
    void            loadLightToWorld(DrThing *thing, double offset_x, double offset_y);
    void            loadObjectToWorld(DrThing *thing, double offset_x, double offset_y);
    void            loadStageToWorld(DrStage *stage, double offset_x, double offset_y);
    void            updateSpace(double time_passed);
    void            updateWorld(double time_passed);
    void            wakeAllBodies();

    // Textures
    DrEngineTexture*    getTexture(long texture_id) { return m_textures[texture_id]; }
    EngineTextureMap&   getTextureMap() { return m_textures; }

    // Cameras
    long                addCamera(long thing_key_to_follow = 0, float x = 0, float y = 0, float z = 800);
    const long&         getActiveCamera()       { return m_active_camera; }
    void                setActiveCamera(long new_camera) { m_active_camera = new_camera; }
    DrEngineCamera*     getCamera(long camera_id) { return m_cameras[camera_id]; }
    EngineCameraMap&    getCameraMap() { return m_cameras; }
    QVector3D           getCameraPos();
    double              getCameraPosXD();
    double              getCameraPosYD();
    double              getCameraPosZD();
    void                moveCameras(double milliseconds);
    void                switchCameraToNext();
    void                switchCameras(long new_camera);
    void                updateCameras();

    // Getter and Setters
    //DrEngine*           getEngine()                 { return m_engine; }
    DrProject*          getProject()                { return m_project; }
    cpSpace*            getSpace()                  { return m_space; }
    EngineThings&       getThings()                 { return m_things; }

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
    const double&       getAmbientLight()           { return m_ambient_light; }
    const double&       getGlowZOrder()             { return m_glow_light_z_order; }

    void                setTimeWarp(double new_time_warp) { m_time_warp = new_time_warp; }
    void                setGravity(cpVect new_gravity) { m_gravity = new_gravity; }
    void                setDamping(cpFloat new_damping) { m_damping = new_damping; }
    void                setFriction(cpFloat new_friction) { m_friction = new_friction; }
    void                setBounce(cpFloat new_bounce) { m_bounce = new_bounce; }

    void                setDeleteThresholdX(double new_x)    { m_delete_threshold_x = new_x; }
    void                setDeleteThresholdY(double new_y)    { m_delete_threshold_y = new_y; }
    void                setBackgroundColor(QColor new_color) { m_background_color = new_color; }
    void                setAmbientLight(double new_ambient)  { m_ambient_light = new_ambient; }
    void                setGlowZOrder(double new_z_order)    { m_glow_light_z_order = new_z_order; }

};




#endif // ENGINE_WORLD_H



















