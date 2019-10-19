//
//      Created by Stephens Nunnally on 12/7/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrWorld Class Definitions
//
//
#include "debug.h"
#include "opengl/opengl.h"
#include "project.h"
#include "project_world.h"
#include "project_stage.h"
#include "project_thing.h"
#include "settings/settings.h"
#include "settings/settings_component.h"
#include "settings/settings_component_property.h"

//####################################################################################
//##    Constructor, Destructor
//####################################################################################
DrWorld::DrWorld(DrProject *parent_project, long key, QString new_world_name, bool add_start_stage) : DrSettings(parent_project) {
    this->setKey(key);

    initializeWorldSettings(new_world_name);

    // Adds the initial "Start Stage"
    if (add_start_stage) {
        m_start_stage_key = addStage();
    }

    m_last_stage_shown_in_editor = m_start_stage_key;
}

DrWorld::~DrWorld() {
    for (auto i: m_stages) { delete i.second; }
}

// Removes a Stage from the Project
void DrWorld::deleteStage(DrStage *stage) {
    m_stages.erase(stage->getKey());
    delete stage;
}


//####################################################################################
//##    Adds a Stage
//####################################################################################
// Adds a Stage to the map container, assins name based on current stage count
long DrWorld::addStage(QString new_stage_name) {
    bool need_start_stage = false;
    if (m_stages.size() < 1) {
        need_start_stage = true;
        new_stage_name = "Start Stage";
    } else if (new_stage_name == "") {
        new_stage_name = QString::number(static_cast<long>(m_stages.size() + 1));
    }

    long new_stage_key = getParentProject()->getNextKey();

    m_stages[new_stage_key] = new DrStage(getParentProject(), this, new_stage_key, new_stage_name, need_start_stage);
    return new_stage_key;
}

void DrWorld::addStage(long stage_key, bool is_start_stage, QPointF center_point) {
    m_stages[stage_key] = new DrStage(getParentProject(), this, stage_key, "TEMP", false);
    m_stages[stage_key]->setIsStartStage(is_start_stage);
    m_stages[stage_key]->setViewCenterPoint(center_point);
}


//####################################################################################
//##    Getters
//####################################################################################
// Returns the map Key of the first World in the map container
long DrWorld::getFirstStageKey() {
    return m_stages.begin()->first;
}

// Returns a pointer to the World with the associated Key from the map container
DrStage* DrWorld::getStageFromKey(long from_stage_key) {
    return m_stages[from_stage_key];
}

// Returns a pointer to the World with the mathcing name
DrStage* DrWorld::getStageWithName(QString stage_name) {
    QString compare_name;
    for (auto i: m_stages) {
        compare_name = i.second->getName();
        if (compare_name == stage_name) { return i.second; }
    }
    return nullptr;
}


//####################################################################################
//##    Property loading - initializeWorldSettings
//####################################################################################

void DrWorld::initializeWorldSettings(QString new_name) {
    DrProperty *property_name = getComponentProperty(Components::Entity_Settings, Properties::Entity_Name);
    property_name->setDisplayName("World Name");
    property_name->setDescription("Name of the current World.");
    property_name->setValue(new_name);

    addComponent(Components::World_Settings, "World Settings", "Settings for current world.", Component_Colors::White_Snow, true);
    getComponent(Components::World_Settings)->setIcon(Component_Icons::Settings);
    addPropertyToComponent(Components::World_Settings, Properties::World_Game_Direction, Property_Type::Double, 0.0,
                           "Game Direction", "Default direction, in degrees, to load new stages, 0 - right, 90 - up, 180 - left, 270 - down, etc.");
    addPropertyToComponent(Components::World_Settings, Properties::World_Score_Multiplier, Property_Type::Double, 1.0,
                           "Score Multiplier", "Value used as multiplier to adjust speed at which distance scoring is calculated.");
    addPropertyToComponent(Components::World_Settings, Properties::World_Use_Background_Color, Property_Type::Bool, false,
                           "Use Bg Color?", "Use Background Color? Check this box to use base Background Color for this world, otherwise"
                                            "background defaults to transparent.");
    addPropertyToComponent(Components::World_Settings, Properties::World_Background_Color, Property_Type::Color, QColor(16, 16, 16, 255).rgba(),
                           "Background", "This is the base Background Color for this world. Must enable to use, otherwise background defaults to "
                                         "transparent.");

    addComponent(Components::World_Physics, "Physics", "Starting physics settings for current world, this affects all Objects that have \"Physics?\" "
                                                       "enabled.", Component_Colors::Orange_Medium, true);
    getComponent(Components::World_Physics)->setIcon(Component_Icons::Physics);
    addPropertyToComponent(Components::World_Physics, Properties::World_Time_Warp, Property_Type::PositiveDouble, 1.0,
                           "Time Warp", "Physics update time multiplier, affects the speed the world changes. Default value of 1.0 is Normal speed. A value of 0.5 "
                                        "will halve the speed, a value of 2.0 will double it. Generally keep this less than 10.");
    addPropertyToComponent(Components::World_Physics, Properties::World_Gravity, Property_Type::PointF, QPointF(0.0, -1000.0),
                           "Gravity", "Amount of gravity in X and Y directions, can be negative or positive. For example, a value of -1000 for Y and Objects will "
                                      "fall down; a value of 1000 for Y and Objects will fall up.");
    addPropertyToComponent(Components::World_Physics, Properties::World_Drag, Property_Type::PositiveDouble, 0.9,
                           "Damping", "Defauly value of 0.9 means objects will lose 10% of their velocity per second. Value of 1.0 means objects will not lose "
                                      "velocity, can be greater than 1.0 (objects will gain velocity). A value of 0.0 causes objects to lose all "
                                      "velocity immediately.");
    addPropertyToComponent(Components::World_Physics, Properties::World_Friction, Property_Type::PositiveDouble, 0.5,
                           "Friction", "Global surface friction setting, greater than or equal to 0.0 (no limit, but generally less than 10.0). Friction can be "
                                       "overriden on a per object basis.");
    addPropertyToComponent(Components::World_Physics, Properties::World_Bounce, Property_Type::PositiveDouble, 0.1,
                           "Bounce", "Global bounce setting, greater than or equal to 0.0 (no limit, but generally less than or equal to 1.0). Bounce can be "
                                     "overriden on a per object basis.");

    addComponent(Components::World_Lighting, "Lighting", "Lighting settings for current World.", Component_Colors::Mustard_Yellow, true);
    getComponent(Components::World_Lighting)->setIcon(Component_Icons::Light);
    addPropertyToComponent(Components::World_Lighting, Properties::World_Light_Ambient, Property_Type::Percent, 50,
                           "Ambient Light", "Used to darken World, mostly for the purpose of lighting it with Glow Lights.");
    addPropertyToComponent(Components::World_Lighting, Properties::World_Light_Layer, Property_Type::Double, 50.0,
                           "Light Layer", "Location along the z axis (Z Order) to draw Glow Lights. Should be between " +
                                          QString::number(double(c_near_plane)) + " and " + QString::number(double(c_far_plane)) + " to be visible.");
    addPropertyToComponent(Components::World_Lighting, Properties::World_Light_Blend, Property_Type::List, 0,
                           "Blend Mode", "This is the blend mode used to add Glow Lights and Object Emitted Light to the Stage. Different blend modes can effect "
                                         "oversaturation levels or be better for dark scenes.");

    addComponent(Components::World_Appearance, "Appearance", "These filters affect the entire world after it has been rendered.",
                                               Component_Colors::Brown_Sugar, true);
    getComponent(Components::World_Appearance)->setIcon(Component_Icons::Appearance);
    addPropertyToComponent(Components::World_Appearance, Properties::World_Filter_Bitrate, Property_Type::RangedInt, QList<QVariant>({ 256, 0, 256, 8 }),
                           "Bit Depth", "Standard output has color channel depth of 256, you can use this value to limit the number of available colors. "
                                        "Combining this with Pixelation gives a great retro look.");
    addPropertyToComponent(Components::World_Appearance, Properties::World_Filter_Pixelation, Property_Type::PositiveSizeF, QPointF(1.0, 1.0),
                           "Pixelation", "Size of x and y pixels, larger numbers provide more pixelation.");
    addPropertyToComponent(Components::World_Appearance, Properties::World_Filter_Brightness, Property_Type::Slider, QList<QVariant>({0, -255, 255, 5, ""}),
                           "Brightness", "How light / dark this world should appear. \nDefault: \t0 \nRange: \t-255 to 255");
    addPropertyToComponent(Components::World_Appearance, Properties::World_Filter_Contrast, Property_Type::Slider, QList<QVariant>({0, -255, 255, 5, ""}),
                           "Contrast", "Amount of distinguishable difference of colors. \nDefault: \t0 \nRange: \t-255 to 255");
    addPropertyToComponent(Components::World_Appearance, Properties::World_Filter_Saturation, Property_Type::Slider, QList<QVariant>({0, -255, 255, 5, ""}),
                           "Saturation", "How colorful the colors appear. \nDefault: \t0 \nRange: \t-255 to 255");
    addPropertyToComponent(Components::World_Appearance, Properties::World_Filter_Hue, Property_Type::Slider, QList<QVariant>({0, 0, 360, 5, ""}),
                           "Hue", "Rotate color values. \nDefault: \t0 \nRange: \t0 to 360");
    addPropertyToComponent(Components::World_Appearance, Properties::World_Filter_Grayscale, Property_Type::Bool, false,
                           "Grayscale", "Should this world be shown grayscale?");
    addPropertyToComponent(Components::World_Appearance, Properties::World_Filter_Negative, Property_Type::Bool, false,
                           "Negative", "Should this world's colors be inverted?");

    addComponent(Components::World_Special_Effects, "Special Effects", "Special effects that affect the entire world.", Component_Colors::Purple_Royal, true);
    getComponent(Components::World_Special_Effects)->setIcon(Component_Icons::Effects);
    addPropertyToComponent(Components::World_Special_Effects, Properties::World_Filter_Wireframe, Property_Type::Bool, false,
                           "Wireframe", "Renders all the world's objects as outlines.");
    addPropertyToComponent(Components::World_Special_Effects, Properties::World_Filter_Cartoon, Property_Type::Bool, false,
                           "Cartoon", "Gives the world a comic book look.");
    addPropertyToComponent(Components::World_Special_Effects, Properties::World_Filter_Wavy, Property_Type::Bool, false,
                           "Wavy", "Gives the world a trippy / drunken look.");
}








