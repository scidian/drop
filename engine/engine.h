//
//      Created by Stephens Nunnally on 5/6/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef ENGINE_H
#define ENGINE_H

#include "constants_engine.h"
#include "globals_engine.h"


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


public:
    // Constructor / Destrcutor / Cleanup
    DrEngine(FormEngine *form_engine, DrProject *project);
    void    clearWorlds();
    void    deleteTextures();

    // Textures
    DrEngineTexture*    addTexture(long texture_id, QPixmap &pixmap);
    DrEngineTexture*    getTexture(long texture_id) { return m_textures[texture_id]; }
    EngineTextureMap&   getTextureMap() { return m_textures; }


    // Getter and Setters    
    FormEngine*         getFormEngine()             { return m_form_engine; }
    DrProject*          getProject()                { return m_project; }

    long                getCurrentEditorWorld()     { return m_current_editor_world; }
    DrEngineWorld*      getCurrentWorld()           { return m_worlds[m_current_world]; }
    DrEngineWorld*      getWorld(long world_key)    { return m_worlds[world_key]; }

    std::string         pedalAsString();

};


#endif // ENGINE_H











