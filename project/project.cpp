//
//      Created by Stephens Nunnally on 12/7/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrProject Class Definitions
//
//
#include <QDebug>
#include <QRandomGenerator>
#include <QTime>

#include "helper.h"
#include "helper_qt.h"
#include "project.h"
#include "project_asset.h"
#include "project_effect.h"
#include "project_font.h"
#include "project_image.h"
#include "project_world.h"
#include "project_world_stage.h"
#include "project_world_stage_thing.h"


//####################################################################################
//##    Constructor, Destructor
//####################################################################################
DrProject::DrProject(long key_generator_starting_number) {
    // #NOTE: Don't alllow key to start at less than 1, having an item with key 0 could conflict with nullptr results
    if (key_generator_starting_number <= 1) key_generator_starting_number = 1;
    m_key_generator = key_generator_starting_number;
}

DrProject::~DrProject() {
    clearProject();
}

void DrProject::clearProject() {
    for (auto it = m_worlds.begin();    it != m_worlds.end(); )     {   delete it->second; it = m_worlds.erase(it);     }
    for (auto it = m_assets.begin();    it != m_assets.end(); )     {   delete it->second; it = m_assets.erase(it);     }
    for (auto it = m_fonts.begin();     it != m_fonts.end(); )      {   delete it->second; it = m_fonts.erase(it);      }
    for (auto it = m_images.begin();    it != m_images.end(); )     {   delete it->second; it = m_images.erase(it);     }
    for (auto it = m_effects.begin();   it != m_effects.end(); )    {   delete it->second; it = m_effects.erase(it);    }
    m_key_generator = 1;
}


//####################################################################################
//##
//##    Functions to add different item types into project
//##
//####################################################################################
long DrProject::addAsset(DrAssetType new_asset_type, long image_key) {
    long new_asset_key = getNextKey();
    m_assets[new_asset_key] = new DrAsset(this, new_asset_key, new_asset_type, image_key);
    return new_asset_key;
}

long DrProject::addEffect(QString effect_name, DrEffectType effect_type) {
    long new_effect_key = getNextKey();
    m_effects[new_effect_key] = new DrEffect(this, new_effect_key, effect_name, effect_type);
    return new_effect_key;
}

long DrProject::addFont(QString font_name, QPixmap font_pixmap, QString font_family, int font_size, bool use_test_rects) {
    long new_font_key = getNextKey();
    m_fonts[new_font_key] = new DrFont(this, new_font_key, font_name, font_pixmap, font_family, font_size, use_test_rects);
    return new_font_key;
}

long DrProject::addImage(QString image_path) {
    long new_image_key = getNextKey();
    m_images[new_image_key] = new DrImage(this, new_image_key, image_path);
    return new_image_key;
}


// Adds a World to the map container, finds next availbable "World xxx" name to assign to World
long DrProject::addWorld() {
    int test_num = 0;
    QString new_name;
    do {
        ++test_num;
        new_name = "World " + QString::number(test_num);
    } while (findWorldWithName(new_name) != nullptr);

    long new_world_key = getNextKey();
    m_worlds[new_world_key] = new DrWorld(this, new_world_key, new_name);
    return new_world_key;
}


//####################################################################################
//##
//##    Key Finding - These functions search the current project for the specified Key and return the requested info
//##                  (key's are generated from Project key generator upon item initialization)
//##
//####################################################################################

// Returns a pointer to the Base DrSettings class of the item with the specified key
DrSettings* DrProject::findSettingsFromKey(long check_key, bool show_warning) {
    AssetMap::iterator asset_iter = m_assets.find(check_key);
    if (asset_iter != m_assets.end())   return asset_iter->second;

    ImageMap::iterator image_iter = m_images.find(check_key);
    if (image_iter != m_images.end())   return image_iter->second;

    EffectMap::iterator effect_iter = m_effects.find(check_key);
    if (effect_iter != m_effects.end()) return effect_iter->second;

    FontMap::iterator font_iter = m_fonts.find(check_key);
    if (font_iter != m_fonts.end())     return font_iter->second;

    WorldMap &worlds = m_worlds;
    WorldMap::iterator world_iter = worlds.find(check_key);
    if (world_iter != worlds.end())   return world_iter->second;

    for (auto world_pair : worlds) {
        StageMap &stages = world_pair.second->getStageMap();
        StageMap::iterator stage_iter = stages.find(check_key);
        if (stage_iter != stages.end())     return stage_iter->second;

        for (auto stage_pair : stages) {
            ThingMap &things = stage_pair.second->getThingMap();
            ThingMap::iterator thing_iter = things.find(check_key);
            if (thing_iter != things.end()) return thing_iter->second;
        }
    }

    if (show_warning) {
        Dr::ShowMessageBox("WARNING: Did not find key (" + QString::number(check_key) +
                           ") in project! \n"
                           "Last key used in project: " + QString::number(m_key_generator - 1) + "!\n\n"
                           "This warning called from \"DrProject::findChildSettingsFromKey\"");
    }
    return nullptr;
}

// Searches all member variables / containers for the specified unique project key
DrType DrProject::findChildTypeFromKey(long check_key) {
    DrSettings *settings = findSettingsFromKey(check_key);
    if (settings == nullptr) {
        return DrType::NotFound;
    } else {
        return settings->getType();
    }
}


DrAsset* DrProject::findAssetFromKey(long check_key) {
    AssetMap::iterator asset_iter = m_assets.find(check_key);
    if (asset_iter != m_assets.end())
        return asset_iter->second;
    else
        return nullptr;
}

DrStage* DrProject::findStageFromKey(long check_key) {
    for (auto world_pair : m_worlds) {
        StageMap &stages = world_pair.second->getStageMap();
        StageMap::iterator stage_iter = stages.find(check_key);

        if (stage_iter != stages.end())
            return stage_iter->second;
    }
    return nullptr;
}

DrThing* DrProject::findThingFromKey(long check_key) {
    for (auto world : m_worlds) {
        for (auto stage : world.second->getStageMap()) {
            ThingMap &things = stage.second->getThingMap();
            ThingMap::iterator thing_iter = things.find(check_key);
            if (thing_iter != things.end())
                return thing_iter->second;
        }
    }
    return nullptr;
}

DrWorld* DrProject::findWorldFromKey(long check_key) {
    WorldMap::iterator world_iter = m_worlds.find(check_key);
    if (world_iter != m_worlds.end())
        return world_iter->second;
    else
        return nullptr;
}

// Returns a pointer to the World with the mathcing name
DrWorld* DrProject::findWorldWithName(QString world_name) {
    QString compare_name;
    for (auto world_pair: m_worlds) {
        compare_name = world_pair.second->getName();
        if (compare_name == world_name) { return world_pair.second; }
    }
    return nullptr;
}


//####################################################################################
//##    Tests find function for speed so we can test speed of std::map vs std::unordered_map, etc
//####################################################################################
QString DrProject::testSpeedFindSettings(int test_size) {
    // Show some initial data
    int     number_of_assets = int(m_key_generator) - 1;
    QString results =   "Find Speed Test \n---------------\n"
                        "Number of Assets in Project: " + QString::number(number_of_assets) + "\n"
                        "Newest Asset Name: " + findSettingsFromKey(number_of_assets)->getName() + "\n"
                        "Newest Asset Type: " + Dr::StringFromType(findSettingsFromKey(number_of_assets)->getType()) + "\n"
                        "First  Asset Name: " + findSettingsFromKey(c_key_starting_number)->getName() + "\n"
                        "First  Asset Type: " + Dr::StringFromType(findSettingsFromKey(c_key_starting_number)->getType()) + "\n\n"
                        "Number of iterations tested: " + QString::number(test_size) + "\n";

    // Create a vector of random keys to find
    QVector<int> keys;
    for (int i = 0; i < test_size; i++)
        keys.append( QRandomGenerator::global()->bounded(c_key_starting_number, number_of_assets) );

    // Find the random keys within the project
    QTime timer;
    timer.restart();
    for (auto key : keys)
        findSettingsFromKey(key);
    results += "Total milliseconds taken to find all settings in project: " + QString::number(timer.elapsed()) + "\n\n";

    return results;
}



//####################################################################################
//##    Creates skeleton of a new Project
//####################################################################################
void DrProject::initializeNewProject(QString project_name, Orientation orientation, int width, int height, bool test) {

    // ***** Set Project Options
    this->setOption(Project_Options::Name,              project_name);
    this->setOption(Project_Options::Orientation,       static_cast<int>(orientation));
    this->setOption(Project_Options::Width,             width);
    this->setOption(Project_Options::Height,            height);

    // ***** Add Initial World
    long world_1 = this->addWorld();
    DrWorld* current_world = this->findWorldFromKey(world_1);
    this->setOption(Project_Options::Current_World, QVariant::fromValue(world_1));
    this->setOption(Project_Options::Current_Stage, QVariant::fromValue(current_world->getFirstStageKey()) );

    // ***** Add Effects
    long effect_1 =  this->addEffect("Light",           DrEffectType::Light);
    long effect_2 =  this->addEffect("Water",           DrEffectType::Water);
    long effect_3 =  this->addEffect("Fire",            DrEffectType::Fire);
    long effect_4 =  this->addEffect("Mirror",          DrEffectType::Mirror);
    long effect_5 =  this->addEffect("Fisheye Lens",    DrEffectType::Fisheye);
    long effect_6 =  this->addEffect("Swirl",           DrEffectType::Swirl);
    //long effect_7 =  this->addEffect("Flag",           DrEffectType::Flag);
    //long effect_8 =  this->addEffect("Rain",           DrEffectType::Rain);
    //long effect_9 =  this->addEffect("Snow",           DrEffectType::Snow);
    //long effect_10 = this->addEffect("Clouds",         DrEffectType::Clouds);
    //long effect_11 = this->addEffect("Fog",            DrEffectType::Fog);

    this->addAsset(DrAssetType::Effect, effect_1);                             // Light
    this->addAsset(DrAssetType::Effect, effect_2);                             // Water
    this->addAsset(DrAssetType::Effect, effect_3);                             // Fire
    this->addAsset(DrAssetType::Effect, effect_4);                             // Mirror
    this->addAsset(DrAssetType::Effect, effect_5);                             // Fisheye
    this->addAsset(DrAssetType::Effect, effect_6);                             // Swirl
    //this->addAsset(DrAssetType::Effect, effect_7);                             // Flag
    //this->addAsset(DrAssetType::Effect, effect_8);                             // Rain
    //this->addAsset(DrAssetType::Effect, effect_9);                             // Snow
    //this->addAsset(DrAssetType::Effect, effect_10);                            // Clouds
    //this->addAsset(DrAssetType::Effect, effect_11);                            // Fog


    // ***** Build Test Project
    if (test) {

        long image_1  = this->addImage(":/assets/test_images/test_square.png");
        long image_2  = this->addImage(":/assets/test_images/ground_fill.png");
        long image_3  = this->addImage(":/assets/test_images/ground_top.png");
        long image_4  = this->addImage(":/assets/test_images/moon_plant_6.png");
        long image_5  = this->addImage(":/assets/test_images/rover_body.png");
        long image_6  = this->addImage(":/assets/test_images/ball_1.png");
        long image_7  = this->addImage(":/assets/test_images/water_1.png");
        long image_8  = this->addImage(":/assets/test_images/rover_wheel.png");
        long image_9  = this->addImage(":/assets/test_images/cake_block.png");
        long image_10 = this->addImage(":/assets/test_images/cake_chocolate.png");
        long image_11 = this->addImage(":/assets/test_images/cake_ice_cube.png");
        long image_12 = this->addImage(":/assets/test_images/planetwithareallylongname.png");
        long image_13 = this->addImage(":/assets/test_images/metal_block.png");
        long image_14 = this->addImage(":/assets/test_images/donut.png");
        long image_15 = this->addImage(":/assets/test_images/shapes2.png");
        long image_16 = this->addImage(":/assets/test_images/dragon.png");

        long font_1 =   this->addFont("Distance Font", QPixmap(":/assets/test_images/test_font.png"),   "Arial",          20, true);
        long font_2 =   this->addFont("Coin Count",    QPixmap(":/assets/test_images/test_font_2.png"), "Britannic Bold", 15, true);
        long font_3 =   this->addFont("I Love Julie",  QPixmap(":/assets/test_images/test_font_3.png"), "Bauhaus 93",     36, true);

                         this->addAsset(DrAssetType::Character, image_6 );         // "Ball 1"
        long a_dragon =  this->addAsset(DrAssetType::Character, image_16 );        // "Dragon"

        long a_square =  this->addAsset(DrAssetType::Object, image_1 );            // "Dr Square"
        long a_ground  = this->addAsset(DrAssetType::Object, image_2 );            // "Ground Fill"
        long a_top  =    this->addAsset(DrAssetType::Object, image_3 );            // "Ground Top"
        long a_plant  =  this->addAsset(DrAssetType::Object, image_4 );            // "Moon Plant 6"
        long a_block  =  this->addAsset(DrAssetType::Object, image_13 );           // "Metal Block"
        long a_planet =  this->addAsset(DrAssetType::Object, image_12 );           // "PlanetWithAReallyLongName"

        long a_cake1  =  this->addAsset(DrAssetType::Object, image_9 );            // "Cake Block"
        long a_cake2 =   this->addAsset(DrAssetType::Object, image_10 );           // "Cake Chocolate"
        long a_cake3 =   this->addAsset(DrAssetType::Object, image_11 );           // "Cake Ice Cube"

        this->addAsset(DrAssetType::Object, image_7 );                             // "Water 1"
        this->addAsset(DrAssetType::Object, image_5 );                             // "Rover Body"
        this->addAsset(DrAssetType::Object, image_8 );                             // "Rover Wheel"
        this->addAsset(DrAssetType::Object, image_14 );                            // "Donut"
        this->addAsset(DrAssetType::Object, image_15 );                            // "Shapes 2"

        long a_font =    this->addAsset(DrAssetType::Text, font_1);                // "Test Font"
        this->addAsset(DrAssetType::Text, font_2);                                 // "Test Font 2"
        this->addAsset(DrAssetType::Text, font_3);                                 // "Test Font 3"

        // !!!!! #TEMP: New Project
        // Create a new project and add some stuff to it
        this->addWorld();                                                                  // "World 2"
        this->findWorldWithName("World 2")->addStage();                                    // Stage 2
        this->findWorldWithName("World 2")->addStage("asdfasdfasdfasdfasdfasdfasd");       // Stage 3
        this->findWorldWithName("World 2")->addStage();                                    // Stage 4


        // Stage 4 Rover Test
        long start_stage = this->findWorldWithName("World 2")->getFirstStageKey();
        this->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Character, a_dragon,  0,    60,   0);

        this->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_planet,   200,   600,  -1);
        this->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_planet,  1200,   475,  -1);

        this->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_plant,    900,    55,  -4, false);
        this->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_plant,   1400,    55,  -4, false);

        this->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_ground,  -411,  -200,  -3);
        this->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_ground,     0,  -200,  -3);
        this->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_ground,   411,  -200,  -3);
        this->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_ground,   822,  -200,  -3);
        this->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_ground,  1233,  -200,  -3);

        this->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_top,     -411,   -50,  -2);
        this->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_top,        0,   -50,  -2);
        this->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_top,      411,   -50,  -2);
        this->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_top,      822,   -50,  -2);
        this->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_top,     1233,   -50,  -2);

        this->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_block,    400,   200,  -3);
        this->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_block,    550,   200,  -3);
        this->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_block,    750,   350,  -3);

        // Stage 2 Misc Test
        this->findWorldWithName("World 2")->getStageWithName("2")->addThing(DrThingType::Object, a_square,   600,   700,  2);
        this->findWorldWithName("World 2")->getStageWithName("2")->addThing(DrThingType::Text,   a_font,     500,   900,  6);

        this->findWorldWithName("World 2")->getStageWithName("2")->addThing(DrThingType::Object, a_ground,   200,  -200,  -3);
        this->findWorldWithName("World 2")->getStageWithName("2")->addThing(DrThingType::Object, a_ground,   600,  -200,  -3);
        this->findWorldWithName("World 2")->getStageWithName("2")->addThing(DrThingType::Object, a_ground,  1000,  -200,  -3);

        // Stage 3 Ground
        this->findWorldWithName("World 2")->getStageWithName("asdfasdfasdfasdfasdfasdfasd")->addThing(DrThingType::Object, a_ground,   200,  -200,  -3);
        this->findWorldWithName("World 2")->getStageWithName("asdfasdfasdfasdfasdfasdfasd")->addThing(DrThingType::Object, a_ground,   600,  -200,  -3);
        this->findWorldWithName("World 2")->getStageWithName("asdfasdfasdfasdfasdfasdfasd")->addThing(DrThingType::Object, a_ground,  1000,  -200,  -3);

        // Stage 4 Iso Cake Test
        this->findWorldWithName("World 2")->getStageWithName("4")->addThing(DrThingType::Object, a_cake1,    200,   500,  2);
        this->findWorldWithName("World 2")->getStageWithName("4")->addThing(DrThingType::Object, a_cake2,    400,   800,  4);
        this->findWorldWithName("World 2")->getStageWithName("4")->addThing(DrThingType::Object, a_cake3,    600,  1100, -2);

        this->findWorldWithName("World 2")->getStageWithName("4")->addThing(DrThingType::Object, a_ground,   200,  -200,  -3);
        this->findWorldWithName("World 2")->getStageWithName("4")->addThing(DrThingType::Object, a_ground,   600,  -200,  -3);
        this->findWorldWithName("World 2")->getStageWithName("4")->addThing(DrThingType::Object, a_ground,  1000,  -200,  -3);

        DrStage* current_stage = this->findWorldWithName("World 2")->getStageFromKey(start_stage);
        this->setOption(Project_Options::Current_Stage, QVariant::fromValue(current_stage->getKey()) );
        this->setOption(Project_Options::Current_World, QVariant::fromValue(current_stage->getParentWorld()->getKey()) );
    }   // End Test Project

}



















