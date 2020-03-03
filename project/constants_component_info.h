//
//      Created by Stephens Nunnally on 12/19/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef CONSTANTS_COMPONENTS_H
#define CONSTANTS_COMPONENTS_H

#include "core/types/dr_color.h"


//####################################################################################
//##    Local Error Codes
//############################
namespace Error_Code {
    const std::string   NoComponent         { "S001" };
    const std::string   NoProperty          { "S002" };

}

//####################################################################################
//##    Possible background colors for Component Categories
//############################
namespace Component_Colors {
    const DrColor Blue_Drop_1       { DrColor(  0, 150, 145, 255) };
    const DrColor Blue_Drop_2       { DrColor(  0, 180, 175, 255) };
    const DrColor Blue_Drop_3       { DrColor(  0, 225, 219, 255) };
    const DrColor Blue_Drop_4       { DrColor( 96, 255, 248, 255) };
    const DrColor Blue_Drop_5       { DrColor(192, 255, 248, 255) };

    // Material Palette Row 4
    const DrColor RGB_01_Red        { DrColor(239,  83,  80, 255) };
    const DrColor RGB_02_Pink       { DrColor(236,  64, 122, 255) };
    const DrColor RGB_03_Violet     { DrColor(171,  71, 188, 255) };
    const DrColor RGB_04_Purple     { DrColor(149, 117, 205, 255) };    // *Row 3
    const DrColor RGB_05_Royal      { DrColor(121, 134, 203, 255) };    // *Row 3
    const DrColor RGB_06_Blue       { DrColor( 66, 165, 245, 255) };
    const DrColor RGB_07_LightBlue  { DrColor( 42, 182, 246, 255) };
    const DrColor RGB_08_SeaFoam    { DrColor( 38, 198, 218, 255) };    // *Close to Drop Blue
    const DrColor RGB_09_Forest     { DrColor( 38, 166, 154, 255) };
    const DrColor RGB_10_Green      { DrColor(102, 187, 106, 255) };
    const DrColor RGB_11_Lime       { DrColor(157, 204, 102, 255) };
    const DrColor RGB_12_Olive      { DrColor(212, 225,  87, 255) };
    const DrColor RGB_13_Yellow     { DrColor(255, 238,  89, 255) };
    const DrColor RGB_14_SchoolBus  { DrColor(255, 202,  40, 255) };
    const DrColor RGB_15_Sunset     { DrColor(255, 167,  39, 255) };
    const DrColor RGB_16_Orange     { DrColor(255, 112,  67, 255) };
    const DrColor RGB_17_Brown      { DrColor(141, 110,  99, 255) };
    const DrColor RGB_18_Gray       { DrColor(224, 224, 224, 255) };    // *Row 3
    const DrColor RGB_19_Silver     { DrColor(144, 164, 174, 255) };    // *Row 3
    const DrColor RGB_20_Tan        { DrColor(233, 216, 202, 255) };
};


//####################################################################################
//##    Possible icons for Component Categories
//############################
namespace Component_Icons {
    // General Components
    const std::string None          { std::string("") };
    const std::string Name          { std::string(":/assets/inspector_icons/comp_name.png") };
    const std::string Hidden        { std::string(":/assets/inspector_icons/comp_hidden.png") };
    const std::string Settings      { std::string(":/assets/inspector_icons/comp_settings.png") };
    const std::string Physics       { std::string(":/assets/inspector_icons/comp_physics.png") };
    const std::string Transform     { std::string(":/assets/inspector_icons/comp_transform.png") };
    const std::string Layering      { std::string(":/assets/inspector_icons/comp_layering.png") };
    const std::string Movement      { std::string(":/assets/inspector_icons/comp_movement.png") };
    const std::string Spawn         { std::string(":/assets/inspector_icons/comp_spawn.png") };
    const std::string Appearance    { std::string(":/assets/inspector_icons/comp_appearance.png") };
    const std::string Collide       { std::string(":/assets/inspector_icons/comp_collide.png") };
    const std::string Animation     { std::string(":/assets/inspector_icons/comp_animation.png") };
    const std::string Effects       { std::string(":/assets/inspector_icons/comp_effects.png") };
    const std::string Health        { std::string(":/assets/inspector_icons/comp_health.png") };
    const std::string Controls      { std::string(":/assets/inspector_icons/comp_controls.png") };

    // Thing Components
    const std::string Character     { std::string(":/assets/inspector_icons/comp_character.png") };
    const std::string Font          { std::string(":/assets/inspector_icons/comp_font.png") };
    const std::string Object        { std::string(":/assets/inspector_icons/comp_object.png") };

    // Device Components
    const std::string Camera        { std::string(":/assets/inspector_icons/comp_camera.png") };

    // Effect Components
    const std::string Fire          { std::string(":/assets/inspector_icons/comp_fire.png") };
    const std::string Fisheye       { std::string(":/assets/inspector_icons/comp_fisheye.png") };
    const std::string Light         { std::string(":/assets/inspector_icons/comp_light.png") };
    const std::string Mirror        { std::string(":/assets/inspector_icons/comp_mirror.png") };
    const std::string Swirl         { std::string(":/assets/inspector_icons/comp_swirl.png") };
    const std::string Water         { std::string(":/assets/inspector_icons/comp_water.png") };
    const std::string Water_Ripple  { std::string(":/assets/inspector_icons/comp_water_ripple.png") };
    const std::string Water_Wave    { std::string(":/assets/inspector_icons/comp_water_wave.png") };
    const std::string Water_Refract { std::string(":/assets/inspector_icons/comp_water_refract.png") };
    const std::string Water_Foam    { std::string(":/assets/inspector_icons/comp_water_foam.png") };

    // Item Components
    const std::string Foliage       { std::string(":/assets/inspector_icons/comp_items.png") };
};


#endif // CONSTANTS_COMPONENTS_H







