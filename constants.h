//
//      Created by Stephens Nunnally on 2/15/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Some Constant Project Data
//
//
#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QColor>
#include <QCursor>
#include <QPixmap>
#include <QString>


//####################################################################################
//##    Project Key Constants
//##        Starting Key
//####################################################################################

constexpr long c_key_starting_number =              1001;       // First 1001 keys are reserved for built in Entities in DrProject
                                                                // ....."1001" is an easily searchable number

//####################################################################################
//##    Basic Images                    001-099
//############################
constexpr long c_key_image_empty =                  1;          //  32 x  32    Alpha value 0 square
constexpr long c_key_image_character =              2;          // 256 x 256    Gray Circle
constexpr long c_key_image_object =                 3;          // 256 x 256    Gray Box


//####################################################################################
//##    Devices                         100-199
//############################
constexpr long c_key_device_camera =                100;        // Camera Device


//####################################################################################
//##    Effects                         200-299
//############################
constexpr long c_key_effect_light =                 200;        // Light Effect
constexpr long c_key_effect_water =                 201;        // Water Effect
constexpr long c_key_effect_fire =                  202;        // Fire Effect
constexpr long c_key_effect_mirror =                203;        // Mirror Effect
constexpr long c_key_effect_fisheye =               204;        // Fisheye Effect
constexpr long c_key_effect_swirl =                 205;        // Swirl Effect


//####################################################################################
//##    Built In Images - Basic         300-399
//############################
constexpr long c_key_image_square_1 =               301;         // 128 x 128    Square 1
constexpr long c_key_image_square_2 =               302;         // 128 x 128    Square 2
constexpr long c_key_image_square_3 =               303;         // 128 x 128    Square 3
constexpr long c_key_image_square_4 =               304;         // 128 x 128    Square 4
constexpr long c_key_image_square_5 =               305;         // 128 x 128    Square 5
constexpr long c_key_image_square_6 =               306;         // 128 x 128    Square 6
constexpr long c_key_image_square_7 =               307;         // 128 x 128    Square 7
constexpr long c_key_image_square_bars =            308;         // 128 x 128    Square Bars
constexpr long c_key_image_square_flat =            309;         // 128 x 128    Square Flat
constexpr long c_key_image_square_stripes =         310;         // 128 x 128    Square Stripes
constexpr long c_key_image_rounded_1 =              311;         // 128 x 128    Rounded Square 1
constexpr long c_key_image_rounded_2 =              312;         // 128 x 128    Rounded Square 2
constexpr long c_key_image_rounded_3 =              313;         // 128 x 128    Rounded Square 3
constexpr long c_key_image_rounded_4 =              314;         // 128 x 128    Rounded Square 4
constexpr long c_key_image_rounded_5 =              315;         // 128 x 128    Rounded Square 5
constexpr long c_key_image_rounded_6 =              316;         // 128 x 128    Rounded Square 6
constexpr long c_key_image_rounded_7 =              317;         // 128 x 128    Rounded Square 7
constexpr long c_key_image_rounded_bars =           318;         // 128 x 128    Rounded Square Bars
constexpr long c_key_image_rounded_flat =           319;         // 128 x 128    Rounded Square Flat
constexpr long c_key_image_rounded_stripes =        320;         // 128 x 128    Rounded Square Stripes
constexpr long c_key_image_circle_1 =               321;         // 128 x 128    Circle 1
constexpr long c_key_image_circle_2 =               322;         // 128 x 128    Circle 2
constexpr long c_key_image_circle_3 =               323;         // 128 x 128    Circle 3
constexpr long c_key_image_circle_4 =               324;         // 128 x 128    Circle 4
constexpr long c_key_image_circle_5 =               325;         // 128 x 128    Circle 5
constexpr long c_key_image_circle_6 =               326;         // 128 x 128    Circle 6
constexpr long c_key_image_circle_7 =               327;         // 128 x 128    Circle 7
constexpr long c_key_image_circle_bars =            328;         // 128 x 128    Circle Bars
constexpr long c_key_image_circle_flat =            329;         // 128 x 128    Circle Flat
constexpr long c_key_image_circle_stripes =         330;         // 128 x 128    Circle Stripes
constexpr long c_key_image_triangle_1 =             331;         // 128 x 128    Triangle 1
constexpr long c_key_image_triangle_2 =             332;         // 128 x 128    Triangle 2
constexpr long c_key_image_triangle_3 =             333;         // 128 x 128    Triangle 3
constexpr long c_key_image_triangle_4 =             334;         // 128 x 128    Triangle 4
constexpr long c_key_image_triangle_5 =             335;         // 128 x 128    Triangle 5
constexpr long c_key_image_triangle_6 =             336;         // 128 x 128    Triangle 6
constexpr long c_key_image_triangle_7 =             337;         // 128 x 128    Triangle 7
constexpr long c_key_image_triangle_bars =          338;         // 128 x 128    Triangle Bars
constexpr long c_key_image_triangle_flat =          339;         // 128 x 128    Triangle Flat
constexpr long c_key_image_triangle_stripes =       340;         // 128 x 128    Triangle Stripes
constexpr long c_key_image_diamond_1 =              341;         // 128 x 128    Diamond 1
constexpr long c_key_image_diamond_2 =              342;         // 128 x 128    Diamond 2
constexpr long c_key_image_diamond_3 =              343;         // 128 x 128    Diamond 3
constexpr long c_key_image_diamond_4 =              344;         // 128 x 128    Diamond 4
constexpr long c_key_image_diamond_5 =              345;         // 128 x 128    Diamond 5
constexpr long c_key_image_diamond_bars =           346;         // 128 x 128    Diamond Bars
constexpr long c_key_image_diamond_flat =           347;         // 128 x 128    Diamond Flat
constexpr long c_key_image_diamond_stripes =        348;         // 128 x 128    Diamond Stripes

constexpr long c_key_image_line_1 =                 351;         // 128 x 128    Line 1
constexpr long c_key_image_line_2 =                 352;         // 128 x 128    Line 2

constexpr long c_key_image_cross_1 =                361;         // 128 x 128    Cross 1
constexpr long c_key_image_cross_2 =                362;         // 128 x 128    Cross 2
constexpr long c_key_image_cross_3 =                363;         // 128 x 128    Cross 3
constexpr long c_key_image_cross_4 =                364;         // 128 x 128    Cross 4
constexpr long c_key_image_cross_5 =                365;         // 128 x 128    Cross 5
constexpr long c_key_image_cross_6 =                366;         // 128 x 128    Cross 6
constexpr long c_key_image_cross_7 =                367;         // 128 x 128    Cross 7
constexpr long c_key_image_cross_bars =             368;         // 128 x 128    Cross Bars
constexpr long c_key_image_cross_flat =             369;         // 128 x 128    Cross Flat
constexpr long c_key_image_cross_stripes =          370;         // 128 x 128    Cross Stripes
constexpr long c_key_image_half_circle_1 =          371;         // 128 x 128    Half Circle 1
constexpr long c_key_image_half_circle_2 =          372;         // 128 x 128    Half Circle 2


//####################################################################################
//##    Built In Images - Outline       400-429
//############################
constexpr long c_key_image_square_outline_1 =       401;        // 128 x 128    Square Outline 1
constexpr long c_key_image_square_outline_2 =       402;        // 128 x 128    Square Outline 2
constexpr long c_key_image_square_outline_3 =       403;        // 128 x 128    Square Outline 3
constexpr long c_key_image_rounded_outline_1 =      404;        // 128 x 128    Rounded Outline 1
constexpr long c_key_image_rounded_outline_2 =      405;        // 128 x 128    Rounded Outline 2
constexpr long c_key_image_rounded_outline_3 =      406;        // 128 x 128    Rounded Outline 3
constexpr long c_key_image_circle_outline_1 =       407;        // 128 x 128    Circle Outline 1
constexpr long c_key_image_circle_outline_2 =       408;        // 128 x 128    Circle Outline 2
constexpr long c_key_image_circle_outline_3 =       409;        // 128 x 128    Circle Outline 3
constexpr long c_key_image_triangle_outline_1 =     410;        // 128 x 128    Triangle Outline 1
constexpr long c_key_image_triangle_outline_2 =     411;        // 128 x 128    Triangle Outline 2
constexpr long c_key_image_triangle_outline_3 =     412;        // 128 x 128    Triangle Outline 3
constexpr long c_key_image_diamond_outline_1 =      413;        // 128 x 128    Diamond Outline 1
constexpr long c_key_image_diamond_outline_2 =      414;        // 128 x 128    Diamond Outline 2
constexpr long c_key_image_cross_outline_1 =        415;        // 128 x 128    Cross Outline 1
constexpr long c_key_image_cross_outline_2 =        416;        // 128 x 128    Cross Outline 2

//##    Built In Images - Gradient      430-469
constexpr long c_key_image_circle_linear_1 =        431;        // 128 x 128    Circle Linear Gradient 1
constexpr long c_key_image_circle_linear_2 =        432;        // 128 x 128    Circle Linear Gradient 2
constexpr long c_key_image_circle_linear_3 =        433;        // 128 x 128    Circle Linear Gradient 3
constexpr long c_key_image_circle_radial_1 =        434;        // 128 x 128    Circle Radial Gradient 1
constexpr long c_key_image_circle_radial_2 =        435;        // 128 x 128    Circle Radial Gradient 2
constexpr long c_key_image_circle_radial_3 =        436;        // 128 x 128    Circle Radial Gradient 3
constexpr long c_key_image_square_linear_1 =        437;        // 128 x 128    Square Linear Gradient 1
constexpr long c_key_image_square_linear_2 =        438;        // 128 x 128    Square Linear Gradient 2
constexpr long c_key_image_square_linear_3 =        439;        // 128 x 128    Square Linear Gradient 3
constexpr long c_key_image_square_radial_1 =        440;        // 128 x 128    Square Radial Gradient 1
constexpr long c_key_image_square_radial_2 =        441;        // 128 x 128    Square Radial Gradient 2
constexpr long c_key_image_square_radial_3 =        442;        // 128 x 128    Square Radial Gradient 3
constexpr long c_key_image_triangle_linear_1 =      443;        // 128 x 128    Triangle Linear Gradient 1
constexpr long c_key_image_triangle_linear_2 =      444;        // 128 x 128    Triangle Linear Gradient 2
constexpr long c_key_image_triangle_linear_3 =      445;        // 128 x 128    Triangle Linear Gradient 3
constexpr long c_key_image_triangle_radial_1 =      446;        // 128 x 128    Triangle Radial Gradient 1
constexpr long c_key_image_triangle_radial_2 =      447;        // 128 x 128    Triangle Radial Gradient 2
constexpr long c_key_image_triangle_radial_3 =      448;        // 128 x 128    Triangle Radial Gradient 3

//##    Built In Images - Polygon       470-499
constexpr long c_key_image_polygon_5_sides =        470;        // 128 x 128    Pentagon
constexpr long c_key_image_polygon_6_sides =        471;        // 128 x 128    Hexagon
constexpr long c_key_image_polygon_7_sides =        472;        // 128 x 128    Heptagon
constexpr long c_key_image_polygon_8_sides =        473;        // 128 x 128    Octogon
constexpr long c_key_image_polygon_9_sides =        474;        // 128 x 128    Nonagon
constexpr long c_key_image_polygon_10_sides =       475;        // 128 x 128    Decagon
constexpr long c_key_image_star_6_points =          476;        // 128 x 128     6 Point Star
constexpr long c_key_image_star_12_points =         477;        // 128 x 128    12 Point Star
constexpr long c_key_image_star_24_points =         478;        // 128 x 128    24 Point Star


//####################################################################################
//##    Built In Images - Isometric      500-549
//############################
constexpr long c_key_image_isometric_cube_1 =       501;        // 128 x 128    Isometric Cube 1
constexpr long c_key_image_isometric_cube_2 =       502;        // 128 x 128    Isometric Cube 2
constexpr long c_key_image_isometric_cube_3 =       503;        // 128 x 128    Isometric Cube 3
constexpr long c_key_image_isometric_pyramid_1 =    504;        // 128 x 128    Isometric Pyramid 1
constexpr long c_key_image_isometric_pyramid_2 =    505;        // 128 x 128    Isometric Pyramid 2
constexpr long c_key_image_isometric_pyramid_3 =    506;        // 128 x 128    Isometric Pyramid 3
constexpr long c_key_image_isometric_sphere_1 =     507;        // 128 x 128    Isometric Sphere 1
constexpr long c_key_image_isometric_sphere_2 =     508;        // 128 x 128    Isometric Sphere 2
constexpr long c_key_image_isometric_diamond_1 =    509;        // 128 x 128    Isometric Diamond 1
constexpr long c_key_image_isometric_spikes =       510;        // 128 x 128    Isometric Spikes
constexpr long c_key_image_isometric_star =         511;        // 128 x 128    Isometric Star
constexpr long c_key_image_isometric_column_1 =     512;        // 128 x 128    Isometric Column 1
constexpr long c_key_image_isometric_column_2 =     513;        // 128 x 128    Isometric Column 2
constexpr long c_key_image_isometric_flower_1 =     514;        // 128 x 128    Isometric Flower 1
constexpr long c_key_image_isometric_mountain =     515;        // 128 x 128    Isometric Mountain
constexpr long c_key_image_isometric_mushroom =     516;        // 128 x 128    Isometric Mushroom
constexpr long c_key_image_isometric_rock_1 =       517;        // 128 x 128    Isometric Rock 1
constexpr long c_key_image_isometric_rock_2 =       518;        // 128 x 128    Isometric Rock 2
constexpr long c_key_image_isometric_rock_3 =       511;        // 128 x 128    Isometric Rock 3
constexpr long c_key_image_isometric_tree_1 =       519;        // 128 x 128    Isometric Tree 1
constexpr long c_key_image_isometric_tree_2 =       520;        // 128 x 128    Isometric Tree 2


//##    Built In Images - Shapes        550-599
constexpr long c_key_image_star_1 =                 550;        // 128 x 128    Star 1
constexpr long c_key_image_star_2 =                 551;        // 128 x 128    Star 2
constexpr long c_key_image_star_3 =                 552;        // 128 x 128    Rounded Star 1
constexpr long c_key_image_star_4 =                 553;        // 128 x 128    Rounded Star 2
constexpr long c_key_image_heart_1 =                554;        // 128 x 128    Heart 1
constexpr long c_key_image_heart_2 =                555;        // 128 x 128    Heart 2
constexpr long c_key_image_heart_half =             556;        // 128 x 128    Heart Half
constexpr long c_key_image_pie =                    557;        // 128 x 128    Puckman
constexpr long c_key_image_ghost =                  558;        // 128 x 128    Ghost
constexpr long c_key_image_gear_1 =                 559;        // 128 x 128    Small Gear
constexpr long c_key_image_gear_2 =                 560;        // 128 x 128    Big Gear
constexpr long c_key_image_thought_bubble_1 =       561;        // 128 x 128    Oval   Thought Bubble
constexpr long c_key_image_thought_bubble_2 =       562;        // 128 x 128    Square Thought Bubble
constexpr long c_key_image_thought_bubble_3 =       663;        // 128 x 128    Cloud  Thought Bubble
constexpr long c_key_image_exclamation_point =      564;        // 128 x 128    Exclamation Point
constexpr long c_key_image_question_mark =          565;        // 128 x 128    Question Mark
constexpr long c_key_image_arrow_1 =                566;        // 128 x 128    Arrow 1
constexpr long c_key_image_arrow_2 =                567;        // 128 x 128    Arrow 2
constexpr long c_key_image_reset =                  568;        // 128 x 128    Reset Arrow
constexpr long c_key_image_fish =                   569;        // 128 x 128    Fish


//####################################################################################
//##    Built In Images - Ground        600-699
//############################
constexpr long c_key_image_ground_top_1 =           601;        // 128 x 128    Ground Top 1
constexpr long c_key_image_ground_top_2 =           602;        // 128 x 128    Ground Top 2
constexpr long c_key_image_ground_top_3 =           603;        // 128 x 128    Ground Top 3
constexpr long c_key_image_ground_top_4 =           604;        // 128 x 128    Ground Top 4
constexpr long c_key_image_ground_top_5 =           605;        // 128 x 128    Ground Top 5
constexpr long c_key_image_ground_top_6 =           606;        // 128 x 128    Ground Top 6
constexpr long c_key_image_ground_top_7 =           607;        // 128 x 128    Ground Top 7
constexpr long c_key_image_ground_top_8 =           608;        // 128 x 128    Ground Top 8
constexpr long c_key_image_ground_top_left =        609;        // 128 x 128    Ground Top Left
constexpr long c_key_image_ground_top_right =       610;        // 128 x 128    Ground Top Right
constexpr long c_key_image_ground_top_up_right_1 =  611;        // 128 x 128    Ground Top Up Right 1
constexpr long c_key_image_ground_top_up_right_2 =  612;        // 128 x 128    Ground Top Up Right 2
constexpr long c_key_image_ground_top_up_left_1 =   613;        // 128 x 128    Ground Top Up Left 1
constexpr long c_key_image_ground_top_up_left_2 =   614;        // 128 x 128    Ground Top Up Left 2
constexpr long c_key_image_block_1 =                615;        // 128 x 128    Block 1
constexpr long c_key_image_block_2 =                616;        // 128 x 128    Block 2
constexpr long c_key_image_block_lock =             617;        // 128 x 128    Block Lock
constexpr long c_key_image_box_1 =                  618;        // 128 x 128    Box 1
constexpr long c_key_image_box_2 =                  619;        // 128 x 128    Box 2
constexpr long c_key_image_bricks =                 620;        // 128 x 128    Bricks
constexpr long c_key_image_dirt =                   621;        // 128 x 128    Dirt
constexpr long c_key_image_ice =                    622;        // 128 x 128    Ice
constexpr long c_key_image_sand =                   623;        // 128 x 128    Sand
constexpr long c_key_image_stone =                  624;        // 128 x 128    Stone
constexpr long c_key_image_tile =                   625;        // 128 x 128    Tile
constexpr long c_key_image_water =                  626;        // 128 x 128    Water
constexpr long c_key_image_water_top =              627;        // 128 x 128    Water Top
constexpr long c_key_image_wood =                   628;        // 128 x 128    Wood


//####################################################################################
//##    Built In Images - Decoration    700-749
//############################
constexpr long c_key_image_bush_1 =                 701;        // 128 x 128    Bush
constexpr long c_key_image_bush_2 =                 702;        // 128 x 128    Bush w/ Flower
constexpr long c_key_image_bush_3 =                 703;        // 128 x 128    Small Bush
constexpr long c_key_image_cloud =                  704;        // 128 x 128    Cloud
constexpr long c_key_image_flower_1 =               705;        // 128 x 128    Flower 1
constexpr long c_key_image_grass_1 =                706;        // 128 x 128    Grass 1
constexpr long c_key_image_grass_2 =                707;        // 128 x 128    Grass 2
constexpr long c_key_image_grass_3 =                708;        // 128 x 128    Grass 3
constexpr long c_key_image_ladder_1 =               709;        // 128 x 128    Ladder 1
constexpr long c_key_image_ladder_2 =               710;        // 128 x 128    Ladder 2
constexpr long c_key_image_ladder_top =             711;        // 128 x 128    Ladder Top
constexpr long c_key_image_mushroom =               712;        // 128 x 128    Mushroom
constexpr long c_key_image_rock_1 =                 713;        // 128 x 128    Rock 2
constexpr long c_key_image_rock_2 =                 714;        // 128 x 128    Rock 2
constexpr long c_key_image_sign_1 =                 715;        // 128 x 128    Sign
constexpr long c_key_image_sign_post =              716;        // 128 x 128    Sign Post
constexpr long c_key_image_sun =                    717;        // 128 x 128    Sun

//##    Items                            750-799




//####################################################################################
//##    Characters / Enemies             800-899
//############################


//####################################################################################
//##    UI                               900-999
//############################




//####################################################################################
//##    Possible background colors for Component Categories
//############################
namespace Component_Colors {
    const QColor White_Snow         { QColor(255, 252, 249, 255) };
    const QColor Silver_Snow        { QColor(192, 188, 183, 255) };
    const QColor Darkness           { QColor( 40,  33,  33, 255) };

    const QColor Orange_Pastel      { QColor(250, 146,   0, 255) };

    const QColor Pink_Pearl         { QColor(228, 180, 194, 255) };
    const QColor Red_Faded          { QColor(255, 105, 120, 255) };

    const QColor Red_Tuscan         { QColor(104,  71,  86, 255) };
    const QColor Purple_Pastel      { QColor(176, 161, 186, 255) };
    const QColor Purple_Royal       { QColor(126,  87, 194, 255) };
    const QColor Blue_Yonder        { QColor(165, 181, 191, 255) };
    const QColor Blue_Royal         { QColor( 57, 120, 237, 255) };

    const QColor Green_SeaGrass     { QColor(154, 225, 157, 255) };
    const QColor Green_Sickness     { QColor(203, 240,  19, 255) };

    const QColor Mustard_Yellow     { QColor(255, 200,  87, 255) };
    const QColor Mellow_Yellow      { QColor(250, 223, 127, 255) };

    const QColor Brown_Sugar        { QColor(165, 117,  72, 255) };
    const QColor Beige_Apricot      { QColor(252, 215, 173, 255) };

    const QColor Blue_Drop_1        { QColor(  0, 150, 145, 255) };
    const QColor Blue_Drop_2        { QColor(  0, 180, 175, 255) };
    const QColor Blue_Drop_3        { QColor(  0, 225, 219, 255) };
    const QColor Blue_Drop_4        { QColor( 96, 255, 248, 255) };
    const QColor Blue_Drop_5        { QColor(192, 255, 248, 255) };
};


//####################################################################################
//##    Possible icons for Component Categories
//############################
namespace Component_Icons {
    // General Components
    const QString None          { QString("") };
    const QString Name          { QString(":/assets/inspector_icons/comp_name.png") };
    const QString Hidden        { QString(":/assets/inspector_icons/comp_hidden.png") };
    const QString Settings      { QString(":/assets/inspector_icons/comp_settings.png") };
    const QString Physics       { QString(":/assets/inspector_icons/comp_physics.png") };
    const QString Transform     { QString(":/assets/inspector_icons/comp_transform.png") };
    const QString Layering      { QString(":/assets/inspector_icons/comp_layering.png") };
    const QString Movement      { QString(":/assets/inspector_icons/comp_movement.png") };
    const QString Spawn         { QString(":/assets/inspector_icons/comp_spawn.png") };
    const QString Appearance    { QString(":/assets/inspector_icons/comp_appearance.png") };
    const QString Collide       { QString(":/assets/inspector_icons/comp_collide.png") };
    const QString Animation     { QString(":/assets/inspector_icons/comp_animation.png") };
    const QString Effects       { QString(":/assets/inspector_icons/comp_effects.png") };
    const QString Health        { QString(":/assets/inspector_icons/comp_health.png") };
    const QString Controls      { QString(":/assets/inspector_icons/comp_controls.png") };

    // Thing Components
    const QString Camera        { QString(":/assets/inspector_icons/comp_camera.png") };
    const QString Character     { QString(":/assets/inspector_icons/comp_character.png") };
    const QString Font          { QString(":/assets/inspector_icons/comp_font.png") };
    const QString Object        { QString(":/assets/inspector_icons/comp_object.png") };

    // Effect Components
    const QString Fire          { QString(":/assets/inspector_icons/comp_fire.png") };
    const QString Fisheye       { QString(":/assets/inspector_icons/comp_fisheye.png") };
    const QString Light         { QString(":/assets/inspector_icons/comp_light.png") };
    const QString Mirror        { QString(":/assets/inspector_icons/comp_mirror.png") };
    const QString Swirl         { QString(":/assets/inspector_icons/comp_swirl.png") };
    const QString Water         { QString(":/assets/inspector_icons/comp_water.png") };
    const QString Water_Ripple  { QString(":/assets/inspector_icons/comp_water_ripple.png") };
    const QString Water_Wave    { QString(":/assets/inspector_icons/comp_water_wave.png") };
    const QString Water_Refract { QString(":/assets/inspector_icons/comp_water_refract.png") };
    const QString Water_Foam    { QString(":/assets/inspector_icons/comp_water_foam.png") };
};



//####################################################################################
//##    Possible Headers and Descriptions for items within editor
//############################
typedef QList<QString> HeaderBodyList;

namespace Advisor_Info {

    // Toolbar Buttons
    const HeaderBodyList Mode_Map               { "World Map", "This mode will allow you to see the layout of your Project and how the worlds and "
                                                               "user interfaces in your Project connect to each other." };
    const HeaderBodyList Mode_Editor            { "World Editor Mode", "This mode will allow you to edit the contents of each physics world. It is where "
                                                                       "you can design each Stage and lay out your worlds / levels." };
    const HeaderBodyList Mode_UI                { "UI Editor Mode",     "This mode will allow you to edit the user interface layer of your Project." };
    const HeaderBodyList Settings_Playground    { "Physics Playground", "Play with the physics engine to learn more about physics properties." };
    const HeaderBodyList Settings_Image_Viewer  { "Image Viewer",       "View / manage Images used in this Project." };
    const HeaderBodyList Settings_Font_Builder  { "Font Builder",       "Create and edit fonts used for Text Boxes." };
    const HeaderBodyList Settings_Manager       { "Settings Manager",   "View and edit Project settings." };

    const HeaderBodyList Send_to_Back           { "Send to Back",  "Send selected item(s) to the back." };
    const HeaderBodyList Send_to_Front          { "Send to Font",  "Send selected item(s) to the front." };
    const HeaderBodyList Send_Backward          { "Send Backward", "Send selected item(s) back by one." };
    const HeaderBodyList Send_Forward           { "Send Forward",  "Send selected item(s) forward by one." };

    const HeaderBodyList Add_Entity             { "Add Item",      "Adds an item to the Project." };
    const HeaderBodyList Duplicate              { "Duplicate",     "Create a copy of selected item(s)." };
    const HeaderBodyList Trash_Can              { "Delete",        "Remove selected item(s)." };

    const HeaderBodyList Reset                  { "Reset Transform", "Resets selected objects' transforms to a Scale of X:1, Y:1 and Rotation of "
                                                                            "0 Degrees." };
    const HeaderBodyList Flip_H                 { "Flip Horizontal", "Flips the selection horizontally." };
    const HeaderBodyList Flip_V                 { "Flip Vertical",   "Flips the selection vertically." };
    const HeaderBodyList Rotate_L               { "Rotate Left",  "Rotates selection counter-clockwise by 90 degrees." };
    const HeaderBodyList Rotate_R               { "Rotate Right", "Rotates selection clockwise by 90 degrees." };

    const HeaderBodyList Grid_Show_On_Top       { "Grid on Top?",    "Draws grid lines on top of objects in editor." };
    const HeaderBodyList Resize_To_Grid         { "Resize to Grid?", "Snaps corners and edges of objects to grid while resizing. Only works if the angle "
                                                                     "of the object matches the angle of the grid, and the Grid Scale X and Y are the same. "
                                                                     "Selection handles will change to square shape to signify snapping is possible."};
    const HeaderBodyList Grid_Snap_To_Grid      { "Snap to Grid?",   "Snaps objects to grid lines when moving objects around with the mouse." };
    const HeaderBodyList Grid_Snap_Options      { "Snap Options",    "Choose how objects snap to grid while multiple objects are selected, and objects are moved "
                                                                     "with mouse. Either have the center of the selection group snap to the grid, or have each "
                                                                     "object within the group snap to grid individually." };

    const HeaderBodyList Play_Game              { "Play Game",  "Start playing game! Gameplay starts at the Start Stage of the World selected." };
    const HeaderBodyList Play_Stage             { "Play Stage", "Play ONLY the current Stage. Gameplay starts at the Start Stage of the current World "
                                                                "and only loads in the Stage currently selected." };


    // Form Main Widgets
    const HeaderBodyList Advisor_Window         { "Advisor Window", "Shows a brief description of editor items." };
    const HeaderBodyList Inspector_Window       { "Inspector", "Displays components / properties of currently selected item." };

    const HeaderBodyList ColorButton            { "Color Button", "Click this to open a color popup that has a convenient color selection." };
    const HeaderBodyList ColorPicker            { "Color Picker", "Press and hold this button to use a magnifying lens to pick a color off of the screen." };
    const HeaderBodyList ColorDialog            { "Color Dialog", "Opens the system color dialog toolbox." };

    // Editor Widgets
    const HeaderBodyList Asset_List             { "Asset List", "These are items that can be dragged into your Project. Changing the properties "
                                                                "of these items will affect all instances of those items Project wide." };
    const HeaderBodyList Asset_Search           { "Asset Search", "Type the name of an asset here to help find it in the Asset List." };
    const HeaderBodyList Stage_View             { "Stage View", "Shows objects and layout of currently selected Stage. Drop assets into Stage "
                                                                "View to add to Stage." };
    const HeaderBodyList Project_Tree           { "Project Tree", "Lists the items contained within the currently displayed Stage. Select items "
                                                                  "to view / adjust properties for each item." };

    // Project Types
    const HeaderBodyList Asset_Description      { "Asset", "This is an item that can be dragged into your Project. Changing the properties "
                                                           "of this item will affect all instances of this item Project wide." };
    const HeaderBodyList World_Description      { "World", "A World is a container of Stages." };
    const HeaderBodyList Stage_Description      { "Stage", "A Stage is a container of Things." };
    const HeaderBodyList Camera_Thing           { "Camera", "This is a Camera. This will decide what the player sees." };
    const HeaderBodyList Character_Thing        { "Character", "This is a Character." };
    const HeaderBodyList Thing_Description      { "Thing", "This is a Thing in a Stage." };

    // Asset Types
    const HeaderBodyList Asset_Character        { "Character Asset", "This is a player character." };
    const HeaderBodyList Asset_Object           { "Object Asset", "This is an object that can have many copies of itself placed into your game. Any change to "
                                                                  "this Asset will change all occurences of the Asset within your game." };
    const HeaderBodyList Asset_Device           { "Device Asset", "This is a device that is used to add extra control to your game." };
    const HeaderBodyList Asset_Effect           { "Effect Asset", "Special effects provide an extra level of detail to make your game more visually appealing." };
    const HeaderBodyList Asset_Image            { "Image Asset", "This is an Image for use in your game." };
    const HeaderBodyList Asset_Image_Built_In   { "Buil In Image Asset", "This is a simple, easy to use built-in Image for use in your game." };
    const HeaderBodyList Asset_Text             { "Text Asset", "Use Text Assets to show text boxes in your game. You can use them as titles or for showing "
                                                                "values like hit points, coins or distance." };



    // Object Types
    const HeaderBodyList Object_Static          { "Static",     "Object can not move. Best performance, dynamic objects touching static objects can rest "
                                                                "when not moving which reduces physics calculations. Best for ground, walls, fixed enemies "
                                                                "(like spikes), etc." };
    const HeaderBodyList Object_Kinematic       { "Kinematic",  "Object can move at fixed speeds. Kinematic objects are not affected by physical or "
                                                                "gravitational forces. Best for doors, moving platforms, some enemies, etc." };
    const HeaderBodyList Object_Dynamic         { "Dynamic",    "Object obeys the laws of physics. Dynamic objects can be moved by forces and are affected by "
                                                                "gravity. Best for players, small enemies, balls, boxes, cars, wheels, etc."};

    const HeaderBodyList Damage_None            { "No Damage",      "This object does not damage other objects." };
    const HeaderBodyList Damage_Player          { "Damage Player",  "This is an enemy object. It will damage player objects that it comes into contact with." };
    const HeaderBodyList Damage_Enemy           { "Damage Enemy",   "This is a player object. It will damage enemy objects that it comes into contact with." };
    const HeaderBodyList Damage_All             { "Damage All",     "This object can be damaged by players and enemies. It will damage all objects it comes "
                                                                    "into contact with. "};

    const HeaderBodyList Light_Opaque           { "Opaque Light",   "Light is drawn as solid texture, does not provide actual lighting to scene, only shadows. "
                                                                    "However, light is able to be layered in a specific Z-Order." };
    const HeaderBodyList Light_Glow             { "Glow Light",     "Light provides diffuse lighting. Ambient light setting should be adjusted in World Settings "
                                                                    "to take advantage of Glow Lights. All Glow Lights are drawn at Z-Order specified in World Settings."};

    // Inspector Widgets
    const HeaderBodyList Variable_Widget        { "Variable Amount", "Plus or minus modifier to initial value, the following value allows for some "
                                                                     "variable amount to the initial value. For example, an initial value of 100 with "
                                                                     "a variable amount of 5, allows for values ranging from 95 to 105." };

    // Other
    const HeaderBodyList Not_Set                { "Not Set", "Fix me!!!!!!" };
};



//####################################################################################
//##    Local Error Codes
//############################

namespace Error_Code {

    // Class DrSettings Related
    const QString NoComponent          { "S001" };
    const QString NoProperty           { "S002" };

}


//####################################################################################
//##    Mouse cursors to use throughout Project, defined in constants.cpp
//############################
namespace Mouse_Cursors {
    QCursor sizeVertical();
    QCursor size022();
    QCursor size045();
    QCursor size067();
    QCursor sizeHorizontal();
    QCursor size112();
    QCursor size135();
    QCursor size157();
    QCursor rotateAll();
};



#endif // CONSTANTS_H















