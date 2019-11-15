//
//      Created by Stephens Nunnally on 11/13/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QRandomGenerator>

#include "engine/engine_spawner.h"
#include "engine/engine_world.h"
#include "engine_things/engine_thing.h"


//####################################################################################
//##    Constructor
//####################################################################################
DrEngineSpawner::DrEngineSpawner() { }

DrEngineSpawner::DrEngineSpawner(DrEngineWorld *engine_world, DrThing *thing, Spawn_Type type, DrPointF location, double rate, double rate_variable,
                                 bool spawn_instantly, int spawn_count, int spawns_remaining,
                                 DrEngineThing *attached, long attached_id,
                                 double x, double y, double x_variable, double y_variable) : m_world(engine_world) {
    setLocation(location);
    if (thing == nullptr) return;
    setThingToSpawn(thing);

    setSpawnInstantly( spawn_instantly );
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

    // Set up timer for first spawn
    resetSpawnTime();
    if (spawn_instantly) {
        m_next_spawn = 0.0;
    } else {
        setNextSpawnTimeAmount();
    }
}


//####################################################################################
//##    Setters
//####################################################################################
void DrEngineSpawner::setAttachedThing(DrEngineThing *thing)  {
    m_attached = thing;
    m_attached_id_key = thing->getKey();
    m_attached_to_object = true;
}

void DrEngineSpawner::setNextSpawnTimeAmount() {
    double wait_time = getSpawnRate() + (QRandomGenerator::global()->bounded(getSpawnRateVariable() * 2.0) - getSpawnRateVariable());
    if (wait_time < 0) wait_time = 0;
    m_next_spawn = wait_time;
}

//####################################################################################
//##    Update Function, called every physics frame
//####################################################################################
DrEngineObject* DrEngineSpawner::update(double time_passed, double time_warp, QRectF area, bool use_area) {
    Q_UNUSED( time_warp )

    DrEngineObject *object = nullptr;

    // Can't find Thing to Spawn, mark for removal
    if (getThingToSpawn() == nullptr) {
        setReadyForRemoval();
        return object;
    }
    if (getAttachedIDKey() != c_no_key && isAttached() == false) return object;

    // Add milliseconds since last time we were here (time is passed by engine so its possible to pause in game)
    addTimePassed(time_passed);

    // ***** Process Permanent Spawn Type
    if (getSpawnType() == Spawn_Type::Permanent) {
        if (secondsSinceLastSpawn() >= getSecondsUntilNextSpawn() && getSpawnCount() != 0) {
            double x_pos = getLocation().x;
            double y_pos = getLocation().y;

            if (isAttached()) {
                DrEngineThing *thing = getAttachedThing();
                if (thing == nullptr) {
                    setReadyForRemoval();
                    return object;
                }
                x_pos = x_pos - thing->getPosition().x;
                y_pos = y_pos - thing->getPosition().y;
            }
            object = m_world->loadObjectToWorld( getThingToSpawn(), x_pos, y_pos );

            resetSpawnTime();
            setNextSpawnTimeAmount();

            if (getSpawnCount() > 0) setSpawnCount(getSpawnCount() - 1);
        }
        if (getSpawnCount() == 0) setReadyForRemoval();
    }


    // ***** Delete spawner if ends up outside the deletion threshold
    if (use_area && getSpawnType() == Spawn_Type::Permanent) {
        if (area.contains(QPointF(this->getLocation().x, this->getLocation().y)) == false) {
            if (getSpawnType() == Spawn_Type::Permanent)
                setReadyForRemoval();
        }
    }
    return object;
}


















