//
//      Created by Stephens Nunnally on 12/18/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "editor/helper_library.h"
#include "editor/project/project.h"
#include "project/entities/dr_asset.h"
#include "project/entities/dr_image.h"
#include "project/entities/dr_stage.h"
#include "project/entities/dr_world.h"

namespace Dr {



//####################################################################################
//##    Creates skeleton of a new Project
//####################################################################################
void InitializeNewProject(DrProject *project, std::string project_name, Orientation orientation, int width, int height, bool test) {

    // ***** Important! Clear Project / Reset Key Generator
    project->clearProject();                                            // Also adds Devices / Effects
    Dr::AddBuiltInImages(project);                                      // Adds Embedded Images

    // Set if Project is debugging test file or not (test files dont ask to be saved when closed)
    project->setTestOnly(test);

    // ***** Set Project Options
    project->setOption(Project_Options::Name,           project_name);
    project->setOption(Project_Options::File_Name_Path, std::string(""));
    project->setOption(Project_Options::Orientation,    static_cast<int>(orientation));
    project->setOption(Project_Options::Width,          width);
    project->setOption(Project_Options::Height,         height);


    // ***** Add Initial World
    DrWorld *world_1 = project->addWorld();
    DrWorld *current_world = world_1;
    project->setOption(Project_Options::Current_World, world_1->getKey());
    project->setOption(Project_Options::Current_Stage, current_world->getFirstStageKey());


    // ***** Build Test Project
    if (test) {
        project->setOption(Project_Options::Name, std::string("Rocky Rover"));

        DrImage *image_1  = Dr::AddImage(project, ":/assets/test_images/test_square.png");
        DrImage *image_2  = Dr::AddImage(project, ":/assets/test_images/ground_fill.png");
        DrImage *image_3  = Dr::AddImage(project, ":/assets/test_images/ground_top.png");
        DrImage *image_4  = Dr::AddImage(project, ":/assets/test_images/moon_plant_6.png");
        DrImage *image_5  = Dr::AddImage(project, ":/assets/test_images/rover_body.png");
        DrImage *image_6  = Dr::AddImage(project, ":/assets/test_images/ball_1.png");
        DrImage *image_7  = Dr::AddImage(project, ":/assets/test_images/water_1.png");
        DrImage *image_8  = Dr::AddImage(project, ":/assets/test_images/rover_wheel.png");
        DrImage *image_9  = Dr::AddImage(project, ":/assets/test_images/cake_block.png");
        DrImage *image_10 = Dr::AddImage(project, ":/assets/test_images/cake_chocolate.png");
        DrImage *image_11 = Dr::AddImage(project, ":/assets/test_images/cake_ice_cube.png");
        DrImage *image_12 = Dr::AddImage(project, ":/assets/test_images/planetwithareallylongname.png");
        DrImage *image_13 = Dr::AddImage(project, ":/assets/test_images/metal_block.png");
        DrImage *image_14 = Dr::AddImage(project, ":/assets/test_images/donut.png");
        DrImage *image_15 = Dr::AddImage(project, ":/assets/test_images/shapes2.png");
        DrImage *image_16 = Dr::AddImage(project, ":/assets/test_images/dragon.png");

        long font_1 =   project->addFont("Distance Font", Dr::FromQPixmap(QPixmap(":/assets/test_images/test_font.png")),   "Arial",          20, true);
                        project->addFont("Coin Count",    Dr::FromQPixmap(QPixmap(":/assets/test_images/test_font_2.png")), "Britannic Bold", 15, true);
                        project->addFont("I Love Julie",  Dr::FromQPixmap(QPixmap(":/assets/test_images/test_font_3.png")), "Bauhaus 93",     36, true);

                                project->addAsset(DrAssetType::Character, image_6->getKey() );          // "Ball 1"
        DrAsset *a_dragon =     project->addAsset(DrAssetType::Character, image_16->getKey() );         // "Dragon"

        DrAsset *a_square =     project->addAsset(DrAssetType::Object, image_1->getKey() );             // "Dr Square"
        DrAsset *a_ground =     project->addAsset(DrAssetType::Object, image_2->getKey() );             // "Ground Fill"
        DrAsset *a_top =        project->addAsset(DrAssetType::Object, image_3->getKey() );             // "Ground Top"
        DrAsset *a_plant =      project->addAsset(DrAssetType::Object, image_4->getKey() );             // "Moon Plant 6"
        DrAsset *a_block =      project->addAsset(DrAssetType::Object, image_13->getKey() );            // "Metal Block"
        DrAsset *a_planet =     project->addAsset(DrAssetType::Object, image_12->getKey() );            // "PlanetWithAReallyLongName"

        DrAsset *a_cake1 =      project->addAsset(DrAssetType::Object, image_9->getKey() );             // "Cake Block"
        DrAsset *a_cake2 =      project->addAsset(DrAssetType::Object, image_10->getKey() );            // "Cake Chocolate"
        DrAsset *a_cake3 =      project->addAsset(DrAssetType::Object, image_11->getKey() );            // "Cake Ice Cube"

        project->addAsset(DrAssetType::Object, image_7->getKey() );                                     // "Water 1"
        project->addAsset(DrAssetType::Object, image_5->getKey() );                                     // "Rover Body"
        project->addAsset(DrAssetType::Object, image_8->getKey() );                                     // "Rover Wheel"
        project->addAsset(DrAssetType::Object, image_14->getKey() );                                    // "Donut"
        project->addAsset(DrAssetType::Object, image_15->getKey() );                                    // "Shapes 2"


        // Add some Worlds and Stages
        project->addWorld();                                                                            // "World 2"
        project->findWorldWithName("World 2")->addStage();                                              // Stage 2
        project->findWorldWithName("World 2")->addStage("asdfasdfasdfasdfasdfasdfasd");                 // Stage 3
        project->findWorldWithName("World 2")->addStage();                                              // Stage 4


        // Stage 4 Rover Test
        long start_stage = project->findWorldWithName("World 2")->getFirstStageKey();
        project->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Character, a_dragon->getKey(),  0,    80,   5);

        project->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_planet->getKey(),   200,   600,   0);
        project->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_planet->getKey(),  1200,   475,   0);

        project->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_plant->getKey(),    900,    55,   5, false);
        project->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_plant->getKey(),   1400,    55,   5, false);

        project->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_ground->getKey(),  -411,  -200,   0);
        project->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_ground->getKey(),     0,  -200,   0);
        project->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_ground->getKey(),   411,  -200,   0);
        project->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_ground->getKey(),   822,  -200,   0);
        project->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_ground->getKey(),  1233,  -200,   0);

        project->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_top->getKey(),     -411,   -50,   0);
        project->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_top->getKey(),        0,   -50,   0);
        project->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_top->getKey(),      411,   -50,   0);
        project->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_top->getKey(),      822,   -50,   0);
        project->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_top->getKey(),     1233,   -50,   0);

        project->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_block->getKey(),    400,   200,   0);
        project->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_block->getKey(),    550,   200,   0);
        project->findWorldWithName("World 2")->getStageFromKey(start_stage)->addThing(DrThingType::Object, a_block->getKey(),    750,   350,   0);

        // Stage 2 Misc Test
        project->findWorldWithName("World 2")->getStageWithName("2")->addThing(DrThingType::Object, a_square->getKey(),   600,   700,  0);
        project->findWorldWithName("World 2")->getStageWithName("2")->addThing(DrThingType::Text,   font_1,               500,   900,  0);

        project->findWorldWithName("World 2")->getStageWithName("2")->addThing(DrThingType::Object, a_ground->getKey(),   200,  -200,  0);
        project->findWorldWithName("World 2")->getStageWithName("2")->addThing(DrThingType::Object, a_ground->getKey(),   600,  -200,  0);
        project->findWorldWithName("World 2")->getStageWithName("2")->addThing(DrThingType::Object, a_ground->getKey(),  1000,  -200,  0);

        // Stage 3 Ground
        project->findWorldWithName("World 2")->getStageWithName("asdfasdfasdfasdfasdfasdfasd")->addThing(DrThingType::Object, a_ground->getKey(),   200,  -200,  0);
        project->findWorldWithName("World 2")->getStageWithName("asdfasdfasdfasdfasdfasdfasd")->addThing(DrThingType::Object, a_ground->getKey(),   600,  -200,  0);
        project->findWorldWithName("World 2")->getStageWithName("asdfasdfasdfasdfasdfasdfasd")->addThing(DrThingType::Object, a_ground->getKey(),  1000,  -200,  0);

        // Stage 4 Iso Cake Test
        project->findWorldWithName("World 2")->getStageWithName("4")->addThing(DrThingType::Object, a_cake1->getKey(),    200,   500,  0);
        project->findWorldWithName("World 2")->getStageWithName("4")->addThing(DrThingType::Object, a_cake2->getKey(),    400,   800,  0);
        project->findWorldWithName("World 2")->getStageWithName("4")->addThing(DrThingType::Object, a_cake3->getKey(),    600,  1100,  0);

        project->findWorldWithName("World 2")->getStageWithName("4")->addThing(DrThingType::Object, a_ground->getKey(),   200,  -200,  0);
        project->findWorldWithName("World 2")->getStageWithName("4")->addThing(DrThingType::Object, a_ground->getKey(),   600,  -200,  0);
        project->findWorldWithName("World 2")->getStageWithName("4")->addThing(DrThingType::Object, a_ground->getKey(),  1000,  -200,  0);

        DrStage* current_stage = project->findWorldWithName("World 2")->getStageFromKey(start_stage);
        project->setOption(Project_Options::Current_Stage, current_stage->getKey());
        project->setOption(Project_Options::Current_World, current_stage->getParentWorld()->getKey());
    }   // End Test Project


    // ***** Important! Signify we don't need to save at this point!
    project->setHasSaved(true);
}





}   // namespace Dr















