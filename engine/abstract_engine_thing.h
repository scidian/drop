//
//      Created by Stephens Nunnally on 6/19/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef ENGINE_THING_H
#define ENGINE_THING_H

#include <QVector>
#include <QVector3D>
#include <map>

#include "chipmunk/chipmunk.h"
#include "enums_engine.h"
#include "helper.h"

// Type Definitions
typedef std::map<cpShape*, Shape_Type> ShapeMap;


class DrEngineThing
{
public:
    // ***** Object Body and Shapes
    cpBody             *body;                   // Physical Body of object
    Body_Type           body_type;              // Body_Type

    QVector<cpShape*>   shapes;                 // Collision Shapes of object
    ShapeMap            shape_type;             // Shape Types of Shapes of Object

    double              z_order = 0.0;          // Used for layering, needs to be public for sorting

public:
    DrEngineThing();
    virtual ~DrEngineThing();

    // Virtual Functions
    virtual bool    isLight() { return false; }

    // Getters / Setters
    const double&   getZOrder() { return z_order; }
    void            setZOrder(double new_z_order) { z_order = new_z_order; }

};


#endif // ENGINE_THING_H





