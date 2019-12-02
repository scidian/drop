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
#include "project/project_thing.h"


//####################################################################################
//##    Constructor
//####################################################################################
DrEngineSpawner::DrEngineSpawner(DrThing *thing) : m_thing_to_spawn(thing) { }

DrEngineSpawner::DrEngineSpawner(DrEngineWorld *engine_world, DrThing *thing, Spawn_Type type, DrPointF location, double rate, double rate_variable,
                                 bool spawn_instantly, int spawn_count, int spawns_remaining, double spawn_chance,
                                 DrEngineThing *attached, long attached_id,
                                 double x, double y, double x_variable, double y_variable) : m_world(engine_world), m_thing_to_spawn(thing) {
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
    setSpawnChance( spawn_chance );

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

    DrEngineObject *return_object = nullptr;

    // ***** Can't find Thing to Spawn, mark for removal
    if (readyForRemoval()) return nullptr;
    if (getThingToSpawn() == nullptr) { setReadyForRemoval(); return nullptr; }
    if (getAttachedIDKey() != c_no_key && isAttached() == false) return nullptr;

    // ***** Add milliseconds since last time we were here (time is passed by engine so its possible to pause in game)
    addTimePassed(time_passed);

    // ***** Init spawn adjustments
    double x_pos = getLocation().x,     x_scale = 1.0,      x_velocity = 0.0;
    double y_pos = getLocation().y,     y_scale = 1.0,      y_velocity = 0.0;
    double angle = 0.0;
    double rotate_spawn = 0.0;

    // ***** Check for attachment and alter spawn adjustments
    if (isAttached()) {
        DrEngineThing *thing_attached_to = getAttachedThing();
        if (thing_attached_to == nullptr) {
            setReadyForRemoval();
            return nullptr;
        }
        x_pos = thing_attached_to->getPosition().x;
        y_pos = thing_attached_to->getPosition().y;
        x_scale = static_cast<double>(thing_attached_to->getScaleX());
        y_scale = static_cast<double>(thing_attached_to->getScaleY());
        angle = thing_attached_to->getAngle();
        rotate_spawn = angle;

        // Find velocity of follow object
        if (thing_attached_to->getThingType() == DrThingType::Object || thing_attached_to->getThingType() == DrThingType::Character) {
            DrEngineObject *object = dynamic_cast<DrEngineObject*>(thing_attached_to);
            if (object != nullptr) {
                // If character with forced velocity, find forced velocity
                double forced_speed_x = object->getForcedSpeedX();
                double forced_speed_y = object->getForcedSpeedY();
                if (object->getAngleMovement()) {
                    QTransform t = QTransform().rotate(object->getAngle());
                    QPointF forced_angle = t.map( QPointF(forced_speed_x, forced_speed_y) );
                        forced_speed_x = forced_angle.x();
                        forced_speed_y = forced_angle.y();
                }

                // Use Forced Speed for Dynamic objects, otherwise use Current Velocity of Kinematic objects
                if (object->body_type == Body_Type::Dynamic) {
                    x_velocity = forced_speed_x;
                    y_velocity = forced_speed_y;
                } else if (object->body_type == Body_Type::Kinematic) {
                    x_velocity = thing_attached_to->getVelocityX();
                    y_velocity = thing_attached_to->getVelocityY();
                }
            }
        }
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
    if (getSpawnType() == Spawn_Type::Permanent) {
        spawn = true;
    }


    // ********** Perform Spawn
    if (spawn) {
        if (secondsSinceLastSpawn() >= getSecondsUntilNextSpawn() && getSpawnCount() != 0) {
            DrThing *thing_to_spawn = dynamic_cast<DrThing*>(getThingToSpawn());                    // Try to use dynamic_cast to see if object has been deleted
                                                                                                    // Works on macOS, but possibly undefined on other systems
            if (thing_to_spawn == nullptr) { setReadyForRemoval(); return nullptr; }

            double spawn_chance = QRandomGenerator::global()->bounded(100.0);                       // Get a random number between 0 and 100
            if (spawn_chance <= this->getSpawnChance()) {
                return_object = m_world->loadObjectToWorld( thing_to_spawn,
                                                            x_pos, y_pos, x_scale, y_scale, angle,
                                                            x_velocity, y_velocity, rotate_spawn );
            }
            resetSpawnTime();
            setNextSpawnTimeAmount();
            if (getSpawnCount() > 0) setSpawnCount(getSpawnCount() - 1);
        }
    }


    // ***** Delete spawner if ends up outside the deletion threshold
    if (getSpawnType() == Spawn_Type::Permanent) {
        if (getSpawnCount() == 0) setReadyForRemoval();

        if (use_area) {
            if (area.contains(QPointF(getLocation().x, getLocation().y)) == false) {
                if (getSpawnType() == Spawn_Type::Permanent)
                    setReadyForRemoval();
            }
        }
    }
    return return_object;
}


















