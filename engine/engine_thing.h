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

// Type Definitions
typedef std::map<cpShape*, Shape_Type> ShapeMap;

//####################################################################################
//##    DrEngineThing
//##        Abstract base class that represents one DrThing in the physics engine
//############################
class DrEngineThing
{
public:
    // ***** Object Body and Shapes
    cpBody             *body = nullptr;         // Physical Body of object
    Body_Type           body_type;              // Body_Type

    QVector<cpShape*>   shapes;                 // Collision Shapes of object
    ShapeMap            shape_type;             // Shape Types of Shapes of Object

    double              z_order = 0.0;          // Used for layering, needs to be public for use with std::sort


    // ********** Local Variables Updated by Engine
    //                NOT TO BE SET BY USER
    bool        should_process = true;                  // True while object exists in Space
    bool        has_been_processed = false;             // Set to true after an initial update call has been ran once while the object was in the Space
    double      time_since_last_update = 0.0;           // Milliseconds since update() was called last
    DrTime      update_timer = Clock::now();            // Used to keep track of time passed since update() was called last


public:
    DrEngineThing();
    virtual ~DrEngineThing();

    // Virtual Functions
    virtual DrThingType getThingType() = 0;                                                 // Returns DrThingType of this Thing
    virtual bool        update(double time_passed, double time_warp, QRectF &area) = 0;     // Process one update iteration for this Thing

    // Functions
    void                calculateTimeSinceLastUpdate();                                     // Processes update timer

};




#endif // ENGINE_THING_H



















