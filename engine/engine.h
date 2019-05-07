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

enum class Demo {
    Spawn,
    Car,
};

enum class Pedal {
    Brake,
    None,
    Clockwise,
    CounterClockwise,
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
    Polygon                                     // cpPolyShapeNew
};

struct SceneObject {
    cpBody     *body;                       // Physical Body of object
    cpShape    *shape;                      // Collision Shape of object

    Body_Type   body_type;                  // Body_Type
    Shape_Type  shape_type;                 // Shape_Type

    long        texture_number;             // Reference to which texture to use from Engine.EngineTexture map
    double      radius;                     // Used for Shape_Type::Circle collision shapes
    double      width;                      // Used for Shape_Type::Box collisiuon shapes
    double      height;                     // Used for Shape_Type::Box collisiuon shapes

    double      wheel_speed;                // Speed at which wheel should spin when gas pedal is pressed

    bool        is_wheel = false;           // Set to true if we want wheel to spin from mouse press
    bool        in_scene = true;
    bool        follow = false;             // Set to true to have camera follow object

    double      angle;                      // Updated during timer, contains current object angle
    QPointF     position;                   // Updated during timer, contains current object posiiton
    QPointF     velocity;                   // Updated during timer, contains current object velocity

};


// Forward declarations
class DrEngineTexture;
class DrEngineWorld;

// Type definitions
typedef std::map<long, DrEngineTexture*> EngineTextureMap;
typedef std::map<long, DrEngineWorld*>   EngineWorldMap;

// Constants
constexpr int c_texture_border = 0;

//####################################################################################
//##    DrEngine
//##        Holds a project ready for use in an OpenGLWidget
//############################
class DrEngine
{
private:
    EngineTextureMap  m_textures;                               // Map of textures used for this Engine
    EngineWorldMap    m_worlds;                                 // Map of Worlds used for this Engine

    QVector3D   m_camera_pos = QVector3D(0, 0, -800);           // Current camera position

    cpSpace    *m_space;                        // Current physics space shown on screen

    cpFloat     m_time_step = 1 / 60.0;         // Current time step, it is *highly* recommended to use a fixed size time step.
    cpVect      m_gravity;                      // Current global gravity applied to current space. Defaults to cpvzero. Can be overridden on a per body basis
                                                //      by writing custom integration functions. Changing the gravity will activate all sleeping bodies in the space.
    cpFloat     m_damping;                      // A value of 0.9 means that each body will lose 10% of its velocity per second. Defaults to 1.
                                                //      Like gravity, it can be overridden on a per body basis.




public:

    QVector<SceneObject*>   objects;

    Demo            demo = Demo::Spawn;
    QTime           fps_timer;
    int             fps, last_fps;
    bool            debug = false;

    Render_Type     render_type = Render_Type::Orthographic;          // Should render perspective or orthographic?

    bool            has_scene = false;
    Pedal           gas_pedal = Pedal::None;


public:
    DrEngine();

    //  Friction: 0 = frictionless, unknown limit
    //  Bounce:   0 = no bounce, 1.0 will give a “perfect” bounce. However due to inaccuracies in the simulation using 1.0 or greater is not recommended
    //  Mass:     Not sure
    SceneObject*    addLine(Body_Type body_type, QPointF p1, QPointF p2, double friction, double bounce, double mass);
    SceneObject*    addCircle(Body_Type body_type,  long texture_number, double x, double y, double friction, double bounce, double mass, QPointF velocity);
    SceneObject*    addBlock( Body_Type body_type,  long texture_number, double x, double y, double friction, double bounce, double mass, QPointF velocity);
    SceneObject*    addPolygon(Body_Type body_type, long texture_number, double x, double y, QVector<QPointF> points, double friction, double bounce, double mass, QPointF velocity);

    void        buildSpace();
    void        clearSpace();
    void        updateSpace();

    void        deleteResources();


    // Textures
    long                addTexture(QString from_asset_string);
    DrEngineTexture*    getTexture(long number) { return m_textures[number]; }
    EngineTextureMap&   getTextureMap() { return m_textures; }


    // Getter and Setters    
    cpSpace*            getSpace()      { return m_space; }

    const bool&         debugOn() { return debug; }
    const QVector3D&    getCameraPos()  { return m_camera_pos; }
    const cpFloat&      getTimeStep()   { return m_time_step; }
    const cpVect&       getGravity()    { return m_gravity; }
    const cpFloat&      getDamping()    { return m_damping; }
    void                setCameraPos(QVector3D new_pos) { m_camera_pos = new_pos; }
    void                setCameraPos(float x, float y, float z) { m_camera_pos = QVector3D(x, y, z); }
    void                setTimeStep(cpFloat new_time_step) { m_time_step = new_time_step; }
    void                setGravity(cpVect new_gravity) { m_gravity = new_gravity; }
    void                setDamping(cpFloat new_damping) { m_damping = new_damping; }

};



#endif // ENGINE_H











