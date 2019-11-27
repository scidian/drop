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


    // ***** Add Default Assets
    addDefaultAssets();


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
    this->addImage(c_key_asset_empty,       path_empty,         "empty.png",        "empty",            image_empty);
    this->addImage(c_key_asset_character,   path_character,     "circle.png",       "ball",             image_character);
    this->addImage(c_key_asset_object,      path_object,        "box.png",          "block",            image_object);

    this->addImage(":/assets/dr_images/square_1.png",               c_key_asset_square_1);
    this->addImage(":/assets/dr_images/square_2.png",               c_key_asset_square_2);
    this->addImage(":/assets/dr_images/square_3.png",               c_key_asset_square_3);
    this->addImage(":/assets/dr_images/square_4.png",               c_key_asset_square_4);
    this->addImage(":/assets/dr_images/square_5.png",               c_key_asset_square_5);
    this->addImage(":/assets/dr_images/square_6.png",               c_key_asset_square_6);
    this->addImage(":/assets/dr_images/square_7.png",               c_key_asset_square_7);
    this->addImage(":/assets/dr_images/square_8.png",               c_key_asset_square_8);
    this->addImage(":/assets/dr_images/square_9.png",               c_key_asset_square_9);

    this->addImage(":/assets/dr_images/rounded_1.png",              c_key_asset_rounded_1);
    this->addImage(":/assets/dr_images/rounded_2.png",              c_key_asset_rounded_2);
    this->addImage(":/assets/dr_images/rounded_3.png",              c_key_asset_rounded_3);
    this->addImage(":/assets/dr_images/rounded_4.png",              c_key_asset_rounded_4);
    this->addImage(":/assets/dr_images/rounded_5.png",              c_key_asset_rounded_5);
    this->addImage(":/assets/dr_images/rounded_6.png",              c_key_asset_rounded_6);
    this->addImage(":/assets/dr_images/rounded_7.png",              c_key_asset_rounded_7);

    this->addImage(":/assets/dr_images/circle_1.png",               c_key_asset_circle_1);
    this->addImage(":/assets/dr_images/circle_2.png",               c_key_asset_circle_2);
    this->addImage(":/assets/dr_images/circle_3.png",               c_key_asset_circle_3);
    this->addImage(":/assets/dr_images/circle_4.png",               c_key_asset_circle_4);
    this->addImage(":/assets/dr_images/circle_5.png",               c_key_asset_circle_5);
    this->addImage(":/assets/dr_images/circle_6.png",               c_key_asset_circle_6);
    this->addImage(":/assets/dr_images/circle_7.png",               c_key_asset_circle_7);
    this->addImage(":/assets/dr_images/circle_8.png",               c_key_asset_circle_8);
    this->addImage(":/assets/dr_images/circle_9.png",               c_key_asset_circle_9);

    this->addImage(":/assets/dr_images/gradient_circle_1.png",      c_key_asset_gradient_circle_1);
    this->addImage(":/assets/dr_images/gradient_circle_2.png",      c_key_asset_gradient_circle_2);
    this->addImage(":/assets/dr_images/gradient_circle_3.png",      c_key_asset_gradient_circle_3);
    this->addImage(":/assets/dr_images/gradient_circle_4.png",      c_key_asset_gradient_circle_4);
    this->addImage(":/assets/dr_images/gradient_circle_5.png",      c_key_asset_gradient_circle_5);
    this->addImage(":/assets/dr_images/gradient_circle_6.png",      c_key_asset_gradient_circle_6);

    this->addImage(":/assets/dr_images/gradient_square_1.png",      c_key_asset_gradient_square_1);
    this->addImage(":/assets/dr_images/gradient_square_2.png",      c_key_asset_gradient_square_2);
    this->addImage(":/assets/dr_images/gradient_square_3.png",      c_key_asset_gradient_square_3);
    this->addImage(":/assets/dr_images/gradient_square_4.png",      c_key_asset_gradient_square_4);
    this->addImage(":/assets/dr_images/gradient_square_5.png",      c_key_asset_gradient_square_5);
    this->addImage(":/assets/dr_images/gradient_square_6.png",      c_key_asset_gradient_square_6);
    this->addImage(":/assets/dr_images/gradient_square_7.png",      c_key_asset_gradient_square_7);
    this->addImage(":/assets/dr_images/gradient_square_8.png",      c_key_asset_gradient_square_8);

    this->addImage(":/assets/dr_images/ground_1.png",               c_key_asset_ground_1);
    this->addImage(":/assets/dr_images/ground_2.png",               c_key_asset_ground_2);
    this->addImage(":/assets/dr_images/ground_3.png",               c_key_asset_ground_3);
    this->addImage(":/assets/dr_images/ground_4.png",               c_key_asset_ground_4);
    this->addImage(":/assets/dr_images/ground_5.png",               c_key_asset_ground_5);
    this->addImage(":/assets/dr_images/ground_6.png",               c_key_asset_ground_6);
    this->addImage(":/assets/dr_images/ground_7.png",               c_key_asset_ground_7);

    this->addImage(":/assets/dr_images/frame_square_1.png",         c_key_asset_frame_square_1);
    this->addImage(":/assets/dr_images/frame_square_2.png",         c_key_asset_frame_square_2);
    this->addImage(":/assets/dr_images/frame_square_3.png",         c_key_asset_frame_square_3);
    this->addImage(":/assets/dr_images/frame_circle_1.png",         c_key_asset_frame_circle_1);
    this->addImage(":/assets/dr_images/frame_circle_2.png",         c_key_asset_frame_circle_2);
    this->addImage(":/assets/dr_images/frame_circle_3.png",         c_key_asset_frame_circle_3);
    this->addImage(":/assets/dr_images/frame_rounded_1.png",        c_key_asset_frame_rounded_1);
    this->addImage(":/assets/dr_images/frame_triangle_1.png",       c_key_asset_frame_triangle_1);
    this->addImage(":/assets/dr_images/frame_triangle_2.png",       c_key_asset_frame_triangle_2);

    this->addImage(":/assets/dr_images/triangle_1.png",             c_key_asset_triangle_1);
    this->addImage(":/assets/dr_images/triangle_2.png",             c_key_asset_triangle_2);
    this->addImage(":/assets/dr_images/triangle_3.png",             c_key_asset_triangle_3);
    this->addImage(":/assets/dr_images/gradient_triangle_1.png",    c_key_asset_gradient_triangle_1);
    this->addImage(":/assets/dr_images/gradient_triangle_2.png",    c_key_asset_gradient_triangle_2);
    this->addImage(":/assets/dr_images/gradient_triangle_3.png",    c_key_asset_gradient_triangle_3);

    this->addImage(":/assets/dr_images/flat_square.png",            c_key_asset_flat_square);
    this->addImage(":/assets/dr_images/flat_circle.png",            c_key_asset_flat_circle);
    this->addImage(":/assets/dr_images/flat_triangle.png",          c_key_asset_flat_triangle);


    this->addImage(":/assets/dr_images/isometric_cube_1.png",       c_key_asset_isometric_cube_1);
    this->addImage(":/assets/dr_images/isometric_cube_2.png",       c_key_asset_isometric_cube_2);
    this->addImage(":/assets/dr_images/isometric_cube_3.png",       c_key_asset_isometric_cube_3);
    this->addImage(":/assets/dr_images/isometric_pyramid_1.png",    c_key_asset_isometric_pyramid_1);
    this->addImage(":/assets/dr_images/isometric_pyramid_1.png",    c_key_asset_isometric_pyramid_2);
    this->addImage(":/assets/dr_images/isometric_sphere_2.png",     c_key_asset_isometric_sphere_1);
    this->addImage(":/assets/dr_images/isometric_sphere_2.png",     c_key_asset_isometric_sphere_2);




}






















