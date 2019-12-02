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
    // External Borrowed Pointers
    DrEngineWorld  *m_world = nullptr;                          // Points to current parent DrEngineWorld

    // Local Variables
    DrThing        *m_thing_to_spawn = nullptr;                 // Reference to the DrThing pointer we wish to spawn
    DrPointF        m_original_location { 0, 0 };               // Original location of Spawner, used to take Spawner out of scene as
                                                                // gameplay proceeds and not attached to Object
    // Time Variables
    double          m_time_passed;                              // Time last spawn occured
    double          m_next_spawn;                               // Seconds until next spawn
    bool            m_delete_me = false;                        // Mark for deletion when spawner is empty

    // Key Trackers
    bool            m_last_key_jump_status = false;             // Track Jump Key
    bool            m_last_key_shoot_status = false;            // Track Shoot Key

    // Spawn Properties
    bool            m_spawn_instantly = true;                   // Spawn instantly, or wait initial m_spawn_rate before spawning?
    double          m_spawn_rate = 1000;                        // Spawn rate in seconds
    double          m_spawn_rate_variable = 0;                  // Spawn rate + or - seconds
    int             m_spawn_count = 1;                          // Total spawn count
    int             m_spawn_start_count = 1;                    // Keeps track of initial spawn count
    int             m_spawns_remaining = 1;                     // Spawns remaining, can be reset in case of Shoot or Jump button
    Spawn_Type      m_spawn_type = Spawn_Type::Permanent;       // Spawn Type (permanent, shoot button, jump button, object death, etc)
    double          m_spawn_chance = 100.0;                     // Percentage that object will appear when spawned

    bool            m_attached_to_object = false;               // Is attached to object?
    long            m_attached_id_key = c_no_key;               // Attached object ID Key
    DrEngineThing  *m_attached = nullptr;                       // Attached object

    double          m_spawn_offset_x = 0.0;                     // Spawn location offset X
    double          m_spawn_offset_y = 0.0;                     // Spawn location offset Y
    double          m_spawn_variable_x = 0.0;                   // Variable X location amount
    double          m_spawn_variable_y = 0.0;                   // Vartable Y location amount


private:
    void            setReadyForRemoval() { m_delete_me = true; }


public:
    // Constructor / Destructor
    DrEngineSpawner(DrThing *thing);
    DrEngineSpawner(DrEngineWorld *engine_world, DrThing *thing, Spawn_Type type, DrPointF location,
                    double rate, double rate_variable,
                    bool spawn_instantly, int spawn_count, int spawns_remaining, double spawn_chance,
                    DrEngineThing *attached, long attached_id,
                    double x, double y, double x_variable, double y_variable);
    ~DrEngineSpawner() { }

    // Function Calls
    bool            readyForRemoval()       { return m_delete_me; }

    // Spawning
    DrEngineObject* update(double time_passed, double time_warp, QRectF area, bool use_area = true);


    // Timer Calls
    void            addTimePassed(double milliseconds)  { m_time_passed += (milliseconds / 1000.0); }
    double          getSecondsUntilNextSpawn()          { return m_next_spawn; }
    void            resetSpawnTime()                    { m_time_passed = 0.0; }
    double          secondsSinceLastSpawn()             { return m_time_passed; }
    void            setNextSpawnTimeAmount();
    void            setSpawnerForFirstTime();


    // Getters & Setters
    DrThing*        getThingToSpawn()       { return m_thing_to_spawn; }
    DrPointF        getLocation()           { return m_original_location; }
    const bool&     getSpawnInstantly()     { return m_spawn_instantly; }
    const double&   getSpawnRate()          { return m_spawn_rate; }
    const double&   getSpawnRateVariable()  { return m_spawn_rate_variable; }
    const int&      getSpawnCount()         { return m_spawn_count; }
    const int&      getSpawnStartCount()    { return m_spawn_start_count; }
    const int&      getSpawnsRemaining()    { return m_spawns_remaining; }
    Spawn_Type      getSpawnType()          { return m_spawn_type; }
    const double&   getSpawnChance()        { return m_spawn_chance; }

    const bool&     isAttached()            { return m_attached_to_object; }
    const long&     getAttachedIDKey()      { return m_attached_id_key; }
    DrEngineThing*  getAttachedThing()      { return m_attached; }

    const double&   getSpawnOffsetX()       { return m_spawn_offset_x; }
    const double&   getSpawnOffsetY()       { return m_spawn_offset_y; }
    const double&   getSpawnVariableX()     { return m_spawn_variable_x; }
    const double&   getSpawnVariableY()     { return m_spawn_variable_y; }

    void            setThingToSpawn(DrThing *thing)         { m_thing_to_spawn = thing; }
    void            setLocation(DrPointF location)          { m_original_location = location; }
    void            setSpawnInstantly(bool instant)         { m_spawn_instantly = instant; }
    void            setSpawnRate(double spawn_rate)         { m_spawn_rate = spawn_rate; }
    void            setSpawnRateVariable(double variable)   { m_spawn_rate_variable = variable; }
    void            setSpawnCount(int count)                { m_spawn_count = count; }
    void            setSpawnStartCount(int count)           { m_spawn_start_count = count; }
    void            setSpawnsRemaining(int remaining)       { m_spawns_remaining = remaining; }
    void            setSpawnType(Spawn_Type type)           { m_spawn_type = type; }
    void            setSpawnChance(double chance)           { m_spawn_chance = chance; }

    void            setAttachedThingKey(long key)           { m_attached_id_key = key; }
    void            setAttachedThing(DrEngineThing *thing);

    void            setSpawnOffsetX(double x)               { m_spawn_offset_x = x; }
    void            setSpawnOffsetY(double y)               { m_spawn_offset_y = y; }
    void            setSpawnVariableX(double x)             { m_spawn_variable_x = x; }
    void            setSpawnVariableY(double y)             { m_spawn_variable_y = y; }

};

#endif // ENGINE_SPAWNER_H


















