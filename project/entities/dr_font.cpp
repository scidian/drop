//
//      Created by Stephens Nunnally on 3/21/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "project/constants_component_info.h"
#include "project/entities/dr_font.h"
#include "project/settings/settings_component.h"
#include "project/settings/settings_component_property.h"


//####################################################################################
//##    Constructor
//####################################################################################
DrFont::DrFont(DrProject *parent_project, long key,
               std::string font_name, DrBitmap font_bitmap, std::string font_family, int font_size, bool use_test_rects) : DrSettings(parent_project) {
    this->setKey(key);

    m_name =   font_name;
    m_bitmap = font_bitmap;

    if (use_test_rects)
        setTestFontRects();

    p_font_family =     font_family;
    p_font_size =       font_size;
    p_font_padding =    2;
    p_main_color =      Dr::white;

    p_outline =         false;
    p_outline_color =   Dr::black;
    p_outline_width =   1;

    p_shadow = false;
    p_shadow_color =    DrColor(205, 205, 205, 128);
    p_shadow_offset =   DrPoint(4, -4);

    p_gradient =        false;
    p_gradient_color =  Dr::red;


    // ***** Initialize Font Settings
    DrProperty *property_name = getComponentProperty(Comps::Entity_Settings, Props::Entity_Name);
    property_name->setDisplayName("Font Name");
    property_name->setDescription("Name of this Font Asset.");
    property_name->setValue(m_name);

    addComponent(Comps::Asset_Settings_Font, "Font Settings", "Font settings for this Text Asset.", Component_Colors::Orange_Pastel, true);
    getComponent(Comps::Asset_Settings_Font)->setIcon(Component_Icons::Font);

    addPropertyToComponent(Comps::Asset_Settings_Font, Props::Asset_Font_Family, Property_Type::String, getPropertyFontFamily(),
                           "Font Family", "Font used for this text asset.", false, false);
    addPropertyToComponent(Comps::Asset_Settings_Font, Props::Asset_Font_Size, Property_Type::Int, getPropertyFontSize(),
                           "Font Size", "Font size of this text asset.", false, false);
}


//####################################################################################
//##    Grabs test rects for temp font generation
//####################################################################################
void DrFont::setTestFontRects() {
    m_positions['A'] = DrRect(  0,   0, 32, 32);
    m_positions['B'] = DrRect( 32,   0, 32, 32);
    m_positions['C'] = DrRect( 64,   0, 32, 32);
    m_positions['D'] = DrRect( 96,   0, 32, 32);
    m_positions['E'] = DrRect(128,   0, 32, 32);
    m_positions['F'] = DrRect(160,   0, 32, 32);
    m_positions['G'] = DrRect(192,   0, 32, 32);
    m_positions['H'] = DrRect(224,   0, 32, 32);
    m_positions['I'] = DrRect(256,   0, 32, 32);
    m_positions['J'] = DrRect(288,   0, 32, 32);
    m_positions['K'] = DrRect(320,   0, 32, 32);
    m_positions['L'] = DrRect(352,   0, 32, 32);
    m_positions['M'] = DrRect(384,   0, 32, 32);
    m_positions['N'] = DrRect(416,   0, 32, 32);
    m_positions['O'] = DrRect(448,   0, 32, 32);
    m_positions['P'] = DrRect(480,   0, 32, 32);

    m_positions['Q'] = DrRect(  0,  32, 32, 32);
    m_positions['R'] = DrRect( 32,  32, 32, 32);
    m_positions['S'] = DrRect( 64,  32, 32, 32);
    m_positions['T'] = DrRect( 96,  32, 32, 32);
    m_positions['U'] = DrRect(128,  32, 32, 32);
    m_positions['V'] = DrRect(160,  32, 32, 32);
    m_positions['W'] = DrRect(192,  32, 32, 32);
    m_positions['X'] = DrRect(224,  32, 32, 32);
    m_positions['Y'] = DrRect(256,  32, 32, 32);
    m_positions['Z'] = DrRect(288,  32, 32, 32);
    m_positions['a'] = DrRect(320,  32, 32, 32);
    m_positions['b'] = DrRect(352,  32, 32, 32);
    m_positions['c'] = DrRect(384,  32, 32, 32);
    m_positions['d'] = DrRect(416,  32, 32, 32);
    m_positions['e'] = DrRect(448,  32, 32, 32);
    m_positions['f'] = DrRect(480,  32, 32, 32);

    m_positions['g'] = DrRect(  0,  64, 32, 32);
    m_positions['h'] = DrRect( 32,  64, 32, 32);
    m_positions['i'] = DrRect( 64,  64, 32, 32);
    m_positions['j'] = DrRect( 96,  64, 32, 32);
    m_positions['k'] = DrRect(128,  64, 32, 32);
    m_positions['l'] = DrRect(160,  64, 32, 32);
    m_positions['m'] = DrRect(192,  64, 32, 32);
    m_positions['n'] = DrRect(224,  64, 32, 32);
    m_positions['o'] = DrRect(256,  64, 32, 32);
    m_positions['p'] = DrRect(288,  64, 32, 32);
    m_positions['q'] = DrRect(320,  64, 32, 32);
    m_positions['r'] = DrRect(352,  64, 32, 32);
    m_positions['s'] = DrRect(384,  64, 32, 32);
    m_positions['t'] = DrRect(416,  64, 32, 32);
    m_positions['u'] = DrRect(448,  64, 32, 32);
    m_positions['v'] = DrRect(480,  64, 32, 32);

    m_positions['w'] = DrRect(  0,  96, 32, 32);
    m_positions['x'] = DrRect( 32,  96, 32, 32);
    m_positions['y'] = DrRect( 64,  96, 32, 32);
    m_positions['z'] = DrRect( 96,  96, 32, 32);
    m_positions['0'] = DrRect(128,  96, 32, 32);
    m_positions['1'] = DrRect(160,  96, 32, 32);
    m_positions['2'] = DrRect(192,  96, 32, 32);
    m_positions['3'] = DrRect(224,  96, 32, 32);
    m_positions['4'] = DrRect(256,  96, 32, 32);
    m_positions['5'] = DrRect(288,  96, 32, 32);
    m_positions['6'] = DrRect(320,  96, 32, 32);
    m_positions['7'] = DrRect(352,  96, 32, 32);
    m_positions['8'] = DrRect(384,  96, 32, 32);
    m_positions['9'] = DrRect(416,  96, 32, 32);
    m_positions['.'] = DrRect(448,  96, 32, 32);
    m_positions[' '] = DrRect(480,  96, 32, 32);
}





















