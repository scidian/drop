//
//      Created by Stephens Nunnally on 2/15/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Some Constatnt Project Data
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
//####################################################################################

// Starting Key
constexpr long c_key_starting_number =              1001;       // First 1001 keys or so are reserved for built in Entities in DrProject
                                                                // ....."1001" is an easily searchable number
// Effects
constexpr long c_key_effect_light =                 800;        // Light Effect
constexpr long c_key_effect_water =                 801;        // Water Effect
constexpr long c_key_effect_fire =                  802;        // Fire Effect
constexpr long c_key_effect_mirror =                803;        // Mirror Effect
constexpr long c_key_effect_fisheye =               804;        // Fisheye Effect
constexpr long c_key_effect_swirl =                 805;        // Swirl Effect

// Devices
constexpr long c_key_device_camera =                900;        // Camera Device



// Built In Images
constexpr long c_key_image_empty =                  1;          //  32 x  32    Alpha value 0 square
constexpr long c_key_image_character =              2;          // 256 x 256    Gray Circle
constexpr long c_key_image_object =                 3;          // 256 x 256    Gray Box


// Built In Images - Basic
constexpr long c_key_image_square_1 =               11;         // 128 x 128    Square 1
constexpr long c_key_image_square_2 =               12;         // 128 x 128    Square 2
constexpr long c_key_image_square_3 =               13;         // 128 x 128    Square 3
constexpr long c_key_image_square_4 =               14;         // 128 x 128    Square 4
constexpr long c_key_image_square_5 =               15;         // 128 x 128    Square 5
constexpr long c_key_image_square_6 =               16;         // 128 x 128    Square 6
constexpr long c_key_image_square_7 =               17;         // 128 x 128    Square 7
constexpr long c_key_image_square_bars =            18;         // 128 x 128    Square Bars
constexpr long c_key_image_square_flat =            19;         // 128 x 128    Square Flat
constexpr long c_key_image_square_stripes =         20;         // 128 x 128    Square Stripes

constexpr long c_key_image_rounded_1 =              21;         // 128 x 128    Rounded Square 1
constexpr long c_key_image_rounded_2 =              22;         // 128 x 128    Rounded Square 2
constexpr long c_key_image_rounded_3 =              23;         // 128 x 128    Rounded Square 3
constexpr long c_key_image_rounded_4 =              24;         // 128 x 128    Rounded Square 4
constexpr long c_key_image_rounded_5 =              25;         // 128 x 128    Rounded Square 5
constexpr long c_key_image_rounded_6 =              26;         // 128 x 128    Rounded Square 6
constexpr long c_key_image_rounded_7 =              27;         // 128 x 128    Rounded Square 7
constexpr long c_key_image_rounded_bars =           28;         // 128 x 128    Rounded Square Bars
constexpr long c_key_image_rounded_flat =           29;         // 128 x 128    Rounded Square Flat
constexpr long c_key_image_rounded_stripes =        30;         // 128 x 128    Rounded Square Stripes

constexpr long c_key_image_circle_1 =               31;         // 128 x 128    Circle 1
constexpr long c_key_image_circle_2 =               32;         // 128 x 128    Circle 2
constexpr long c_key_image_circle_3 =               33;         // 128 x 128    Circle 3
constexpr long c_key_image_circle_4 =               34;         // 128 x 128    Circle 4
constexpr long c_key_image_circle_5 =               35;         // 128 x 128    Circle 5
constexpr long c_key_image_circle_6 =               36;         // 128 x 128    Circle 6
constexpr long c_key_image_circle_7 =               37;         // 128 x 128    Circle 7
constexpr long c_key_image_circle_bars =            38;         // 128 x 128    Circle Bars
constexpr long c_key_image_circle_flat =            39;         // 128 x 128    Circle Flat
constexpr long c_key_image_circle_stripes =         40;         // 128 x 128    Circle Stripes

constexpr long c_key_image_triangle_1 =             41;         // 128 x 128    Triangle 1
constexpr long c_key_image_triangle_2 =             42;         // 128 x 128    Triangle 2
constexpr long c_key_image_triangle_3 =             43;         // 128 x 128    Triangle 3
constexpr long c_key_image_triangle_4 =             44;         // 128 x 128    Triangle 4
constexpr long c_key_image_triangle_5 =             45;         // 128 x 128    Triangle 5
constexpr long c_key_image_triangle_6 =             46;         // 128 x 128    Triangle 6
constexpr long c_key_image_triangle_7 =             47;         // 128 x 128    Triangle 7
constexpr long c_key_image_triangle_bars =          48;         // 128 x 128    Triangle Bars
constexpr long c_key_image_triangle_flat =          49;         // 128 x 128    Triangle Flat
constexpr long c_key_image_triangle_stripes =       50;         // 128 x 128    Triangle Stripes

constexpr long c_key_image_diamond_1 =              51;         // 128 x 128    Diamond 1
constexpr long c_key_image_diamond_2 =              52;         // 128 x 128    Diamond 2
constexpr long c_key_image_diamond_3 =              53;         // 128 x 128    Diamond 3
constexpr long c_key_image_diamond_4 =              54;         // 128 x 128    Diamond 4
constexpr long c_key_image_diamond_5 =              55;         // 128 x 128    Diamond 5
constexpr long c_key_image_diamond_bars =           56;         // 128 x 128    Diamond Bars
constexpr long c_key_image_diamond_flat =           57;         // 128 x 128    Diamond Flat
constexpr long c_key_image_diamond_stripes =        58;         // 128 x 128    Diamond Stripes

constexpr long c_key_image_line_1 =                 61;         // 128 x 128    Line 1
constexpr long c_key_image_line_2 =                 62;         // 128 x 128    Line 2

constexpr long c_key_image_cross_1 =                71;         // 128 x 128    Cross 1
constexpr long c_key_image_cross_2 =                72;         // 128 x 128    Cross 2
constexpr long c_key_image_cross_3 =                73;         // 128 x 128    Cross 3
constexpr long c_key_image_cross_4 =                74;         // 128 x 128    Cross 4
constexpr long c_key_image_cross_5 =                75;         // 128 x 128    Cross 5
constexpr long c_key_image_cross_6 =                76;         // 128 x 128    Cross 6
constexpr long c_key_image_cross_7 =                77;         // 128 x 128    Cross 7
constexpr long c_key_image_cross_bars =             78;         // 128 x 128    Cross Bars
constexpr long c_key_image_cross_flat =             79;         // 128 x 128    Cross Flat
constexpr long c_key_image_cross_stripes =          80;         // 128 x 128    Cross Stripes

constexpr long c_key_image_half_circle_1 =          81;         // 128 x 128    Half Circle 1
constexpr long c_key_image_half_circle_2 =          82;         // 128 x 128    Half Circle 2


// Built In Images - Outline
constexpr long c_key_image_square_outline_1 =       101;        // 128 x 128    Square Outline 1
constexpr long c_key_image_square_outline_2 =       102;        // 128 x 128    Square Outline 2
constexpr long c_key_image_square_outline_3 =       103;        // 128 x 128    Square Outline 3
constexpr long c_key_image_rounded_outline_1 =      104;        // 128 x 128    Rounded Outline 1
constexpr long c_key_image_rounded_outline_2 =      105;        // 128 x 128    Rounded Outline 2
constexpr long c_key_image_rounded_outline_3 =      106;        // 128 x 128    Rounded Outline 3
constexpr long c_key_image_circle_outline_1 =       107;        // 128 x 128    Circle Outline 1
constexpr long c_key_image_circle_outline_2 =       108;        // 128 x 128    Circle Outline 2
constexpr long c_key_image_circle_outline_3 =       109;        // 128 x 128    Circle Outline 3
constexpr long c_key_image_triangle_outline_1 =     110;        // 128 x 128    Triangle Outline 1
constexpr long c_key_image_triangle_outline_2 =     111;        // 128 x 128    Triangle Outline 2
constexpr long c_key_image_triangle_outline_3 =     112;        // 128 x 128    Triangle Outline 3
constexpr long c_key_image_diamond_outline_1 =      113;        // 128 x 128    Diamond Outline 1
constexpr long c_key_image_diamond_outline_2 =      114;        // 128 x 128    Diamond Outline 2
constexpr long c_key_image_cross_outline_1 =        115;        // 128 x 128    Cross Outline 1
constexpr long c_key_image_cross_outline_2 =        116;        // 128 x 128    Cross Outline 2


// Built In Images - Gradient
constexpr long c_key_image_circle_linear_1 =        131;        // 128 x 128    Circle Linear Gradient 1
constexpr long c_key_image_circle_linear_2 =        132;        // 128 x 128    Circle Linear Gradient 2
constexpr long c_key_image_circle_linear_3 =        133;        // 128 x 128    Circle Linear Gradient 3
constexpr long c_key_image_circle_radial_1 =        134;        // 128 x 128    Circle Radial Gradient 1
constexpr long c_key_image_circle_radial_2 =        135;        // 128 x 128    Circle Radial Gradient 2
constexpr long c_key_image_circle_radial_3 =        136;        // 128 x 128    Circle Radial Gradient 3
constexpr long c_key_image_square_linear_1 =        137;        // 128 x 128    Square Linear Gradient 1
constexpr long c_key_image_square_linear_2 =        138;        // 128 x 128    Square Linear Gradient 2
constexpr long c_key_image_square_linear_3 =        139;        // 128 x 128    Square Linear Gradient 3
constexpr long c_key_image_square_radial_1 =        140;        // 128 x 128    Square Radial Gradient 1
constexpr long c_key_image_square_radial_2 =        141;        // 128 x 128    Square Radial Gradient 2
constexpr long c_key_image_square_radial_3 =        142;        // 128 x 128    Square Radial Gradient 3
constexpr long c_key_image_triangle_linear_1 =      143;        // 128 x 128    Triangle Linear Gradient 1
constexpr long c_key_image_triangle_linear_2 =      144;        // 128 x 128    Triangle Linear Gradient 2
constexpr long c_key_image_triangle_linear_3 =      145;        // 128 x 128    Triangle Linear Gradient 3
constexpr long c_key_image_triangle_radial_1 =      146;        // 128 x 128    Triangle Radial Gradient 1
constexpr long c_key_image_triangle_radial_2 =      147;        // 128 x 128    Triangle Radial Gradient 2
constexpr long c_key_image_triangle_radial_3 =      148;        // 128 x 128    Triangle Radial Gradient 3


// Built In Images - Polygon
constexpr long c_key_image_polygon_5_sides =        170;        // 128 x 128    Pentagon
constexpr long c_key_image_polygon_6_sides =        171;        // 128 x 128    Hexagon
constexpr long c_key_image_polygon_7_sides =        172;        // 128 x 128    Heptagon
constexpr long c_key_image_polygon_8_sides =        173;        // 128 x 128    Octogon
constexpr long c_key_image_polygon_9_sides =        174;        // 128 x 128    Nonagon
constexpr long c_key_image_polygon_10_sides =       175;        // 128 x 128    Decagon
constexpr long c_key_image_star_6_points =          176;        // 128 x 128     6 Point Star
constexpr long c_key_image_star_12_points =         177;        // 128 x 128    12 Point Star
constexpr long c_key_image_star_24_points =         178;        // 128 x 128    24 Point Star


// Built In Images - Decoration
constexpr long c_key_image_cloud =                  201;        // 128 x 128    Cloud
constexpr long c_key_image_flower_1 =               202;        // 128 x 128    Flower 1
constexpr long c_key_image_grass_1 =                203;        // 128 x 128    Grass 1
constexpr long c_key_image_grass_2 =                204;        // 128 x 128    Grass 2
constexpr long c_key_image_grass_3 =                205;        // 128 x 128    Grass 3
constexpr long c_key_image_ladder_1 =               206;        // 128 x 128    Ladder 1
constexpr long c_key_image_ladder_2 =               207;        // 128 x 128    Ladder 2
constexpr long c_key_image_ladder_top =             208;        // 128 x 128    Ladder Top
constexpr long c_key_image_mushroom =               209;        // 128 x 128    Mushroom
constexpr long c_key_image_plant_1 =                210;        // 128 x 128    Bush
constexpr long c_key_image_plant_2 =                211;        // 128 x 128    Bush w/ Flower
constexpr long c_key_image_rock_1 =                 213;        // 128 x 128    Rock 2
constexpr long c_key_image_rock_2 =                 214;        // 128 x 128    Rock 2
constexpr long c_key_image_sign_1 =                 215;        // 128 x 128    Sign
constexpr long c_key_image_sign_post =              216;        // 128 x 128    Sign Post
constexpr long c_key_image_sun =                    217;        // 128 x 128    Sun



// Built In Images - Ground
constexpr long c_key_image_ground_top_1 =           301;        // 128 x 128    Ground Top 1
constexpr long c_key_image_ground_top_2 =           302;        // 128 x 128    Ground Top 2
constexpr long c_key_image_ground_top_3 =           303;        // 128 x 128    Ground Top 3
constexpr long c_key_image_ground_top_4 =           304;        // 128 x 128    Ground Top 4
constexpr long c_key_image_ground_top_5 =           305;        // 128 x 128    Ground Top 5
constexpr long c_key_image_ground_top_6 =           306;        // 128 x 128    Ground Top 6
constexpr long c_key_image_ground_top_7 =           307;        // 128 x 128    Ground Top 7
constexpr long c_key_image_ground_top_8 =           308;        // 128 x 128    Ground Top 8
constexpr long c_key_image_ground_top_left =        309;        // 128 x 128    Ground Top Left
constexpr long c_key_image_ground_top_right =       310;        // 128 x 128    Ground Top Right
constexpr long c_key_image_ground_top_up_right_1 =  311;        // 128 x 128    Ground Top Up Right 1
constexpr long c_key_image_ground_top_up_right_2 =  312;        // 128 x 128    Ground Top Up Right 2
constexpr long c_key_image_ground_top_up_left_1 =   313;        // 128 x 128    Ground Top Up Left 1
constexpr long c_key_image_ground_top_up_left_2 =   314;        // 128 x 128    Ground Top Up Left 2
constexpr long c_key_image_block_1 =                315;        // 128 x 128    Block 1
constexpr long c_key_image_block_2 =                316;        // 128 x 128    Block 2
constexpr long c_key_image_block_lock =             317;        // 128 x 128    Block Lock
constexpr long c_key_image_box_1 =                  318;        // 128 x 128    Box 1
constexpr long c_key_image_box_2 =                  319;        // 128 x 128    Box 2
constexpr long c_key_image_bricks =                 320;        // 128 x 128    Bricks
constexpr long c_key_image_dirt =                   321;        // 128 x 128    Dirt
constexpr long c_key_image_ice =                    322;        // 128 x 128    Ice
constexpr long c_key_image_sand =                   323;        // 128 x 128    Sand
constexpr long c_key_image_stone =                  324;        // 128 x 128    Stone
constexpr long c_key_image_tile =                   325;        // 128 x 128    Tile
constexpr long c_key_image_water =                  326;        // 128 x 128    Water
constexpr long c_key_image_water_top =              327;        // 128 x 128    Water Top
constexpr long c_key_image_wood =                   328;        // 128 x 128    Wood


// Built In Images - Shapes
constexpr long c_key_image_star_1 =                 350;        // 128 x 128    Star 1
constexpr long c_key_image_star_2 =                 351;        // 128 x 128    Star 2
constexpr long c_key_image_star_3 =                 352;        // 128 x 128    Rounded Star 1
constexpr long c_key_image_star_4 =                 353;        // 128 x 128    Rounded Star 2
constexpr long c_key_image_heart_1 =                354;        // 128 x 128    Heart 1
constexpr long c_key_image_heart_2 =                355;        // 128 x 128    Heart 2
constexpr long c_key_image_heart_half =             356;        // 128 x 128    Heart Half
constexpr long c_key_image_pie =                    357;        // 128 x 128    Puckman
constexpr long c_key_image_ghost =                  358;        // 128 x 128    Ghost
constexpr long c_key_image_gear_1 =                 359;        // 128 x 128    Small Gear
constexpr long c_key_image_gear_2 =                 360;        // 128 x 128    Big Gear
constexpr long c_key_image_thought_bubble_1 =       361;        // 128 x 128    Oval   Thought Bubble
constexpr long c_key_image_thought_bubble_2 =       362;        // 128 x 128    Square Thought Bubble
constexpr long c_key_image_thought_bubble_3 =       363;        // 128 x 128    Cloud  Thought Bubble
constexpr long c_key_image_exclamation_point =      364;        // 128 x 128    Exclamation Point
constexpr long c_key_image_question_mark =          365;        // 128 x 128    Question Mark
constexpr long c_key_image_arrow_1 =                366;        // 128 x 128    Arrow 1
constexpr long c_key_image_arrow_2 =                367;        // 128 x 128    Arrow 2
constexpr long c_key_image_reset =                  368;        // 128 x 128    Reset Arrow
constexpr long c_key_image_fish =                   369;        // 128 x 128    Fish


// Built In Images - Isometric
constexpr long c_key_image_isometric_cube_1 =       401;        // 128 x 128    Isometric Cube 1
constexpr long c_key_image_isometric_cube_2 =       402;        // 128 x 128    Isometric Cube 2
constexpr long c_key_image_isometric_cube_3 =       403;        // 128 x 128    Isometric Cube 3
constexpr long c_key_image_isometric_pyramid_1 =    404;        // 128 x 128    Isometric Pyramid 1
constexpr long c_key_image_isometric_pyramid_2 =    405;        // 128 x 128    Isometric Pyramid 2
constexpr long c_key_image_isometric_pyramid_3 =    406;        // 128 x 128    Isometric Pyramid 3
constexpr long c_key_image_isometric_sphere_1 =     407;        // 128 x 128    Isometric Sphere 1
constexpr long c_key_image_isometric_sphere_2 =     408;        // 128 x 128    Isometric Sphere 2
constexpr long c_key_image_isometric_diamond_1 =    409;        // 128 x 128    Isometric Diamond 1
constexpr long c_key_image_isometric_spikes =       410;        // 128 x 128    Isometric Spikes
constexpr long c_key_image_isometric_column =       411;        // 128 x 128    Isometric Column
constexpr long c_key_image_isometric_star =         412;        // 128 x 128    Isometric Star


// Items                450-499

// Characters / Enemies 500-599

// UI                   600-699




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















