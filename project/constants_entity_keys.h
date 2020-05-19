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
//####################################################################################

//####################################################################################
//##        Starting Keys       !!!!! #NOTE: Don't allow keys to start at less than 1, having an item
//##                                         with key 0 (or less) could conflict with nullptr results
//############################

constexpr long c_starting_key_entity =              1001;       // First 1001 keys are currently reserved for built in Entities in DrProject
                                                                // ....."1001" is a relatively easily searchable number
constexpr long c_starting_key_component =           1;
constexpr long c_starting_key_property =            1;
constexpr long c_starting_key_slot =                1;


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
//##    Items                           300-399
//############################
constexpr long c_key_item_tile =                    300;        // Tile Item


//####################################################################################
//##    Prefabs                         500+
//############################
constexpr long c_key_prefab_blob =                  500;        // Blob Prefab
constexpr long c_key_prefab_character =             501;        // Character Prefab
constexpr long c_key_prefab_foliage =               502;        // Foliage Prefab
constexpr long c_key_prefab_ground =                503;        // Ground Prefab
constexpr long c_key_prefab_ladder =                504;        // Ladder Prefab
constexpr long c_key_prefab_object =                505;        // Object Prefab
constexpr long c_key_prefab_spike =                 506;        // Spike Prefab
constexpr long c_key_prefab_spring =                507;        // Spring Prefab


//####################################################################################
//##    Images
//############################
constexpr long c_key_external_image =               999;        // Key that represents using an external image


#endif // CONSTANTS_KEYS_H



















