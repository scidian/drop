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
#include "globals.h"
#include "helper.h"

typedef std::map<Color_Scheme,   std::map<Window_Colors, QColor>> Color_Scheme_Map;
typedef std::map<Color_Palettes, Palette_Info> Color_Palette_Map;

namespace Dr {

//####################################################################################
//##    Local Static Variables
//####################################################################################
static Color_Scheme      g_current_color_scheme = Color_Scheme::Dark;
static Color_Scheme_Map  g_color_schemes;
static Color_Palette_Map g_color_palettes;                                      // Stores color palettes

QString BorderWidth()       { return "1px"; }                                   // Project wide border width for Style Sheets, as QString
int     BorderWidthAsInt()  { return 1; }                                       // Project wide border width for Style Sheets, as Int


//####################################################################################
//##    Color Helper Functions
//####################################################################################
// Compares 2 colors, returns true if they are the same
bool IsSameColor(QColor color1, QColor color2, double tolerance) {
    return ( Dr::IsCloseTo(color1.redF(),   color2.redF(),   tolerance) &&
             Dr::IsCloseTo(color1.greenF(), color2.greenF(), tolerance) &&
             Dr::IsCloseTo(color1.blueF(),  color2.blueF(),  tolerance) &&
             Dr::IsCloseTo(color1.alphaF(), color2.alphaF(), tolerance) );
}

// Returns a random color
QColor RandomColor() {
    return QColor::fromRgb(QRandomGenerator::global()->generate());
}


//####################################################################################
//##    Retreive custom colors
//####################################################################################
Color_Scheme    GetColorScheme()                                { return g_current_color_scheme; }
void            SetColorScheme(Color_Scheme new_scheme)         { g_current_color_scheme = new_scheme; }

QColor          GetColor(Window_Colors color_role)              { return g_color_schemes[g_current_color_scheme][color_role]; }

QColor          GetColorFromPalette(Color_Palettes palette, int color_index) {
    if ( color_index >= 0 && color_index <= (g_color_palettes[palette].number_of_colors - 1) )
        return g_color_palettes[palette].colors[color_index];
    else
        return QColor(QRgb(0x00000000));
}

int             GetPaletteColorCount(Color_Palettes palette)    { return g_color_palettes[palette].number_of_colors; }
QString         GetPaletteName(Color_Palettes palette)          { return g_color_palettes[palette].name; }
bool            GetPaletteShowInList(Color_Palettes palette)    { return g_color_palettes[palette].show_in_list; }


//####################################################################################
//##    Color History
//####################################################################################
void AddToColorHistory(QColor color) {
    // Load old color history list (up to 36 colors)
    QList<QVariant> variant_list = Dr::GetPreference(Preferences::Color_Popup_History).toList();

    // Creat new list and add color as first item
    QList<QVariant> color_list;
    color_list.append( color.rgba() );

    // Add all the old colors skipping the lastest color
    for (auto item : variant_list) {
        QRgb rgba = QRgb(item.toUInt());
        if (color.rgb() != rgba)
            color_list.append( rgba );
    }

    // Make sure list is <= 36 items
    while (color_list.count() > 36)
        color_list.removeLast();

    // Store new color list in preferences
    QVariant colors;
    colors.setValue( color_list );
    Dr::SetPreference(Preferences::Color_Popup_History, colors);
}


//####################################################################################
//##    Store custom window palette colors
//####################################################################################
void LoadPalettes() {
    LoadPaletteBasic(        g_color_palettes[Color_Palettes::Basic] );
    LoadPaletteBlank(        g_color_palettes[Color_Palettes::Blank] );
    LoadPaletteMaterial(     g_color_palettes[Color_Palettes::Material] );
    LoadPaletteRockyRover(   g_color_palettes[Color_Palettes::Rocky_Rover] );
    LoadPaletteWindowThemes( g_color_palettes[Color_Palettes::Window_Themes] );

    g_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Seperator,         QColor( 16,  16,  16)));
    g_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Shadow,            QColor(  0,   0,   0)));
    g_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Background_Dark,   QColor( 24,  24,  24)));
    g_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Background_Light,  QColor( 32,  32,  32)));
    g_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Button_Dark,       QColor( 40,  40,  40)));
    g_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Button_Light,      QColor( 60,  60,  60)));
    g_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Midlight,          QColor( 80,  80,  80)));
    g_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Text_Dark,         QColor(145, 145, 145)));
    g_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Text,              QColor(205, 205, 205)));
    g_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Text_Light,        QColor(225, 225, 225)));
    g_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Highlight,         QColor(255, 255, 255)));
    g_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Icon_Dark,         QColor(  0, 150, 145)));
    g_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Icon_Light,        QColor(  0, 180, 175)));

    g_color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Seperator,        QColor(185, 185, 185)));
    g_color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Shadow,           QColor(175, 175, 175)));
    g_color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Background_Dark,  QColor(190, 190, 190)));
    g_color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Background_Light, QColor(200, 200, 200)));
    g_color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Button_Dark,      QColor(150, 150, 150)));
    g_color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Button_Light,     QColor(160, 160, 160)));
    g_color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Midlight,         QColor(120, 120, 120)));
    g_color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Text_Dark,        QColor( 10,  10,  10)));
    g_color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Text,             QColor( 25,  25,  25)));
    g_color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Text_Light,       QColor( 40,  40,  40)));
    g_color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Highlight,        QColor(  0,   0,   0)));
    g_color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Icon_Dark,        QColor( 50,  50,  50)));
    g_color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Icon_Light,       QColor( 70,  70,  70)));

    g_color_schemes[Color_Scheme::Blue].insert(std::make_pair(Window_Colors::Seperator,         QColor( 29,  77, 144)));
    g_color_schemes[Color_Scheme::Blue].insert(std::make_pair(Window_Colors::Shadow,            QColor(  5,  16,  28)));
    g_color_schemes[Color_Scheme::Blue].insert(std::make_pair(Window_Colors::Background_Dark,   QColor( 20,  55, 103)));
    g_color_schemes[Color_Scheme::Blue].insert(std::make_pair(Window_Colors::Background_Light,  QColor( 29,  77, 144)));
    g_color_schemes[Color_Scheme::Blue].insert(std::make_pair(Window_Colors::Button_Dark,       QColor(  8,  24,  41)));
    g_color_schemes[Color_Scheme::Blue].insert(std::make_pair(Window_Colors::Button_Light,      QColor( 11,  32,  55)));
    g_color_schemes[Color_Scheme::Blue].insert(std::make_pair(Window_Colors::Midlight,          QColor( 29,  77, 144)));
    g_color_schemes[Color_Scheme::Blue].insert(std::make_pair(Window_Colors::Text_Dark,         QColor(106, 160, 216)));
    g_color_schemes[Color_Scheme::Blue].insert(std::make_pair(Window_Colors::Text,              QColor(112, 170, 229)));
    g_color_schemes[Color_Scheme::Blue].insert(std::make_pair(Window_Colors::Text_Light,        QColor(118, 180, 242)));
    g_color_schemes[Color_Scheme::Blue].insert(std::make_pair(Window_Colors::Highlight,         QColor(212, 234, 254)));
    g_color_schemes[Color_Scheme::Blue].insert(std::make_pair(Window_Colors::Icon_Dark,         QColor(106, 160, 216)));
    g_color_schemes[Color_Scheme::Blue].insert(std::make_pair(Window_Colors::Icon_Light,        QColor(118, 180, 242)));

    g_color_schemes[Color_Scheme::Autumn].insert(std::make_pair(Window_Colors::Seperator,       QColor(166,  59,  31)));
    g_color_schemes[Color_Scheme::Autumn].insert(std::make_pair(Window_Colors::Shadow,          QColor( 39,  18,  11)));
    g_color_schemes[Color_Scheme::Autumn].insert(std::make_pair(Window_Colors::Background_Dark, QColor(225, 167,  69)));
    g_color_schemes[Color_Scheme::Autumn].insert(std::make_pair(Window_Colors::Background_Light,QColor(166,  59,  31)));
    g_color_schemes[Color_Scheme::Autumn].insert(std::make_pair(Window_Colors::Button_Dark,     QColor(233, 125,  49)));
    g_color_schemes[Color_Scheme::Autumn].insert(std::make_pair(Window_Colors::Button_Light,    QColor(213, 112,  41)));
    g_color_schemes[Color_Scheme::Autumn].insert(std::make_pair(Window_Colors::Midlight,        QColor(197,  79,  32)));
    g_color_schemes[Color_Scheme::Autumn].insert(std::make_pair(Window_Colors::Text_Dark,       QColor( 50,  21,  13)));
    g_color_schemes[Color_Scheme::Autumn].insert(std::make_pair(Window_Colors::Text,            QColor( 55,  26,  18)));
    g_color_schemes[Color_Scheme::Autumn].insert(std::make_pair(Window_Colors::Text_Light,      QColor( 60,  31,  23)));
    g_color_schemes[Color_Scheme::Autumn].insert(std::make_pair(Window_Colors::Highlight,       QColor(109,  95,  49)));
    g_color_schemes[Color_Scheme::Autumn].insert(std::make_pair(Window_Colors::Icon_Dark,       QColor( 65,  71,  20)));
    g_color_schemes[Color_Scheme::Autumn].insert(std::make_pair(Window_Colors::Icon_Light,      QColor( 53,  58,  16)));
}

void LoadPaletteBasic(Palette_Info &palette) {
    palette.name = "Basic";
    palette.show_in_list = false;

    palette.colors.append( QColor(QRgb(0xFF0000)) );    //  0 Red
    palette.colors.append( QColor(QRgb(0xFF8000)) );    //  1 Orange
    palette.colors.append( QColor(QRgb(0xFFFF00)) );    //  2 Yellow
    palette.colors.append( QColor(QRgb(0x00FF00)) );    //  3 Green

    palette.colors.append( QColor(QRgb(0x00FFFF)) );    //  4 Cyan
    palette.colors.append( QColor(QRgb(0x0000FF)) );    //  5 Blue
    palette.colors.append( QColor(QRgb(0x800080)) );    //  6 Purple
    palette.colors.append( QColor(QRgb(0xFF00FF)) );    //  7 Pink

    palette.colors.append( QColor(QRgb(0xFFFFFF)) );    //  8 White
    palette.colors.append( QColor(QRgb(0xAAAAAA)) );    //  9 Light Gray
    palette.colors.append( QColor(QRgb(0x555555)) );    // 10 Dark Gray
    palette.colors.append( QColor(QRgb(0x000000)) );    // 11 Black

    palette.number_of_colors = palette.colors.count();
}


}   // namespace Dr



















