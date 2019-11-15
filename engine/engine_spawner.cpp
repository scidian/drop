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
#include "engine_things/engine_thing_object.h"


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
    setSpawnStartCount( spawn_count );
    setSpawnsRemaining( spawns_remaining );

    if (attached_id != c_no_key) setAttachedThingKey(attached_id);
    if (attached != nullptr)     setAttachedThing(attached);

    setSpawnOffsetX( x );
    setSpawnOffsetY( y );
    setSpawnVariableX( x_variable );
    setSpawnVariableY( y_variable );

    // Set up timer for first spawn
    setSpawnerForFirstTime();
}


//####################################################################################
//##    Setters
//####################################################################################
void DrEngineSpawner::setAttachedThing(DrEngineThing *thing)  {
    m_attached = thing;
    if (thing != nullptr) {
        m_attached_id_key = thing->getKey();
        thing->addSpawner(this);
    }
    m_attached_to_object = true;
}

void DrEngineSpawner::setNextSpawnTimeAmount() {
    double wait_time = getSpawnRate() + (QRandomGenerator::global()->bounded(getSpawnRateVariable() * 2.0) - getSpawnRateVariable());
    if (wait_time < 0) wait_time = 0;
    m_next_spawn = wait_time;
}

void DrEngineSpawner::setSpawnerForFirstTime() {
    resetSpawnTime();
    setSpawnCount( getSpawnStartCount() );
    if (getSpawnInstantly()) {
        m_next_spawn = 0.0;
    } else {
        setNextSpawnTimeAmount();
    }
}


//####################################################################################
//##    Update Function, called every physics frame
//####################################################################################
DrEngineObject* DrEngineSpawner::update(double time_passed, double time_warp, QRectF area, bool use_area) {
    Q_UNUSED( time_warp )

    DrEngineObject *object = nullptr;

    // ***** Can't find Thing to Spawn, mark for removal
    if (readyForRemoval()) return nullptr;
    if (getThingToSpawn() == nullptr) {
        setReadyForRemoval();
        return nullptr;
    }
    if (getAttachedIDKey() != c_no_key && isAttached() == false) return nullptr;

    // ***** Add milliseconds since last time we were here (time is passed by engine so its possible to pause in game)
    addTimePassed(time_passed);

    // ***** Init spawn adjustments
    double x_pos = getLocation().x,     x_scale = 1.0,      x_velocity = 0.0;
    double y_pos = getLocation().y,     y_scale = 1.0,      y_velocity = 0.0;
    double angle = 0.0;

    // ***** Check for attachment and alter spawn adjustments
    if (isAttached()) {
        DrEngineThing  *thing = getAttachedThing();
        if (thing == nullptr) {
            setReadyForRemoval();
            return object;
        }
        x_pos = thing->getPosition().x;
        y_pos = thing->getPosition().y;
        x_scale = static_cast<double>(thing->getScaleX());
        y_scale = static_cast<double>(thing->getScaleY());
        angle = thing->getAngle();
        x_velocity = thing->getVelocityX();
        y_velocity = thing->getVelocityY();
    }

    // ##### NEED IMPLEMENT
    // Spawn_Type::Object_Death

    bool spawn = false;

    // ***** Process Jump Spawn Type
    if (getSpawnType() == Spawn_Type::Jump_Button) {
        if (m_last_key_jump_status == false && g_jump_button) setSpawnerForFirstTime();
        if (g_jump_button) spawn = true;
    }
    m_last_key_jump_status = g_jump_button;

    // ***** Process Shoot Spawn Type
    if (getSpawnType() == Spawn_Type::Shoot_Button) {
        if (m_last_key_shoot_status == false && g_shoot_button) setSpawnerForFirstTime();
        if (g_shoot_button) spawn = true;
    }
    m_last_key_shoot_status = g_shoot_button;

    // ***** Process Permanent Spawn Type
    if (getSpawnType() == Spawn_Type::Permanent) spawn = true;


    // ***** Perform Spawn
    if (spawn) {
        if (secondsSinceLastSpawn() >= getSecondsUntilNextSpawn() && getSpawnCount() != 0) {
            object = m_world->loadObjectToWorld( getThingToSpawn(), x_pos, y_pos, x_scale, y_scale, angle, x_velocity, y_velocity );
            resetSpawnTime();
            setNextSpawnTimeAmount();
            if (getSpawnCount() > 0) setSpawnCount(getSpawnCount() - 1);
        }
    }


    // ***** Delete spawner if ends up outside the deletion threshold
    if (getSpawnType() == Spawn_Type::Permanent) {
        if (getSpawnCount() == 0) setReadyForRemoval();
    }
    if (use_area && getSpawnType() == Spawn_Type::Permanent) {
        if (area.contains(QPointF(getLocation().x, getLocation().y)) == false) {
            if (getSpawnType() == Spawn_Type::Permanent)
                setReadyForRemoval();
        }
    }
    return object;
}


















