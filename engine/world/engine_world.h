//
//      Created by Stephens Nunnally on 6/12/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef ENGINE_WORLD_H
#define ENGINE_WORLD_H

#include "engine/globals_engine.h"

// Local Structs
struct Thing_Info {                                         // Basic Thing Properties
    double      angle =         0.0;
    float       opacity =       1.0;
    DrPointF    position =      DrPointF(0.0, 0.0);
    DrPointF    scale =         DrPointF(1.0, 1.0);
    DrPointF    size =          DrPointF(1.0, 1.0);
    double      z_order =       0.0;
};

struct Cam_Info {                                           // Character Camera Properties
    DrPointF    position =      DrPointF(0, 0);
    DrPointF    rotation =      DrPointF(0, 0);
    double      tilt =          0.0;
    double      distance =      c_default_camera_z;
    double      zoom =          1.0;
    DrPointF    lag =           DrPointF(0, 0);
    bool        match_angle =   false;
    Up_Vector   up =            Up_Vector::Y;
    Frame_Edge  frame_top =     Frame_Edge::Normal;
    Frame_Edge  frame_right =   Frame_Edge::Normal;
    Frame_Edge  frame_bottom =  Frame_Edge::Normal;
    Frame_Edge  frame_left =    Frame_Edge::Normal;
};

// Global Forward Declarations for static Chipmunk callbacks
extern cpBool   BeginFuncWildcard(cpArbiter *arb, cpSpace *, void *);                                       // defined in engine_world_collision.cpp
extern cpBool   PreSolveFuncWildcard(cpArbiter *arb, cpSpace *, void *);                                    // defined in engine_world_collision.cpp
extern void     PostSolveFuncWildcard(cpArbiter *arb, cpSpace *, void *);                                   // defined in engine_world_collision.cpp
extern void     SeperateFuncWildcard(cpArbiter *arb, cpSpace *, void *);                                    // defined in engine_world_collision.cpp

extern void     KinematicUpdateVelocity(cpBody *body, cpVect gravity, cpFloat damping, cpFloat dt);         // defined in engine_world__update.cpp
extern void     ObjectUpdateVelocity(cpBody *body, cpVect gravity, cpFloat damping, cpFloat dt);            // defined in engine_world__update.cpp
extern void     PlayerUpdateVelocity(cpBody *body, cpVect gravity, cpFloat damping, cpFloat dt);            // defined in engine_world__update_player.cpp

extern cpBool   WaterPreSolve(cpArbiter *arb, cpSpace *space, void *);


//####################################################################################
//##    DrEngineWorld
//##        Holds one Physics World for use in Engine
//############################
class DrEngineWorld
{
public:
    // Constructor / Destrcutor
    DrEngineWorld(DrEngine *engine, DrProject *project, EngineTextureMap &textures, long world_key);
    ~DrEngineWorld();


    // #################### VARIABLES ####################
private:
    // External Borrowed Pointers
    DrEngine           *m_engine    { nullptr };    // Pointer to Engine
    DrProject          *m_project   { nullptr };    // Pointer to Project loaded into Engine
    EngineTextureMap   &m_textures;                 // Reference to map of Textures used for Rendering

    // Local Variables
    long                m_key_generator = 100;      // Variable to hand out unique id key's to all children items, keys start at 1
    long                m_world;                    // Key of world from Project represented in this instance of DrEngineWorld

    // **********
    //      Every Item In These Projects Maps:
    //          - Is assigned an unique key upon creation from DrEngineWorld::getNextKey()
    //
    EngineCameraMap     m_cameras;                  // Map of Cameras used for this Engine
    EngineSpawners      m_spawners;                 // Holds all recurring Spawners loaded into this world
    EngineThings        m_things;                   // Holds all things being used in this World / cpSpace



    // Chipmunk Physics Space
    cpSpace        *m_space = nullptr;              // Current physics space shown on screen
                                                    //
    const int       m_iterations = 10;              // Times physics are processed each update, 10 is default and should be good enough for most games
    const double    m_time_step =  1.0 / 60.0;      // Speed at which want to try to update the Space, 1 / 60 = 60 times per second to up
                                                    //      It is *highly* recommended to use a fixed size time step (calling Update at a fixed interval)
    cpFloat         m_time_warp =  1.0;             // Speeds up or slows down physics time, 1 = 100% = Normal Time, Lower than 1 = slower, Higher = faster
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
    long            m_active_camera = c_no_key;                             // Key to active camera in the Engine, c_no_key == No Camera
    bool            m_switching_cameras = false;                            // True when we want to start tweening towards a new camera

    double          m_switch_milliseconds;                                  // Used for tweening between cameras
    glm::vec3       m_switch_position,          m_temp_position;            // Used for tweening between cameras
    glm::vec3       m_switch_rotation,          m_temp_rotation;            // Used for tweening between cameras
    double          m_switch_zoom,              m_temp_zoom;                // Used for tweening between cameras
    double          m_switch_follow_angle,      m_temp_follow_angle;        // Used for tweening between cameras
    glm::vec3       m_switch_up_vector,         m_temp_up_vector;           // Used for tweening between cameras
    double          m_switch_z_order,           m_temp_z_order;             // Used for tweening between cameras
    bool            m_switch_match = false,     m_temp_match = false;       // Used for tweening between cameras


    // Scene Variables
    double          m_delete_threshold_x    { 5000.0 };                     // X distance away from camera an object can be before it's removed from the scene
    double          m_delete_threshold_y    { 5000.0 };                     // Y distance away from camera an object can be before it's removed from the scene

    double          m_ambient_light         { 50.0 };                       // Ambient light percentage to use for World, mostly for use with Glow Lights
    double          m_glow_light_z_order    { 0.0 };                        // Z-Ordering for Glow Lights
    int             m_glow_blend_mode       { 0 };                          // enum class Blend_Mode decides how light layer is applied to screen

    DrColor         m_background_color      { Dr::black };                  // Background color to use to clear screen during render
    double          m_game_direction        { 0.0 };                        // Direction to load new levels, 0 = to the right, 90 = up, 180 = to the left, etc
    DrPointF        m_game_start            { 0, 0 };                       // Origin point start stage loaded at
    double          m_game_distance         { 0.0 };                        // Keeps track of distance traveled linearly from the start
    double          m_max_game_distance     { 0.0 };                        // Keeps track of maximum distance character has traveled linearly from the start
    double          m_load_buffer           { 3000 };                       // Distance further from m_game_distance we should already have preloaded
    double          m_loaded_to             { 0.0 };                        // Last distance we loaded a new stage


// ***** Public Variables not yet implemented into function calls / getters / setters
public:
    bool            has_scene               { false };                      // True after a scene has been loaded into cpSpace
    Render_Type     render_type             { Render_Type::Perspective };   // Should render Perspective or Orthographic?
    Render_Mode     render_mode             { Render_Mode::Mode_2D };       // Should render in 2D or 3D?
    double          cam_switch_speed        { 1.0 };                        // Multiplier for Camera switching speed
    bool            zoom_from_movement      { false };                      // When true, camera changes zoom based on camera target average speed
    Auto_Zoom       zoom_type               { Auto_Zoom::Zoom_Out };        // When zoom_from_movement is true, decides whether cameras should zoom out or in upon movement
    double          zoom_multiplier         { 1.0 };                        // When zoom_from_movement is true, decides multiplier of zoom change (1 = 100%, 2 = 200%, etc)
    double          zoom_damping            { 1.0 };                        // When zoom_from_movement is true, decides damping of zoom change (1 = normal, 100 = slow)

    long            effect_count            { 0 };                          // Stores number of effects being rendered
    long            light_count             { 0 };                          // Stores number of lights in scene
    std::list<long> mark_light_as_deleted;                                  // Marks a light as removed from scene for use by other parts of engine (shadow fbos)

    // !!!!! #TEMP: Variables
    Demo_Player     demo_player             { Demo_Player::Jump };
    DrEngineObject *ball                    { nullptr };
    DrEngineLight  *light1                  { nullptr };
    DrEngineLight  *light2                  { nullptr };

    // Image Post Processing Variables
    float           bitrate                 { 256.0f };                     // Bitrate              1 to 256
    float           pixel_x                 { 1.0f };                       // Pixelation X         1.0+
    float           pixel_y                 { 1.0f };                       // Pixelation Y         1.0+
    Pixel_Texture   pixel_texture           { Pixel_Texture::None };        // Pixel Texture        None, Kintted, Woven, Etc
    bool            negative                { false };                      // Negative             True / False
    bool            grayscale               { false };                      // Grayscale            True / False
    float           hue                     { 0.0f };                       // Hue                  Editor:    0 to 360     Shader:  0.0 to 1.0
    float           saturation              { 0.0f };                       // Saturation           Editor: -255 to 255     Shader: -1.0 to 1.0
    float           contrast                { 0.0f };                       // Contrast             Editor: -255 to 255     Shader: -1.0 to 1.0
    float           brightness              { 0.0f };                       // Brightness           Editor: -255 to 255     Shader: -1.0 to 1.0

    bool            wireframe               { false };                      // Wireframe            True / False
    float           wireframe_width         { 1.0f };                       // Wireframe Width
    bool            cartoon                 { false };                      // Cartoon              True / False
    float           cartoon_width           { 5.0f };                       // Cartoon Width
    bool            cross_hatch             { false };                      // Cross Hatch          True / False
    float           cross_hatch_width       { 5.0f };                       // Cross Hatch Width
    bool            wavy                    { false };                      // Wavy (Ripple)        True / False



    // #################### FUNCTIONS TO BE EXPOSED TO API ####################
public:
    // Basics
    ///void             addStage(std::string name);                         // Adds DrStage with name to the World
    ///DrEngineThing   *clone();                                            // Will clone a Thing and return a copy of it
    ///DrEngineThing   *create(std::string name);                           // Will create a Thing based on an DrAsset name
    EngineThings        find(std::string name);                             // Returns a list of Things (entities) with designated name

    // Cameras
    ///DrEngineCamera  *camera(long camera_id);                             // Returns a camera by ID number

    // Variables
    ///DrVariant        global(std::string name);                           // Returns global variable
    ///void             setGlobal(std::string name, DrVariant value);       // Sets global variable

    // Input
    ///DrPointF         mousePosition();                                    // Returns mouse world coordinate position

    // Math
    /// Comparison, Random, Vectors

    // Logic
    /// If Then, Loop

    // Sound

    // Timers

    // System
    ///std::string      device();                                           // Returns device name
    ///DrPointF         screenSize();                                       // Returns screen size of rendering window
    ///void             vibrate(double intensity);                          // Cause device to vibrate



    // #################### INTERNAL FUNCTIONS ####################
public:
    // Important Functions
    long                getNextKey()            { return m_key_generator++; }

    // Thing Finding
    DrEngineThing*      findThingByKey(long key);
    DrEngineObject*     findObjectByKey(long key);



    // Soft Bodies
    DrEngineObject*     addBall(DrEngineWorld *world, long original_key, long asset_key, Soft_Body_Shape shape,
                                double pos_x, double pos_y, double pos_z, DrPointF scale, DrPointF radius_multiplier, double friction, double bounce, bool collides, bool can_rotate);

    DrEngineObject*     addSoftBodyCircle(long original_key, long asset_key,
                                          double pos_x, double pos_y, double pos_z, double diameter, double stiffness, double friction, double bounce, bool can_rotate = true);

    DrEngineObject*     addSoftBodyDouble(long texture,
                                          double pos_x, double pos_y, double pos_z, double diameter, double stiffness, double friction, double bounce, bool can_rotate = true);
    DrEngineObject*     addSoftBodySquare(long texture,
                                          double pos_x, double pos_y, double pos_z, DrPointF scale,  double stiffness, double friction, double bounce, bool can_rotate = true);
    DrEngineObject*     addSoftBodyMesh(  long texture,
                                          double pos_x, double pos_y, double pos_z, DrPointF scale,  double stiffness, double friction, double bounce, bool can_rotate = true);


    // Physics
    void                applyCategoryMask(DrEngineObject *central, long group_id);

    // Building / Updating
    void                addPlayer(Demo_Player new_player_type);
    void                addStage();
    void                addThing(DrEngineThing *thing);
    void                addThings(std::list<DrEngineThing*> things);

    void                assignPlayerControls(DrEngineObject *object, bool has_controls_now, bool set_active_camera);
    void                buildWorld(long world_id_to_build, Demo_Player player_to_use = Demo_Player::Player);
    void                clearWorld();
    int                 countCharacters();

    static Cam_Info     loadCharacterCameraSettings(DrThing *thing);
    void                loadThing3DSettings(DrThing *thing, DrEngineThing *object);
    void                loadThingAppearanceSettings(DrThing *thing, DrEngineObject *object);
    static Thing_Info   loadThingBasicInfo(DrThing *thing);
    void                loadThingCollisionShape(DrAsset *asset, DrEngineObject *object);
    void                loadThingControlsSettings(DrAsset *asset, DrEngineObject *object);
    void                loadThingHealthSettings(DrAsset *asset, DrEngineObject *object);

    void                loadCameraToWorld(DrThing *thing, double offset_x, double offset_y);
    void                loadCharacterToWorld(DrThing *thing);
    void                loadFireToWorld(DrThing *thing, double offset_x, double offset_y);
    void                loadFisheyeToWorld(DrThing *thing, double offset_x, double offset_y);
    void                loadLightToWorld(DrThing *thing, double offset_x, double offset_y);
    void                loadMirrorToWorld(DrThing *thing, double offset_x, double offset_y);
    DrEngineObject*     loadObjectToWorld(DrThing *thing, double offset_x, double offset_y, double scale_x, double scale_y,
                                          double angle, double x_velocity, double y_velocity, double rotate_spawn = 0.0);
    void                loadStageToWorld(DrStage *stage, double offset_x, double offset_y, bool start_stage = false);
    void                loadSwirlToWorld(DrThing *thing, double offset_x, double offset_y);
    void                loadWaterToWorld(DrThing *thing, double offset_x, double offset_y);

    void                updateSpace(double time_passed);
    void                updateSpawners(double time_passed, double time_warp, DrRectF &area);
    void                updateThings(double time_passed, double time_warp, DrRectF &area);
    void                updateWorld(double time_passed);
    void                wakeAllBodies();


    // Textures
    DrEngineTexture*    getTexture(long texture_id) { return m_textures[texture_id]; }
    EngineTextureMap&   getTextureMap() { return m_textures; }


    // Spawners
    void                addSpawner(DrEngineSpawner* spawner) { m_spawners.push_back(spawner); }


    // Cameras
    DrEngineCamera*     addCamera(long thing_key_to_follow = 0, float x = 0, float y = 0, float z = c_default_camera_z, int buffer_size = c_slop_buffer_size);
    const long&         getActiveCamera() { return m_active_camera; }
    void                setActiveCamera(long new_camera) { m_active_camera = new_camera; }
    DrEngineCamera*     getCamera(long camera_id);
    EngineCameraList    getCamerasFollowThing(long thing_id);
    EngineCameraMap&    getCameraMap() { return m_cameras; }
    glm::vec3           getCameraPosition();
    double              getCameraPositionX();
    double              getCameraPositionY();
    double              getCameraPositionZ();
    glm::vec3           getCameraRotation();
    double              getCameraRotationX();
    double              getCameraRotationY();
    double              getCameraRotationZ();
    double              getCameraFollowingRotation();
    float               getCameraFollowingZ();
    bool                getCameraMatching();
    glm::vec3           getCameraUpVector();
    double              getCameraZoom();
    void                moveCameras(double milliseconds);
    void                switchCameraToNext(bool only_switch_to_character_cameras, bool switch_player_controls);
    void                switchCameras(long new_camera);
    void                updateCameras();


    // Getter and Setters
    DrEngine*           getEngine()                 { return m_engine; }
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
    DrColor&            getBackgroundColor()        { return m_background_color; }
    const double&       getAmbientLight()           { return m_ambient_light; }
    const double&       getGlowZOrder()             { return m_glow_light_z_order; }
    Blend_Mode          getGlowBlendMode()          { return static_cast<Blend_Mode>(m_glow_blend_mode); }

    double              getDistance()               { return m_game_distance; }
    double              getMaxDistance()            { return m_max_game_distance; }
    double              getLoadedTo()               { return m_loaded_to; }

    void                setTimeWarp(double new_time_warp) { m_time_warp = new_time_warp; }
    void                setGravity(cpVect new_gravity) { m_gravity = new_gravity; }
    void                setDamping(cpFloat new_damping) { m_damping = new_damping; }
    void                setFriction(cpFloat new_friction) { m_friction = new_friction; }
    void                setBounce(cpFloat new_bounce) { m_bounce = new_bounce; }

    void                setDeleteThresholdX(double new_x)       { m_delete_threshold_x = new_x; }
    void                setDeleteThresholdY(double new_y)       { m_delete_threshold_y = new_y; }
    void                setBackgroundColor(DrColor new_color)   { m_background_color = new_color; }
    void                setAmbientLight(double new_ambient)     { m_ambient_light = new_ambient; }
    void                setGlowZOrder(double new_z_order)       { m_glow_light_z_order = new_z_order; }
    void                setGlowBlendMode(Blend_Mode mode)       { m_glow_blend_mode = static_cast<int>(mode); }

};




#endif // ENGINE_WORLD_H



















