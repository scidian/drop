//
//      Created by Stephens Nunnally on 11/13/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "engine_spawner.h"
#include "engine_things/engine_thing.h"


//####################################################################################
//##    Constructor
//####################################################################################
DrEngineSpawner::DrEngineSpawner() { }

DrEngineSpawner::DrEngineSpawner(DrThing *thing, Spawn_Type type, DrPointF location, int rate, int rate_variable,
                                 int spawn_count, int spawns_remaining,
                                 DrEngineThing *attached, long attached_id,
                                 double x, double y, double x_variable, double y_variable) {
    setLocation(location);
    if (thing == nullptr) return;
    setThingToSpawn(thing);

    setSpawnType( type );
    setSpawnRate( rate );
    setSpawnRateVariable( rate_variable );
    setSpawnCount( spawn_count );
    setSpawnsRemaining( spawns_remaining );

    if (attached_id != c_no_key) setAttachedThingKey(attached_id);
    if (attached != nullptr)     setAttachedThing(attached);

    setSpawnOffsetX( x );
    setSpawnOffsetY( y );
    setSpawnVariableX( x_variable );
    setSpawnVariableY( y_variable );
}


//####################################################################################
//##    Setters
//####################################################################################
void DrEngineSpawner::setAttachedThing(DrEngineThing *thing)  {
    m_attached = thing;
    m_attached_id_key = thing->getKey();
    m_attached_to_object = true;
}
