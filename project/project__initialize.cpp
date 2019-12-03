//
//      Created by Stephens Nunnally on 10/10/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "project/project.h"
#include "project/project_asset.h"
#include "project/project_image.h"
#include "project/project_stage.h"
#include "project/project_thing.h"
#include "project/project_world.h"


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


    // ***** Build Test Project
    if (test) {
        this->setOption(Project_Options::Name, "Rocky Rover");

        DrImage *image_1  = this->addImage(":/assets/test_images/test_square.png");
        DrImage *image_2  = this->addImage(":/assets/test_images/ground_fill.png");
        DrImage *image_3  = this->addImage(":/assets/test_images/ground_top.png");
        DrImage *image_4  = this->addImage(":/assets/test_images/moon_plant_6.png");
        DrImage *image_5  = this->addImage(":/assets/test_images/rover_body.png");
        DrImage *image_6  = this->addImage(":/assets/test_images/ball_1.png");
        DrImage *image_7  = this->addImage(":/assets/test_images/water_1.png");
        DrImage *image_8  = this->addImage(":/assets/test_images/rover_wheel.png");
        DrImage *image_9  = this->addImage(":/assets/test_images/cake_block.png");
        DrImage *image_10 = this->addImage(":/assets/test_images/cake_chocolate.png");
        DrImage *image_11 = this->addImage(":/assets/test_images/cake_ice_cube.png");
        DrImage *image_12 = this->addImage(":/assets/test_images/planetwithareallylongname.png");
        DrImage *image_13 = this->addImage(":/assets/test_images/metal_block.png");
        DrImage *image_14 = this->addImage(":/assets/test_images/donut.png");
        DrImage *image_15 = this->addImage(":/assets/test_images/shapes2.png");
        DrImage *image_16 = this->addImage(":/assets/test_images/dragon.png");

        long font_1 =   this->addFont("Distance Font", QPixmap(":/assets/test_images/test_font.png"),   "Arial",          20, true);
                        this->addFont("Coin Count",    QPixmap(":/assets/test_images/test_font_2.png"), "Britannic Bold", 15, true);
                        this->addFont("I Love Julie",  QPixmap(":/assets/test_images/test_font_3.png"), "Bauhaus 93",     36, true);

                                this->addAsset(DrAssetType::Character, image_6->getKey() );         // "Ball 1"
        DrAsset *a_dragon =     this->addAsset(DrAssetType::Character, image_16->getKey() );        // "Dragon"

        DrAsset *a_square =     this->addAsset(DrAssetType::Object, image_1->getKey() );            // "Dr Square"
        DrAsset *a_ground =     this->addAsset(DrAssetType::Object, image_2->getKey() );            // "Ground Fill"
        DrAsset *a_top =        this->addAsset(DrAssetType::Object, image_3->getKey() );            // "Ground Top"
        DrAsset *a_plant =      this->addAsset(DrAssetType::Object, image_4->getKey() );            // "Moon Plant 6"
        DrAsset *a_block =      this->addAsset(DrAssetType::Object, image_13->getKey() );           // "Metal Block"
        DrAsset *a_planet =     this->addAsset(DrAssetType::Object, image_12->getKey() );           // "PlanetWithAReallyLongName"

        DrAsset *a_cake1 =      this->addAsset(DrAssetType::Object, image_9->getKey() );            // "Cake Block"
        DrAsset *a_cake2 =      this->addAsset(DrAssetType::Object, image_10->getKey() );           // "Cake Chocolate"
        DrAsset *a_cake3 =      this->addAsset(DrAssetType::Object, image_11->getKey() );           // "Cake Ice Cube"

        this->addAsset(DrAssetType::Object, image_7->getKey() );                                    // "Water 1"
        this->addAsset(DrAssetType::Object, image_5->getKey() );                                    // "Rover Body"
        this->addAsset(DrAssetType::Object, image_8->getKey() );                                    // "Rover Wheel"
        this->addAsset(DrAssetType::Object, image_14->getKey() );                                   // "Donut"
        this->addAsset(DrAssetType::Object, image_15->getKey() );                                   // "Shapes 2"


        // Add some Worlds and Stages
        this->addWorld();                                                                           // "World 2"
        this->findWorldWithName("World 2")->addStage();                                             // Stage 2
        this->findWorldWithName("World 2")->addStage("asdfasdfasdfasdfasdfasdfasd");                // Stage 3
        this->findWorldWithName("World 2")->addStage();                                             // Stage 4


        // Stage 4 Rover Test
        long start_stage = this->findWorldWithName("World 2")->getFirstStageKey();
        this->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Character, a_dragon->getKey(),  0,    60,   5);

        this->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_planet->getKey(),   200,   600,   0);
        this->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_planet->getKey(),  1200,   475,   0);

        this->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_plant->getKey(),    900,    55,   5, false);
        this->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_plant->getKey(),   1400,    55,   5, false);

        this->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_ground->getKey(),  -411,  -200,   0);
        this->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_ground->getKey(),     0,  -200,   0);
        this->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_ground->getKey(),   411,  -200,   0);
        this->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_ground->getKey(),   822,  -200,   0);
        this->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_ground->getKey(),  1233,  -200,   0);

        this->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_top->getKey(),     -411,   -50,   0);
        this->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_top->getKey(),        0,   -50,   0);
        this->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_top->getKey(),      411,   -50,   0);
        this->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_top->getKey(),      822,   -50,   0);
        this->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_top->getKey(),     1233,   -50,   0);

        this->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_block->getKey(),    400,   200,   0);
        this->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_block->getKey(),    550,   200,   0);
        this->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_block->getKey(),    750,   350,   0);

        // Stage 2 Misc Test
        this->findWorldWithName("World 2")->getStageWithName("2")->addThing(DrThingType::Object, a_square->getKey(),   600,   700,  0);
        this->findWorldWithName("World 2")->getStageWithName("2")->addThing(DrThingType::Text,   font_1,               500,   900,  0);

        this->findWorldWithName("World 2")->getStageWithName("2")->addThing(DrThingType::Object, a_ground->getKey(),   200,  -200,  0);
        this->findWorldWithName("World 2")->getStageWithName("2")->addThing(DrThingType::Object, a_ground->getKey(),   600,  -200,  0);
        this->findWorldWithName("World 2")->getStageWithName("2")->addThing(DrThingType::Object, a_ground->getKey(),  1000,  -200,  0);

        // Stage 3 Ground
        this->findWorldWithName("World 2")->getStageWithName("asdfasdfasdfasdfasdfasdfasd")->addThing(DrThingType::Object, a_ground->getKey(),   200,  -200,  0);
        this->findWorldWithName("World 2")->getStageWithName("asdfasdfasdfasdfasdfasdfasd")->addThing(DrThingType::Object, a_ground->getKey(),   600,  -200,  0);
        this->findWorldWithName("World 2")->getStageWithName("asdfasdfasdfasdfasdfasdfasd")->addThing(DrThingType::Object, a_ground->getKey(),  1000,  -200,  0);

        // Stage 4 Iso Cake Test
        this->findWorldWithName("World 2")->getStageWithName("4")->addThing(DrThingType::Object, a_cake1->getKey(),    200,   500,  0);
        this->findWorldWithName("World 2")->getStageWithName("4")->addThing(DrThingType::Object, a_cake2->getKey(),    400,   800,  0);
        this->findWorldWithName("World 2")->getStageWithName("4")->addThing(DrThingType::Object, a_cake3->getKey(),    600,  1100,  0);

        this->findWorldWithName("World 2")->getStageWithName("4")->addThing(DrThingType::Object, a_ground->getKey(),   200,  -200,  0);
        this->findWorldWithName("World 2")->getStageWithName("4")->addThing(DrThingType::Object, a_ground->getKey(),   600,  -200,  0);
        this->findWorldWithName("World 2")->getStageWithName("4")->addThing(DrThingType::Object, a_ground->getKey(),  1000,  -200,  0);

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
    if (findEffectFromType(DrEffectType::Light) == nullptr)     this->addEffect(DrEffectType::Light,    c_key_effect_light);
    if (findEffectFromType(DrEffectType::Water) == nullptr)     this->addEffect(DrEffectType::Water,    c_key_effect_water);
    if (findEffectFromType(DrEffectType::Fire) == nullptr)      this->addEffect(DrEffectType::Fire,     c_key_effect_fire);
    if (findEffectFromType(DrEffectType::Mirror) == nullptr)    this->addEffect(DrEffectType::Mirror,   c_key_effect_mirror);
    if (findEffectFromType(DrEffectType::Fisheye) == nullptr)   this->addEffect(DrEffectType::Fisheye,  c_key_effect_fisheye);
    if (findEffectFromType(DrEffectType::Swirl) == nullptr)     this->addEffect(DrEffectType::Swirl,    c_key_effect_swirl);

    // ***** Add Devices
    if (findDeviceFromType(DrDeviceType::Camera) == nullptr)    this->addDevice(DrDeviceType::Camera,   c_key_device_camera);
}

//####################################################################################
//##    Adds Built In Images to Project
//####################################################################################
void DrProject::addBuiltInImages() {
    QImage::Format format = QImage::Format::Format_ARGB32;

    QString path_empty =     ":/assets/dr_images/empty.png";            QImage image_empty =        QImage(path_empty).convertToFormat(format);
    QString path_character = ":/assets/dr_images/circle.png";           QImage image_character =    QImage(path_character).convertToFormat(format);
    QString path_object =    ":/assets/dr_images/box.png";              QImage image_object =       QImage(path_object).convertToFormat(format);
    this->addImage(c_key_image_empty,       path_empty,         "empty.png",        "empty",            image_empty);
    this->addImage(c_key_image_character,   path_character,     "circle.png",       "ball",             image_character);
    this->addImage(c_key_image_object,      path_object,        "box.png",          "block",            image_object);

    //##    Built In Images - Basic         300-399
    this->addImage(":/assets/dr_images/basic/square_1.png",                 c_key_image_square_1,               Asset_Category::Basic);
    this->addImage(":/assets/dr_images/basic/square_2.png",                 c_key_image_square_2,               Asset_Category::Basic);
    this->addImage(":/assets/dr_images/basic/square_3.png",                 c_key_image_square_3,               Asset_Category::Basic);
    this->addImage(":/assets/dr_images/basic/square_4.png",                 c_key_image_square_4,               Asset_Category::Basic);
    this->addImage(":/assets/dr_images/basic/square_5.png",                 c_key_image_square_5,               Asset_Category::Basic);
    this->addImage(":/assets/dr_images/basic/square_6.png",                 c_key_image_square_6,               Asset_Category::Basic);
    this->addImage(":/assets/dr_images/basic/square_7.png",                 c_key_image_square_7,               Asset_Category::Basic);
    this->addImage(":/assets/dr_images/basic/square_bars.png",              c_key_image_square_bars,            Asset_Category::Basic);
    this->addImage(":/assets/dr_images/basic/square_flat.png",              c_key_image_square_flat,            Asset_Category::Basic);
    this->addImage(":/assets/dr_images/basic/square_stripes.png",           c_key_image_square_stripes,         Asset_Category::Basic);
    this->addImage(":/assets/dr_images/basic/rounded_1.png",                c_key_image_rounded_1,              Asset_Category::Basic);
    this->addImage(":/assets/dr_images/basic/rounded_2.png",                c_key_image_rounded_2,              Asset_Category::Basic);
    this->addImage(":/assets/dr_images/basic/rounded_3.png",                c_key_image_rounded_3,              Asset_Category::Basic);
    this->addImage(":/assets/dr_images/basic/rounded_4.png",                c_key_image_rounded_4,              Asset_Category::Basic);
    this->addImage(":/assets/dr_images/basic/rounded_5.png",                c_key_image_rounded_5,              Asset_Category::Basic);
    this->addImage(":/assets/dr_images/basic/rounded_6.png",                c_key_image_rounded_6,              Asset_Category::Basic);
    this->addImage(":/assets/dr_images/basic/rounded_7.png",                c_key_image_rounded_7,              Asset_Category::Basic);
    this->addImage(":/assets/dr_images/basic/rounded_bars.png",             c_key_image_rounded_bars,           Asset_Category::Basic);
    this->addImage(":/assets/dr_images/basic/rounded_flat.png",             c_key_image_rounded_flat,           Asset_Category::Basic);
    this->addImage(":/assets/dr_images/basic/rounded_stripes.png",          c_key_image_rounded_stripes,        Asset_Category::Basic);
    this->addImage(":/assets/dr_images/basic/circle_1.png",                 c_key_image_circle_1,               Asset_Category::Basic);
    this->addImage(":/assets/dr_images/basic/circle_2.png",                 c_key_image_circle_2,               Asset_Category::Basic);
    this->addImage(":/assets/dr_images/basic/circle_3.png",                 c_key_image_circle_3,               Asset_Category::Basic);
    this->addImage(":/assets/dr_images/basic/circle_4.png",                 c_key_image_circle_4,               Asset_Category::Basic);
    this->addImage(":/assets/dr_images/basic/circle_5.png",                 c_key_image_circle_5,               Asset_Category::Basic);
    this->addImage(":/assets/dr_images/basic/circle_6.png",                 c_key_image_circle_6,               Asset_Category::Basic);
    this->addImage(":/assets/dr_images/basic/circle_7.png",                 c_key_image_circle_7,               Asset_Category::Basic);
    this->addImage(":/assets/dr_images/basic/circle_bars.png",              c_key_image_circle_bars,            Asset_Category::Basic);
    this->addImage(":/assets/dr_images/basic/circle_flat.png",              c_key_image_circle_flat,            Asset_Category::Basic);
    this->addImage(":/assets/dr_images/basic/circle_stripes.png",           c_key_image_circle_stripes,         Asset_Category::Basic);
    this->addImage(":/assets/dr_images/basic/triangle_1.png",               c_key_image_triangle_1,             Asset_Category::Basic);
    this->addImage(":/assets/dr_images/basic/triangle_2.png",               c_key_image_triangle_2,             Asset_Category::Basic);
    this->addImage(":/assets/dr_images/basic/triangle_3.png",               c_key_image_triangle_3,             Asset_Category::Basic);
    this->addImage(":/assets/dr_images/basic/triangle_4.png",               c_key_image_triangle_4,             Asset_Category::Basic);
    this->addImage(":/assets/dr_images/basic/triangle_5.png",               c_key_image_triangle_5,             Asset_Category::Basic);
    this->addImage(":/assets/dr_images/basic/triangle_6.png",               c_key_image_triangle_6,             Asset_Category::Basic);
    this->addImage(":/assets/dr_images/basic/triangle_7.png",               c_key_image_triangle_7,             Asset_Category::Basic);
    this->addImage(":/assets/dr_images/basic/triangle_bars.png",            c_key_image_triangle_bars,          Asset_Category::Basic);
    this->addImage(":/assets/dr_images/basic/triangle_flat.png",            c_key_image_triangle_flat,          Asset_Category::Basic);
    this->addImage(":/assets/dr_images/basic/triangle_stripes.png",         c_key_image_triangle_stripes,       Asset_Category::Basic);
    this->addImage(":/assets/dr_images/basic/diamond_1.png",                c_key_image_diamond_1,              Asset_Category::Basic);
    this->addImage(":/assets/dr_images/basic/diamond_2.png",                c_key_image_diamond_2,              Asset_Category::Basic);
    this->addImage(":/assets/dr_images/basic/diamond_3.png",                c_key_image_diamond_3,              Asset_Category::Basic);
    this->addImage(":/assets/dr_images/basic/diamond_4.png",                c_key_image_diamond_4,              Asset_Category::Basic);
    this->addImage(":/assets/dr_images/basic/diamond_5.png",                c_key_image_diamond_5,              Asset_Category::Basic);
    this->addImage(":/assets/dr_images/basic/diamond_bars.png",             c_key_image_diamond_bars,           Asset_Category::Basic);
    this->addImage(":/assets/dr_images/basic/diamond_flat.png",             c_key_image_diamond_flat,           Asset_Category::Basic);
    this->addImage(":/assets/dr_images/basic/diamond_stripes.png",          c_key_image_diamond_stripes,        Asset_Category::Basic);
    this->addImage(":/assets/dr_images/basic/line_1.png",                   c_key_image_line_1,                 Asset_Category::Basic);
    this->addImage(":/assets/dr_images/basic/line_2.png",                   c_key_image_line_2,                 Asset_Category::Basic);
    this->addImage(":/assets/dr_images/basic/cross_1.png",                  c_key_image_cross_1,                Asset_Category::Basic);
    this->addImage(":/assets/dr_images/basic/cross_2.png",                  c_key_image_cross_2,                Asset_Category::Basic);
    this->addImage(":/assets/dr_images/basic/cross_3.png",                  c_key_image_cross_3,                Asset_Category::Basic);
    this->addImage(":/assets/dr_images/basic/cross_4.png",                  c_key_image_cross_4,                Asset_Category::Basic);
    this->addImage(":/assets/dr_images/basic/cross_5.png",                  c_key_image_cross_5,                Asset_Category::Basic);
    this->addImage(":/assets/dr_images/basic/cross_6.png",                  c_key_image_cross_6,                Asset_Category::Basic);
    this->addImage(":/assets/dr_images/basic/cross_7.png",                  c_key_image_cross_7,                Asset_Category::Basic);
    this->addImage(":/assets/dr_images/basic/cross_bars.png",               c_key_image_cross_bars,             Asset_Category::Basic);
    this->addImage(":/assets/dr_images/basic/cross_flat.png",               c_key_image_cross_flat,             Asset_Category::Basic);
    this->addImage(":/assets/dr_images/basic/cross_stripes.png",            c_key_image_cross_stripes,          Asset_Category::Basic);
    this->addImage(":/assets/dr_images/basic/half_circle_1.png",            c_key_image_half_circle_1,          Asset_Category::Basic);
    this->addImage(":/assets/dr_images/basic/half_circle_2.png",            c_key_image_half_circle_2,          Asset_Category::Basic);

    //##    Built In Images - Outline       400-429
    this->addImage(":/assets/dr_images/outlines/square_outline_1.png",      c_key_image_square_outline_1,       Asset_Category::Outlines);
    this->addImage(":/assets/dr_images/outlines/square_outline_2.png",      c_key_image_square_outline_2,       Asset_Category::Outlines);
    this->addImage(":/assets/dr_images/outlines/square_outline_3.png",      c_key_image_square_outline_3,       Asset_Category::Outlines);
    this->addImage(":/assets/dr_images/outlines/rounded_outline_1.png",     c_key_image_rounded_outline_1,      Asset_Category::Outlines);
    this->addImage(":/assets/dr_images/outlines/rounded_outline_2.png",     c_key_image_rounded_outline_2,      Asset_Category::Outlines);
    this->addImage(":/assets/dr_images/outlines/rounded_outline_3.png",     c_key_image_rounded_outline_3,      Asset_Category::Outlines);
    this->addImage(":/assets/dr_images/outlines/circle_outline_1.png",      c_key_image_circle_outline_1,       Asset_Category::Outlines);
    this->addImage(":/assets/dr_images/outlines/circle_outline_2.png",      c_key_image_circle_outline_2,       Asset_Category::Outlines);
    this->addImage(":/assets/dr_images/outlines/circle_outline_3.png",      c_key_image_circle_outline_3,       Asset_Category::Outlines);
    this->addImage(":/assets/dr_images/outlines/triangle_outline_1.png",    c_key_image_triangle_outline_1,     Asset_Category::Outlines);
    this->addImage(":/assets/dr_images/outlines/triangle_outline_2.png",    c_key_image_triangle_outline_2,     Asset_Category::Outlines);
    this->addImage(":/assets/dr_images/outlines/triangle_outline_3.png",    c_key_image_triangle_outline_3,     Asset_Category::Outlines);
    this->addImage(":/assets/dr_images/outlines/diamond_outline_1.png",     c_key_image_diamond_outline_1,      Asset_Category::Outlines);
    this->addImage(":/assets/dr_images/outlines/diamond_outline_2.png",     c_key_image_diamond_outline_2,      Asset_Category::Outlines);
    this->addImage(":/assets/dr_images/outlines/cross_outline_1.png",       c_key_image_cross_outline_1,        Asset_Category::Outlines);
    this->addImage(":/assets/dr_images/outlines/cross_outline_2.png",       c_key_image_cross_outline_2,        Asset_Category::Outlines);

    //##    Built In Images - Gradient      430-469
    this->addImage(":/assets/dr_images/gradient/circle_linear_1.png",       c_key_image_circle_linear_1,        Asset_Category::Gradient);
    this->addImage(":/assets/dr_images/gradient/circle_linear_2.png",       c_key_image_circle_linear_2,        Asset_Category::Gradient);
    this->addImage(":/assets/dr_images/gradient/circle_linear_3.png",       c_key_image_circle_linear_3,        Asset_Category::Gradient);
    this->addImage(":/assets/dr_images/gradient/circle_radial_1.png",       c_key_image_circle_radial_1,        Asset_Category::Gradient);
    this->addImage(":/assets/dr_images/gradient/circle_radial_2.png",       c_key_image_circle_radial_2,        Asset_Category::Gradient);
    this->addImage(":/assets/dr_images/gradient/circle_radial_3.png",       c_key_image_circle_radial_3,        Asset_Category::Gradient);
    this->addImage(":/assets/dr_images/gradient/square_linear_1.png",       c_key_image_square_linear_1,        Asset_Category::Gradient);
    this->addImage(":/assets/dr_images/gradient/square_linear_2.png",       c_key_image_square_linear_2,        Asset_Category::Gradient);
    this->addImage(":/assets/dr_images/gradient/square_linear_3.png",       c_key_image_square_linear_3,        Asset_Category::Gradient);
    this->addImage(":/assets/dr_images/gradient/square_radial_1.png",       c_key_image_square_radial_1,        Asset_Category::Gradient);
    this->addImage(":/assets/dr_images/gradient/square_radial_2.png",       c_key_image_square_radial_2,        Asset_Category::Gradient);
    this->addImage(":/assets/dr_images/gradient/square_radial_3.png",       c_key_image_square_radial_3,        Asset_Category::Gradient);
    this->addImage(":/assets/dr_images/gradient/triangle_linear_1.png",     c_key_image_triangle_linear_1,      Asset_Category::Gradient);
    this->addImage(":/assets/dr_images/gradient/triangle_linear_2.png",     c_key_image_triangle_linear_2,      Asset_Category::Gradient);
    this->addImage(":/assets/dr_images/gradient/triangle_linear_3.png",     c_key_image_triangle_linear_3,      Asset_Category::Gradient);
    this->addImage(":/assets/dr_images/gradient/triangle_radial_1.png",     c_key_image_triangle_radial_1,      Asset_Category::Gradient);
    this->addImage(":/assets/dr_images/gradient/triangle_radial_2.png",     c_key_image_triangle_radial_2,      Asset_Category::Gradient);
    this->addImage(":/assets/dr_images/gradient/triangle_radial_3.png",     c_key_image_triangle_radial_3,      Asset_Category::Gradient);

    //##    Built In Images - Polygon       470-499
    this->addImage(":/assets/dr_images/polygons/polygon_5_sides.png",       c_key_image_polygon_5_sides,        Asset_Category::Polygons);
    this->addImage(":/assets/dr_images/polygons/polygon_6_sides.png",       c_key_image_polygon_6_sides,        Asset_Category::Polygons);
    this->addImage(":/assets/dr_images/polygons/polygon_7_sides.png",       c_key_image_polygon_7_sides,        Asset_Category::Polygons);
    this->addImage(":/assets/dr_images/polygons/polygon_8_sides.png",       c_key_image_polygon_8_sides,        Asset_Category::Polygons);
    this->addImage(":/assets/dr_images/polygons/polygon_9_sides.png",       c_key_image_polygon_9_sides,        Asset_Category::Polygons);
    this->addImage(":/assets/dr_images/polygons/polygon_10_sides.png",      c_key_image_polygon_10_sides,       Asset_Category::Polygons);
    this->addImage(":/assets/dr_images/polygons/star_6_points.png",         c_key_image_star_6_points,          Asset_Category::Polygons);
    this->addImage(":/assets/dr_images/polygons/star_12_points.png",        c_key_image_star_12_points,         Asset_Category::Polygons);
    this->addImage(":/assets/dr_images/polygons/star_24_points.png",        c_key_image_star_24_points,         Asset_Category::Polygons);

    //##    Built In Images - Isometric      500-549
    this->addImage(":/assets/dr_images/isometric/isometric_cube_1.png",     c_key_image_isometric_cube_1,       Asset_Category::Isometric);
    this->addImage(":/assets/dr_images/isometric/isometric_cube_2.png",     c_key_image_isometric_cube_2,       Asset_Category::Isometric);
    this->addImage(":/assets/dr_images/isometric/isometric_cube_3.png",     c_key_image_isometric_cube_3,       Asset_Category::Isometric);
    this->addImage(":/assets/dr_images/isometric/isometric_cube_grass.png", c_key_image_isometric_cube_grass,   Asset_Category::Isometric);
    this->addImage(":/assets/dr_images/isometric/isometric_cube_ice.png",   c_key_image_isometric_cube_ice,     Asset_Category::Isometric);
    this->addImage(":/assets/dr_images/isometric/isometric_pyramid_1.png",  c_key_image_isometric_pyramid_1,    Asset_Category::Isometric);
    this->addImage(":/assets/dr_images/isometric/isometric_pyramid_2.png",  c_key_image_isometric_pyramid_2,    Asset_Category::Isometric);
    this->addImage(":/assets/dr_images/isometric/isometric_pyramid_3.png",  c_key_image_isometric_pyramid_3,    Asset_Category::Isometric);
    this->addImage(":/assets/dr_images/isometric/isometric_sphere_1.png",   c_key_image_isometric_sphere_1,     Asset_Category::Isometric);
    this->addImage(":/assets/dr_images/isometric/isometric_sphere_2.png",   c_key_image_isometric_sphere_2,     Asset_Category::Isometric);
    this->addImage(":/assets/dr_images/isometric/isometric_diamond_1.png",  c_key_image_isometric_diamond_1,    Asset_Category::Isometric);
    this->addImage(":/assets/dr_images/isometric/isometric_spikes.png",     c_key_image_isometric_spikes,       Asset_Category::Isometric);
    this->addImage(":/assets/dr_images/isometric/isometric_star.png",       c_key_image_isometric_star,         Asset_Category::Isometric);
    this->addImage(":/assets/dr_images/isometric/isometric_column_1.png",   c_key_image_isometric_column_1,     Asset_Category::Isometric);
    this->addImage(":/assets/dr_images/isometric/isometric_column_2.png",   c_key_image_isometric_column_2,     Asset_Category::Isometric);
    this->addImage(":/assets/dr_images/isometric/isometric_flower_1.png",   c_key_image_isometric_flower_1,     Asset_Category::Isometric);
    this->addImage(":/assets/dr_images/isometric/isometric_mountain.png",   c_key_image_isometric_mountain,     Asset_Category::Isometric);
    this->addImage(":/assets/dr_images/isometric/isometric_mushroom.png",   c_key_image_isometric_mushroom,     Asset_Category::Isometric);
    this->addImage(":/assets/dr_images/isometric/isometric_rock_1.png",     c_key_image_isometric_rock_1,       Asset_Category::Isometric);
    this->addImage(":/assets/dr_images/isometric/isometric_rock_2.png",     c_key_image_isometric_rock_2,       Asset_Category::Isometric);
    this->addImage(":/assets/dr_images/isometric/isometric_rock_3.png",     c_key_image_isometric_rock_3,       Asset_Category::Isometric);
    this->addImage(":/assets/dr_images/isometric/isometric_tree_1.png",     c_key_image_isometric_tree_1,       Asset_Category::Isometric);
    this->addImage(":/assets/dr_images/isometric/isometric_tree_2.png",     c_key_image_isometric_tree_2,       Asset_Category::Isometric);

    //##    Built In Images - Shapes        550-599
    this->addImage(":/assets/dr_images/shapes/star_1.png",                  c_key_image_star_1,                 Asset_Category::Shapes);
    this->addImage(":/assets/dr_images/shapes/star_2.png",                  c_key_image_star_2,                 Asset_Category::Shapes);
    this->addImage(":/assets/dr_images/shapes/star_3.png",                  c_key_image_star_3,                 Asset_Category::Shapes);
    this->addImage(":/assets/dr_images/shapes/star_4.png",                  c_key_image_star_4,                 Asset_Category::Shapes);
    this->addImage(":/assets/dr_images/shapes/heart_1.png",                 c_key_image_heart_1,                Asset_Category::Shapes);
    this->addImage(":/assets/dr_images/shapes/heart_2.png",                 c_key_image_heart_2,                Asset_Category::Shapes);
    this->addImage(":/assets/dr_images/shapes/heart_half.png",              c_key_image_heart_half,             Asset_Category::Shapes);
    this->addImage(":/assets/dr_images/shapes/pie.png",                     c_key_image_pie,                    Asset_Category::Shapes);
    this->addImage(":/assets/dr_images/shapes/ghost.png",                   c_key_image_ghost,                  Asset_Category::Shapes);
    this->addImage(":/assets/dr_images/shapes/gear_1.png",                  c_key_image_gear_1,                 Asset_Category::Shapes);
    this->addImage(":/assets/dr_images/shapes/gear_2.png",                  c_key_image_gear_2,                 Asset_Category::Shapes);
    this->addImage(":/assets/dr_images/shapes/thought_bubble_1.png",        c_key_image_thought_bubble_1,       Asset_Category::Shapes);
    this->addImage(":/assets/dr_images/shapes/thought_bubble_2.png",        c_key_image_thought_bubble_2,       Asset_Category::Shapes);
    this->addImage(":/assets/dr_images/shapes/thought_bubble_3.png",        c_key_image_thought_bubble_3,       Asset_Category::Shapes);
    this->addImage(":/assets/dr_images/shapes/exclamation_point.png",       c_key_image_exclamation_point,      Asset_Category::Shapes);
    this->addImage(":/assets/dr_images/shapes/question_mark.png",           c_key_image_question_mark,          Asset_Category::Shapes);
    this->addImage(":/assets/dr_images/shapes/arrow_1.png",                 c_key_image_arrow_1,                Asset_Category::Shapes);
    this->addImage(":/assets/dr_images/shapes/arrow_2.png",                 c_key_image_arrow_2,                Asset_Category::Shapes);
    this->addImage(":/assets/dr_images/shapes/arrow_3.png",                 c_key_image_arrow_3,                Asset_Category::Shapes);
    this->addImage(":/assets/dr_images/shapes/reset.png",                   c_key_image_reset,                  Asset_Category::Shapes);
    this->addImage(":/assets/dr_images/shapes/fish.png",                    c_key_image_fish,                   Asset_Category::Shapes);

    //##    Built In Images - Ground        600-699
    this->addImage(":/assets/dr_images/ground/ground_top_1.png",            c_key_image_ground_top_1,           Asset_Category::Ground);
    this->addImage(":/assets/dr_images/ground/ground_top_2.png",            c_key_image_ground_top_2,           Asset_Category::Ground);
    this->addImage(":/assets/dr_images/ground/ground_top_3.png",            c_key_image_ground_top_3,           Asset_Category::Ground);
    this->addImage(":/assets/dr_images/ground/ground_top_4.png",            c_key_image_ground_top_4,           Asset_Category::Ground);
    this->addImage(":/assets/dr_images/ground/ground_top_5.png",            c_key_image_ground_top_5,           Asset_Category::Ground);
    this->addImage(":/assets/dr_images/ground/ground_top_6.png",            c_key_image_ground_top_6,           Asset_Category::Ground);
    this->addImage(":/assets/dr_images/ground/ground_top_7.png",            c_key_image_ground_top_7,           Asset_Category::Ground);
    this->addImage(":/assets/dr_images/ground/ground_top_8.png",            c_key_image_ground_top_8,           Asset_Category::Ground);
    this->addImage(":/assets/dr_images/ground/ground_top_left.png",         c_key_image_ground_top_left,        Asset_Category::Ground);
    this->addImage(":/assets/dr_images/ground/ground_top_right.png",        c_key_image_ground_top_right,       Asset_Category::Ground);
    this->addImage(":/assets/dr_images/ground/ground_top_up_left_1.png",    c_key_image_ground_top_up_left_1,   Asset_Category::Ground);
    this->addImage(":/assets/dr_images/ground/ground_top_up_left_2.png",    c_key_image_ground_top_up_left_2,   Asset_Category::Ground);
    this->addImage(":/assets/dr_images/ground/ground_top_up_right_1.png",   c_key_image_ground_top_up_right_1,  Asset_Category::Ground);
    this->addImage(":/assets/dr_images/ground/ground_top_up_right_2.png",   c_key_image_ground_top_up_right_2,  Asset_Category::Ground);
    this->addImage(":/assets/dr_images/ground/block_1.png",                 c_key_image_block_1,                Asset_Category::Ground);
    this->addImage(":/assets/dr_images/ground/block_2.png",                 c_key_image_block_2,                Asset_Category::Ground);
    this->addImage(":/assets/dr_images/ground/block_lock.png",              c_key_image_block_lock,             Asset_Category::Ground);
    this->addImage(":/assets/dr_images/ground/box_1.png",                   c_key_image_box_1,                  Asset_Category::Ground);
    this->addImage(":/assets/dr_images/ground/box_2.png",                   c_key_image_box_2,                  Asset_Category::Ground);
    this->addImage(":/assets/dr_images/ground/bricks.png",                  c_key_image_bricks,                 Asset_Category::Ground);
    this->addImage(":/assets/dr_images/ground/dirt.png",                    c_key_image_dirt,                   Asset_Category::Ground);
    this->addImage(":/assets/dr_images/ground/ice.png",                     c_key_image_ice,                    Asset_Category::Ground);
    this->addImage(":/assets/dr_images/ground/sand.png",                    c_key_image_sand,                   Asset_Category::Ground);
    this->addImage(":/assets/dr_images/ground/stone.png",                   c_key_image_stone,                  Asset_Category::Ground);
    this->addImage(":/assets/dr_images/ground/tile.png",                    c_key_image_tile,                   Asset_Category::Ground);
    this->addImage(":/assets/dr_images/ground/water.png",                   c_key_image_water,                  Asset_Category::Ground);
    this->addImage(":/assets/dr_images/ground/water_top.png",               c_key_image_water_top,              Asset_Category::Ground);
    this->addImage(":/assets/dr_images/ground/wood.png",                    c_key_image_wood,                   Asset_Category::Ground);
    this->addImage(":/assets/dr_images/ground/wall_1.png",                  c_key_image_wall_1,                 Asset_Category::Ground);
    this->addImage(":/assets/dr_images/ground/wall_2.png",                  c_key_image_wall_2,                 Asset_Category::Ground);
    this->addImage(":/assets/dr_images/ground/window.png",                  c_key_image_window,                 Asset_Category::Ground);
    this->addImage(":/assets/dr_images/ground/bridge_left.png",             c_key_image_bridge_left,            Asset_Category::Ground);
    this->addImage(":/assets/dr_images/ground/bridge_mid.png",              c_key_image_bridge_mid,             Asset_Category::Ground);
    this->addImage(":/assets/dr_images/ground/bridge_right.png",            c_key_image_bridge_right,           Asset_Category::Ground);
    this->addImage(":/assets/dr_images/ground/platform_left.png",           c_key_image_platform_left,          Asset_Category::Ground);
    this->addImage(":/assets/dr_images/ground/platform_mid.png",            c_key_image_platform_mid,           Asset_Category::Ground);
    this->addImage(":/assets/dr_images/ground/platform_right.png",          c_key_image_platform_right,         Asset_Category::Ground);
    this->addImage(":/assets/dr_images/ground/logs.png",                    c_key_image_logs,                   Asset_Category::Ground);

    //##    Built In Images - Decoration    700-749
    this->addImage(":/assets/dr_images/decoration/bush_1.png",              c_key_image_bush_1,                 Asset_Category::Decoration);
    this->addImage(":/assets/dr_images/decoration/bush_2.png",              c_key_image_bush_2,                 Asset_Category::Decoration);
    this->addImage(":/assets/dr_images/decoration/bush_3.png",              c_key_image_bush_3,                 Asset_Category::Decoration);
    this->addImage(":/assets/dr_images/decoration/cloud_1.png",             c_key_image_cloud_1,                Asset_Category::Decoration);
    this->addImage(":/assets/dr_images/decoration/cloud_2.png",             c_key_image_cloud_2,                Asset_Category::Decoration);
    this->addImage(":/assets/dr_images/decoration/flower_1.png",            c_key_image_flower_1,               Asset_Category::Decoration);
    this->addImage(":/assets/dr_images/decoration/grass_1.png",             c_key_image_grass_1,                Asset_Category::Decoration);
    this->addImage(":/assets/dr_images/decoration/grass_2.png",             c_key_image_grass_2,                Asset_Category::Decoration);
    this->addImage(":/assets/dr_images/decoration/grass_3.png",             c_key_image_grass_3,                Asset_Category::Decoration);
    this->addImage(":/assets/dr_images/decoration/ladder_1.png",            c_key_image_ladder_1,               Asset_Category::Decoration);
    this->addImage(":/assets/dr_images/decoration/ladder_2.png",            c_key_image_ladder_2,               Asset_Category::Decoration);
    this->addImage(":/assets/dr_images/decoration/ladder_top.png",          c_key_image_ladder_top,             Asset_Category::Decoration);
    this->addImage(":/assets/dr_images/decoration/mushroom.png",            c_key_image_mushroom,               Asset_Category::Decoration);
    this->addImage(":/assets/dr_images/decoration/rock_1.png",              c_key_image_rock_1,                 Asset_Category::Decoration);
    this->addImage(":/assets/dr_images/decoration/rock_2.png",              c_key_image_rock_2,                 Asset_Category::Decoration);
    this->addImage(":/assets/dr_images/decoration/sign_1.png",              c_key_image_sign_1,                 Asset_Category::Decoration);
    this->addImage(":/assets/dr_images/decoration/sign_2.png",              c_key_image_sign_2,                 Asset_Category::Decoration);
    this->addImage(":/assets/dr_images/decoration/sign_post.png",           c_key_image_sign_post,              Asset_Category::Decoration);
    this->addImage(":/assets/dr_images/decoration/sign_post_top.png",       c_key_image_sign_post_top,          Asset_Category::Decoration);
    this->addImage(":/assets/dr_images/decoration/sun.png",                 c_key_image_sun,                    Asset_Category::Decoration);


}






















