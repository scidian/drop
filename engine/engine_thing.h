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
    // ***** Layout Variable
    double          z_order = 0.0;                      // Used for layering, needs to be public for use with std::sort

private:
    // ***** Engine Info
    long            m_key;                              // Unique key for this item

    // ***** Basic Thing Settings
    QPointF         m_position;                         // Current center posiiton
    float           m_alpha = 1.0f;                     // Transparency of object (0.0 invisible, 1.0 opaque)

    // ***** Thing Properties - Camera
    long            m_active_camera = 0;                // Set to ID of last camera that followed this object, 0 == no camera


public:
    // ********** Local Variables Updated by Engine
    //                NOT TO BE SET BY USER
    bool        should_process = true;                  // True while object exists in Space
    bool        has_been_processed = false;             // Set to true after an initial update call has been ran once while the object was in the Space
    double      time_since_last_update = 0.0;           // Milliseconds since update() was called last
    DrTime      update_timer = Clock::now();            // Used to keep track of time passed since update() was called last


public:
    DrEngineThing(long unique_key);
    virtual ~DrEngineThing();

    // Abstract Virtual Functions
    virtual DrThingType getThingType() = 0;                                                 // Returns DrThingType of this Thing
    virtual bool        update(double time_passed, double time_warp, QRectF &area) = 0;     // Process one update iteration for this Thing

    // Functions
    void                calculateTimeSinceLastUpdate();                                     // Processes update timer
    long                getKey() { return m_key; }                                          // Gets unique item key

    // Getters / Setters
    virtual QPointF         getPosition() { return m_position; }                            // Returns Thing center position in world coordinates
    virtual const float&    getOpacity() { return m_alpha; }                                // Returns Opacity (alpha 0.0 to 1.0) of Thing

    virtual void            setPosition(QPointF position) { m_position = position; }
    virtual void            setOpacity(float  new_alpha) { m_alpha = new_alpha; }


    // Object Properties - Camera
    const long&     getActiveCameraKey() { return m_active_camera; }
    bool            hasActiveCamera() { return (m_active_camera == 0) ? false : true; }
    void            setActiveCameraKey(const long& new_camera_key) { m_active_camera = new_camera_key; }

};




#endif // ENGINE_THING_H



















