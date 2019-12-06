//
//      Created by Stephens Nunnally on 6/19/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Abstract base class that represents one DrThing in the physics engine
//
//
#ifndef ENGINE_THING_H
#define ENGINE_THING_H

#include "engine/globals_engine.h"
#include "library/helper.h"


// Type Definitions
typedef std::map<cpShape*, Shape_Type> ShapeMap;


//####################################################################################
//##    DrEngineThing
//##        Abstract base class that represents one DrThing in the physics engine
//############################
class DrEngineThing
{
private:
    // External Borrowed Pointers
    DrEngineWorld  *m_world = nullptr;                  // Points to current parent DrEngineWorld

    // Containers
    EngineSpawners  m_spawners;                         // Holds all Spawners that have been attached to this Object

    // Engine Info
    long            m_key;                              // Unique key for this item
    long            m_original_project_key;             // Key of item when it was in Project Data Model

    // Basic Thing Properties
    double          m_angle_z = 0.0;                    // Current angle (on Z axis), (for DrEngineObject this is updated every frame by update())
    float           m_opacity = 1.0f;                   // Transparency of Thing (0.0 invisible, 1.0 opaque)
    DrPointF        m_position;                         // Current center posiiton
    double          m_z_order = 0.0;                    // Used for layering, used for distance sorting
    float           m_scale_x = 1.0f;                   // Scale of Thing in world
    float           m_scale_y = 1.0f;                   // Scale of Thing in world
    DrPointF        m_size;                             // Original size of Thing
    double          m_velocity_x = 0.0;                 // Current velocity
    double          m_velocity_y = 0.0;                 // Current velocity

    // Thing Properties - 3D
    Convert_3D_Type m_3d_type = Convert_3D_Type::Extrusion;
    double          m_angle_x =   0.0;                  // X axis rotation
    double          m_angle_y =   0.0;                  // Y axis rotation
    bool            m_billboard = false;                // Keeps object facing camera
    double          m_depth = 0.0;                      // Desired 3D Depth of 2D Objects
    double          m_rotate_x_speed = 0.0;             // X axis rotation speed
    double          m_rotate_y_speed = 0.0;             // Y axis rotation speed

    // Thing Properties - Camera
    long            m_active_camera = 0;                            // Set to ID of last camera that followed this object, 0 == no camera
    QVector3D       m_camera_position = c_default_camera_pos;       // X, Y, and Z Camera Position Offset for this objects Active (Follow) Camera
    QVector3D       m_camera_rotation { 0, 0, 0 };                  // X, Y, and Z Camera Rotation        for this objects Active (Follow) Camera
    double          m_camera_zoom = 1.0;                            // Camera Magnification Level         for this objects Active (Follow) Camera
    DrPointF        m_camera_lag { 0.0, 0.0 };                      // Camera Movement Lag Amount         for this objects Active (Follow) Camera
    Up_Vector       m_camera_up = Up_Vector::Y;                     // Camera Up Vector                   usually Y, Z for first person
    bool            m_camera_match_angle = false;                   // Should Camera Match Angle of Object Following

public:
    // ***** Image Post Processing Attributes
    bool        extrude_3d = false;                     // Auto convert to 3D   True / False
    bool        wireframe = false;                      // Wireframe            True / False
    bool        cartoon = false;                        // Cartoon              True / False
    float       cartoon_width = 5.0;                    // Cartoon Width
    bool        cross_hatch = false;                    // Cross Hatch          True / False
    float       cross_hatch_width = 5.0;                // Cross Hatch Width

    float       bitrate = 256.0;                        // Bitrate              1 to 256
    float       pixel_x = 1.0;                          // Pixelation X         1.0+
    float       pixel_y = 1.0;                          // Pixelation Y         1.0+
    bool        negative = false;                       // Negative             True / False
    bool        grayscale = false;                      // Grayscale            True / False
    float       hue = 0.0f;                             // Hue                  Editor:    0 to 360     Shader:  0.0 to 1.0
    float       saturation = 0.0f;                      // Saturation           Editor: -255 to 255     Shader: -1.0 to 1.0
    float       contrast = 0.0f;                        // Contrast             Editor: -255 to 255     Shader: -1.0 to 1.0
    float       brightness = 0.0f;                      // Brightness           Editor: -255 to 255     Shader: -1.0 to 1.0


    // ********** Local Variables Updated by Engine
    //                NOT TO BE SET BY USER
    double      time_since_last_update = 0.0;           // Milliseconds since update() was called last
    DrTime      update_timer = Clock::now();            // Used to keep track of time passed since update() was called last



public:
    DrEngineThing(DrEngineWorld *world, long unique_key, long original_key);
    virtual ~DrEngineThing();


    // Getters / Setters
    long                getKey() { return m_key; }                                          // Gets unique item key
    long                getOriginalKey() { return m_original_project_key; }                 // Gets original Project Data Model key

    // Spawners
    void                addSpawner(DrEngineSpawner *spawner) { m_spawners.push_back(spawner); }
    EngineSpawners&     getSpawners() { return m_spawners; }
    void                removeSpawner(DrEngineSpawner *spawner) {
        for (auto it = m_spawners.begin(); it != m_spawners.end(); ) {
            if ((*it) == spawner) { it = m_spawners.erase(it); } else { ++it; }
        }
    }

    // Abstract Virtual Functions
    virtual DrThingType getThingType() = 0;                                                 // Returns DrThingType of this Thing

    // Basic Update Functions
    virtual void        addToWorld();                                                       // Called when Thing is added to m_things DrEngineWorld vector
    void                calculateTimeSinceLastUpdate();                                     // Processes update timer
    virtual bool        update(double time_passed, double time_warp, QRectF &area);         // Process one update iteration for this Thing

    // Virtual Event Functions
    virtual void        onCollide() { }
    virtual void        onDamaged() { }
    virtual void        onDeath()   { }

    // Basic Properties
    virtual double          getAngle()      { return m_angle_z; }                           // Returns Thing angle (in degrees)
    virtual const float&    getOpacity()    { return m_opacity; }                           // Returns Opacity (alpha 0.0 to 1.0) of Thing
    virtual DrPointF        getPosition()   { return m_position; }                          // Returns Thing center position in world coordinates
    virtual const float&    getScaleX()     { return m_scale_x; }
    virtual const float&    getScaleY()     { return m_scale_y; }
    virtual DrPointF        getSize()       { return m_size; }                              // Returns original Thing size
    virtual const double&   getVelocityX()  { return m_velocity_x; }
    virtual const double&   getVelocityY()  { return m_velocity_y; }
    DrEngineWorld*          getWorld()      { return m_world; }
    virtual const double&   getZOrder()     { return m_z_order; }

    virtual void            setAngle(double new_angle)      { m_angle_z = new_angle; }
    virtual void            setOpacity(float new_opacity)   { m_opacity = new_opacity; }
    virtual void            setPosition(DrPointF position)  { m_position = position; }
    void                    setScaleX(float new_scale_x)    { m_scale_x = new_scale_x; }
    void                    setScaleX(double new_scale_x)   { m_scale_x = static_cast<float>(new_scale_x); }
    void                    setScaleY(float new_scale_y)    { m_scale_y = new_scale_y; }
    void                    setScaleY(double new_scale_y)   { m_scale_y = static_cast<float>(new_scale_y); }
    virtual void            setSize(DrPointF size)          { m_size = size; }
    void                    setVelocityX(double x_vel)      { m_velocity_x = x_vel; }
    void                    setVelocityY(double y_vel)      { m_velocity_y = y_vel; }
    void                    setWorld(DrEngineWorld *world)  { m_world = world; }
    virtual void            setZOrder(double z_order)       { m_z_order = z_order; }

    // Thing Properties - Camera
    const long&             getActiveCameraKey()    { return m_active_camera; }
    bool                    hasActiveCamera()       { return (m_active_camera == 0) ? false : true; }
    void                    setActiveCameraKey(const long &new_camera_key) { m_active_camera = new_camera_key; }

    QVector3D&              getCameraPosition()     { return m_camera_position; }
    void                    setCameraPosition(QVector3D position) { m_camera_position = position; }
    void                    setCameraPosition(float x, float y, float z) { m_camera_position = QVector3D(x, y, z); }
    void                    setCameraPositionXY(QPointF point) { m_camera_position = QVector3D(float(point.x()), float(point.y()), m_camera_position.z()); }

    QVector3D&              getCameraRotation()     { return m_camera_rotation; }
    void                    setCameraRotation(QVector3D rotation) { m_camera_rotation = rotation; }
    void                    setCameraRotation(float x_up_down, float y_left_right, float z_rotate) {
                                                    m_camera_rotation = QVector3D(x_up_down, y_left_right, z_rotate); }

    DrPointF                getCameraLag()              { return m_camera_lag; }
    void                    setCameraLag(DrPointF lag)  { m_camera_lag.x = (lag.x <= 0) ? 0 : lag.x;
                                                          m_camera_lag.y = (lag.y <= 0) ? 0 : lag.y; }
    bool                    getCameraMatch()            { return m_camera_match_angle; }
    void                    setCameraMatch(bool match)  { m_camera_match_angle = match; }
    Up_Vector               getCameraUpVector()         { return m_camera_up; }
    void                    setCameraUpVector(Up_Vector up) { m_camera_up = up; }
    double                  getCameraZoom()             { return m_camera_zoom; }
    void                    setCameraZoom(double zoom)  { m_camera_zoom = zoom; }



    // 3D Properties
    Convert_3D_Type         get3DType()         { return m_3d_type; }
    const double&           getAngleX()         { return m_angle_x; }                       // Returns Thing X Axis Rotation (in degrees)
    const double&           getAngleY()         { return m_angle_y; }                       // Returns Thing Y Axis Rotation (in degrees)
    const bool&             getBillboard()      { return m_billboard; }
    const double&           getDepth()          { return m_depth; }                         // Returns Thing 3D Extrusion Depth
    const double&           getRotateSpeedX()   { return m_rotate_x_speed; }                // Returns Thing X Axis Rotational Speed
    const double&           getRotateSpeedY()   { return m_rotate_y_speed; }                // Returns Thing Y Axis Rotational Speed

    void                    set3DType(Convert_3D_Type type) {   m_3d_type = type; }
    void                    setAngleX(double new_angle_x) {     m_angle_x = new_angle_x; }
    void                    setAngleY(double new_angle_y) {     m_angle_y = new_angle_y; }
    void                    setBillboard(bool billboard) {      m_billboard = billboard; }
    void                    setDepth(double new_depth) {        m_depth = new_depth; }
    void                    setRotateSpeedX(double new_speed) { m_rotate_x_speed = new_speed; }
    void                    setRotateSpeedY(double new_speed) { m_rotate_y_speed = new_speed; }
};




#endif // ENGINE_THING_H



















