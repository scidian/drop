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

#include "engine/globals_engine.h"

// Local Type Definitions
typedef std::map<std::string, EngineSignals>    SignalMapByName;
typedef std::map<long,        EngineSignals>    SignalMapByThing;
typedef std::map<long,        SignalMapByName>  SignalMapByThingName;

//####################################################################################
//##    DrEngine
//##        Holds a project ready for use in an OpenGLWidget
//############################
class DrEngine
{
public:
    // Constructor / Destrcutor / Cleanup
    DrEngine(DrProject *project, long stage_key);
    ~DrEngine();


    // #################### VARIABLES ####################
private:
    // External Borrowed Pointers
    DrOpenGL               *m_opengl                        { nullptr };            // Pointer to QOpenGLWidget instance
    DrProject              *m_project                       { nullptr };            // Pointer to Project to load into Engine

    // Signals
    EngineSignals           m_signals;                                              // List of all Signals
    EngineSignals               m_signal_list;                                      // List of active Signals
    SignalMapByName             m_signal_map_by_name;                               // Map  of active Signals, grouped into lists by Signal Name
    SignalMapByThing            m_signal_map_by_thing;                              // Map  of active Signals, grouped into lists by Thing Key
    SignalMapByThingName        m_signal_map_by_thing_name;                         // Map  of active Signals, grouped into lists by Thing Key, then grouped by Signal Name

    // Textures
    EngineTextureMap        m_textures;                                             // Map of Textures used for this Engine

    // Worlds
    EngineWorldMap          m_worlds;                                               // Map of Physics Worlds loaded into the Engine
    long                        m_current_world             { c_no_key };           // Project Key of current world shown in Engine
    long                        m_current_editor_world      { c_no_key };           // Project Key of current world shown in EDITOR
    long                        m_stage_key                 { c_no_key };           // If "Play Stage" was clicked in EDITOR, this will hold the Project Key of Stage


public:
    cpBody                 *mouse_body                      { nullptr };            // A Body to use for mouse tracking / grabbing
    cpConstraint           *mouse_joint                     { nullptr };            // A Joint to use to attach to mouse


    // #################### FUNCTIONS ####################
public:
    // Cleanup
    void                    clearWorlds();
    void                    deleteTextures();

    // Textures
    DrEngineTexture*        addTexture(long texture_id, QPixmap &pixmap);
    DrEngineTexture*        getTexture(long texture_id) { return m_textures[texture_id]; }
    EngineTextureMap&       getTextureMap() { return m_textures; }

    // Signals
    void                    clearSignals();
    void                    pushSignal(std::string name, DrVariant value, DrEngineThing *thing_a = nullptr, DrEngineThing *thing_b = nullptr);
    EngineSignals           signalList(std::string name = "", long thing_key = c_no_key);
    void                    updateSignalList();

    // Getter and Setters    
    DrOpenGL*               getOpenGL()                 { return m_opengl; }
    void                    setOpenGl(DrOpenGL *opengl) { m_opengl = opengl; }
    DrProject*              getProject()                { return m_project; }

    long                    getCurrentEditorWorld()     { return m_current_editor_world; }
    long                    getCurrentEngineWorld()     { return m_current_world; }
    DrEngineWorld*          getCurrentWorld()           { return m_worlds[m_current_world]; }
    DrEngineWorld*          getWorld(long world_key)    { return m_worlds[world_key]; }

    long                    getStageKey()               { return m_stage_key; }


};


#endif // ENGINE_H











