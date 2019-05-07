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
#include <map>

#include "chipmunk/chipmunk.h"

enum class RenderType {
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

enum class BodyType {
    Dynamic,
    Kinematic,
    Static,
};

enum class ShapeType {
    Circle,                                     // cpCircleShapeNew
    Box,                                        // cpBoxShapeNew
    Segment,    // "Line"                       // cpSegmentShapeNew
    Polygon                                     // cpPolyShapeNew
};

struct SceneObject {
    long        texture_number;

    cpBody     *body;
    cpShape    *shape;

    BodyType    body_type;
    ShapeType   shape_type;

    double      wheel_speed;

    bool        is_wheel = false;
    bool        in_scene = true;

    bool        follow = false;

    double      angle;
    QPointF     position;
    QPointF     velocity;

};


// Forward declarations
class EngineTexture;
class EngineWorld;

// Type definitions
typedef std::map<long, EngineTexture*> EngineTextureMap;
typedef std::map<long, EngineWorld*>   EngineWorldMap;

// Constants
constexpr int c_texture_border = 0;

//####################################################################################
//##    Engine
//##        Holds a project ready for use in an OpenGLWidget
//############################
class Engine
{
private:
    EngineTextureMap  m_textures;
    EngineWorldMap    m_worlds;

    cpSpace    *m_space;                        // Current physics space shown on screen

    cpFloat     m_time_step = 1 / 60.0;         // Current time step, it is *highly* recommended to use a fixed size time step.
    cpVect      m_gravity;                      // Current global gravity applied to current space. Defaults to cpvzero. Can be overridden on a per body basis
                                                //      by writing custom integration functions. Changing the gravity will activate all sleeping bodies in the space.
    cpFloat     m_damping;                      // A value of 0.9 means that each body will lose 10% of its velocity per second. Defaults to 1.
                                                //      Like gravity, it can be overridden on a per body basis.


public:

    QVector<SceneObject*>   objects;

    Demo        demo = Demo::Spawn;
    QTime       fps_timer;
    int         fps, last_fps;
    bool        debug = false;

    RenderType  render_type = RenderType::Orthographic;          // Should render perspective or orthographic?
    QPointF     camera_pos = QPointF(0, 0);                      // Tells renderer where to center view

    bool        has_scene = false;
    Pedal       gas_pedal = Pedal::None;


public:
    Engine();

    //  Friction: 0 = frictionless, unknown limit
    //  Bounce:   0 = no bounce, 1.0 will give a “perfect” bounce. However due to inaccuracies in the simulation using 1.0 or greater is not recommended
    //  Mass:     Not sure
    SceneObject*    addLine(BodyType body_type, QPointF p1, QPointF p2, double friction, double bounce, double mass);
    SceneObject*    addCircle(BodyType body_type,  long texture_number, double x, double y, double friction, double bounce, double mass, QPointF velocity);
    SceneObject*    addBlock( BodyType body_type,  long texture_number, double x, double y, double friction, double bounce, double mass, QPointF velocity);
    SceneObject*    addPolygon(BodyType body_type, long texture_number, double x, double y, QVector<QPointF> points, double friction, double bounce, double mass, QPointF velocity);

    void        buildSpace();
    void        clearSpace();
    void        updateSpace();

    void        deleteResources();


    // Textures
    long                addTexture(QString from_asset_string);
    EngineTexture*      getTexture(long number) { return m_textures[number]; }
    EngineTextureMap&   getTextureMap() { return m_textures; }


    // Getter and Setters    
    bool        debugOn() { return debug; }

    cpSpace*    getSpace()      { return m_space; }
    cpFloat     getTimeStep()   { return m_time_step; }
    cpVect      getGravity()    { return m_gravity; }
    cpFloat     getDamping()    { return m_damping; }

    void        setTimeStep(cpFloat new_time_step) { m_time_step = new_time_step; }
    void        setGravity(cpVect new_gravity) { m_gravity = new_gravity; }
    void        setDamping(cpFloat new_damping) { m_damping = new_damping; }

};



#endif // ENGINE_H











