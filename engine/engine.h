//
//      Created by Stephens Nunnally on 5/6/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef ENGINE_H
#define ENGINE_H

#include "engine/globals_engine.h"


//####################################################################################
//##    DrEngine
//##        Holds a project ready for use in an OpenGLWidget
//############################
class DrEngine
{
private:
    // External Borrowed Pointers
    DrOpenGL           *m_opengl;                       // Pointer to QOpenGLWidget instance
    DrProject          *m_project;                      // Pointer to Project to load into Engine

    // Local Variables
    EngineTextureMap    m_textures;                     // Map of Textures used for this Engine
    EngineWorldMap      m_worlds;                       // Map of Physics Worlds loaded into the Engine
    long                m_current_world;                // Project Key of current world shown in Engine
    long                m_current_editor_world;         // Project Key of current world shown in EDITOR

    // Stage Variables
    long                m_stage_key = c_no_key;         // If "Play Stage" was clicked in editor, this will hold the Key of Stage


public:
    cpBody             *mouse_body = nullptr;           // A Body to use for mouse tracking / grabbing
    cpConstraint       *mouse_joint = nullptr;          // A Joint to use to attach to mouse

public:
    // Constructor / Destrcutor / Cleanup
    DrEngine(DrProject *project, long stage_key);
    void    clearWorlds();
    void    deleteTextures();

    // Textures
    DrEngineTexture*    addTexture(long texture_id, QPixmap &pixmap);
    DrEngineTexture*    getTexture(long texture_id) { return m_textures[texture_id]; }
    EngineTextureMap&   getTextureMap() { return m_textures; }


    // Getter and Setters    
    DrOpenGL*           getOpenGL()                 { return m_opengl; }
    void                setOpenGl(DrOpenGL *opengl) { m_opengl = opengl; }
    DrProject*          getProject()                { return m_project; }

    long                getCurrentEditorWorld()     { return m_current_editor_world; }
    DrEngineWorld*      getCurrentWorld()           { return m_worlds[m_current_world]; }
    DrEngineWorld*      getWorld(long world_key)    { return m_worlds[world_key]; }

    long                getStageKey()               { return m_stage_key; }

    std::string         pedalAsString();

};


#endif // ENGINE_H











