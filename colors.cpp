//
//      Created by Stephens Nunnally on 1/21/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Holds all of our custom colors
//
//
#include <QRandomGenerator>

#include "colors.h"
#include "enums.h"

namespace Dr {

//####################################################################################
//##        Local Static Variables
//####################################################################################
static std::map<Color_Scheme, std::map<Window_Colors, QColor>> g_color_schemes;
static Color_Scheme g_current_color_scheme = Color_Scheme::Dark;


QString BorderWidth()       { return "1px"; }                       // Project wide border width for Style Sheets, as QString
int     BorderWidthAsInt()  { return 1; }                           // Project wide border width for Style Sheets, as Int
int     FontSize()          { return 11;  }                         // Project wide font size


//####################################################################################
//##        Retreive custom colors
//####################################################################################
QColor          GetColor(Window_Colors color_role)      { return g_color_schemes[g_current_color_scheme][color_role]; }

Color_Scheme    GetColorScheme()                        { return g_current_color_scheme; }
void            SetColorScheme(Color_Scheme new_scheme) { g_current_color_scheme = new_scheme; }

QColor          GenerateRandomColor()                   { return QColor::fromRgb(QRandomGenerator::global()->generate()); }


//####################################################################################
//##        Store custom window palette colors
//####################################################################################
void    LoadPalettes()
{
    g_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Seperator, QColor(16, 16, 16)));
    g_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Shadow, QColor(0, 0, 0)));
    g_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Background_Dark, QColor(24, 24, 24)));
    g_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Background_Light, QColor(32, 32, 32)));
    g_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Button_Dark, QColor(40, 40, 40)));
    g_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Button_Light, QColor(52, 52, 52)));
    g_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Midlight, QColor(64, 64, 64)));
    g_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Text_Dark, QColor(195, 195, 195)));
    g_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Text, QColor(205, 205, 205)));
    g_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Text_Light, QColor(215, 215, 215)));
    g_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Highlight, QColor(255, 255, 255)));
    g_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Icon_Dark, QColor(0, 150, 145)));
    g_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Icon_Light, QColor(0, 180, 175)));

    g_color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Seperator, QColor(225, 225, 225)));
    g_color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Shadow, QColor(255, 255, 255)));
    g_color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Background_Dark, QColor(200, 200, 200)));
    g_color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Background_Light, QColor(225, 225, 225)));
    g_color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Button_Dark, QColor(195, 195, 195)));
    g_color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Button_Light, QColor(215, 215, 215)));
    g_color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Midlight, QColor(128, 128, 128)));
    g_color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Text_Dark, QColor(45, 45, 45)));
    g_color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Text, QColor(55, 55, 55)));
    g_color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Text_Light, QColor(65, 65, 65)));
    g_color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Highlight, QColor(0, 0, 0)));
    g_color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Icon_Dark, QColor(0, 150, 145)));
    g_color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Icon_Light, QColor(0, 180, 175)));

    g_color_schemes[Color_Scheme::Blue].insert(std::make_pair(Window_Colors::Seperator, QColor(29, 77, 144)));
    g_color_schemes[Color_Scheme::Blue].insert(std::make_pair(Window_Colors::Shadow, QColor(5, 16, 28)));
    g_color_schemes[Color_Scheme::Blue].insert(std::make_pair(Window_Colors::Background_Dark, QColor(20,55,103)));
    g_color_schemes[Color_Scheme::Blue].insert(std::make_pair(Window_Colors::Background_Light, QColor(29, 77, 144)));
    g_color_schemes[Color_Scheme::Blue].insert(std::make_pair(Window_Colors::Button_Dark, QColor(5, 16, 28)));
    g_color_schemes[Color_Scheme::Blue].insert(std::make_pair(Window_Colors::Button_Light, QColor(11, 32, 55)));
    g_color_schemes[Color_Scheme::Blue].insert(std::make_pair(Window_Colors::Midlight, QColor(29, 77, 144)));
    g_color_schemes[Color_Scheme::Blue].insert(std::make_pair(Window_Colors::Text_Dark, QColor(106, 160, 216)));
    g_color_schemes[Color_Scheme::Blue].insert(std::make_pair(Window_Colors::Text, QColor(112, 170, 229)));
    g_color_schemes[Color_Scheme::Blue].insert(std::make_pair(Window_Colors::Text_Light, QColor(118, 180, 242)));
    g_color_schemes[Color_Scheme::Blue].insert(std::make_pair(Window_Colors::Highlight, QColor(212, 234, 254)));
    g_color_schemes[Color_Scheme::Blue].insert(std::make_pair(Window_Colors::Icon_Dark, QColor(0, 150, 145)));
    g_color_schemes[Color_Scheme::Blue].insert(std::make_pair(Window_Colors::Icon_Light, QColor(0, 180, 175)));

    g_color_schemes[Color_Scheme::Autumn].insert(std::make_pair(Window_Colors::Seperator, QColor(166, 59, 31)));
    g_color_schemes[Color_Scheme::Autumn].insert(std::make_pair(Window_Colors::Shadow, QColor(39, 18, 11)));
    g_color_schemes[Color_Scheme::Autumn].insert(std::make_pair(Window_Colors::Background_Dark, QColor(225, 167, 69)));
    g_color_schemes[Color_Scheme::Autumn].insert(std::make_pair(Window_Colors::Background_Light, QColor(166, 59, 31)));
    g_color_schemes[Color_Scheme::Autumn].insert(std::make_pair(Window_Colors::Button_Dark, QColor(233,125,49)));
    g_color_schemes[Color_Scheme::Autumn].insert(std::make_pair(Window_Colors::Button_Light, QColor(213,112,41)));
    g_color_schemes[Color_Scheme::Autumn].insert(std::make_pair(Window_Colors::Midlight, QColor(197, 79, 32)));
    g_color_schemes[Color_Scheme::Autumn].insert(std::make_pair(Window_Colors::Text_Dark, QColor(100, 41, 23)));
    g_color_schemes[Color_Scheme::Autumn].insert(std::make_pair(Window_Colors::Text, QColor(109, 51, 33)));
    g_color_schemes[Color_Scheme::Autumn].insert(std::make_pair(Window_Colors::Text_Light, QColor(120, 61, 43)));
    g_color_schemes[Color_Scheme::Autumn].insert(std::make_pair(Window_Colors::Highlight, QColor(218, 190, 98)));
    g_color_schemes[Color_Scheme::Autumn].insert(std::make_pair(Window_Colors::Icon_Dark,  QColor(131, 142, 41)));
    g_color_schemes[Color_Scheme::Autumn].insert(std::make_pair(Window_Colors::Icon_Light,  QColor(107, 116, 33)));
}

}






