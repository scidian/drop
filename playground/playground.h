//
//      Created by   on 6/15/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Physics Playground
//
//
#ifndef PLAYGROUND_H
#define PLAYGROUND_H

#include <QColor>
#include <QVector>
#include <QVector3D>
#include <map>

#include "chipmunk/chipmunk.h"
#include "enums_engine.h"

// Forward Declarations
class DrToy;
class FormPlayground;

// Type Definitions
typedef QVector<DrToy*> Toys;

//####################################################################################
//##    DrPlayground
//##        Holds one Physics Playground simulation
//############################
class DrPlayground
{
private:
    // External Borrowed Pointers
    FormPlayground *m_form_playground;              // Parent form

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

public:
    // Local Variables
    Toys            objects;                        // Holds all objects shown in cpSpace
    bool            has_scene = false;              // True after a scene has been loaded into cpSpace

    cpBody         *mouse_body;                     // A body to use for mouse tracking / grabbing

public:
    // Constructor / Destrcutor / Cleanup
    DrPlayground(FormPlayground *form_playground);
    ~DrPlayground();

    // Space Construction / Handling
    DrToy*      addLine(  Body_Type body_type, QColor color, QPointF p1, QPointF p2, double friction, double bounce, double mass);
    DrToy*      addCircle(Body_Type body_type, QColor color, double x, double y, double angle, double shape_radius,
                          double friction, double bounce, bool should_collide, bool can_rotate);
    DrToy*      addBlock( Body_Type body_type, QColor color, double x, double y, double angle, double width, double height,
                          double friction, double bounce, bool should_collide, bool can_rotate);

    void        buildSpace();
    void        clearSpace();
    DrToy*      findToyAtPoint(cpVect point, double max_distance);
    void        removeObject(DrToy *toy);
    void        setWorldInfo();
    void        updateSpace(double time_passed);
    void        updateSpaceHelper();
    void        wakeAllBodies();

    // Getters / Setters
    FormPlayground*     getFormPlayground() { return m_form_playground; }
    cpSpace*            getSpace() { return m_space; }
    cpFloat             getTimeStepAsMilliseconds() { return (1000.0 * m_time_step); }

    void                setGravityX(double gravity_x);
    void                setGravityY(double gravity_y);
    void                setTimeWarp(double time_warp) { m_time_warp = time_warp; }
};


#endif // PLAYGROUND_H
















