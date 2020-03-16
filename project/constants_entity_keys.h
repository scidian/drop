//
//      Created by Stephens Nunnally on 12/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef CONSTANTS_KEYS_H
#define CONSTANTS_KEYS_H


//####################################################################################
//##    Project Key Constants
//##        Starting Key
//####################################################################################

constexpr long c_key_starting_number =              1001;       // First 1001 keys are reserved for built in Entities in DrProject
                                                                // ....."1001" is an easily searchable number

//####################################################################################
//##    Basic Images                    001-099
//############################
constexpr long c_key_image_empty =                  1;          //  32 x  32    Empty Square

constexpr long c_key_image_blob =                   2;          // 128 x 128    Blue Split Circle
constexpr long c_key_image_character =              3;          // 128 x 128    Tan Checker Circle
constexpr long c_key_image_foliage =                4;          //  32 x 128    Green / Brown Flower
constexpr long c_key_image_ground =                 5;          // 256 x 256    Green Square
constexpr long c_key_image_ladder =                 6;          // 128 x 128    Ladder Piece
constexpr long c_key_image_object =                 7;          // 128 x 128    Brown Box
constexpr long c_key_image_spike =                  8;          // 128 x 128    Red Spike Triangle
constexpr long c_key_image_spring =                 9;          // 128 x 128    Red w/ Gray Spring


//####################################################################################
//##    Devices                         100-149
//############################
constexpr long c_key_device_camera =                100;        // Camera Device


//####################################################################################
//##    Effects                         150-199
//############################
constexpr long c_key_effect_light =                 150;        // Light Effect
constexpr long c_key_effect_water =                 151;        // Water Effect
constexpr long c_key_effect_fire =                  152;        // Fire Effect
constexpr long c_key_effect_mirror =                153;        // Mirror Effect
constexpr long c_key_effect_fisheye =               154;        // Fisheye Effect
constexpr long c_key_effect_swirl =                 155;        // Swirl Effect


//####################################################################################
//##    Items                           200-249
//############################
constexpr long c_key_item_tile =                    200;        // Tile Item


//####################################################################################
//##    Prefabs                         250-299
//############################
constexpr long c_key_prefab_blob =                  250;        // Blob Prefab
constexpr long c_key_prefab_character =             251;        // Character Prefab
constexpr long c_key_prefab_foliage =               252;        // Foliage Prefab
constexpr long c_key_prefab_ground =                253;        // Ground Prefab
constexpr long c_key_prefab_ladder =                254;        // Ladder Prefab
constexpr long c_key_prefab_object =                255;        // Object Prefab
constexpr long c_key_prefab_spike =                 256;        // Spike Prefab
constexpr long c_key_prefab_spring =                257;        // Spring Prefab


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
constexpr long c_key_image_slope_left =             373;         // 128 x 128    Slope Left
constexpr long c_key_image_slope_right =            374;         // 128 x 128    Slope Right


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
constexpr long c_key_image_isometric_cube_grass =   504;        // 128 x 128    Isometric Cube Grass
constexpr long c_key_image_isometric_cube_ice =     505;        // 128 x 128    Isometric Cube Ice

constexpr long c_key_image_isometric_pyramid_1 =    511;        // 128 x 128    Isometric Pyramid 1
constexpr long c_key_image_isometric_pyramid_2 =    512;        // 128 x 128    Isometric Pyramid 2
constexpr long c_key_image_isometric_pyramid_3 =    513;        // 128 x 128    Isometric Pyramid 3
constexpr long c_key_image_isometric_sphere_1 =     514;        // 128 x 128    Isometric Sphere 1
constexpr long c_key_image_isometric_sphere_2 =     515;        // 128 x 128    Isometric Sphere 2
constexpr long c_key_image_isometric_diamond_1 =    517;        // 128 x 128    Isometric Diamond 1
constexpr long c_key_image_isometric_spikes =       518;        // 128 x 128    Isometric Spikes
constexpr long c_key_image_isometric_star =         519;        // 128 x 128    Isometric Star
constexpr long c_key_image_isometric_column_1 =     520;        // 128 x 128    Isometric Column 1
constexpr long c_key_image_isometric_column_2 =     521;        // 128 x 128    Isometric Column 2
constexpr long c_key_image_isometric_flower_1 =     522;        // 128 x 128    Isometric Flower 1
constexpr long c_key_image_isometric_mountain =     523;        // 128 x 128    Isometric Mountain
constexpr long c_key_image_isometric_mushroom =     524;        // 128 x 128    Isometric Mushroom
constexpr long c_key_image_isometric_rock_1 =       525;        // 128 x 128    Isometric Rock 1
constexpr long c_key_image_isometric_rock_2 =       526;        // 128 x 128    Isometric Rock 2
constexpr long c_key_image_isometric_rock_3 =       527;        // 128 x 128    Isometric Rock 3
constexpr long c_key_image_isometric_tree_1 =       528;        // 128 x 128    Isometric Tree 1
constexpr long c_key_image_isometric_tree_2 =       529;        // 128 x 128    Isometric Tree 2


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
constexpr long c_key_image_arrow_3 =                568;        // 128 x 128    Arrow 3
constexpr long c_key_image_reset =                  569;        // 128 x 128    Reset Arrow
constexpr long c_key_image_fish =                   570;        // 128 x 128    Fish


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
constexpr long c_key_image_bricks_left =            621;        // 128 x 128    Bricks
constexpr long c_key_image_bricks_right =           622;        // 128 x 128    Bricks
constexpr long c_key_image_dirt =                   623;        // 128 x 128    Dirt
constexpr long c_key_image_dirt_block =             624;        // 128 x 128    Dirt
constexpr long c_key_image_dirt_left =              625;        // 128 x 128    Dirt
constexpr long c_key_image_dirt_right =             626;        // 128 x 128    Dirt
constexpr long c_key_image_ice =                    627;        // 128 x 128    Ice
constexpr long c_key_image_ice_left =               628;        // 128 x 128    Ice
constexpr long c_key_image_ice_right =              629;        // 128 x 128    Ice
constexpr long c_key_image_sand =                   630;        // 128 x 128    Sand
constexpr long c_key_image_sand_left =              631;        // 128 x 128    Sand
constexpr long c_key_image_sand_right =             632;        // 128 x 128    Sand
constexpr long c_key_image_stone =                  633;        // 128 x 128    Stone
constexpr long c_key_image_stone_left =             634;        // 128 x 128    Stone
constexpr long c_key_image_stone_right =            635;        // 128 x 128    Stone
constexpr long c_key_image_tile =                   636;        // 128 x 128    Tile
constexpr long c_key_image_tile_left =              637;        // 128 x 128    Tile
constexpr long c_key_image_tile_right =             638;        // 128 x 128    Tile
constexpr long c_key_image_water =                  639;        // 128 x 128    Water
constexpr long c_key_image_water_top =              640;        // 128 x 128    Water Top
constexpr long c_key_image_wood =                   641;        // 128 x 128    Wood
constexpr long c_key_image_wood_left =              642;        // 128 x 128    Wood
constexpr long c_key_image_wood_right =             643;        // 128 x 128    Wood
constexpr long c_key_image_wall_1 =                 644;        // 128 x 128    Background Wall 1
constexpr long c_key_image_wall_2 =                 645;        // 128 x 128    Background Wall 2
constexpr long c_key_image_window =                 646;        // 128 x 128    Window
constexpr long c_key_image_bridge_left =            647;        // 128 x 128    Bridge Left
constexpr long c_key_image_bridge_mid =             648;        // 128 x 128    Bridge Middle
constexpr long c_key_image_bridge_right =           649;        // 128 x 128    Bridge Right
constexpr long c_key_image_platform_left =          650;        // 128 x 128    Platform Left
constexpr long c_key_image_platform_mid =           651;        // 128 x 128    Platform Middle
constexpr long c_key_image_platform_right =         652;        // 128 x 128    Platform Right
constexpr long c_key_image_logs =                   653;        // 128 x 128    Bridge Logs


//####################################################################################
//##    Built In Images - Decoration    700-749
//############################
constexpr long c_key_image_bush_1 =                 701;        // 128 x 128    Bush
constexpr long c_key_image_bush_2 =                 702;        // 128 x 128    Bush w/ Flower
constexpr long c_key_image_bush_3 =                 703;        // 128 x 128    Small Bush
constexpr long c_key_image_cloud_1 =                704;        // 128 x 128    Cloud 1
constexpr long c_key_image_cloud_2 =                705;        // 128 x 128    Cloud 2
constexpr long c_key_image_flower_1 =               706;        // 128 x 128    Flower 1
constexpr long c_key_image_grass_1 =                707;        // 128 x 128    Grass 1
constexpr long c_key_image_grass_2 =                708;        // 128 x 128    Grass 2
constexpr long c_key_image_grass_3 =                709;        // 128 x 128    Grass 3
constexpr long c_key_image_ladder_1 =               710;        // 128 x 128    Ladder 1
constexpr long c_key_image_ladder_2 =               711;        // 128 x 128    Ladder 2
constexpr long c_key_image_ladder_top =             712;        // 128 x 128    Ladder Top
constexpr long c_key_image_mushroom =               713;        // 128 x 128    Mushroom
constexpr long c_key_image_rock_1 =                 714;        // 128 x 128    Rock 2
constexpr long c_key_image_rock_2 =                 715;        // 128 x 128    Rock 2
constexpr long c_key_image_sign_1 =                 716;        // 128 x 128    Sign Blank
constexpr long c_key_image_sign_2 =                 717;        // 128 x 128    Sign Arrow
constexpr long c_key_image_sign_post =              718;        // 128 x 128    Sign Post
constexpr long c_key_image_sign_post_top =          719;        // 128 x 128    Sign Post Top
constexpr long c_key_image_sign_warning =           720;        // 128 x 128    Sign Warning
constexpr long c_key_image_sun =                    721;        // 128 x 128    Sun

//##    Items                            750-799




//####################################################################################
//##    Characters / Enemies             800-899
//############################




//####################################################################################
//##    UI                               900-999
//############################





#endif // CONSTANTS_KEYS_H











