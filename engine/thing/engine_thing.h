//
//      Created by Stephens Nunnally on 6/19/2019, (c) 2019 Scidian Studios, All Rights Reserved
//
//  File:
//      Base class that represents one DrThing within a DrEngineWorld
//
//
#ifndef ENGINE_THING_H
#define ENGINE_THING_H

#include "core/dr_time.h"
#include "engine/globals_engine.h"

// Forward Declarations
class ThingComp3D;
class ThingCompCamera;
class ThingCompFoliage;
class ThingCompPhysics;
class ThingCompPlayer;
class ThingCompSoftBody;


//####################################################################################
//##    DrEngineThing
//##        Base class that represents one DrThing within a DrEngineWorld
//############################
class DrEngineThing
{
public:
    // Constructor / Destructor
    DrEngineThing(DrEngineWorld *world, long unique_key, long original_key);
    DrEngineThing(DrEngineWorld *world, long unique_key, long original_key, double x, double y, double z,
                  DrPointF scale, double angle, float opacity, DrPointF size);
    virtual ~DrEngineThing();


    // #################### VARIABLES ####################
private:
    // External Borrowed Pointers
    DrEngineWorld      *m_world                 { nullptr };                // Points to current parent DrEngineWorld

    // Engine Info
    long                m_key                   { c_no_key };               // Unique key for this item
    long                m_original_project_key  { c_no_key };               // Key of item when it was in Project Data Model

    // Containers
    EngineSpawners      m_spawners;                                         // Holds all Spawners that have been attached to this Thing
    ThingComponents     m_components;                                       // Container of all components this Thing owns

    // Built In Components
    ThingComp3D        *m_comp_3d           { nullptr };                    // Component that handles 3D rendering
    ThingCompCamera    *m_comp_camera       { nullptr };                    // Component that handles a following camera
    ThingCompFoliage   *m_comp_foliage      { nullptr };                    // Component that handles using Thing as Foliage
    ThingCompPhysics   *m_comp_physics      { nullptr };                    // Component that handles using Thing as a Physics Object
    ThingCompPlayer    *m_comp_player       { nullptr };                    // Component that handles using Thing as a Player (character)
    ThingCompSoftBody  *m_comp_soft_body    { nullptr };                    // Component that handles giving Thing a Soft Body

    // Basic Thing Properties
    double          m_angle_z               { 0.0 };                        // Current rotation angle (on Z axis), (for physics objects this is updated every frame by update())
    float           m_opacity               { 1.0f };                       // Transparency of Thing (0.0 invisible, 1.0 opaque)
    DrPointF        m_position              { 0.0, 0.0 };                   // Current center posiiton
    double          m_z_order               { 0.0 };                        // Used for layering, used for distance sorting
    float           m_scale_x               { 1.0f };                       // Scale of Thing in world
    float           m_scale_y               { 1.0f };                       // Scale of Thing in world
    DrPointF        m_size                  { 1.0, 1.0 };                   // Original size of Thing
    double          m_velocity_x            { 0.0 };                        // Current velocity
    double          m_velocity_y            { 0.0 };                        // Current velocity

public:
    // ***** Image Post Processing Attributes
    bool            extrude_3d              { false };                      // Auto convert to 3D   True / False
    bool            wireframe               { false };                      // Wireframe            True / False
    float           wireframe_width         { 1.0f };                       // Wireframe Width
    bool            cartoon                 { false };                      // Cartoon              True / False
    float           cartoon_width           { 5.0f };                       // Cartoon Width
    bool            cross_hatch             { false };                      // Cross Hatch          True / False
    float           cross_hatch_width       { 5.0f };                       // Cross Hatch Width

    float           bitrate                 { 256.0f };                     // Bitrate              1 to 256
    float           pixel_x                 { 1.0f };                       // Pixelation X         1.0+
    float           pixel_y                 { 1.0f };                       // Pixelation Y         1.0+
    Pixel_Texture   pixel_texture =         Pixel_Texture::None;            // Pixel Texture        None, Knit, Woven, Wood, etc
    bool            negative                { false };                      // Negative             True / False
    bool            grayscale               { false };                      // Grayscale            True / False
    float           hue                     { 0.0f };                       // Hue                  Editor:    0 to 360     Shader:  0.0 to 1.0
    float           saturation              { 0.0f };                       // Saturation           Editor: -255 to 255     Shader: -1.0 to 1.0
    float           contrast                { 0.0f };                       // Contrast             Editor: -255 to 255     Shader: -1.0 to 1.0
    float           brightness              { 0.0f };                       // Brightness           Editor: -255 to 255     Shader: -1.0 to 1.0

    Blend_Object    blend_type              { Blend_Object::Standard };     // Blend mode for current object
    bool            cast_shadows            { true };                       // Will cast shadows when in front of a Light


    // ********** Internal Variables Updated by Engine
    //                NOT TO BE SET BY USER
    bool        m_remove_me                 { false };                      // Set to true for forced removal next update cycle
    double      time_since_last_update      { 0.0 };                        // Milliseconds since update() was called last
    DrTime      update_timer                { Clock::now() };               // Used to keep track of time passed since update() was called last



    // #################### FUNCTIONS TO BE EXPOSED TO API ####################
public:
    // Misc Functions
    std::string         name();                                                                     // Returns name of this Entity
    void                remove()                    { m_remove_me = true; }                         // Call to remove this Thing
    DrEngineWorld*      world()                     { return m_world; }                             // Returns DrWorld this Entity is currently in
    ThingCompPhysics*   physics()                   { return compPhysics(); }                       // Returns physics component of Entity (if never initialized returns nullptr)
    ThingCompPlayer*    player()                    { return compPlayer(); }                        // Returns player component of Entity (if never initialized returns nullptr)

    // Thing Events
    void                init();                                                                     // Called when Thing is first created
    void                addToWorld();                                                               // Called when Thing is added to m_things DrEngineWorld vector
    void                draw();                                                                     // Called when it is time to Render Thing
    bool                update(double time_passed, double time_warp, DrRectF &area);                // Process one update iteration for this Thing
    void                destroy();                                                                  // Called when Thing is destroyed

    // Thing Components
    DrThingComponent*   component(std::string component_name);
    void                removeComponent(std::string component_name);
    void                setComponent(std::string component_name, DrThingComponent *component);

    // Messages
    void                emitMessage(std::string name, DrVariant value, DrEngineThing* thing_b);     // Adds message to stack
    EngineMessages      messageList(std::string name, long thing_key = c_no_key);                   // Returns list of messages with name, optional attached to thing

    // Helper Functions
    DrPointF            mapPositionToScreen();

    // Cameras
    ///long             followCameraID();                                                           // Returns ID of following camera if there is one

    // Properties
    /// Disable, Enable
    /// Position, Rotation, Scale, Size, Velocity
    /// SetColor, GetColor??


    // #################### INTERNAL FUNCTIONS ####################
public:
    // Entity Properties
    long                    getKey() { return m_key; }                                              // Gets unique item key
    long                    getOriginalKey() { return m_original_project_key; }                     // Gets original Project Data Model key

    // Spawners
    void                    addSpawner(DrEngineSpawner *spawner) { m_spawners.push_back(spawner); }
    EngineSpawners&         getSpawners() { return m_spawners; }
    void                    removeSpawner(DrEngineSpawner *spawner) {
        for (auto it = m_spawners.begin(); it != m_spawners.end(); ) {
            if ((*it) == spawner) it = m_spawners.erase(it); else ++it;
        }
    }

    // Built In Thing Components
    ThingComponents&        componentMap()      { return m_components; }
    ThingComp3D*            comp3D()            { return m_comp_3d; }
    ThingCompCamera*        compCamera()        { return m_comp_camera; }
    ThingCompFoliage*       compFoliage()       { return m_comp_foliage; }
    ThingCompPhysics*       compPhysics()       { return m_comp_physics; }
    ThingCompPlayer*        compPlayer()        { return m_comp_player; }
    ThingCompSoftBody*      compSoftBody()      { return m_comp_soft_body; }
    void                    setComponent3D(ThingComp3D *component);
    void                    setComponentCamera(ThingCompCamera *component);
    void                    setComponentFoliage(ThingCompFoliage *component);
    void                    setComponentPhysics(ThingCompPhysics *component);
    void                    setComponentPlayer(ThingCompPlayer *component);
    void                    setComponentSoftBody(ThingCompSoftBody *component);

    // Misc Internal Functions
    void                    calculateTimeSinceLastUpdate();

    // Basic Properties (Used to be virtual before Components)
    double                  getAngle();
    void                    setAngle(double new_angle);

    // Basic Properties
    const float&            getOpacity() const      { return m_opacity; }                           // Returns Opacity (alpha 0.0 to 1.0) of Thing
    DrPointF                getPosition() const     { return m_position; }                          // Returns Thing center position in world coordinates
    const float&            getScaleX() const       { return m_scale_x; }
    const float&            getScaleY() const       { return m_scale_y; }
    DrPointF                getSize()  const        { return m_size; }                              // Returns original Thing size
    const double&           getVelocityX() const    { return m_velocity_x; }
    const double&           getVelocityY() const    { return m_velocity_y; }
    const double&           getZOrder() const       { return m_z_order; }

    void                    setOpacity(float new_opacity)   { m_opacity = new_opacity; }
    void                    setPosition(DrPointF position)  { m_position = position; }
    void                    setScaleX(float new_scale_x)    { m_scale_x = new_scale_x; }
    void                    setScaleX(double new_scale_x)   { m_scale_x = static_cast<float>(new_scale_x); }
    void                    setScaleY(float new_scale_y)    { m_scale_y = new_scale_y; }
    void                    setScaleY(double new_scale_y)   { m_scale_y = static_cast<float>(new_scale_y); }
    void                    setSize(DrPointF size)          { m_size = size; }
    void                    setVelocityX(double x_vel)      { m_velocity_x = x_vel; }
    void                    setVelocityY(double y_vel)      { m_velocity_y = y_vel; }
    void                    setZOrder(double z_order)       { m_z_order = z_order; }

};




#endif // ENGINE_THING_H



















