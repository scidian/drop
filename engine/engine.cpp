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
    for (auto world : m_worlds) {
        delete world.second;
    }
}

// Needs to be explicitly called by OpenGLWidget
void DrEngine::deleteTextures() {
    for (auto texture : m_textures) {
        if (texture.second->isTextureLoaded()) {
            texture.second->deleteTexture();
        }
        delete texture.second;
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

// Returns first found instance of a signal if it exists, otherwise returns 0
EngineSignals DrEngine::signalList(std::string name) {
    std::list<DrEngineSignal*> signal_list;
    for (auto &signal : m_signals) {
        if (signal->name() == name && signal->getLife() == Signal_Life::Active) {
            signal_list.push_back(signal);
        }
    }
    return signal_list;
}

// Add signal to stack
void DrEngine::pushSignal(std::string name, DrVariant value, long thing_key) {
    std::string thing_name          { "" };
    long        thing_asset_key     { c_no_key };
    long        thing_engine_key    { thing_key };
    if (thing_key != c_no_key) {
        if (getCurrentWorld() != nullptr) {
            DrEngineThing *engine_thing = getCurrentWorld()->findThingByKey(thing_key);
            if (engine_thing != nullptr) {
                thing_asset_key = engine_thing->getOriginalKey();
                DrSettings *settings = m_project->findSettingsFromKey(thing_asset_key, false);
                if (settings != nullptr) {
                    if (settings->getType() == DrType::Thing) {
                        DrThing *thing = dynamic_cast<DrThing*>(settings);
                        if (thing != nullptr) thing_asset_key = thing->getAssetKey();
                    }
                    thing_name = settings->getName();
                }
            }
        }
    }
    m_signals.push_back(new DrEngineSignal(name, value, thing_name, thing_asset_key, thing_engine_key));
}

// Sets new signals to active, kills old signals
void DrEngine::updateSignalList() {
    for (auto it = m_signals.begin(); it != m_signals.end(); ) {
        if ((*it)->getLife() == Signal_Life::Active) {
            delete (*it);
            it = m_signals.erase(it);
            continue;
        } else if ((*it)->getLife() == Signal_Life::Born) {
            (*it)->setLife(Signal_Life::Active);
        }
        it++;
    }
}





























