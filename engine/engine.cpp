//
//      Created by Stephens Nunnally on 5/6/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "engine/engine.h"
#include "engine/engine_camera.h"
#include "engine/engine_signal.h"
#include "engine/engine_texture.h"
#include "engine/thing/engine_thing.h"
#include "engine/world/engine_world.h"
#include "project/dr_project.h"
#include "project/entities/dr_asset.h"
#include "project/entities/dr_thing.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
DrEngine::DrEngine(DrProject *project, long stage_key) : m_project(project) {
    m_stage_key = stage_key;

    m_current_editor_world = m_project->getOption(Project_Options::Current_World).toLong();
    m_current_world = m_current_editor_world;

    m_worlds[m_current_world] = new DrEngineWorld(this, project, this->getTextureMap(), m_current_world);
}

DrEngine::~DrEngine() {
    clearSignals();
}

// Needs to be explicitly called by OpenGLWidget
void DrEngine::clearWorlds() {
    clearSignals();
    for (auto &world_pair : m_worlds) { delete world_pair.second; }
}

// Needs to be explicitly called by OpenGLWidget
void DrEngine::deleteTextures() {
    for (auto &texture_pair : m_textures) {
        if (texture_pair.second->isTextureLoaded()) {
            texture_pair.second->deleteTexture();
        }
        delete texture_pair.second;
    }
}


//####################################################################################
//##    DrEngineTexture
//####################################################################################
DrEngineTexture* DrEngine::addTexture(long texture_id, QPixmap &pixmap) {
    m_textures[texture_id] = new DrEngineTexture(pixmap);
    return m_textures[texture_id];
}


//####################################################################################
//##    Signal Processing
//####################################################################################
void DrEngine::clearSignals() {
    for (auto it = m_signals.begin(); it != m_signals.end(); ) {
        delete (*it);
        it = m_signals.erase(it);
    }
}

// Returns list of signals by name, by key, or by both.
//      Passing value of "" for name returns all signals with any name
//      Passing default optional value of c_no_key for thing_key includes all signals with name
EngineSignals DrEngine::signalList(std::string name, long thing_key) {
    if (thing_key == c_no_key) {
        if (name == "")
            return m_signal_list;
        else
            return m_signal_map_by_name[name];
    } else {
        if (name == "") {
            return m_signal_map_by_thing[thing_key];
        } else {
            return m_signal_map_by_thing_name[thing_key][name];
        }
    }
}

// Add signal to stack
void DrEngine::pushSignal(std::string name, DrVariant value, DrEngineThing *thing_a, DrEngineThing *thing_b) {
    m_signals.push_back(new DrEngineSignal(name, value, thing_a, thing_b));
}

// Sets new signals to active, kills old signals
void DrEngine::updateSignalList() {
    m_signal_list.clear();
    m_signal_map_by_name.clear();
    m_signal_map_by_thing.clear();
    m_signal_map_by_thing_name.clear();

    for (auto it = m_signals.begin(); it != m_signals.end(); ) {
        if ((*it)->getLife() == Signal_Life::Active) {
            delete (*it);
            it = m_signals.erase(it);
            continue;
        } else if ((*it)->getLife() == Signal_Life::Born) {
            (*it)->setLife(Signal_Life::Active);

            // Push to full list of active things
            m_signal_list.push_back(*it);

            // Push to map sorted by Signal Names
            m_signal_map_by_name[(*it)->name()].push_back(*it);

            // If signal is associated with a Thing, push onto list for each Thing
            if ((*it)->thingA() != nullptr) {
                m_signal_map_by_thing[(*it)->thingA()->getKey()].push_back(*it);
                m_signal_map_by_thing_name[(*it)->thingA()->getKey()][(*it)->name()].push_back(*it);
            }
        }
        it++;
    }
}





























