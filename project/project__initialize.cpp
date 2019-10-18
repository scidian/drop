//
//      Created by Stephens Nunnally on 10/10/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "project.h"
#include "project_world.h"
#include "project_world_stage.h"
#include "project_world_stage_thing.h"


//####################################################################################
//##    Creates skeleton of a new Project
//####################################################################################
void DrProject::initializeNewProject(QString project_name, Orientation orientation, int width, int height, bool test) {
    m_test_only = false;

    // ***** Set Project Options
    this->setOption(Project_Options::Name,              project_name);
    this->setOption(Project_Options::File_Name_Path,    "");
    this->setOption(Project_Options::Orientation,       static_cast<int>(orientation));
    this->setOption(Project_Options::Width,             width);
    this->setOption(Project_Options::Height,            height);


    // ***** Add Initial World
    long world_1 = this->addWorld();
    DrWorld* current_world = this->findWorldFromKey(world_1);
    this->setOption(Project_Options::Current_World, QVariant::fromValue(world_1));
    this->setOption(Project_Options::Current_Stage, QVariant::fromValue(current_world->getFirstStageKey()) );


    // ***** Add Effects
    long effect_1 =  this->addEffect(DrEffectType::Light);
    long effect_2 =  this->addEffect(DrEffectType::Water);
    long effect_3 =  this->addEffect(DrEffectType::Fire);
    long effect_4 =  this->addEffect(DrEffectType::Mirror);
    long effect_5 =  this->addEffect(DrEffectType::Fisheye);
    long effect_6 =  this->addEffect(DrEffectType::Swirl);
    //long effect_7 =  this->addEffect(DrEffectType::Flag);
    //long effect_8 =  this->addEffect(DrEffectType::Rain);
    //long effect_9 =  this->addEffect(DrEffectType::Snow);
    //long effect_10 = this->addEffect(DrEffectType::Clouds);
    //long effect_11 = this->addEffect(DrEffectType::Fog);

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
        setTestOnly(true);

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


    // ***** Important! Signify we don't need to save at this point!
    setHasSaved(true);
}









