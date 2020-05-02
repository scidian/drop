//
//      Created by Stephens Nunnally on 1/21/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Holds all of our custom colors
//
//
#include "core/colors/colors.h"
#include "core/dr_math.h"
#include "core/dr_random.h"
#include "core/types/dr_color.h"
#include "project/enums_entity_types.h"

typedef std::map<Color_Scheme,   std::map<Window_Colors, DrColor>> Color_Scheme_Map;
typedef std::map<Color_Palettes, Palette_Info> Color_Palette_Map;

namespace Dr {


//####################################################################################
//##    Local Static Variables
//####################################################################################
static Color_Scheme      l_current_color_scheme = Color_Scheme::Dark;
static Color_Scheme_Map  l_color_schemes;
static Color_Palette_Map l_color_palettes;                                      // Stores color palettes

std::string BorderWidth()       { return "1px"; }                               // Project wide border width for Style Sheets, as QString
int         BorderWidthAsInt()  { return 1; }                                   // Project wide border width for Style Sheets, as Int


//####################################################################################
//##    Color Helper Functions
//####################################################################################
// Compares 2 colors, returns true if they are the same
bool IsSameColor(const DrColor &color1, const DrColor &color2, double tolerance) {
    return ( Dr::IsCloseTo(color1.redF(),   color2.redF(),   tolerance) &&
             Dr::IsCloseTo(color1.greenF(), color2.greenF(), tolerance) &&
             Dr::IsCloseTo(color1.blueF(),  color2.blueF(),  tolerance) &&
             Dr::IsCloseTo(color1.alphaF(), color2.alphaF(), tolerance) );
}

// Returns a random color with fully opaque alpha
DrColor RandomColor() {
    DrColor color(0, 0, 0, 255);
    color.setRed(   RandomInt(0, 256) );
    color.setGreen( RandomInt(0, 256) );
    color.setBlue(  RandomInt(0, 256) );
    return color;
}


//####################################################################################
//##    Retreive custom colors
//####################################################################################
Color_Scheme    GetColorScheme()                                { return l_current_color_scheme; }
void            SetColorScheme(Color_Scheme new_scheme)         { l_current_color_scheme = new_scheme; }

DrColor         GetColor(Window_Colors color_role)              { return l_color_schemes[l_current_color_scheme][color_role]; }

DrColor         GetColorFromPalette(Color_Palettes palette, int color_index) {
    if (color_index >= 0 && color_index <= (l_color_palettes[palette].number_of_colors - 1))
        return l_color_palettes[palette].colors[color_index];
    else
        return DrColor(0, 0, 0, 0);
}

int             GetPaletteColorCount(Color_Palettes palette)    { return l_color_palettes[palette].number_of_colors; }
std::string     GetPaletteName(Color_Palettes palette)          { return l_color_palettes[palette].name; }
bool            GetPaletteShowInList(Color_Palettes palette)    { return l_color_palettes[palette].show_in_list; }


//####################################################################################
//##    Store custom window palette colors
//####################################################################################
void LoadPalettes() {
    LoadPaletteBasic(        l_color_palettes[Color_Palettes::Basic] );
    LoadPaletteBlank(        l_color_palettes[Color_Palettes::Blank] );
    LoadPaletteMaterial(     l_color_palettes[Color_Palettes::Material] );
    LoadPaletteRockyRover(   l_color_palettes[Color_Palettes::Rocky_Rover] );
    LoadPaletteWindowThemes( l_color_palettes[Color_Palettes::Window_Themes] );

    l_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Shadow,                DrColor(  0,   0,   0)));
    l_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Seperator,             DrColor( 16,  16,  16)));
    l_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Background_Dark,       DrColor( 24,  24,  24)));
    l_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Background_Light,      DrColor( 32,  32,  32)));
    l_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Button_Dark,           DrColor( 40,  40,  40)));
    l_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Button_Light,          DrColor( 60,  60,  60)));
    l_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Midlight,              DrColor( 85,  85,  85)));
    l_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Text_Dark,             DrColor(100, 100, 100)));
    l_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Text,                  DrColor(205, 205, 205)));
    l_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Text_Light,            DrColor(225, 225, 225)));
    l_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Highlight,             DrColor(255, 255, 255)));
    l_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Header_Text,           DrColor(225, 225, 225)));
    l_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Header_Fade,           DrColor( 20,  20,  20)));
    l_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Icon_Dark,             DrColor(  0, 150, 145)));
    l_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Icon_Light,            DrColor(  0, 180, 175)));

    l_color_schemes[Color_Scheme::Mid].insert(std::make_pair(Window_Colors::Shadow,                 DrColor( 30,  30,  30)));
    l_color_schemes[Color_Scheme::Mid].insert(std::make_pair(Window_Colors::Seperator,              DrColor( 51,  51,  51)));
    l_color_schemes[Color_Scheme::Mid].insert(std::make_pair(Window_Colors::Background_Dark,        DrColor( 71,  71,  71)));
    l_color_schemes[Color_Scheme::Mid].insert(std::make_pair(Window_Colors::Background_Light,       DrColor( 91,  91,  91)));
    l_color_schemes[Color_Scheme::Mid].insert(std::make_pair(Window_Colors::Button_Dark,            DrColor(112, 112, 112)));
    l_color_schemes[Color_Scheme::Mid].insert(std::make_pair(Window_Colors::Button_Light,           DrColor(132, 132, 132)));
    l_color_schemes[Color_Scheme::Mid].insert(std::make_pair(Window_Colors::Midlight,               DrColor(153, 153, 153)));
    l_color_schemes[Color_Scheme::Mid].insert(std::make_pair(Window_Colors::Text_Dark,              DrColor(178, 178, 178)));
    l_color_schemes[Color_Scheme::Mid].insert(std::make_pair(Window_Colors::Text,                   DrColor(204, 204, 204)));
    l_color_schemes[Color_Scheme::Mid].insert(std::make_pair(Window_Colors::Text_Light,             DrColor(229, 229, 229)));
    l_color_schemes[Color_Scheme::Mid].insert(std::make_pair(Window_Colors::Highlight,              DrColor(255, 255, 255)));
    l_color_schemes[Color_Scheme::Mid].insert(std::make_pair(Window_Colors::Header_Text,            DrColor(229, 229, 229)));
    l_color_schemes[Color_Scheme::Mid].insert(std::make_pair(Window_Colors::Header_Fade,            DrColor(127,   8, 117)));
    l_color_schemes[Color_Scheme::Mid].insert(std::make_pair(Window_Colors::Icon_Dark,              DrColor(216,   4, 201)));
    l_color_schemes[Color_Scheme::Mid].insert(std::make_pair(Window_Colors::Icon_Light,             DrColor(255,  19, 237)));

    l_color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Shadow,               DrColor(230, 230, 230)));
    l_color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Seperator,            DrColor(200, 200, 200)));
    l_color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Background_Dark,      DrColor(180, 180, 180)));
    l_color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Background_Light,     DrColor(190, 190, 190)));
    l_color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Button_Dark,          DrColor(170, 170, 170)));
    l_color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Button_Light,         DrColor(140, 140, 140)));
    l_color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Midlight,             DrColor(220, 220, 220)));
    l_color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Text_Dark,            DrColor( 80,  80,  80)));
    l_color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Text,                 DrColor( 50,  50,  50)));
    l_color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Text_Light,           DrColor( 25,  25,  25)));
    l_color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Highlight,            DrColor(  4,   4,   4)));
    l_color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Header_Text,          DrColor(255, 255, 255)));
    l_color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Header_Fade,          DrColor(  0,  80,  75)));
    l_color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Icon_Dark,            DrColor(  0, 160, 155)));
    l_color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Icon_Light,           DrColor(  0, 190, 185)));

    l_color_schemes[Color_Scheme::Navy].insert(std::make_pair(Window_Colors::Shadow,                DrColor(  5,   6,   7)));
    l_color_schemes[Color_Scheme::Navy].insert(std::make_pair(Window_Colors::Seperator,             DrColor( 14,  16,  20)));
    l_color_schemes[Color_Scheme::Navy].insert(std::make_pair(Window_Colors::Background_Dark,       DrColor( 24,  27,  34)));
    l_color_schemes[Color_Scheme::Navy].insert(std::make_pair(Window_Colors::Background_Light,      DrColor( 32,  37,  45)));
    l_color_schemes[Color_Scheme::Navy].insert(std::make_pair(Window_Colors::Button_Dark,           DrColor( 40,  47,  57)));
    l_color_schemes[Color_Scheme::Navy].insert(std::make_pair(Window_Colors::Button_Light,          DrColor( 50,  58,  72)));
    l_color_schemes[Color_Scheme::Navy].insert(std::make_pair(Window_Colors::Midlight,              DrColor( 64,  74,  91)));
    l_color_schemes[Color_Scheme::Navy].insert(std::make_pair(Window_Colors::Text_Dark,             DrColor(103, 120, 147)));
    l_color_schemes[Color_Scheme::Navy].insert(std::make_pair(Window_Colors::Text,                  DrColor(139, 162, 199)));
    l_color_schemes[Color_Scheme::Navy].insert(std::make_pair(Window_Colors::Text_Light,            DrColor(153, 179, 219)));
    l_color_schemes[Color_Scheme::Navy].insert(std::make_pair(Window_Colors::Highlight,             DrColor(177, 207, 254)));
    l_color_schemes[Color_Scheme::Navy].insert(std::make_pair(Window_Colors::Header_Text,           DrColor(189, 214, 254)));
    l_color_schemes[Color_Scheme::Navy].insert(std::make_pair(Window_Colors::Header_Fade,           DrColor(  9,  27,  78)));
    l_color_schemes[Color_Scheme::Navy].insert(std::make_pair(Window_Colors::Icon_Dark,             DrColor( 23,  64, 181)));
    l_color_schemes[Color_Scheme::Navy].insert(std::make_pair(Window_Colors::Icon_Light,            DrColor( 26,  72, 207)));

    l_color_schemes[Color_Scheme::Grape].insert(std::make_pair(Window_Colors::Shadow,               DrColor(  6,   5,   7)));
    l_color_schemes[Color_Scheme::Grape].insert(std::make_pair(Window_Colors::Seperator,            DrColor( 19,  15,  22)));
    l_color_schemes[Color_Scheme::Grape].insert(std::make_pair(Window_Colors::Background_Dark,      DrColor( 28,  23,  33)));
    l_color_schemes[Color_Scheme::Grape].insert(std::make_pair(Window_Colors::Background_Light,     DrColor( 37,  30,  43)));
    l_color_schemes[Color_Scheme::Grape].insert(std::make_pair(Window_Colors::Button_Dark,          DrColor( 51,  41,  59)));
    l_color_schemes[Color_Scheme::Grape].insert(std::make_pair(Window_Colors::Button_Light,         DrColor( 61,  50,  71)));
    l_color_schemes[Color_Scheme::Grape].insert(std::make_pair(Window_Colors::Midlight,             DrColor( 79,  64,  91)));
    l_color_schemes[Color_Scheme::Grape].insert(std::make_pair(Window_Colors::Text_Dark,            DrColor(125, 101, 144)));
    l_color_schemes[Color_Scheme::Grape].insert(std::make_pair(Window_Colors::Text,                 DrColor(172, 139, 198)));
    l_color_schemes[Color_Scheme::Grape].insert(std::make_pair(Window_Colors::Text_Light,           DrColor(191, 154, 220)));
    l_color_schemes[Color_Scheme::Grape].insert(std::make_pair(Window_Colors::Highlight,            DrColor(221, 178, 255)));
    // Gold highlights
    ///l_color_schemes[Color_Scheme::Grape].insert(std::make_pair(Window_Colors::Header_Text,       DrColor(  6,   5,   7)));
    ///l_color_schemes[Color_Scheme::Grape].insert(std::make_pair(Window_Colors::Header_Fade,       DrColor( 61,  50,  17)));
    ///l_color_schemes[Color_Scheme::Grape].insert(std::make_pair(Window_Colors::Icon_Dark,         DrColor(224, 185,   1)));
    ///l_color_schemes[Color_Scheme::Grape].insert(std::make_pair(Window_Colors::Icon_Light,        DrColor(249, 206,   1)));
    l_color_schemes[Color_Scheme::Grape].insert(std::make_pair(Window_Colors::Header_Text,          DrColor(227, 191, 255)));
    l_color_schemes[Color_Scheme::Grape].insert(std::make_pair(Window_Colors::Header_Fade,          DrColor( 39,   3,  64)));
    l_color_schemes[Color_Scheme::Grape].insert(std::make_pair(Window_Colors::Icon_Dark,            DrColor(134,  12, 218)));
    l_color_schemes[Color_Scheme::Grape].insert(std::make_pair(Window_Colors::Icon_Light,           DrColor(150,  14, 244)));

    l_color_schemes[Color_Scheme::Rust].insert(std::make_pair(Window_Colors::Shadow,                DrColor(  7,   5,   5)));
    l_color_schemes[Color_Scheme::Rust].insert(std::make_pair(Window_Colors::Seperator,             DrColor( 20,  14,  15)));
    l_color_schemes[Color_Scheme::Rust].insert(std::make_pair(Window_Colors::Background_Dark,       DrColor( 32,  23,  24)));
    l_color_schemes[Color_Scheme::Rust].insert(std::make_pair(Window_Colors::Background_Light,      DrColor( 45,  31,  34)));
    l_color_schemes[Color_Scheme::Rust].insert(std::make_pair(Window_Colors::Button_Dark,           DrColor( 58,  41,  44)));
    l_color_schemes[Color_Scheme::Rust].insert(std::make_pair(Window_Colors::Button_Light,          DrColor( 71,  50,  53)));
    l_color_schemes[Color_Scheme::Rust].insert(std::make_pair(Window_Colors::Midlight,              DrColor( 93,  65,  69)));
    l_color_schemes[Color_Scheme::Rust].insert(std::make_pair(Window_Colors::Text_Dark,             DrColor(147, 104, 110)));
    l_color_schemes[Color_Scheme::Rust].insert(std::make_pair(Window_Colors::Text,                  DrColor(200, 141, 149)));
    l_color_schemes[Color_Scheme::Rust].insert(std::make_pair(Window_Colors::Text_Light,            DrColor(220, 155, 164)));
    l_color_schemes[Color_Scheme::Rust].insert(std::make_pair(Window_Colors::Highlight,             DrColor(254, 179, 189)));
    l_color_schemes[Color_Scheme::Rust].insert(std::make_pair(Window_Colors::Header_Text,           DrColor(254, 192, 200)));
    l_color_schemes[Color_Scheme::Rust].insert(std::make_pair(Window_Colors::Header_Fade,           DrColor( 51,  11,  10)));
    l_color_schemes[Color_Scheme::Rust].insert(std::make_pair(Window_Colors::Icon_Dark,             DrColor(149,  24,  21)));
    l_color_schemes[Color_Scheme::Rust].insert(std::make_pair(Window_Colors::Icon_Light,            DrColor(175,  28,  25)));

    l_color_schemes[Color_Scheme::Coffee].insert(std::make_pair(Window_Colors::Shadow,              DrColor(  9,   7,   5)));
    l_color_schemes[Color_Scheme::Coffee].insert(std::make_pair(Window_Colors::Seperator,           DrColor( 22,  19,  14)));
    l_color_schemes[Color_Scheme::Coffee].insert(std::make_pair(Window_Colors::Background_Dark,     DrColor( 31,  27,  20)));
    l_color_schemes[Color_Scheme::Coffee].insert(std::make_pair(Window_Colors::Background_Light,    DrColor( 43,  37,  27)));
    l_color_schemes[Color_Scheme::Coffee].insert(std::make_pair(Window_Colors::Button_Dark,         DrColor( 58,  50,  37)));
    l_color_schemes[Color_Scheme::Coffee].insert(std::make_pair(Window_Colors::Button_Light,        DrColor( 72,  62,  46)));
    l_color_schemes[Color_Scheme::Coffee].insert(std::make_pair(Window_Colors::Midlight,            DrColor( 92,  78,  59)));
    l_color_schemes[Color_Scheme::Coffee].insert(std::make_pair(Window_Colors::Text_Dark,           DrColor(142, 121,  91)));
    l_color_schemes[Color_Scheme::Coffee].insert(std::make_pair(Window_Colors::Text,                DrColor(197, 168, 126)));
    l_color_schemes[Color_Scheme::Coffee].insert(std::make_pair(Window_Colors::Text_Light,          DrColor(218, 186, 139)));
    l_color_schemes[Color_Scheme::Coffee].insert(std::make_pair(Window_Colors::Highlight,           DrColor(255, 217, 162)));
    l_color_schemes[Color_Scheme::Coffee].insert(std::make_pair(Window_Colors::Header_Text,         DrColor(255, 222, 176)));
    l_color_schemes[Color_Scheme::Coffee].insert(std::make_pair(Window_Colors::Header_Fade,         DrColor( 51,  32,   7)));
    l_color_schemes[Color_Scheme::Coffee].insert(std::make_pair(Window_Colors::Icon_Dark,           DrColor(150,  94,  23)));
    l_color_schemes[Color_Scheme::Coffee].insert(std::make_pair(Window_Colors::Icon_Light,          DrColor(177, 111,  27)));

    l_color_schemes[Color_Scheme::Emerald].insert(std::make_pair(Window_Colors::Shadow,             DrColor(  7,   9,   6)));
    l_color_schemes[Color_Scheme::Emerald].insert(std::make_pair(Window_Colors::Seperator,          DrColor( 17,  23,  16)));
    l_color_schemes[Color_Scheme::Emerald].insert(std::make_pair(Window_Colors::Background_Dark,    DrColor( 23,  32,  22)));
    l_color_schemes[Color_Scheme::Emerald].insert(std::make_pair(Window_Colors::Background_Light,   DrColor( 32,  44,  31)));
    l_color_schemes[Color_Scheme::Emerald].insert(std::make_pair(Window_Colors::Button_Dark,        DrColor( 42,  59,  41)));
    l_color_schemes[Color_Scheme::Emerald].insert(std::make_pair(Window_Colors::Button_Light,       DrColor( 52,  72,  50)));
    l_color_schemes[Color_Scheme::Emerald].insert(std::make_pair(Window_Colors::Midlight,           DrColor( 66,  91,  64)));
    l_color_schemes[Color_Scheme::Emerald].insert(std::make_pair(Window_Colors::Text_Dark,          DrColor(106, 146, 103)));
    l_color_schemes[Color_Scheme::Emerald].insert(std::make_pair(Window_Colors::Text,               DrColor(144, 199, 140)));
    l_color_schemes[Color_Scheme::Emerald].insert(std::make_pair(Window_Colors::Text_Light,         DrColor(159, 220, 155)));
    l_color_schemes[Color_Scheme::Emerald].insert(std::make_pair(Window_Colors::Highlight,          DrColor(183, 253, 178)));
    // Brown Highlights
    ///l_color_schemes[Color_Scheme::Emerald].insert(std::make_pair(Window_Colors::Header_Text,     DrColor(183, 253, 178)));
    ///l_color_schemes[Color_Scheme::Emerald].insert(std::make_pair(Window_Colors::Header_Fade,     DrColor( 65,  48,  26)));
    ///l_color_schemes[Color_Scheme::Emerald].insert(std::make_pair(Window_Colors::Icon_Dark,       DrColor(160, 121,  67)));
    ///l_color_schemes[Color_Scheme::Emerald].insert(std::make_pair(Window_Colors::Icon_Light,      DrColor(187, 143,  79)));
    l_color_schemes[Color_Scheme::Emerald].insert(std::make_pair(Window_Colors::Header_Text,        DrColor(194, 253, 190)));
    l_color_schemes[Color_Scheme::Emerald].insert(std::make_pair(Window_Colors::Header_Fade,        DrColor(  6,  52,   8)));
    l_color_schemes[Color_Scheme::Emerald].insert(std::make_pair(Window_Colors::Icon_Dark,          DrColor( 22, 176,  29)));
    l_color_schemes[Color_Scheme::Emerald].insert(std::make_pair(Window_Colors::Icon_Light,         DrColor( 26, 202,  34)));

}

void LoadPaletteBasic(Palette_Info &palette) {
    palette.name = "Basic";
    palette.show_in_list = false;

    palette.colors.push_back( DrColor(0xFFFF0000) );    //  0 Red
    palette.colors.push_back( DrColor(0xFFFF8000) );    //  1 Orange
    palette.colors.push_back( DrColor(0xFFFFFF00) );    //  2 Yellow
    palette.colors.push_back( DrColor(0xFF00FF00) );    //  3 Green

    palette.colors.push_back( DrColor(0xFF00FFFF) );    //  4 Cyan
    palette.colors.push_back( DrColor(0xFF0000FF) );    //  5 Blue
    palette.colors.push_back( DrColor(0xFF800080) );    //  6 Purple
    palette.colors.push_back( DrColor(0xFFFF00FF) );    //  7 Pink

    palette.colors.push_back( DrColor(0xFFFFFFFF) );    //  8 White
    palette.colors.push_back( DrColor(0xFFAAAAAA) );    //  9 Light Gray
    palette.colors.push_back( DrColor(0xFF555555) );    // 10 Dark Gray
    palette.colors.push_back( DrColor(0xFF000000) );    // 11 Black

    palette.number_of_colors = static_cast<int>(palette.colors.size());
}


}   // namespace Dr



















