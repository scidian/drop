//
//      Created by Stephens Nunnally on 11/6/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef GLOBALS_ENGINE_H
#define GLOBALS_ENGINE_H

#include <QColor>
#include <QOpenGLTexture>
#include <QPixmap>
#include <QString>
#include <QVector>
#include <QVector2D>
#include <QVector3D>

#include <map>
#include <string>
#include <vector>

#include "3rd_party/chipmunk/chipmunk.h"
#include "3rd_party/vec3.h"
#include "engine/constants_engine.h"
#include "engine/enums_engine.h"
#include "model/constants_keys.h"
#include "model/enums_model_types.h"
#include "model/types/pointf.h"


// Forward declarations
class DrAsset;
class DrEngine;
class DrEngineLight;
class DrEngineCamera;
class DrEngineObject;
class DrEngineSpawner;
class DrEngineThing;
class DrEngineTexture;
class DrEngineWorld;
class DrOpenGL;
class DrProject;
class DrStage;
class DrThing;
class FormEngine;


// Type definitions
typedef std::chrono::high_resolution_clock  Clock;
typedef std::map<long, DrEngineCamera*>     EngineCameraMap;
typedef std::vector<   DrEngineSpawner*>    EngineSpawners;
typedef std::map<long, DrEngineTexture*>    EngineTextureMap;
typedef std::vector<   DrEngineThing*>      EngineThings;
typedef std::map<long, DrEngineWorld*>      EngineWorldMap;


// Input / Engine Globals - defined in engine_update_player.cpp
extern double        g_keyboard_x;                      // Used to pass keyboard x button state to static callback functions
extern double        g_keyboard_y;                      // Used to pass keyboard y button state to static callback functions
extern bool          g_jump_button;                     // Used to pass jump  button state to static callback functions
extern bool          g_shoot_button;                    // Used to pass shoot button state to static callback functions
extern Pedal         g_pedal;                           // Used to pass Pedal button state

extern cpVect        g_gravity_normal;                  // Stores a gravity as a normalized vector for use in static callback functions
extern DrPointF      g_player_position;                 // Stores active player location for Kinematic objects trying to rotate toward player

extern DrPointF      g_mouse_position;                  // Stores last known mouse position

extern QString       g_info;                            // Used to draw some debug text onto the Engine screen


#endif // GLOBALS_ENGINE_H















