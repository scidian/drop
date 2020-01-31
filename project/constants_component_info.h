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
    const std::string NoComponent          { "S001" };
    const std::string NoProperty           { "S002" };

}

//####################################################################################
//##    Possible background colors for Component Categories
//############################
namespace Component_Colors {
    const DrColor White_Snow        { DrColor(255, 252, 249, 255) };
    const DrColor Silver_Snow       { DrColor(192, 188, 183, 255) };
    const DrColor Darkness          { DrColor( 40,  33,  33, 255) };

    const DrColor Orange_Pastel     { DrColor(250, 146,   0, 255) };

    const DrColor Pink_Pearl        { DrColor(228, 180, 194, 255) };
    const DrColor Red_Faded         { DrColor(255, 105, 120, 255) };

    const DrColor Red_Tuscan        { DrColor(104,  71,  86, 255) };
    const DrColor Purple_Pastel     { DrColor(176, 161, 186, 255) };
    const DrColor Purple_Royal      { DrColor(126,  87, 194, 255) };
    const DrColor Blue_Yonder       { DrColor(165, 181, 191, 255) };
    const DrColor Blue_Royal        { DrColor( 57, 120, 237, 255) };

    const DrColor Green_SeaGrass    { DrColor(154, 225, 157, 255) };
    const DrColor Green_Sickness    { DrColor(203, 240,  19, 255) };

    const DrColor Mustard_Yellow    { DrColor(255, 200,  87, 255) };
    const DrColor Mellow_Yellow     { DrColor(250, 223, 127, 255) };

    const DrColor Brown_Sugar       { DrColor(165, 117,  72, 255) };
    const DrColor Beige_Apricot     { DrColor(252, 215, 173, 255) };

    const DrColor Blue_Drop_1       { DrColor(  0, 150, 145, 255) };
    const DrColor Blue_Drop_2       { DrColor(  0, 180, 175, 255) };
    const DrColor Blue_Drop_3       { DrColor(  0, 225, 219, 255) };
    const DrColor Blue_Drop_4       { DrColor( 96, 255, 248, 255) };
    const DrColor Blue_Drop_5       { DrColor(192, 255, 248, 255) };
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
    const std::string Camera        { std::string(":/assets/inspector_icons/comp_camera.png") };
    const std::string Character     { std::string(":/assets/inspector_icons/comp_character.png") };
    const std::string Font          { std::string(":/assets/inspector_icons/comp_font.png") };
    const std::string Object        { std::string(":/assets/inspector_icons/comp_object.png") };

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
};


#endif // CONSTANTS_COMPONENTS_H







