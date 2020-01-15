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
static Color_Scheme      g_current_color_scheme = Color_Scheme::Dark;
static Color_Scheme_Map  g_color_schemes;
static Color_Palette_Map g_color_palettes;                                      // Stores color palettes

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
Color_Scheme    GetColorScheme()                                { return g_current_color_scheme; }
void            SetColorScheme(Color_Scheme new_scheme)         { g_current_color_scheme = new_scheme; }

DrColor         GetColor(Window_Colors color_role)              { return g_color_schemes[g_current_color_scheme][color_role]; }

DrColor         GetColorFromPalette(Color_Palettes palette, int color_index) {
    if (color_index >= 0 && color_index <= (g_color_palettes[palette].number_of_colors - 1))
        return g_color_palettes[palette].colors[color_index];
    else
        return DrColor(0, 0, 0, 0);
}

int             GetPaletteColorCount(Color_Palettes palette)    { return g_color_palettes[palette].number_of_colors; }
std::string     GetPaletteName(Color_Palettes palette)          { return g_color_palettes[palette].name; }
bool            GetPaletteShowInList(Color_Palettes palette)    { return g_color_palettes[palette].show_in_list; }


//####################################################################################
//##    Store custom window palette colors
//####################################################################################
void LoadPalettes() {
    LoadPaletteBasic(        g_color_palettes[Color_Palettes::Basic] );
    LoadPaletteBlank(        g_color_palettes[Color_Palettes::Blank] );
    LoadPaletteMaterial(     g_color_palettes[Color_Palettes::Material] );
    LoadPaletteRockyRover(   g_color_palettes[Color_Palettes::Rocky_Rover] );
    LoadPaletteWindowThemes( g_color_palettes[Color_Palettes::Window_Themes] );

    g_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Shadow,                DrColor(  0,   0,   0)));
    g_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Seperator,             DrColor( 16,  16,  16)));
    g_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Background_Dark,       DrColor( 24,  24,  24)));
    g_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Background_Light,      DrColor( 32,  32,  32)));
    g_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Button_Dark,           DrColor( 40,  40,  40)));
    g_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Button_Light,          DrColor( 60,  60,  60)));
    g_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Midlight,              DrColor( 85,  85,  85)));
    g_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Text_Dark,             DrColor(100, 100, 100)));
    g_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Text,                  DrColor(205, 205, 205)));
    g_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Text_Light,            DrColor(225, 225, 225)));
    g_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Highlight,             DrColor(255, 255, 255)));
    g_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Header_Text,           DrColor(225, 225, 225)));
    g_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Header_Fade,           DrColor( 20,  20,  20)));
    g_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Icon_Dark,             DrColor(  0, 150, 145)));
    g_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Icon_Light,            DrColor(  0, 180, 175)));

    g_color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Shadow,               DrColor(230, 230, 230)));
    g_color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Seperator,            DrColor(200, 200, 200)));
    g_color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Background_Dark,      DrColor(180, 180, 180)));
    g_color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Background_Light,     DrColor(190, 190, 190)));
    g_color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Button_Dark,          DrColor(170, 170, 170)));
    g_color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Button_Light,         DrColor(140, 140, 140)));
    g_color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Midlight,             DrColor(220, 220, 220)));
    g_color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Text_Dark,            DrColor( 80,  80,  80)));
    g_color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Text,                 DrColor( 50,  50,  50)));
    g_color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Text_Light,           DrColor( 25,  25,  25)));
    g_color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Highlight,            DrColor(  4,   4,   4)));
    g_color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Header_Text,          DrColor(255, 255, 255)));
    g_color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Header_Fade,          DrColor(  0,  80,  75)));
    g_color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Icon_Dark,            DrColor(  0, 160, 155)));
    g_color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Icon_Light,           DrColor(  0, 190, 185)));

    g_color_schemes[Color_Scheme::Navy].insert(std::make_pair(Window_Colors::Shadow,                DrColor(  5,   6,   7)));
    g_color_schemes[Color_Scheme::Navy].insert(std::make_pair(Window_Colors::Seperator,             DrColor( 14,  16,  20)));
    g_color_schemes[Color_Scheme::Navy].insert(std::make_pair(Window_Colors::Background_Dark,       DrColor( 24,  27,  34)));
    g_color_schemes[Color_Scheme::Navy].insert(std::make_pair(Window_Colors::Background_Light,      DrColor( 32,  37,  45)));
    g_color_schemes[Color_Scheme::Navy].insert(std::make_pair(Window_Colors::Button_Dark,           DrColor( 40,  47,  57)));
    g_color_schemes[Color_Scheme::Navy].insert(std::make_pair(Window_Colors::Button_Light,          DrColor( 50,  58,  72)));
    g_color_schemes[Color_Scheme::Navy].insert(std::make_pair(Window_Colors::Midlight,              DrColor( 64,  74,  91)));
    g_color_schemes[Color_Scheme::Navy].insert(std::make_pair(Window_Colors::Text_Dark,             DrColor(103, 120, 147)));
    g_color_schemes[Color_Scheme::Navy].insert(std::make_pair(Window_Colors::Text,                  DrColor(139, 162, 199)));
    g_color_schemes[Color_Scheme::Navy].insert(std::make_pair(Window_Colors::Text_Light,            DrColor(153, 179, 219)));
    g_color_schemes[Color_Scheme::Navy].insert(std::make_pair(Window_Colors::Highlight,             DrColor(177, 207, 254)));
    g_color_schemes[Color_Scheme::Navy].insert(std::make_pair(Window_Colors::Header_Text,           DrColor(189, 214, 254)));
    g_color_schemes[Color_Scheme::Navy].insert(std::make_pair(Window_Colors::Header_Fade,           DrColor(  9,  27,  78)));
    g_color_schemes[Color_Scheme::Navy].insert(std::make_pair(Window_Colors::Icon_Dark,             DrColor( 23,  64, 181)));
    g_color_schemes[Color_Scheme::Navy].insert(std::make_pair(Window_Colors::Icon_Light,            DrColor( 26,  72, 207)));

    g_color_schemes[Color_Scheme::Grape].insert(std::make_pair(Window_Colors::Shadow,               DrColor(  6,   5,   7)));
    g_color_schemes[Color_Scheme::Grape].insert(std::make_pair(Window_Colors::Seperator,            DrColor( 19,  15,  22)));
    g_color_schemes[Color_Scheme::Grape].insert(std::make_pair(Window_Colors::Background_Dark,      DrColor( 28,  23,  33)));
    g_color_schemes[Color_Scheme::Grape].insert(std::make_pair(Window_Colors::Background_Light,     DrColor( 37,  30,  43)));
    g_color_schemes[Color_Scheme::Grape].insert(std::make_pair(Window_Colors::Button_Dark,          DrColor( 51,  41,  59)));
    g_color_schemes[Color_Scheme::Grape].insert(std::make_pair(Window_Colors::Button_Light,         DrColor( 61,  50,  71)));
    g_color_schemes[Color_Scheme::Grape].insert(std::make_pair(Window_Colors::Midlight,             DrColor( 79,  64,  91)));
    g_color_schemes[Color_Scheme::Grape].insert(std::make_pair(Window_Colors::Text_Dark,            DrColor(125, 101, 144)));
    g_color_schemes[Color_Scheme::Grape].insert(std::make_pair(Window_Colors::Text,                 DrColor(172, 139, 198)));
    g_color_schemes[Color_Scheme::Grape].insert(std::make_pair(Window_Colors::Text_Light,           DrColor(191, 154, 220)));
    g_color_schemes[Color_Scheme::Grape].insert(std::make_pair(Window_Colors::Highlight,            DrColor(221, 178, 255)));
    // Gold highlights
    ///g_color_schemes[Color_Scheme::Grape].insert(std::make_pair(Window_Colors::Header_Text,       DrColor(  6,   5,   7)));
    ///g_color_schemes[Color_Scheme::Grape].insert(std::make_pair(Window_Colors::Header_Fade,       DrColor( 61,  50,  17)));
    ///g_color_schemes[Color_Scheme::Grape].insert(std::make_pair(Window_Colors::Icon_Dark,         DrColor(224, 185,   1)));
    ///g_color_schemes[Color_Scheme::Grape].insert(std::make_pair(Window_Colors::Icon_Light,        DrColor(249, 206,   1)));
    g_color_schemes[Color_Scheme::Grape].insert(std::make_pair(Window_Colors::Header_Text,          DrColor(227, 191, 255)));
    g_color_schemes[Color_Scheme::Grape].insert(std::make_pair(Window_Colors::Header_Fade,          DrColor( 39,   3,  64)));
    g_color_schemes[Color_Scheme::Grape].insert(std::make_pair(Window_Colors::Icon_Dark,            DrColor(134,  12, 218)));
    g_color_schemes[Color_Scheme::Grape].insert(std::make_pair(Window_Colors::Icon_Light,           DrColor(150,  14, 244)));

    g_color_schemes[Color_Scheme::Rust].insert(std::make_pair(Window_Colors::Shadow,                DrColor(  7,   5,   5)));
    g_color_schemes[Color_Scheme::Rust].insert(std::make_pair(Window_Colors::Seperator,             DrColor( 20,  14,  15)));
    g_color_schemes[Color_Scheme::Rust].insert(std::make_pair(Window_Colors::Background_Dark,       DrColor( 32,  23,  24)));
    g_color_schemes[Color_Scheme::Rust].insert(std::make_pair(Window_Colors::Background_Light,      DrColor( 45,  31,  34)));
    g_color_schemes[Color_Scheme::Rust].insert(std::make_pair(Window_Colors::Button_Dark,           DrColor( 58,  41,  44)));
    g_color_schemes[Color_Scheme::Rust].insert(std::make_pair(Window_Colors::Button_Light,          DrColor( 71,  50,  53)));
    g_color_schemes[Color_Scheme::Rust].insert(std::make_pair(Window_Colors::Midlight,              DrColor( 93,  65,  69)));
    g_color_schemes[Color_Scheme::Rust].insert(std::make_pair(Window_Colors::Text_Dark,             DrColor(147, 104, 110)));
    g_color_schemes[Color_Scheme::Rust].insert(std::make_pair(Window_Colors::Text,                  DrColor(200, 141, 149)));
    g_color_schemes[Color_Scheme::Rust].insert(std::make_pair(Window_Colors::Text_Light,            DrColor(220, 155, 164)));
    g_color_schemes[Color_Scheme::Rust].insert(std::make_pair(Window_Colors::Highlight,             DrColor(254, 179, 189)));
    g_color_schemes[Color_Scheme::Rust].insert(std::make_pair(Window_Colors::Header_Text,           DrColor(254, 192, 200)));
    g_color_schemes[Color_Scheme::Rust].insert(std::make_pair(Window_Colors::Header_Fade,           DrColor( 51,  11,  10)));
    g_color_schemes[Color_Scheme::Rust].insert(std::make_pair(Window_Colors::Icon_Dark,             DrColor(149,  24,  21)));
    g_color_schemes[Color_Scheme::Rust].insert(std::make_pair(Window_Colors::Icon_Light,            DrColor(175,  28,  25)));

    g_color_schemes[Color_Scheme::Coffee].insert(std::make_pair(Window_Colors::Shadow,              DrColor(  9,   7,   5)));
    g_color_schemes[Color_Scheme::Coffee].insert(std::make_pair(Window_Colors::Seperator,           DrColor( 22,  19,  14)));
    g_color_schemes[Color_Scheme::Coffee].insert(std::make_pair(Window_Colors::Background_Dark,     DrColor( 31,  27,  20)));
    g_color_schemes[Color_Scheme::Coffee].insert(std::make_pair(Window_Colors::Background_Light,    DrColor( 43,  37,  27)));
    g_color_schemes[Color_Scheme::Coffee].insert(std::make_pair(Window_Colors::Button_Dark,         DrColor( 58,  50,  37)));
    g_color_schemes[Color_Scheme::Coffee].insert(std::make_pair(Window_Colors::Button_Light,        DrColor( 72,  62,  46)));
    g_color_schemes[Color_Scheme::Coffee].insert(std::make_pair(Window_Colors::Midlight,            DrColor( 92,  78,  59)));
    g_color_schemes[Color_Scheme::Coffee].insert(std::make_pair(Window_Colors::Text_Dark,           DrColor(142, 121,  91)));
    g_color_schemes[Color_Scheme::Coffee].insert(std::make_pair(Window_Colors::Text,                DrColor(197, 168, 126)));
    g_color_schemes[Color_Scheme::Coffee].insert(std::make_pair(Window_Colors::Text_Light,          DrColor(218, 186, 139)));
    g_color_schemes[Color_Scheme::Coffee].insert(std::make_pair(Window_Colors::Highlight,           DrColor(255, 217, 162)));
    g_color_schemes[Color_Scheme::Coffee].insert(std::make_pair(Window_Colors::Header_Text,         DrColor(255, 222, 176)));
    g_color_schemes[Color_Scheme::Coffee].insert(std::make_pair(Window_Colors::Header_Fade,         DrColor( 51,  32,   7)));
    g_color_schemes[Color_Scheme::Coffee].insert(std::make_pair(Window_Colors::Icon_Dark,           DrColor(150,  94,  23)));
    g_color_schemes[Color_Scheme::Coffee].insert(std::make_pair(Window_Colors::Icon_Light,          DrColor(177, 111,  27)));

    g_color_schemes[Color_Scheme::Emerald].insert(std::make_pair(Window_Colors::Shadow,             DrColor(  7,   9,   6)));
    g_color_schemes[Color_Scheme::Emerald].insert(std::make_pair(Window_Colors::Seperator,          DrColor( 17,  23,  16)));
    g_color_schemes[Color_Scheme::Emerald].insert(std::make_pair(Window_Colors::Background_Dark,    DrColor( 23,  32,  22)));
    g_color_schemes[Color_Scheme::Emerald].insert(std::make_pair(Window_Colors::Background_Light,   DrColor( 32,  44,  31)));
    g_color_schemes[Color_Scheme::Emerald].insert(std::make_pair(Window_Colors::Button_Dark,        DrColor( 42,  59,  41)));
    g_color_schemes[Color_Scheme::Emerald].insert(std::make_pair(Window_Colors::Button_Light,       DrColor( 52,  72,  50)));
    g_color_schemes[Color_Scheme::Emerald].insert(std::make_pair(Window_Colors::Midlight,           DrColor( 66,  91,  64)));
    g_color_schemes[Color_Scheme::Emerald].insert(std::make_pair(Window_Colors::Text_Dark,          DrColor(106, 146, 103)));
    g_color_schemes[Color_Scheme::Emerald].insert(std::make_pair(Window_Colors::Text,               DrColor(144, 199, 140)));
    g_color_schemes[Color_Scheme::Emerald].insert(std::make_pair(Window_Colors::Text_Light,         DrColor(159, 220, 155)));
    g_color_schemes[Color_Scheme::Emerald].insert(std::make_pair(Window_Colors::Highlight,          DrColor(183, 253, 178)));
    // Brown Highlights
    ///g_color_schemes[Color_Scheme::Emerald].insert(std::make_pair(Window_Colors::Header_Text,     DrColor(183, 253, 178)));
    ///g_color_schemes[Color_Scheme::Emerald].insert(std::make_pair(Window_Colors::Header_Fade,     DrColor( 65,  48,  26)));
    ///g_color_schemes[Color_Scheme::Emerald].insert(std::make_pair(Window_Colors::Icon_Dark,       DrColor(160, 121,  67)));
    ///g_color_schemes[Color_Scheme::Emerald].insert(std::make_pair(Window_Colors::Icon_Light,      DrColor(187, 143,  79)));
    g_color_schemes[Color_Scheme::Emerald].insert(std::make_pair(Window_Colors::Header_Text,        DrColor(194, 253, 190)));
    g_color_schemes[Color_Scheme::Emerald].insert(std::make_pair(Window_Colors::Header_Fade,        DrColor(  6,  52,   8)));
    g_color_schemes[Color_Scheme::Emerald].insert(std::make_pair(Window_Colors::Icon_Dark,          DrColor( 22, 176,  29)));
    g_color_schemes[Color_Scheme::Emerald].insert(std::make_pair(Window_Colors::Icon_Light,         DrColor( 26, 202,  34)));

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



















