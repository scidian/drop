//
//      Created by Stephens Nunnally on 6/19/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Abstract base class that represents one DrThing in the physics engine
//
//
#ifndef ENGINE_THING_H
#define ENGINE_THING_H

#include <QVector>
#include <QVector3D>
#include <map>

#include "chipmunk/chipmunk.h"
#include "enums.h"
#include "enums_engine.h"
#include "helper.h"

// Forward Declarations
class DrEngineWorld;

// Type Definitions
typedef std::map<cpShape*, Shape_Type> ShapeMap;


//####################################################################################
//##    DrEngineThing
//##        Abstract base class that represents one DrThing in the physics engine
//############################
class DrEngineThing
{
public:
    // Layout Variable
    double          z_order = 0.0;                      // Used for layering, needs to be public for use with std::sort

private:
    // External Borrowed Pointers
    DrEngineWorld  *m_world = nullptr;                  // Points to current parent DrEngineWorld

    // Engine Info
    long            m_key;                              // Unique key for this item

    // Basic Thing Properties
    double          m_angle_z = 0.0;                    // Current angle (on Z axis), (for DrEngineObject this is updated every frame by update())
    float           m_opacity = 1.0f;                   // Transparency of Thing (0.0 invisible, 1.0 opaque)
    QPointF         m_position;                         // Current center posiiton
    float           m_scale_x = 1.0f;                   // Scale of Thing in world
    float           m_scale_y = 1.0f;                   // Scale of Thing in world
    QPointF         m_size;                             // Original size of Thing

    // Thing Properties - 3D
    Convert_3D_Type m_3d_type = Convert_3D_Type::Extrusion;
    double          m_angle_x =   0.0;                  // X axis rotation speed
    double          m_angle_y =   0.0;                  // Y axis rotation speed
    bool            m_billboard = false;                // Keeps object facing camera
    double          m_depth = 0.0;                      // Desired 3D Depth of 2D Objects

    // Thing Properties - Camera
    long            m_active_camera = 0;                // Set to ID of last camera that followed this object, 0 == no camera


public:
    // ***** Image Post Processing Attributes
    float       bitrate = 256.0;                        // Bitrate          1 to 256
    float       pixel_x = 1.0;                          // Pixelation X     1.0+
    float       pixel_y = 1.0;                          // Pixelation Y     1.0+
    bool        negative = false;                       // Negative         True / False
    bool        grayscale = false;                      // Grayscale        True / False
    float       hue = 0.0f;                             // Hue              Editor:    0 to 360     Shader:  0.0 to 1.0
    float       saturation = 0.0f;                      // Saturation       Editor: -255 to 255     Shader: -1.0 to 1.0
    float       contrast = 0.0f;                        // Contrast         Editor: -255 to 255     Shader: -1.0 to 1.0
    float       brightness = 0.0f;                      // Brightness       Editor: -255 to 255     Shader: -1.0 to 1.0


    // ********** Local Variables Updated by Engine
    //                NOT TO BE SET BY USER
    double      time_since_last_update = 0.0;           // Milliseconds since update() was called last
    DrTime      update_timer = Clock::now();            // Used to keep track of time passed since update() was called last



public:
    DrEngineThing(DrEngineWorld *world, long unique_key);
    virtual ~DrEngineThing();

    // Abstract Virtual Functions
    virtual void        addToWorld() = 0;                                                   // Called when Thing is added to m_things DrEngineWorld vector
    virtual DrThingType getThingType() = 0;                                                 // Returns DrThingType of this Thing
    virtual bool        update(double time_passed, double time_warp, QRectF &area) = 0;     // Process one update iteration for this Thing

    // Functions
    void                calculateTimeSinceLastUpdate();                                     // Processes update timer
    long                getKey() { return m_key; }                                          // Gets unique item key


    // Thing Properties - Camera
    const long&             getActiveCameraKey() { return m_active_camera; }
    bool                    hasActiveCamera() { return (m_active_camera == 0) ? false : true; }
    void                    setActiveCameraKey(const long& new_camera_key) { m_active_camera = new_camera_key; }

    // Basic Properties
    virtual double          getAngle() {    return m_angle_z; }                             // Returns Thing angle (in degrees)
    virtual const float&    getOpacity() {  return m_opacity; }                             // Returns Opacity (alpha 0.0 to 1.0) of Thing
    virtual QPointF         getPosition() { return m_position; }                            // Returns Thing center position in world coordinates
    virtual const float&    getScaleX() {   return m_scale_x; }
    virtual const float&    getScaleY() {   return m_scale_y; }
    virtual QPointF         getSize() {     return m_size; }                                // Returns original Thing size
    DrEngineWorld*          getWorld() {    return m_world; }

    virtual void            setAngle(double new_angle) {        m_angle_z = new_angle; }
    virtual void            setOpacity(float new_opacity) {     m_opacity = new_opacity; }
    virtual void            setPosition(QPointF position) {     m_position = position; }
    void                    setScaleX(float new_scale_x)  {     m_scale_x = new_scale_x; }
    void                    setScaleX(double new_scale_x) {     m_scale_x = static_cast<float>(new_scale_x); }
    void                    setScaleY(float new_scale_y)  {     m_scale_y = new_scale_y; }
    void                    setScaleY(double new_scale_y) {     m_scale_y = static_cast<float>(new_scale_y); }
    virtual void            setSize(QPointF size) {             m_size = size; }
    void                    setWorld(DrEngineWorld *world) {    m_world = world; }

    // 3D Properties
    Convert_3D_Type         get3DType() {       return m_3d_type; }
    const double&           getAngleX() {       return m_angle_x; }                         // Returns Thing X Axis Rotation (in degrees)
    const double&           getAngleY() {       return m_angle_y; }                         // Returns Thing Y Axis Rotation (in degrees)
    const bool&             getBillboard() {    return m_billboard; }
    const double&           getDepth() {        return m_depth; }                           // Returns Thing 3D Extrusion Depth

    void                    set3DType(Convert_3D_Type type) {   m_3d_type = type; }
    void                    setAngleX(double new_angle_x) {     m_angle_x = new_angle_x; }
    void                    setAngleY(double new_angle_y) {     m_angle_y = new_angle_y; }
    void                    setBillboard(bool billboard) {      m_billboard = billboard; }
    void                    setDepth(double new_depth) {        m_depth = new_depth; }

};




#endif // ENGINE_THING_H



















