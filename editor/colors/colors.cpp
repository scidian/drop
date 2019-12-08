//
//      Created by Stephens Nunnally on 1/21/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Holds all of our custom colors
//
//
#include <QRandomGenerator>

#include "editor/colors/colors.h"
#include "editor/globals_editor.h"
#include "library/dr_math.h"
#include "model/enums_model_types.h"

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
bool IsSameColor(const QColor &color1, const QColor &color2, double tolerance) {
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
    std::vector<DrVariant> variant_vector = Dr::GetPreference(Preferences::Color_Popup_History).toVector();

    // Creat new list and add color as first item
    std::vector<DrVariant> color_vector;
    color_vector.push_back( color.rgba() );

    // Add all the old colors skipping the lastest color
    for (auto item : variant_vector) {
        QRgb rgba = QRgb(item.toUInt());
        if (color.rgb() != rgba) {
            color_vector.push_back( rgba );
        }
    }

    // Make sure list is <= 36 items
    while (color_vector.size() > 36) {
        color_vector.pop_back();
    }

    // Store new color list in preferences
    Dr::SetPreference(Preferences::Color_Popup_History, color_vector);
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

    g_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Shadow,                QColor(  0,   0,   0)));
    g_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Seperator,             QColor( 16,  16,  16)));
    g_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Background_Dark,       QColor( 24,  24,  24)));
    g_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Background_Light,      QColor( 32,  32,  32)));
    g_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Button_Dark,           QColor( 40,  40,  40)));
    g_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Button_Light,          QColor( 60,  60,  60)));
    g_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Midlight,              QColor( 85,  85,  85)));
    g_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Text_Dark,             QColor(100, 100, 100)));
    g_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Text,                  QColor(205, 205, 205)));
    g_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Text_Light,            QColor(225, 225, 225)));
    g_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Highlight,             QColor(255, 255, 255)));
    g_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Header_Text,           QColor(225, 225, 225)));
    g_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Header_Fade,           QColor( 20,  20,  20)));
    g_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Icon_Dark,             QColor(  0, 150, 145)));
    g_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Icon_Light,            QColor(  0, 180, 175)));

    g_color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Shadow,               QColor(235, 235, 235)));
    g_color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Seperator,            QColor(220, 220, 220)));
    g_color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Background_Dark,      QColor(205, 205, 205)));
    g_color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Background_Light,     QColor(190, 190, 190)));
    g_color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Button_Dark,          QColor(170, 170, 170)));
    g_color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Button_Light,         QColor(150, 150, 150)));
    g_color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Midlight,             QColor(130, 130, 130)));
    g_color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Text_Dark,            QColor(110, 110, 110)));
    g_color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Text,                 QColor( 50,  50,  50)));
    g_color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Text_Light,           QColor( 25,  25,  25)));
    g_color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Highlight,            QColor(  4,   4,   4)));
    g_color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Header_Text,          QColor(225, 225, 225)));
    g_color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Header_Fade,          QColor( 60,  60,  60)));
    g_color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Icon_Dark,            QColor(  0, 150, 145)));
    g_color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Icon_Light,           QColor(  0, 180, 175)));

    g_color_schemes[Color_Scheme::Navy].insert(std::make_pair(Window_Colors::Shadow,                QColor(  5,   6,   7)));
    g_color_schemes[Color_Scheme::Navy].insert(std::make_pair(Window_Colors::Seperator,             QColor( 14,  16,  20)));
    g_color_schemes[Color_Scheme::Navy].insert(std::make_pair(Window_Colors::Background_Dark,       QColor( 24,  27,  34)));
    g_color_schemes[Color_Scheme::Navy].insert(std::make_pair(Window_Colors::Background_Light,      QColor( 32,  37,  45)));
    g_color_schemes[Color_Scheme::Navy].insert(std::make_pair(Window_Colors::Button_Dark,           QColor( 40,  47,  57)));
    g_color_schemes[Color_Scheme::Navy].insert(std::make_pair(Window_Colors::Button_Light,          QColor( 50,  58,  72)));
    g_color_schemes[Color_Scheme::Navy].insert(std::make_pair(Window_Colors::Midlight,              QColor( 64,  74,  91)));
    g_color_schemes[Color_Scheme::Navy].insert(std::make_pair(Window_Colors::Text_Dark,             QColor(103, 120, 147)));
    g_color_schemes[Color_Scheme::Navy].insert(std::make_pair(Window_Colors::Text,                  QColor(139, 162, 199)));
    g_color_schemes[Color_Scheme::Navy].insert(std::make_pair(Window_Colors::Text_Light,            QColor(153, 179, 219)));
    g_color_schemes[Color_Scheme::Navy].insert(std::make_pair(Window_Colors::Highlight,             QColor(177, 207, 254)));
    g_color_schemes[Color_Scheme::Navy].insert(std::make_pair(Window_Colors::Header_Text,           QColor(189, 214, 254)));
    g_color_schemes[Color_Scheme::Navy].insert(std::make_pair(Window_Colors::Header_Fade,           QColor(  9,  27,  78)));
    g_color_schemes[Color_Scheme::Navy].insert(std::make_pair(Window_Colors::Icon_Dark,             QColor( 23,  64, 181)));
    g_color_schemes[Color_Scheme::Navy].insert(std::make_pair(Window_Colors::Icon_Light,            QColor( 26,  72, 207)));

    g_color_schemes[Color_Scheme::Grape].insert(std::make_pair(Window_Colors::Shadow,               QColor(  6,   5,   7)));
    g_color_schemes[Color_Scheme::Grape].insert(std::make_pair(Window_Colors::Seperator,            QColor( 19,  15,  22)));
    g_color_schemes[Color_Scheme::Grape].insert(std::make_pair(Window_Colors::Background_Dark,      QColor( 28,  23,  33)));
    g_color_schemes[Color_Scheme::Grape].insert(std::make_pair(Window_Colors::Background_Light,     QColor( 37,  30,  43)));
    g_color_schemes[Color_Scheme::Grape].insert(std::make_pair(Window_Colors::Button_Dark,          QColor( 51,  41,  59)));
    g_color_schemes[Color_Scheme::Grape].insert(std::make_pair(Window_Colors::Button_Light,         QColor( 61,  50,  71)));
    g_color_schemes[Color_Scheme::Grape].insert(std::make_pair(Window_Colors::Midlight,             QColor( 79,  64,  91)));
    g_color_schemes[Color_Scheme::Grape].insert(std::make_pair(Window_Colors::Text_Dark,            QColor(125, 101, 144)));
    g_color_schemes[Color_Scheme::Grape].insert(std::make_pair(Window_Colors::Text,                 QColor(172, 139, 198)));
    g_color_schemes[Color_Scheme::Grape].insert(std::make_pair(Window_Colors::Text_Light,           QColor(191, 154, 220)));
    g_color_schemes[Color_Scheme::Grape].insert(std::make_pair(Window_Colors::Highlight,            QColor(221, 178, 255)));
    // Gold highlights
    ///g_color_schemes[Color_Scheme::Grape].insert(std::make_pair(Window_Colors::Header_Text,       QColor(  6,   5,   7)));
    ///g_color_schemes[Color_Scheme::Grape].insert(std::make_pair(Window_Colors::Header_Fade,       QColor( 61,  50,  17)));
    ///g_color_schemes[Color_Scheme::Grape].insert(std::make_pair(Window_Colors::Icon_Dark,         QColor(224, 185,   1)));
    ///g_color_schemes[Color_Scheme::Grape].insert(std::make_pair(Window_Colors::Icon_Light,        QColor(249, 206,   1)));
    g_color_schemes[Color_Scheme::Grape].insert(std::make_pair(Window_Colors::Header_Text,          QColor(227, 191, 255)));
    g_color_schemes[Color_Scheme::Grape].insert(std::make_pair(Window_Colors::Header_Fade,          QColor( 39,   3,  64)));
    g_color_schemes[Color_Scheme::Grape].insert(std::make_pair(Window_Colors::Icon_Dark,            QColor(134,  12, 218)));
    g_color_schemes[Color_Scheme::Grape].insert(std::make_pair(Window_Colors::Icon_Light,           QColor(150,  14, 244)));

    g_color_schemes[Color_Scheme::Rust].insert(std::make_pair(Window_Colors::Shadow,                QColor(  7,   5,   5)));
    g_color_schemes[Color_Scheme::Rust].insert(std::make_pair(Window_Colors::Seperator,             QColor( 20,  14,  15)));
    g_color_schemes[Color_Scheme::Rust].insert(std::make_pair(Window_Colors::Background_Dark,       QColor( 32,  23,  24)));
    g_color_schemes[Color_Scheme::Rust].insert(std::make_pair(Window_Colors::Background_Light,      QColor( 45,  31,  34)));
    g_color_schemes[Color_Scheme::Rust].insert(std::make_pair(Window_Colors::Button_Dark,           QColor( 58,  41,  44)));
    g_color_schemes[Color_Scheme::Rust].insert(std::make_pair(Window_Colors::Button_Light,          QColor( 71,  50,  53)));
    g_color_schemes[Color_Scheme::Rust].insert(std::make_pair(Window_Colors::Midlight,              QColor( 93,  65,  69)));
    g_color_schemes[Color_Scheme::Rust].insert(std::make_pair(Window_Colors::Text_Dark,             QColor(147, 104, 110)));
    g_color_schemes[Color_Scheme::Rust].insert(std::make_pair(Window_Colors::Text,                  QColor(200, 141, 149)));
    g_color_schemes[Color_Scheme::Rust].insert(std::make_pair(Window_Colors::Text_Light,            QColor(220, 155, 164)));
    g_color_schemes[Color_Scheme::Rust].insert(std::make_pair(Window_Colors::Highlight,             QColor(254, 179, 189)));
    g_color_schemes[Color_Scheme::Rust].insert(std::make_pair(Window_Colors::Header_Text,           QColor(254, 192, 200)));
    g_color_schemes[Color_Scheme::Rust].insert(std::make_pair(Window_Colors::Header_Fade,           QColor( 51,  11,  10)));
    g_color_schemes[Color_Scheme::Rust].insert(std::make_pair(Window_Colors::Icon_Dark,             QColor(149,  24,  21)));
    g_color_schemes[Color_Scheme::Rust].insert(std::make_pair(Window_Colors::Icon_Light,            QColor(175,  28,  25)));

    g_color_schemes[Color_Scheme::Coffee].insert(std::make_pair(Window_Colors::Shadow,              QColor(  9,   7,   5)));
    g_color_schemes[Color_Scheme::Coffee].insert(std::make_pair(Window_Colors::Seperator,           QColor( 22,  19,  14)));
    g_color_schemes[Color_Scheme::Coffee].insert(std::make_pair(Window_Colors::Background_Dark,     QColor( 31,  27,  20)));
    g_color_schemes[Color_Scheme::Coffee].insert(std::make_pair(Window_Colors::Background_Light,    QColor( 43,  37,  27)));
    g_color_schemes[Color_Scheme::Coffee].insert(std::make_pair(Window_Colors::Button_Dark,         QColor( 58,  50,  37)));
    g_color_schemes[Color_Scheme::Coffee].insert(std::make_pair(Window_Colors::Button_Light,        QColor( 72,  62,  46)));
    g_color_schemes[Color_Scheme::Coffee].insert(std::make_pair(Window_Colors::Midlight,            QColor( 92,  78,  59)));
    g_color_schemes[Color_Scheme::Coffee].insert(std::make_pair(Window_Colors::Text_Dark,           QColor(142, 121,  91)));
    g_color_schemes[Color_Scheme::Coffee].insert(std::make_pair(Window_Colors::Text,                QColor(197, 168, 126)));
    g_color_schemes[Color_Scheme::Coffee].insert(std::make_pair(Window_Colors::Text_Light,          QColor(218, 186, 139)));
    g_color_schemes[Color_Scheme::Coffee].insert(std::make_pair(Window_Colors::Highlight,           QColor(255, 217, 162)));
    g_color_schemes[Color_Scheme::Coffee].insert(std::make_pair(Window_Colors::Header_Text,         QColor(255, 222, 176)));
    g_color_schemes[Color_Scheme::Coffee].insert(std::make_pair(Window_Colors::Header_Fade,         QColor( 51,  32,   7)));
    g_color_schemes[Color_Scheme::Coffee].insert(std::make_pair(Window_Colors::Icon_Dark,           QColor(150,  94,  23)));
    g_color_schemes[Color_Scheme::Coffee].insert(std::make_pair(Window_Colors::Icon_Light,          QColor(177, 111,  27)));

    g_color_schemes[Color_Scheme::Emerald].insert(std::make_pair(Window_Colors::Shadow,             QColor(  7,   9,   6)));
    g_color_schemes[Color_Scheme::Emerald].insert(std::make_pair(Window_Colors::Seperator,          QColor( 17,  23,  16)));
    g_color_schemes[Color_Scheme::Emerald].insert(std::make_pair(Window_Colors::Background_Dark,    QColor( 23,  32,  22)));
    g_color_schemes[Color_Scheme::Emerald].insert(std::make_pair(Window_Colors::Background_Light,   QColor( 32,  44,  31)));
    g_color_schemes[Color_Scheme::Emerald].insert(std::make_pair(Window_Colors::Button_Dark,        QColor( 42,  59,  41)));
    g_color_schemes[Color_Scheme::Emerald].insert(std::make_pair(Window_Colors::Button_Light,       QColor( 52,  72,  50)));
    g_color_schemes[Color_Scheme::Emerald].insert(std::make_pair(Window_Colors::Midlight,           QColor( 66,  91,  64)));
    g_color_schemes[Color_Scheme::Emerald].insert(std::make_pair(Window_Colors::Text_Dark,          QColor(106, 146, 103)));
    g_color_schemes[Color_Scheme::Emerald].insert(std::make_pair(Window_Colors::Text,               QColor(144, 199, 140)));
    g_color_schemes[Color_Scheme::Emerald].insert(std::make_pair(Window_Colors::Text_Light,         QColor(159, 220, 155)));
    g_color_schemes[Color_Scheme::Emerald].insert(std::make_pair(Window_Colors::Highlight,          QColor(183, 253, 178)));
    // Brown Highlights
    ///g_color_schemes[Color_Scheme::Emerald].insert(std::make_pair(Window_Colors::Header_Text,     QColor(183, 253, 178)));
    ///g_color_schemes[Color_Scheme::Emerald].insert(std::make_pair(Window_Colors::Header_Fade,     QColor( 65,  48,  26)));
    ///g_color_schemes[Color_Scheme::Emerald].insert(std::make_pair(Window_Colors::Icon_Dark,       QColor(160, 121,  67)));
    ///g_color_schemes[Color_Scheme::Emerald].insert(std::make_pair(Window_Colors::Icon_Light,      QColor(187, 143,  79)));
    g_color_schemes[Color_Scheme::Emerald].insert(std::make_pair(Window_Colors::Header_Text,        QColor(194, 253, 190)));
    g_color_schemes[Color_Scheme::Emerald].insert(std::make_pair(Window_Colors::Header_Fade,        QColor(  6,  52,   8)));
    g_color_schemes[Color_Scheme::Emerald].insert(std::make_pair(Window_Colors::Icon_Dark,          QColor( 22, 176,  29)));
    g_color_schemes[Color_Scheme::Emerald].insert(std::make_pair(Window_Colors::Icon_Light,         QColor( 26, 202,  34)));

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



















