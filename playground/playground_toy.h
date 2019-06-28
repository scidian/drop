//
//      Created by Stephens Nunnally on 6/15/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef PLAYGROUND_TOY_H
#define PLAYGROUND_TOY_H

#include <QGraphicsItem>
#include <map>

#include "chipmunk/chipmunk.h"
#include "enums_engine.h"

// Forward Declarations
class DrPlayground;

// Local Constants
constexpr double    c_friction =  -1;                   // Flag for add**** call to use world friction setting
constexpr double    c_bounce =    -1;                   // Flag for add**** call to use world bounce setting

//####################################################################################
//##    DrToy
//##        Holds one object for use in a Playground cpSpace
//############################
class DrToy
{

public:
    // External Borrowed Pointers
    DrPlayground       *m_playground;           // Parent DrPlayground


    // Object Body and Shape
    cpBody             *body;                   // Physical Body of object
    Body_Type           body_type;              // Body_Type

    cpShape            *shape;                  // Collision Shape of object
    Shape_Type          shape_type;             // Shape_Type

    QGraphicsItem      *graphic;                // Pointer to the QGraphicsItem that represents this toy on the Playground QGraphicsScene


    // ***** Object Component Properties
    //      To be set for each object as desired
    //
    // Object Basic Settings
    bool            m_does_collide = true;          // Set to false to have this object not collide with anything

    // Object Properties - Bounce / Friction
    double          m_custom_friction = c_friction; // Defaults to c_friction (-1) if this item uses global m_friction, otherwise stores custom friction
    double          m_custom_bounce = c_bounce;     // Defaults to c_bounce (-1) if this item uses global m_bounce, otherwise stores custom bounce

    // Object Movement - Rotation
    bool            m_can_rotate = true;            // To be set during object creation, moment of inertia is set to Infinity to stop rotation

    // ********** Local Variables Updated by Engine
    //                NOT TO BE SET BY USER
    //
    bool        m_remove = false;                           // Set to true to mark for removal

    double      m_angle = 0.0;                              // Current object->body angle, updated every frame by updateWorld()
    QPointF     m_position;                                 // Current object->body posiiton, updated every frame by updateWorld()
    double      m_width;                                    // Stores width of Box toys
    double      m_height;                                   // Stores height of Box toys


public:
    DrToy(DrPlayground *playground) : m_playground(playground) {}


};


#endif // PLAYGROUND_TOY_H















