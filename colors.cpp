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
static Color_Scheme     g_current_color_scheme = Color_Scheme::Dark;

static QVector<QColor>  g_palette_material;                                     // Google material.io palette
static QVector<QColor>  g_palette_basic;                                        // Basic colors

QString BorderWidth()       { return "1px"; }                                   // Project wide border width for Style Sheets, as QString
int     BorderWidthAsInt()  { return 1; }                                       // Project wide border width for Style Sheets, as Int
int     FontSize()          { return 11;  }                                     // Project wide font size


//####################################################################################
//##        Retreive custom colors
//####################################################################################
QColor          GetColor(Window_Colors color_role)      { return g_color_schemes[g_current_color_scheme][color_role]; }
QColor          GetColorBasic(int color)                { return g_palette_basic[color]; }
QColor          GetColorMaterial(int color)             { return g_palette_material[color]; }

Color_Scheme    GetColorScheme()                        { return g_current_color_scheme; }
void            SetColorScheme(Color_Scheme new_scheme) { g_current_color_scheme = new_scheme; }

QColor          GenerateRandomColor()                   { return QColor::fromRgb(QRandomGenerator::global()->generate()); }


//####################################################################################
//##        Store custom window palette colors
//####################################################################################
void    LoadPalettes()
{
    LoadPaletteBasic();
    LoadPaletteMaterial();

    g_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Seperator, QColor(16, 16, 16)));
    g_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Shadow, QColor(0, 0, 0)));
    g_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Background_Dark, QColor(24, 24, 24)));
    g_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Background_Light, QColor(32, 32, 32)));
    g_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Button_Dark, QColor(40, 40, 40)));
    g_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Button_Light, QColor(60, 60, 60)));
    g_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Midlight, QColor(80, 80, 80)));
    g_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Text_Dark, QColor(145, 145, 145)));
    g_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Text, QColor(205, 205, 205)));
    g_color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Text_Light, QColor(225, 225, 225)));
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
    g_color_schemes[Color_Scheme::Blue].insert(std::make_pair(Window_Colors::Background_Dark, QColor(20,55, 103)));
    g_color_schemes[Color_Scheme::Blue].insert(std::make_pair(Window_Colors::Background_Light, QColor(29, 77, 144)));
    g_color_schemes[Color_Scheme::Blue].insert(std::make_pair(Window_Colors::Button_Dark, QColor(8, 24, 41)));
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

void LoadPaletteBasic()
{
    for(int i = 0; i < 12; i++)
        g_palette_basic.append(QColor(0, 0, 0));

    g_palette_basic[ 0] = QColor(QRgb(0xFF0000));     //Red
    g_palette_basic[ 1] = QColor(QRgb(0xFF8000));     //Orange
    g_palette_basic[ 2] = QColor(QRgb(0xFFFF00));     //Yellow
    g_palette_basic[ 3] = QColor(QRgb(0x00FF00));     //Green

    g_palette_basic[ 4] = QColor(QRgb(0x00FFFF));     //Cyan
    g_palette_basic[ 5] = QColor(QRgb(0x0000FF));     //Blue
    g_palette_basic[ 6] = QColor(QRgb(0x800080));     //Purple
    g_palette_basic[ 7] = QColor(QRgb(0xFF00FF));     //Pink

    g_palette_basic[ 8] = QColor(QRgb(0xFFFFFF));     //White
    g_palette_basic[ 9] = QColor(QRgb(0xAAAAAA));     //Light Gray
    g_palette_basic[10] = QColor(QRgb(0x555555));     //Dark Gray
    g_palette_basic[11] = QColor(QRgb(0x000000));     //Black
}

void LoadPaletteMaterial()
{
    for(int i = 0; i < 256; i++)
        g_palette_material.append(QColor(0, 0, 0));

    g_palette_material[  0] = QColor(QRgb(0x000000));   //Black
    g_palette_material[  1] = QColor(QRgb(0xFFFFFF));   //White

    g_palette_material[  2] = QColor(QRgb(0xFAFAFA));   //Gray
    g_palette_material[  3] = QColor(QRgb(0xF5F5F5));
    g_palette_material[  4] = QColor(QRgb(0xEEEEEE));
    g_palette_material[  5] = QColor(QRgb(0xE0E0E0));
    g_palette_material[  6] = QColor(QRgb(0xBDBDBD));
    g_palette_material[  7] = QColor(QRgb(0x9E9E9E));
    g_palette_material[  8] = QColor(QRgb(0x757575));
    g_palette_material[  9] = QColor(QRgb(0x616161));
    g_palette_material[ 10] = QColor(QRgb(0x424242));
    g_palette_material[ 11] = QColor(QRgb(0x212121));

    g_palette_material[ 12] = QColor(QRgb(0xECEFF1));   //Blue Gray
    g_palette_material[ 13] = QColor(QRgb(0xCFD8DC));
    g_palette_material[ 14] = QColor(QRgb(0xB0BEC5));
    g_palette_material[ 15] = QColor(QRgb(0x90A4AE));
    g_palette_material[ 16] = QColor(QRgb(0x78909C));
    g_palette_material[ 17] = QColor(QRgb(0x607D8B));
    g_palette_material[ 18] = QColor(QRgb(0x546E7A));
    g_palette_material[ 19] = QColor(QRgb(0x455A64));
    g_palette_material[ 20] = QColor(QRgb(0x37474F));
    g_palette_material[ 21] = QColor(QRgb(0x263238));

    g_palette_material[ 22] = QColor(QRgb(0xEFEBE9));   //Brown
    g_palette_material[ 23] = QColor(QRgb(0xD7CCC8));
    g_palette_material[ 24] = QColor(QRgb(0xBCAAA4));
    g_palette_material[ 25] = QColor(QRgb(0xA1887F));
    g_palette_material[ 26] = QColor(QRgb(0x8D6E63));
    g_palette_material[ 27] = QColor(QRgb(0x795548));
    g_palette_material[ 28] = QColor(QRgb(0x6D4C41));
    g_palette_material[ 29] = QColor(QRgb(0x5D4037));
    g_palette_material[ 30] = QColor(QRgb(0x4E342E));
    g_palette_material[ 31] = QColor(QRgb(0x3E2723));

    g_palette_material[ 32] = QColor(QRgb(0xFFEBEE));   //Red
    g_palette_material[ 33] = QColor(QRgb(0xFFCDD2));
    g_palette_material[ 34] = QColor(QRgb(0xEF9A9A));
    g_palette_material[ 35] = QColor(QRgb(0xE57373));
    g_palette_material[ 36] = QColor(QRgb(0xEF5350));
    g_palette_material[ 37] = QColor(QRgb(0xF44336));
    g_palette_material[ 38] = QColor(QRgb(0xE53935));
    g_palette_material[ 39] = QColor(QRgb(0xD32F2F));
    g_palette_material[ 40] = QColor(QRgb(0xC62828));
    g_palette_material[ 41] = QColor(QRgb(0xB71C1C));
    g_palette_material[ 42] = QColor(QRgb(0xFF8A80));   //Red Accent
    g_palette_material[ 43] = QColor(QRgb(0xFF5252));
    g_palette_material[ 44] = QColor(QRgb(0xFF1744));
    g_palette_material[ 45] = QColor(QRgb(0xD50000));

    g_palette_material[ 46] = QColor(QRgb(0xFCE4EC));   //Pink
    g_palette_material[ 47] = QColor(QRgb(0xF8BBD0));
    g_palette_material[ 48] = QColor(QRgb(0xF48FB1));
    g_palette_material[ 49] = QColor(QRgb(0xF06292));
    g_palette_material[ 50] = QColor(QRgb(0xEC407A));
    g_palette_material[ 51] = QColor(QRgb(0xE91E63));
    g_palette_material[ 52] = QColor(QRgb(0xD81B60));
    g_palette_material[ 53] = QColor(QRgb(0xC2185B));
    g_palette_material[ 54] = QColor(QRgb(0xAD1457));
    g_palette_material[ 55] = QColor(QRgb(0x880E4F));
    g_palette_material[ 56] = QColor(QRgb(0xFF80AB));   //Pink Accent
    g_palette_material[ 57] = QColor(QRgb(0xFF4081));
    g_palette_material[ 58] = QColor(QRgb(0xF50057));
    g_palette_material[ 59] = QColor(QRgb(0xC51162));

    g_palette_material[ 60] = QColor(QRgb(0xF3E5F5));   //Purple
    g_palette_material[ 61] = QColor(QRgb(0xE1BEE7));
    g_palette_material[ 62] = QColor(QRgb(0xCE93D8));
    g_palette_material[ 63] = QColor(QRgb(0xBA68C8));
    g_palette_material[ 64] = QColor(QRgb(0xAB47BC));
    g_palette_material[ 65] = QColor(QRgb(0x9C27B0));
    g_palette_material[ 66] = QColor(QRgb(0x8E24AA));
    g_palette_material[ 67] = QColor(QRgb(0x7B1FA2));
    g_palette_material[ 68] = QColor(QRgb(0x6A1B9A));
    g_palette_material[ 69] = QColor(QRgb(0x4A148C));
    g_palette_material[ 70] = QColor(QRgb(0xEA80FC));   //Purple Accent
    g_palette_material[ 71] = QColor(QRgb(0xE040FB));
    g_palette_material[ 72] = QColor(QRgb(0xD500F9));
    g_palette_material[ 73] = QColor(QRgb(0xAA00FF));

    g_palette_material[ 74] = QColor(QRgb(0xEDE7F6));   //Deep Purple
    g_palette_material[ 75] = QColor(QRgb(0xD1C4E9));
    g_palette_material[ 76] = QColor(QRgb(0xB39DDB));
    g_palette_material[ 77] = QColor(QRgb(0x9575CD));
    g_palette_material[ 78] = QColor(QRgb(0x7E57C2));
    g_palette_material[ 79] = QColor(QRgb(0x673AB7));
    g_palette_material[ 80] = QColor(QRgb(0x5E35B1));
    g_palette_material[ 81] = QColor(QRgb(0x512DA8));
    g_palette_material[ 82] = QColor(QRgb(0x4527A0));
    g_palette_material[ 83] = QColor(QRgb(0x311B92));
    g_palette_material[ 84] = QColor(QRgb(0xB388FF));   //Deep Purple Accent
    g_palette_material[ 85] = QColor(QRgb(0x7C4DFF));
    g_palette_material[ 86] = QColor(QRgb(0x651FFF));
    g_palette_material[ 87] = QColor(QRgb(0x6200EA));

    g_palette_material[ 88] = QColor(QRgb(0xE8EAF6));   //Indigo
    g_palette_material[ 89] = QColor(QRgb(0xC5CAE9));
    g_palette_material[ 90] = QColor(QRgb(0x9FA8DA));
    g_palette_material[ 91] = QColor(QRgb(0x7986CB));
    g_palette_material[ 92] = QColor(QRgb(0x5C6BC0));
    g_palette_material[ 93] = QColor(QRgb(0x3F51B5));
    g_palette_material[ 94] = QColor(QRgb(0x3949AB));
    g_palette_material[ 95] = QColor(QRgb(0x303F9F));
    g_palette_material[ 96] = QColor(QRgb(0x283593));
    g_palette_material[ 97] = QColor(QRgb(0x1A237E));
    g_palette_material[ 98] = QColor(QRgb(0x8C9EFF));   //Indigo Accent
    g_palette_material[ 99] = QColor(QRgb(0x536DFE));
    g_palette_material[100] = QColor(QRgb(0x3D5AFE));
    g_palette_material[101] = QColor(QRgb(0x304FFE));

    g_palette_material[102] = QColor(QRgb(0xE3F2FD));   //Blue
    g_palette_material[103] = QColor(QRgb(0xBBDEFB));
    g_palette_material[104] = QColor(QRgb(0x90CAF9));
    g_palette_material[105] = QColor(QRgb(0x64B5F6));
    g_palette_material[106] = QColor(QRgb(0x42A5F5));
    g_palette_material[107] = QColor(QRgb(0x2196F3));
    g_palette_material[108] = QColor(QRgb(0x1E88E5));
    g_palette_material[109] = QColor(QRgb(0x1976D2));
    g_palette_material[110] = QColor(QRgb(0x1565C0));
    g_palette_material[111] = QColor(QRgb(0x0D47A1));
    g_palette_material[112] = QColor(QRgb(0x82B1FF));   //Blue Accent
    g_palette_material[113] = QColor(QRgb(0x448AFF));
    g_palette_material[114] = QColor(QRgb(0x2979FF));
    g_palette_material[115] = QColor(QRgb(0x2962FF));

    g_palette_material[116] = QColor(QRgb(0xE1F5FE));   //Light Blue
    g_palette_material[117] = QColor(QRgb(0xB3E5FC));
    g_palette_material[118] = QColor(QRgb(0x81D4FA));
    g_palette_material[119] = QColor(QRgb(0x4FC3F7));
    g_palette_material[120] = QColor(QRgb(0x29B6F6));
    g_palette_material[121] = QColor(QRgb(0x03A9F4));
    g_palette_material[122] = QColor(QRgb(0x039BE5));
    g_palette_material[123] = QColor(QRgb(0x0288D1));
    g_palette_material[124] = QColor(QRgb(0x0277BD));
    g_palette_material[125] = QColor(QRgb(0x01579B));
    g_palette_material[126] = QColor(QRgb(0x80D8FF));   //Light Blue Accent
    g_palette_material[127] = QColor(QRgb(0x40C4FF));
    g_palette_material[128] = QColor(QRgb(0x00B0FF));
    g_palette_material[129] = QColor(QRgb(0x0091EA));

    g_palette_material[130] = QColor(QRgb(0xE0F7FA));   //Cyan
    g_palette_material[131] = QColor(QRgb(0xB2EBF2));
    g_palette_material[132] = QColor(QRgb(0x80DEEA));
    g_palette_material[133] = QColor(QRgb(0x4DD0E1));
    g_palette_material[134] = QColor(QRgb(0x26C6DA));
    g_palette_material[135] = QColor(QRgb(0x00BCD4));
    g_palette_material[136] = QColor(QRgb(0x00ACC1));
    g_palette_material[137] = QColor(QRgb(0x0097A7));
    g_palette_material[138] = QColor(QRgb(0x00838F));
    g_palette_material[139] = QColor(QRgb(0x006064));
    g_palette_material[140] = QColor(QRgb(0x84FFFF));   //Cyan Accent
    g_palette_material[141] = QColor(QRgb(0x18FFFF));
    g_palette_material[142] = QColor(QRgb(0x00E5FF));
    g_palette_material[143] = QColor(QRgb(0x00B8D4));

    g_palette_material[144] = QColor(QRgb(0xE0F2F1));   //Teal
    g_palette_material[145] = QColor(QRgb(0xB2DFDB));
    g_palette_material[146] = QColor(QRgb(0x80CBC4));
    g_palette_material[147] = QColor(QRgb(0x4DB6AC));
    g_palette_material[148] = QColor(QRgb(0x26A69A));
    g_palette_material[149] = QColor(QRgb(0x009688));
    g_palette_material[150] = QColor(QRgb(0x00897B));
    g_palette_material[151] = QColor(QRgb(0x00796B));
    g_palette_material[152] = QColor(QRgb(0x00695C));
    g_palette_material[153] = QColor(QRgb(0x004D40));
    g_palette_material[154] = QColor(QRgb(0xA7FFEB));   //Teal Accent
    g_palette_material[155] = QColor(QRgb(0x64FFDA));
    g_palette_material[156] = QColor(QRgb(0x1DE9B6));
    g_palette_material[157] = QColor(QRgb(0x00BFA5));

    g_palette_material[158] = QColor(QRgb(0xE8F5E9));   //Green
    g_palette_material[159] = QColor(QRgb(0xC8E6C9));
    g_palette_material[160] = QColor(QRgb(0xA5D6A7));
    g_palette_material[161] = QColor(QRgb(0x81C784));
    g_palette_material[162] = QColor(QRgb(0x66BB6A));
    g_palette_material[163] = QColor(QRgb(0x4CAF50));
    g_palette_material[164] = QColor(QRgb(0x43A047));
    g_palette_material[165] = QColor(QRgb(0x388E3C));
    g_palette_material[166] = QColor(QRgb(0x2E7D32));
    g_palette_material[167] = QColor(QRgb(0x1B5E20));
    g_palette_material[168] = QColor(QRgb(0xB9F6CA));   //Green Accent
    g_palette_material[169] = QColor(QRgb(0x69F0AE));
    g_palette_material[170] = QColor(QRgb(0x00E676));
    g_palette_material[171] = QColor(QRgb(0x00C853));

    g_palette_material[172] = QColor(QRgb(0xF1F8E9));   //Light Green
    g_palette_material[173] = QColor(QRgb(0xDCEDC8));
    g_palette_material[174] = QColor(QRgb(0xC5E1A5));
    g_palette_material[175] = QColor(QRgb(0xAED581));
    g_palette_material[176] = QColor(QRgb(0x9CCC65));
    g_palette_material[177] = QColor(QRgb(0x8BC34A));
    g_palette_material[178] = QColor(QRgb(0x7CB342));
    g_palette_material[179] = QColor(QRgb(0x689F38));
    g_palette_material[180] = QColor(QRgb(0x558B2F));
    g_palette_material[181] = QColor(QRgb(0x33691E));
    g_palette_material[182] = QColor(QRgb(0xCCFF90));   //Light Green Accent
    g_palette_material[183] = QColor(QRgb(0xB2FF59));
    g_palette_material[184] = QColor(QRgb(0x76FF03));
    g_palette_material[185] = QColor(QRgb(0x64DD17));

    g_palette_material[186] = QColor(QRgb(0xF9FBE7));   //Lime
    g_palette_material[187] = QColor(QRgb(0xF0F4C3));
    g_palette_material[188] = QColor(QRgb(0xE6EE9C));
    g_palette_material[189] = QColor(QRgb(0xDCE775));
    g_palette_material[190] = QColor(QRgb(0xD4E157));
    g_palette_material[191] = QColor(QRgb(0xCDDC39));
    g_palette_material[192] = QColor(QRgb(0xC0CA33));
    g_palette_material[193] = QColor(QRgb(0xAFB42B));
    g_palette_material[194] = QColor(QRgb(0x9E9D24));
    g_palette_material[195] = QColor(QRgb(0x827717));
    g_palette_material[196] = QColor(QRgb(0xF4FF81));   //Lime Accent
    g_palette_material[197] = QColor(QRgb(0xEEFF41));
    g_palette_material[198] = QColor(QRgb(0xC6FF00));
    g_palette_material[199] = QColor(QRgb(0xAEEA00));

    g_palette_material[200] = QColor(QRgb(0xFFFDE7));   //Yellow
    g_palette_material[201] = QColor(QRgb(0xFFF9C4));
    g_palette_material[202] = QColor(QRgb(0xFFF59D));
    g_palette_material[203] = QColor(QRgb(0xFFF176));
    g_palette_material[204] = QColor(QRgb(0xFFEE58));
    g_palette_material[205] = QColor(QRgb(0xFFEB3B));
    g_palette_material[206] = QColor(QRgb(0xFDD835));
    g_palette_material[207] = QColor(QRgb(0xFBC02D));
    g_palette_material[208] = QColor(QRgb(0xF9A825));
    g_palette_material[209] = QColor(QRgb(0xF57F17));
    g_palette_material[210] = QColor(QRgb(0xFFFF8D));   //Yellow Accent
    g_palette_material[211] = QColor(QRgb(0xFFFF00));
    g_palette_material[212] = QColor(QRgb(0xFFEA00));
    g_palette_material[213] = QColor(QRgb(0xFFD600));

    g_palette_material[214] = QColor(QRgb(0xFFF8E1));   //Amber
    g_palette_material[215] = QColor(QRgb(0xFFECB3));
    g_palette_material[216] = QColor(QRgb(0xFFE082));
    g_palette_material[217] = QColor(QRgb(0xFFD54F));
    g_palette_material[218] = QColor(QRgb(0xFFCA28));
    g_palette_material[219] = QColor(QRgb(0xFFC107));
    g_palette_material[220] = QColor(QRgb(0xFFB300));
    g_palette_material[221] = QColor(QRgb(0xFFA000));
    g_palette_material[222] = QColor(QRgb(0xFF8F00));
    g_palette_material[223] = QColor(QRgb(0xFF6F00));
    g_palette_material[224] = QColor(QRgb(0xFFE57F));   //Amber Accent
    g_palette_material[225] = QColor(QRgb(0xFFD740));
    g_palette_material[226] = QColor(QRgb(0xFFC400));
    g_palette_material[227] = QColor(QRgb(0xFFAB00));

    g_palette_material[228] = QColor(QRgb(0xFFF3E0));   //Orange
    g_palette_material[229] = QColor(QRgb(0xFFE0B2));
    g_palette_material[230] = QColor(QRgb(0xFFCC80));
    g_palette_material[231] = QColor(QRgb(0xFFB74D));
    g_palette_material[232] = QColor(QRgb(0xFFA726));
    g_palette_material[233] = QColor(QRgb(0xFF9800));
    g_palette_material[234] = QColor(QRgb(0xFB8C00));
    g_palette_material[235] = QColor(QRgb(0xF57C00));
    g_palette_material[236] = QColor(QRgb(0xEF6C00));
    g_palette_material[237] = QColor(QRgb(0xE65100));
    g_palette_material[238] = QColor(QRgb(0xFFD180));   //Orange Accent
    g_palette_material[239] = QColor(QRgb(0xFFAB40));
    g_palette_material[240] = QColor(QRgb(0xFF9100));
    g_palette_material[241] = QColor(QRgb(0xFF6D00));

    g_palette_material[242] = QColor(QRgb(0xFBE9E7));   //Deep Orange
    g_palette_material[243] = QColor(QRgb(0xFFCCBC));
    g_palette_material[244] = QColor(QRgb(0xFFAB91));
    g_palette_material[245] = QColor(QRgb(0xFF8A65));
    g_palette_material[246] = QColor(QRgb(0xFF7043));
    g_palette_material[247] = QColor(QRgb(0xFF5722));
    g_palette_material[248] = QColor(QRgb(0xF4511E));
    g_palette_material[249] = QColor(QRgb(0xE64A19));
    g_palette_material[250] = QColor(QRgb(0xD84315));
    g_palette_material[251] = QColor(QRgb(0xBF360C));
    g_palette_material[252] = QColor(QRgb(0xFF9E80));   //Deep Orange Accent
    g_palette_material[253] = QColor(QRgb(0xFF6E40));
    g_palette_material[254] = QColor(QRgb(0xFF3D00));
    g_palette_material[255] = QColor(QRgb(0xDD2C00));
}



}   // namespace Dr



















