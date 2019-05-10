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
    Jump,
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
    Jumped
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


enum Test_Textures {
    Ball =  -1,
    Block = -2,
    Plant = -3,
    Rover = -4,
    Wheel = -5,
    Spare = -6
};

struct SceneObject {
    // ***** Object Properties
    cpBody     *body;                       // Physical Body of object
    cpShape    *shape;                      // Collision Shape of object

    Body_Type   body_type;                  // Body_Type
    Shape_Type  shape_type;                 // Shape_Type

    bool        in_scene = true;            // True while object exists in Space
    bool        collide = true;             // Set to false to have this object not collide with anything

    long        texture_number;             // Reference to which texture to use from Engine.EngineTexture map
    double      radius;                     // Used for Shape_Type::Circle collision shapes
    double      width;                      // Used for Shape_Type::Box collision shapes
    double      height;                     // Used for Shape_Type::Box collision shapes


    // ***** Object interaction
    bool        follow = false;             // Set to true to have camera follow object
    bool        player_controls = false;    // Set to true to have object controlled by player control buttons, keyboard_x, keyboard_y
    bool        is_wheel = false;           // Set to true if we want wheel to spin from button press
    double      wheel_speed;                // If is_wheel, Speed at which wheel should spin when gas pedal is pressed

    // ***** Updated by Engine:
    double      angle = 0;                  // Current object angle
    QPointF     position;                   // Current object posiiton
    QPointF     velocity;                   // Current object velocity
};

// Forward declarations
class DrEngineTexture;
class DrEngineWorld;
class DrProject;

// Type definitions
typedef std::map<long, DrEngineTexture*> EngineTextureMap;
typedef std::map<long, DrEngineWorld*>   EngineWorldMap;


//####################################################################################
//##    DrEngine
//##        Holds a project ready for use in an OpenGLWidget
//############################
class DrEngine
{
private:
    DrProject          *m_project;                              // Pointer to Project to load into Engine
    EngineTextureMap    m_textures;                             // Map of textures used for this Engine
    EngineWorldMap      m_worlds;                               // Map of Worlds used for this Engine


    cpSpace        *m_space;                                    // Current physics space shown on screen
    QVector3D       m_camera_pos = QVector3D(0, 0, 800);        // Current camera position

    const cpFloat   m_time_step = 1 / 90.0;     // Speed at which want to try to update the Space, 1 / 90 = 90 times per second to up
                                                //      It is *highly* recommended to use a fixed size time step (calling Update at a fixed interval)
    cpFloat         m_time_warp = 1.0;          // Speeds up or slows down physics time, 1 = 100% = Normal Time, Lower than 1 = slower, Higher = faster

    cpVect          m_gravity;                  // Current global gravity applied to current space. Defaults to cpvzero. Can be overridden on a per body basis
                                                //      by writing custom integration functions. Changing the gravity will activate all sleeping bodies in the space.
    cpFloat         m_damping;                  // A value of 0.9 means that each body will lose 10% of its velocity per second. Defaults to 1.
                                                //      Like gravity, it can be overridden on a per body basis.


public:
    QVector<SceneObject*>   objects;

    QTime           fps_timer;
    int             fps, last_fps;
    bool            debug = false;

    Demo            demo = Demo::Spawn;
    Render_Type     render_type = Render_Type::Orthographic;          // Should render perspective or orthographic?

    bool            has_scene = false;

    Pedal           gas_pedal = Pedal::None;

    int             keyboard_x;                     // Set to -1 for left, 1 for right
    bool            keyboard_y;                     // Set to -1 for down, 1 for up

    cpBody         *player_body;
    cpShape        *player_shape;
    int             jump_count;
    int             remaining_jumps;
    cpFloat         remaining_boost;
    bool            grounded;
    Jump_State      jump_state;


public:
    DrEngine(DrProject *project);

    //  Friction: 0 = frictionless, unknown limit
    //  Bounce:   0 = no bounce, 1.0 will give a “perfect” bounce. However due to inaccuracies in the simulation using 1.0 or greater is not recommended
    //  Mass:     Not sure
    SceneObject*    addLine(  Body_Type body_type,  QPointF p1, QPointF p2, double friction, double bounce, double mass);
    SceneObject*    addCircle(Body_Type body_type,  long texture_number, double x, double y, double friction, double bounce, double mass, QPointF velocity,
                              bool can_rotate = true);
    SceneObject*    addBlock( Body_Type body_type,  long texture_number, double x, double y, double angle, QPointF scale,
                              double friction, double bounce, double mass, QPointF velocity, bool should_collide = true);
    SceneObject*    addPolygon(Body_Type body_type, long texture_number, double x, double y, QVector<QPointF> points, double friction, double bounce, double mass, QPointF velocity);

    void        buildSpace();
    void        clearSpace();
    void        loadSpace();
    void        playerUpdateVelocity(cpBody *body, cpVect gravity, cpFloat dt);
    void        updateSpace(double time_passed);

    void        deleteResources();


    // Textures
    DrEngineTexture*    addTexture(long texture_id, QString from_asset_string);
    DrEngineTexture*    addTexture(long texture_id, QPixmap &pixmap);
    DrEngineTexture*    getTexture(long texture_id) { return m_textures[texture_id]; }
    EngineTextureMap&   getTextureMap() { return m_textures; }


    // Getter and Setters    
    DrProject*          getProject()    { return m_project; }
    cpSpace*            getSpace()      { return m_space; }

    const bool&         debugOn() { return debug; }

    const QVector3D&    getCameraPos()  { return m_camera_pos; }
    const cpFloat&      getTimeStep()   { return m_time_step; }
    const cpFloat&      getTimeWarp()   { return m_time_warp; }
    const cpVect&       getGravity()    { return m_gravity; }
    const cpFloat&      getDamping()    { return m_damping; }
    void                setCameraPos(QVector3D new_pos) { m_camera_pos = new_pos; }
    void                setCameraPos(float x, float y, float z) { m_camera_pos = QVector3D(x, y, z); }
    void                setTimeWarp(double new_time_warp) { m_time_warp = new_time_warp; }
    void                setGravity(cpVect new_gravity) { m_gravity = new_gravity; }
    void                setDamping(cpFloat new_damping) { m_damping = new_damping; }


};



#endif // ENGINE_H











