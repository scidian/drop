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

#include "3rd_party_chipmunk/chipmunk.h"
#include "3rd_party/vec3.h"
#include "enums.h"
#include "enums_engine.h"
#include "types/pointf.h"


// Forward declarations
class DrAsset;
class DrEngine;
class DrEngineLight;
class DrEngineCamera;
class DrEngineObject;
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
typedef std::map<long, DrEngineTexture*>    EngineTextureMap;
typedef std::map<long, DrEngineWorld*>      EngineWorldMap;


// Input / Engine Globals - defined in engine_update_player.cpp
extern int          g_keyboard_x;                       // Used to pass keyboard x button state to static callback functions
extern int          g_keyboard_y;                       // Used to pass keyboard y button state to static callback functions
extern bool         g_jump_button;                      // Used to pass jump button state to static callback functions
extern Pedal        g_pedal;                            // Used to pass Pedal button state
extern cpVect       g_gravity_normal;                   // Stores a gravity as a normalized vector for use in static callback functions
extern QString      g_info;                             // Used to draw some debug text onto the Engine screen



#endif // GLOBALS_ENGINE_H






















