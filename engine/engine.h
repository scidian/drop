//
//      Created by Stephens Nunnally on 5/6/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef ENGINE_H
#define ENGINE_H

#include <QPixmap>
#include <QString>
#include <map>

#include "chipmunk/chipmunk.h"
#include "enums_engine.h"

// Forward declarations
class DrEngineLight;
class DrEngineObject;
class DrEngineTexture;
class DrEngineWorld;
class DrProject;
class DrStage;
class FormEngine;

// Type definitions
typedef std::map<long, DrEngineTexture*> EngineTextureMap;
typedef std::map<long, DrEngineWorld*>   EngineWorldMap;

//####################################################################################
//##    DrEngine
//##        Holds a project ready for use in an OpenGLWidget
//############################
class DrEngine
{
private:
    // External Borrowed Pointers
    FormEngine         *m_form_engine;              // Pointer to Parent FormEngine
    DrProject          *m_project;                  // Pointer to Project to load into Engine

    // Local Variables
    EngineTextureMap    m_textures;                 // Map of Textures used for this Engine
    EngineWorldMap      m_worlds;                   // Map of Physics Worlds loaded into the Engine
    long                m_current_world;            // Project Key of current world shown in Engine
    long                m_current_editor_world;     // Project Key of current world shown in EDITOR



// ***** Public Variables not yet implemented into function calls / getters / setters
public:
    // Input Keys
    int             keyboard_x;                             // Set to -1 for left, 1 for right
    int             keyboard_y;                             // Set to -1 for down, 1 for up
    bool            jump_button = false;                    // Set to False for Not-Pressed, True for Pressed
    Pedal           gas_pedal = Pedal::None;                // Pedal enumeration for rotate key state

    // Debug Variables
    bool            debug_shapes =      false;
    bool            debug_collisions =  false;

    // Demo Variables
    Demo_Space      demo_space =  Demo_Space::Project;
    Demo_Player     demo_player = Demo_Player::Jump;
    DrEngineObject *demo_jumper_1;
    DrEngineObject *demo_jumper_2;


public:
    // Constructor / Destrcutor / Cleanup
    DrEngine(FormEngine *form_engine, DrProject *project);
    void    clearWorlds();
    void    deleteTextures();

    // Textures
    DrEngineTexture*    addTexture(long texture_id, QString from_asset_string);
    DrEngineTexture*    addTexture(long texture_id, QPixmap &pixmap);
    DrEngineTexture*    getTexture(long texture_id) { return m_textures[texture_id]; }
    EngineTextureMap&   getTextureMap() { return m_textures; }


    // Getter and Setters    
    FormEngine*         getFormEngine()             { return m_form_engine; }
    DrProject*          getProject()                { return m_project; }

    DrEngineWorld*      getCurrentWorld()           { return m_worlds[m_current_world]; }
    DrEngineWorld*      getWorld(long world_key)    { return m_worlds[world_key]; }

    long                getCurrentEditorWorld()     { return m_current_editor_world; }

    QString             pedalAsString();

};



#endif // ENGINE_H











