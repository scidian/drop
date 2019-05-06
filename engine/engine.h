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
    QOpenGLTexture  *texture;

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


constexpr int c_texture_border = 0;


class EngineImage;
class EngineWorld;

class Engine
{
private:
    std::map<long, EngineImage*>    images;
    std::map<long, EngineWorld*>    worlds;

    cpSpace    *space;                          // Current physics space shown on screen

    cpFloat     time_step = 1 / 60.0;           // Current time step, it is *highly* recommended to use a fixed size time step.
    cpVect      gravity;                        // Current global gravity applied to current space. Defaults to cpvzero. Can be overridden on a per body basis
                                                //      by writing custom integration functions. Changing the gravity will activate all sleeping bodies in the space.
    cpFloat     damping;                        // A value of 0.9 means that each body will lose 10% of its velocity per second. Defaults to 1.
                                                //      Like gravity, it can be overridden on a per body basis.


public:

    QVector<SceneObject*>   objects;

    // Textures
    QOpenGLTexture     *t_ball;
    QOpenGLTexture     *t_metal_block;
    QOpenGLTexture     *t_moon_plant;
    QOpenGLTexture     *t_rover_body;
    QOpenGLTexture     *t_rover_wheel;
    QOpenGLTexture     *t_spare_wheel;


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
    SceneObject*    addCircle(BodyType body_type, QOpenGLTexture *txt, double x, double y, double friction, double bounce, double mass, QPointF velocity);
    SceneObject*    addBlock(BodyType body_type, QOpenGLTexture *txt, double x, double y, double friction, double bounce, double mass, QPointF velocity);
    SceneObject*    addPolygon(BodyType body_type, QOpenGLTexture *txt, double x, double y, QVector<QPointF> points, double friction, double bounce, double mass, QPointF velocity);

    void        buildSpace();
    void        clearSpace();
    void        updateSpace();


    // Getter and Setters
    bool        debugOn() { return debug; }

    cpSpace*    getSpace()      { return space; }
    cpFloat     getTimeStep()   { return time_step; }
    cpVect      getGravity()    { return gravity; }
    cpFloat     getDamping()    { return damping; }

    void        setTimeStep(cpFloat new_time_step) { time_step = new_time_step; }
    void        setGravity(cpVect new_gravity) { gravity = new_gravity; }
    void        setDamping(cpFloat new_damping) { damping = new_damping; }

};



#endif // ENGINE_H











