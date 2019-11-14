//
//      Created by Stephens Nunnally on 11/13/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef ENGINE_SPAWNER_H
#define ENGINE_SPAWNER_H

#include "constants_engine.h"
#include "globals_engine.h"


//####################################################################################
//##    DrEngineSpawner
//##        Holds a Spawner for use in a DrEngineWorld
//############################
class DrEngineSpawner
{
private:
    // Local Variables
    DrThing        *m_thing_to_spawn = nullptr;                 // Reference to the object we wish to spawn

    DrPointF        m_original_location { 0, 0 };               // Original location of Spawner, used to take Spawner out of scene as
                                                                // gameplay proceeds and not attached to Object

    int             m_spawn_rate = 1000;                        // Spawn rate in milliseconds
    int             m_spawn_rate_variable = 0;                  // Spawn rate + or - milliseconds

    int             m_spawn_count = 1;                          // Total spawn count
    int             m_spawns_remaining = 1;                     // Spawns remaining, can be reset in case of Shoot or Jump button

    Spawn_Type      m_spawn_type = Spawn_Type::Permanent;       // Spawn Type (permanent, shoot button, jump button, object death, etc)

    bool            m_attached_to_object = false;               // Is attached to object?
    long            m_attached_id_key = c_no_key;               // Attached object ID Key
    DrEngineThing  *m_attached = nullptr;                       // Attached object

    double          m_spawn_offset_x = 0.0;                     // Spawn location offset X
    double          m_spawn_offset_y = 0.0;                     // Spawn location offset Y
    double          m_spawn_variable_x = 0.0;                   // Variable X location amount
    double          m_spawn_variable_y = 0.0;                   // Vartable Y location amount


public:
    // Constructor / Destructor
    DrEngineSpawner();
    DrEngineSpawner(DrThing *thing, Spawn_Type type, DrPointF location, int rate, int rate_variable,
                    int spawn_count, int spawns_remaining,
                    DrEngineThing *attached, long attached_id,
                    double x, double y, double x_variable, double y_variable);
    ~DrEngineSpawner() { }

    // Function Calls


    // Getters & Setters
    DrThing*        getThingToSpawn()       { return m_thing_to_spawn; }
    DrPointF        getLocation()           { return m_original_location; }
    const int&      getSpawnRate()          { return m_spawn_rate; }
    const int&      getSpawnRateVariable()  { return m_spawn_rate_variable; }
    const int&      getSpawnCount()         { return m_spawn_count; }
    const int&      getSpawnsRemaining()    { return m_spawns_remaining; }
    Spawn_Type      getSpawnType()          { return m_spawn_type; }

    const bool&     isAttached()            { return m_attached_to_object; }
    const long&     getAttachedIDKey()      { return m_attached_id_key; }
    DrEngineThing*  getAttachedThing()      { return m_attached; }

    const double&   getSpawnOffsetX()       { return m_spawn_offset_x; }
    const double&   getSpawnOffsetY()       { return m_spawn_offset_y; }
    const double&   getSpawnVariableX()     { return m_spawn_variable_x; }
    const double&   getSpawnVariableY()     { return m_spawn_variable_y; }

    void            setThingToSpawn(DrThing *thing)     { m_thing_to_spawn = thing; }
    void            setLocation(DrPointF location)      { m_original_location = location; }
    void            setSpawnRate(int spawn_rate)        { m_spawn_rate = spawn_rate; }
    void            setSpawnRateVariable(int variable)  { m_spawn_rate_variable = variable; }
    void            setSpawnCount(int count)            { m_spawn_count = count; }
    void            setSpawnsRemaining(int remaining)   { m_spawns_remaining = remaining; }
    void            setSpawnType(Spawn_Type type)       { m_spawn_type = type; }

    void            setAttachedThingKey(long key)       { m_attached_id_key = key; }
    void            setAttachedThing(DrEngineThing *thing);

    void            setSpawnOffsetX(double x)           { m_spawn_offset_x = x; }
    void            setSpawnOffsetY(double y)           { m_spawn_offset_y = y; }
    void            setSpawnVariableX(double x)         { m_spawn_variable_x = x; }
    void            setSpawnVariableY(double y)         { m_spawn_variable_y = y; }

};

#endif // ENGINE_SPAWNER_H


















