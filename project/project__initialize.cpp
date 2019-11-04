//
//      Created by Stephens Nunnally on 10/10/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "project.h"
#include "project_world.h"
#include "project_stage.h"
#include "project_thing.h"


//####################################################################################
//##    Creates skeleton of a new Project
//####################################################################################
void DrProject::initializeNewProject(QString project_name, Orientation orientation, int width, int height, bool test) {

    // ***** Important! Clear Project / Reset Key Generator / Add Embedded Images
    this->clearProject();

    // Set if Project is debugging test fiile or not (test files dont ask to be saved when closed)
    setTestOnly(test);


    // ***** Set Project Options
    this->setOption(Project_Options::Name,              project_name);
    this->setOption(Project_Options::File_Name_Path,    "");
    this->setOption(Project_Options::Orientation,       static_cast<int>(orientation));
    this->setOption(Project_Options::Width,             width);
    this->setOption(Project_Options::Height,            height);


    // ***** Add Initial World
    DrWorld *world_1 = this->addWorld();
    DrWorld *current_world = world_1;
    this->setOption(Project_Options::Current_World, QVariant::fromValue(world_1->getKey()));
    this->setOption(Project_Options::Current_Stage, QVariant::fromValue(current_world->getFirstStageKey()) );


    // ***** Add Default Assets
    addDefaultAssets();


    // ***** Build Test Project
    if (test) {
        this->setOption(Project_Options::Name, "Rocky Rover");

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

        // Add some Worlds and Stages
        this->addWorld();                                                                  // "World 2"
        this->findWorldWithName("World 2")->addStage();                                    // Stage 2
        this->findWorldWithName("World 2")->addStage("asdfasdfasdfasdfasdfasdfasd");       // Stage 3
        this->findWorldWithName("World 2")->addStage();                                    // Stage 4


        // Stage 4 Rover Test
        long start_stage = this->findWorldWithName("World 2")->getFirstStageKey();
        this->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Character, a_dragon,  0,    60,   5);

        this->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_planet,   200,   600,   0);
        this->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_planet,  1200,   475,   0);

        this->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_plant,    900,    55,   5, false);
        this->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_plant,   1400,    55,   5, false);

        this->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_ground,  -411,  -200,   0);
        this->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_ground,     0,  -200,   0);
        this->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_ground,   411,  -200,   0);
        this->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_ground,   822,  -200,   0);
        this->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_ground,  1233,  -200,   0);

        this->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_top,     -411,   -50,   0);
        this->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_top,        0,   -50,   0);
        this->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_top,      411,   -50,   0);
        this->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_top,      822,   -50,   0);
        this->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_top,     1233,   -50,   0);

        this->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_block,    400,   200,   0);
        this->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_block,    550,   200,   0);
        this->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_block,    750,   350,   0);

        // Stage 2 Misc Test
        this->findWorldWithName("World 2")->getStageWithName("2")->addThing(DrThingType::Object, a_square,   600,   700,  0);
        this->findWorldWithName("World 2")->getStageWithName("2")->addThing(DrThingType::Text,   a_font,     500,   900,  0);

        this->findWorldWithName("World 2")->getStageWithName("2")->addThing(DrThingType::Object, a_ground,   200,  -200,  0);
        this->findWorldWithName("World 2")->getStageWithName("2")->addThing(DrThingType::Object, a_ground,   600,  -200,  0);
        this->findWorldWithName("World 2")->getStageWithName("2")->addThing(DrThingType::Object, a_ground,  1000,  -200,  0);

        // Stage 3 Ground
        this->findWorldWithName("World 2")->getStageWithName("asdfasdfasdfasdfasdfasdfasd")->addThing(DrThingType::Object, a_ground,   200,  -200,  0);
        this->findWorldWithName("World 2")->getStageWithName("asdfasdfasdfasdfasdfasdfasd")->addThing(DrThingType::Object, a_ground,   600,  -200,  0);
        this->findWorldWithName("World 2")->getStageWithName("asdfasdfasdfasdfasdfasdfasd")->addThing(DrThingType::Object, a_ground,  1000,  -200,  0);

        // Stage 4 Iso Cake Test
        this->findWorldWithName("World 2")->getStageWithName("4")->addThing(DrThingType::Object, a_cake1,    200,   500,  0);
        this->findWorldWithName("World 2")->getStageWithName("4")->addThing(DrThingType::Object, a_cake2,    400,   800,  0);
        this->findWorldWithName("World 2")->getStageWithName("4")->addThing(DrThingType::Object, a_cake3,    600,  1100,  0);

        this->findWorldWithName("World 2")->getStageWithName("4")->addThing(DrThingType::Object, a_ground,   200,  -200,  0);
        this->findWorldWithName("World 2")->getStageWithName("4")->addThing(DrThingType::Object, a_ground,   600,  -200,  0);
        this->findWorldWithName("World 2")->getStageWithName("4")->addThing(DrThingType::Object, a_ground,  1000,  -200,  0);

        DrStage* current_stage = this->findWorldWithName("World 2")->getStageFromKey(start_stage);
        this->setOption(Project_Options::Current_Stage, QVariant::fromValue(current_stage->getKey()) );
        this->setOption(Project_Options::Current_World, QVariant::fromValue(current_stage->getParentWorld()->getKey()) );
    }   // End Test Project


    // ***** Important! Signify we don't need to save at this point!
    setHasSaved(true);
}


//####################################################################################
//##    Adds Default Assets to Project
//####################################################################################
void DrProject::addDefaultAssets() {

    // ***** Add Effects
    if (findEffectFromType(DrEffectType::Light) == nullptr) {
        this->addEffect(DrEffectType::Light,    c_key_effect_light);
        this->addAsset(DrAssetType::Effect,     c_key_effect_light,     c_key_asset_light);
    }
    if (findEffectFromType(DrEffectType::Water) == nullptr) {
        this->addEffect(DrEffectType::Water,    c_key_effect_water);
        this->addAsset(DrAssetType::Effect,     c_key_effect_water,     c_key_asset_water);
    }
    if (findEffectFromType(DrEffectType::Fire) == nullptr) {
        this->addEffect(DrEffectType::Fire,     c_key_effect_fire);
        this->addAsset(DrAssetType::Effect,     c_key_effect_fire,      c_key_asset_fire);
    }
    if (findEffectFromType(DrEffectType::Mirror) == nullptr) {
        this->addEffect(DrEffectType::Mirror,   c_key_effect_mirror);
        this->addAsset(DrAssetType::Effect,     c_key_effect_mirror,    c_key_asset_mirror);
    }
    if (findEffectFromType(DrEffectType::Fisheye) == nullptr) {
        this->addEffect(DrEffectType::Fisheye,  c_key_effect_fisheye);
        this->addAsset(DrAssetType::Effect,     c_key_effect_fisheye,   c_key_asset_fisheye);
    }
    if (findEffectFromType(DrEffectType::Swirl) == nullptr) {
        this->addEffect(DrEffectType::Swirl,    c_key_effect_swirl);
        this->addAsset(DrAssetType::Effect,     c_key_effect_swirl,     c_key_asset_swirl);
    }

    // ***** Add Devices
    if (findDeviceFromType(DrDeviceType::Camera) == nullptr) {
        this->addDevice(DrDeviceType::Camera,   c_key_device_camera);
        this->addAsset(DrAssetType::Device,     c_key_device_camera,    c_key_asset_camera);
    }
}














