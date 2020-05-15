//
//      Created by Stephens Nunnally on 11/6/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      01-13-20: Now Qt Free!
//
//
#ifndef GLOBALS_ENGINE_H
#define GLOBALS_ENGINE_H

#include <string>

#include "3rd_party/chipmunk/chipmunk.h"
#include "core/dr_containers.h"
#include "core/dr_math.h"
#include "core/dr_time.h"
#include "core/types/dr_variant.h"
#include "engine/constants_engine.h"
#include "engine/enums_engine.h"
#include "project/constants_comps_and_props.h"
#include "project/constants_entity_keys.h"
#include "project/constants_messages.h"
#include "project/enums_entity_types.h"

// Forward Declarations
class DrAsset;
class DrEngine;
class DrEngineCamera;
class DrEngineMessage;
class DrEngineSpawner;
class DrEngineThing;
class DrEngineTexture;
class DrEngineWorld;
class DrOpenGL;
class DrProject;
class DrStage;
class DrThing;
class DrThingComponent;
class FormEngine;
class ThingCompPhysics;

// Type Definitions
typedef std::chrono::high_resolution_clock          Clock;

typedef std::vector<   DrEngineCamera*>             EngineCameraList;
typedef std::map<long, DrEngineCamera*>             EngineCameraMap;
typedef std::list<     DrEngineMessage*>            EngineMessages;
typedef std::vector<   DrEngineSpawner*>            EngineSpawners;
typedef std::map<long, DrEngineTexture*>            EngineTextureMap;
typedef std::vector<   DrEngineThing*>              EngineThings;
typedef std::map<long, DrEngineWorld*>              EngineWorldMap;

typedef std::map<std::string, DrThingComponent*>    ThingComponents;


// Input / Engine Globals - defined in engine_world__velocity_player.cpp
extern double        g_keyboard_x;                      // Used to pass keyboard x button state to static callback functions
extern double        g_keyboard_y;                      // Used to pass keyboard y button state to static callback functions
extern bool          g_jump_button;                     // Used to pass jump  button state to static callback functions
extern bool          g_shoot_button;                    // Used to pass shoot button state to static callback functions
extern Pedal         g_pedal;                           // Used to pass Pedal button state

extern cpVect        g_gravity_normal;                  // Stores a gravity as a normalized vector for use in static callback functions
extern DrPointF      g_player_position;                 // Stores active player location for Kinematic objects trying to rotate toward player

extern DrPointF      g_mouse_position;                  // Stores last known mouse position

extern std::string   g_info;                            // Used to draw some debug text onto the Engine screen

extern bool          g_bool;                            // Global bool   to use for debugging
extern double        g_double;                          // Global double to use for debugging
extern int           g_int;                             // Global int    to use for debugging
extern long          g_long;                            // Global long   to use for debugging
extern size_t        g_size_t;                          // Global size_t to use for debugging



#endif // GLOBALS_ENGINE_H















