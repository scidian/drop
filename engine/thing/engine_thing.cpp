//
//      Created by Stephens Nunnally on 6/19/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Abstract Item representing a thing that has a body in the Physics Engine (object, character, light, etc)
//
//
#include "engine/engine.h"
#include "engine/engine_signal.h"
#include "engine/opengl/opengl.h"
#include "engine/thing/components/thing_comp_3d.h"
#include "engine/thing/components/thing_comp_camera.h"
#include "engine/thing/components/thing_comp_foliage.h"
#include "engine/thing/components/thing_comp_physics.h"
#include "engine/thing/components/thing_comp_player.h"
#include "engine/thing/components/thing_comp_soft_body.h"
#include "engine/thing/engine_thing.h"
#include "engine/thing/engine_thing_component.h"
#include "engine/world/engine_world.h"
#include "project/entities/dr_asset.h"
#include "project/dr_project.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
DrEngineThing::DrEngineThing(DrEngineWorld* world, long unique_key, long original_key) {
    m_world = world;
    m_key = unique_key;
    m_original_project_key = original_key;
}

DrEngineThing::DrEngineThing(DrEngineWorld *world, long unique_key, long original_key, double x, double y, double z,
                             DrPointF scale, double angle, float opacity, DrPointF size)
    : DrEngineThing(world, unique_key, original_key) {

    this->setAngle(angle);
    this->setOpacity(opacity);
    this->setPosition(DrPointF(x, y));
    this->setScaleX(scale.x);                                                   // Save x scale for later
    this->setScaleY(scale.y);                                                   // Save y scale for later
    this->setSize(size);
    this->setZOrder(z);    
}

DrEngineThing::~DrEngineThing() {
    for (auto component_pair : m_components) {
        delete component_pair.second;
    }
}


//####################################################################################
//####################################################################################
//##
//##    EXPOSED API FUNCTIONS
//##
//####################################################################################
//####################################################################################

//####################################################################################
//##    Basic Virtual Component Functions
//####################################################################################
// Called when Thing is added to world
void DrEngineThing::addToWorld() {
    Dr::ResetTimer( update_timer );
    for (auto component : m_components) {
        component.second->addToWorld();
    }
}

// Called when it is time to Render Thing
void DrEngineThing::draw() {
    for (auto component : m_components) {
        component.second->draw();
    }
}

// Update Function, Called every physics frame
bool DrEngineThing::update(double time_passed, double time_warp, DrRectF &area) {
    bool remove = false;

    // ***** Call update() for each Component
    for (auto component : m_components) {
        remove = (remove || component.second->update(time_passed, time_warp));
    }

    // ***** Delete object if ends up outside the deletion threshold
    if (area.contains(getPosition()) == false) remove = true;
    return (remove || m_remove_me);
}


//####################################################################################
//##    Info Functions
//####################################################################################
// Returns name of this Entity (Thing)
std::string DrEngineThing::name() {
    if (m_world == nullptr) return "";
    DrSettings *entity = m_world->getProject()->findSettingsFromKey(m_original_project_key);
    if (entity == nullptr)   return "";
    return (entity->getName());
}

DrPointF DrEngineThing::mapPositionToScreen() {
    return world()->getEngine()->getOpenGL()->mapToScreen( getPosition().x, getPosition().y, getZOrder() );
}

//####################################################################################
//##    Components Functions
//####################################################################################
// Returns component with matching component_name
DrThingComponent* DrEngineThing::component(std::string component_name) {
    if (component_name == "") return nullptr;
    auto it = m_components.find(component_name);
    if (it == m_components.end()) { return nullptr; }
    return (*it).second;
}

// Removes component with component_name
void DrEngineThing::removeComponent(std::string component_name) {
    if (component_name == "") return;
    for (auto it = m_components.begin(); it != m_components.end(); ) {
        if ((*it).first == component_name) {
            if (component_name == Comps::Thing_3D)                  m_comp_3d =         nullptr;
            if (component_name == Comps::Thing_Settings_Camera)     m_comp_camera =     nullptr;
            if (component_name == Comps::Thing_Player)              m_comp_player =     nullptr;
            if (component_name == Comps::Thing_Soft_Body)           m_comp_soft_body=   nullptr;
            delete (*it).second;
            it = m_components.erase(it);
            continue;
        }
        it++;
    }
}

// Sets Component to 'component', if nullptr is passed in, removes component
void DrEngineThing::setComponent(std::string component_name, DrThingComponent *component) {
    if (component_name == "") return;
    if (component == nullptr) {
        removeComponent(component_name);
    } else {
        // Delete possible existing component
        if (m_components[component_name] != nullptr) {
            delete m_components[component_name];
        }
        // Set to new component
        m_components[component_name] = component;
    }
}



//####################################################################################
//####################################################################################
//##
//##    INTERNAL FUNCTIONS
//##
//####################################################################################
//####################################################################################

//####################################################################################
//##    Components Functions
//####################################################################################
void DrEngineThing::setComponent3D(ThingComp3D *component)              { m_comp_3d = component;        setComponent(component->name(), component); }
void DrEngineThing::setComponentCamera(ThingCompCamera *component)      { m_comp_camera = component;    setComponent(component->name(), component); }
void DrEngineThing::setComponentPhysics(ThingCompPhysics *component)    { m_comp_physics = component;   setComponent(component->name(), component); }
void DrEngineThing::setComponentPlayer(ThingCompPlayer *component)      { m_comp_player = component;    setComponent(component->name(), component); }
void DrEngineThing::setComponentSoftBody(ThingCompSoftBody *component)  { m_comp_soft_body = component; setComponent(component->name(), component); }
void DrEngineThing::setComponentFoliage(ThingCompFoliage *component)    { m_comp_foliage = component;   setComponent(component->name(), component); }


//####################################################################################
//##    Signals
//####################################################################################
// Adds signal to stack
void DrEngineThing::emitSignal(std::string name, DrVariant value, long thing_b) {
    m_world->getEngine()->pushSignal(name, value, this->getKey(), thing_b);
}


//####################################################################################
//##    Get time since last update
//####################################################################################
void DrEngineThing::calculateTimeSinceLastUpdate() {
    time_since_last_update = Dr::MillisecondsElapsed( update_timer );
    Dr::ResetTimer( update_timer );
}


//####################################################################################
//##    Old Virtual Getters / Setters
//####################################################################################
// Returns Thing angle (in degrees)
double DrEngineThing::getAngle() {
    if (compPhysics() != nullptr) {
        if (compPhysics()->body == nullptr) return 0.0;
        return Dr::RadiansToDegrees( cpBodyGetAngle(compPhysics()->body) );
    } else {
        return m_angle_z;
    }
}

void DrEngineThing::setAngle(double new_angle) {
    if (compPhysics() != nullptr) {
        if (compPhysics()->body == nullptr) return;

        // Get current angle, set new angle
        double current_angle = Dr::RadiansToDegrees( cpBodyGetAngle(compPhysics()->body) );
        cpBodySetAngle( compPhysics()->body, Dr::DegreesToRadians(new_angle) );

        // Set angle of all soft body physics children
        if (this->compSoftBody() != nullptr && compPhysics()->isPhysicsChild() == false) {
            for (size_t i = 0; i < this->compSoftBody()->soft_balls.size(); ++i) {
                DrEngineThing *next_ball = world()->findPhysicsObjectByKey(this->compSoftBody()->soft_balls[i]);
                if (next_ball == this) continue;
                if (next_ball == nullptr) continue;
                if (next_ball->compPhysics()->body == nullptr) continue;
                DrPointF new_position = Dr::RotatePointAroundOrigin(next_ball->getPosition(), this->getPosition(), new_angle - current_angle);
                cpBodySetPosition(next_ball->compPhysics()->body, cpv(new_position.x, new_position.y));
                next_ball->setAngle(new_angle);
            }
        }
    }

    m_angle_z = new_angle;
}














