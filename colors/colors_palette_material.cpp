//
//      Created by Stephens Nunnally on 4/4/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QColor>

#include "colors.h"

namespace Dr {

void LoadPaletteMaterial(Palette_Info &palette) {
    palette.name = "Material";
    palette.show_in_list = true;

    palette.colors.append( QColor(QRgb(0x000000)) );    //   0 Black
    palette.colors.append( QColor(QRgb(0xFFFFFF)) );    //   1 White

    palette.colors.append( QColor(QRgb(0xFAFAFA)) );    //   2 Gray
    palette.colors.append( QColor(QRgb(0xF5F5F5)) );    //   3
    palette.colors.append( QColor(QRgb(0xEEEEEE)) );    //   4
    palette.colors.append( QColor(QRgb(0xE0E0E0)) );    //   5
    palette.colors.append( QColor(QRgb(0xBDBDBD)) );    //   6
    palette.colors.append( QColor(QRgb(0x9E9E9E)) );    //   7
    palette.colors.append( QColor(QRgb(0x757575)) );    //   8
    palette.colors.append( QColor(QRgb(0x616161)) );    //   9
    palette.colors.append( QColor(QRgb(0x424242)) );    //  10
    palette.colors.append( QColor(QRgb(0x212121)) );    //  11

    palette.colors.append( QColor(QRgb(0xECEFF1)) );    //  12 Blue Gray
    palette.colors.append( QColor(QRgb(0xCFD8DC)) );    //  13
    palette.colors.append( QColor(QRgb(0xB0BEC5)) );    //  14
    palette.colors.append( QColor(QRgb(0x90A4AE)) );    //  15
    palette.colors.append( QColor(QRgb(0x78909C)) );    //  16
    palette.colors.append( QColor(QRgb(0x607D8B)) );    //  17
    palette.colors.append( QColor(QRgb(0x546E7A)) );    //  18
    palette.colors.append( QColor(QRgb(0x455A64)) );    //  19
    palette.colors.append( QColor(QRgb(0x37474F)) );    //  20
    palette.colors.append( QColor(QRgb(0x263238)) );    //  21

    palette.colors.append( QColor(QRgb(0xEFEBE9)) );    //  22 Brown
    palette.colors.append( QColor(QRgb(0xD7CCC8)) );    //  23
    palette.colors.append( QColor(QRgb(0xBCAAA4)) );    //  24
    palette.colors.append( QColor(QRgb(0xA1887F)) );    //  25
    palette.colors.append( QColor(QRgb(0x8D6E63)) );    //  26
    palette.colors.append( QColor(QRgb(0x795548)) );    //  27
    palette.colors.append( QColor(QRgb(0x6D4C41)) );    //  29
    palette.colors.append( QColor(QRgb(0x5D4037)) );    //  29
    palette.colors.append( QColor(QRgb(0x4E342E)) );    //  30
    palette.colors.append( QColor(QRgb(0x3E2723)) );    //  31

    palette.colors.append( QColor(QRgb(0xFFEBEE)) );    //  32 Red
    palette.colors.append( QColor(QRgb(0xFFCDD2)) );    //  33
    palette.colors.append( QColor(QRgb(0xEF9A9A)) );    //  34
    palette.colors.append( QColor(QRgb(0xE57373)) );    //  35
    palette.colors.append( QColor(QRgb(0xEF5350)) );    //  36
    palette.colors.append( QColor(QRgb(0xF44336)) );    //  37
    palette.colors.append( QColor(QRgb(0xE53935)) );    //  38
    palette.colors.append( QColor(QRgb(0xD32F2F)) );    //  39
    palette.colors.append( QColor(QRgb(0xC62828)) );    //  40
    palette.colors.append( QColor(QRgb(0xB71C1C)) );    //  41
    palette.colors.append( QColor(QRgb(0xFF8A80)) );    //  42 Red Accent
    palette.colors.append( QColor(QRgb(0xFF5252)) );    //  43
    palette.colors.append( QColor(QRgb(0xFF1744)) );    //  44
    palette.colors.append( QColor(QRgb(0xD50000)) );    //  45

    palette.colors.append( QColor(QRgb(0xFCE4EC)) );    //  46 Pink
    palette.colors.append( QColor(QRgb(0xF8BBD0)) );    //  47
    palette.colors.append( QColor(QRgb(0xF48FB1)) );    //  48
    palette.colors.append( QColor(QRgb(0xF06292)) );    //  49
    palette.colors.append( QColor(QRgb(0xEC407A)) );    //  50
    palette.colors.append( QColor(QRgb(0xE91E63)) );    //  51
    palette.colors.append( QColor(QRgb(0xD81B60)) );    //  52
    palette.colors.append( QColor(QRgb(0xC2185B)) );    //  53
    palette.colors.append( QColor(QRgb(0xAD1457)) );    //  54
    palette.colors.append( QColor(QRgb(0x880E4F)) );    //  55
    palette.colors.append( QColor(QRgb(0xFF80AB)) );    //  56 Pink Accent
    palette.colors.append( QColor(QRgb(0xFF4081)) );    //  57
    palette.colors.append( QColor(QRgb(0xF50057)) );    //  58
    palette.colors.append( QColor(QRgb(0xC51162)) );    //  59

    palette.colors.append( QColor(QRgb(0xF3E5F5)) );    //  60 Purple
    palette.colors.append( QColor(QRgb(0xE1BEE7)) );    //
    palette.colors.append( QColor(QRgb(0xCE93D8)) );    //
    palette.colors.append( QColor(QRgb(0xBA68C8)) );    //
    palette.colors.append( QColor(QRgb(0xAB47BC)) );    //
    palette.colors.append( QColor(QRgb(0x9C27B0)) );    //
    palette.colors.append( QColor(QRgb(0x8E24AA)) );    //
    palette.colors.append( QColor(QRgb(0x7B1FA2)) );    //
    palette.colors.append( QColor(QRgb(0x6A1B9A)) );    //
    palette.colors.append( QColor(QRgb(0x4A148C)) );    //
    palette.colors.append( QColor(QRgb(0xEA80FC)) );    //  70 Purple Accent
    palette.colors.append( QColor(QRgb(0xE040FB)) );    //
    palette.colors.append( QColor(QRgb(0xD500F9)) );    //
    palette.colors.append( QColor(QRgb(0xAA00FF)) );    //  73

    palette.colors.append( QColor(QRgb(0xEDE7F6)) );    //  74 Deep Purple
    palette.colors.append( QColor(QRgb(0xD1C4E9)) );    //
    palette.colors.append( QColor(QRgb(0xB39DDB)) );    //
    palette.colors.append( QColor(QRgb(0x9575CD)) );    //
    palette.colors.append( QColor(QRgb(0x7E57C2)) );    //
    palette.colors.append( QColor(QRgb(0x673AB7)) );    //
    palette.colors.append( QColor(QRgb(0x5E35B1)) );    //
    palette.colors.append( QColor(QRgb(0x512DA8)) );    //
    palette.colors.append( QColor(QRgb(0x4527A0)) );    //
    palette.colors.append( QColor(QRgb(0x311B92)) );    //
    palette.colors.append( QColor(QRgb(0xB388FF)) );    //  84 Deep Purple Accent
    palette.colors.append( QColor(QRgb(0x7C4DFF)) );    //
    palette.colors.append( QColor(QRgb(0x651FFF)) );    //
    palette.colors.append( QColor(QRgb(0x6200EA)) );    //  87

    palette.colors.append( QColor(QRgb(0xE8EAF6)) );    //  88 Indigo
    palette.colors.append( QColor(QRgb(0xC5CAE9)) );    //
    palette.colors.append( QColor(QRgb(0x9FA8DA)) );    //
    palette.colors.append( QColor(QRgb(0x7986CB)) );    //
    palette.colors.append( QColor(QRgb(0x5C6BC0)) );    //
    palette.colors.append( QColor(QRgb(0x3F51B5)) );    //
    palette.colors.append( QColor(QRgb(0x3949AB)) );    //
    palette.colors.append( QColor(QRgb(0x303F9F)) );    //
    palette.colors.append( QColor(QRgb(0x283593)) );    //
    palette.colors.append( QColor(QRgb(0x1A237E)) );    //
    palette.colors.append( QColor(QRgb(0x8C9EFF)) );    //  98 Indigo Accent
    palette.colors.append( QColor(QRgb(0x536DFE)) );    //
    palette.colors.append( QColor(QRgb(0x3D5AFE)) );    //
    palette.colors.append( QColor(QRgb(0x304FFE)) );    // 101

    palette.colors.append( QColor(QRgb(0xE3F2FD)) );    // 102 Blue
    palette.colors.append( QColor(QRgb(0xBBDEFB)) );    //
    palette.colors.append( QColor(QRgb(0x90CAF9)) );    //
    palette.colors.append( QColor(QRgb(0x64B5F6)) );    //
    palette.colors.append( QColor(QRgb(0x42A5F5)) );    //
    palette.colors.append( QColor(QRgb(0x2196F3)) );    //
    palette.colors.append( QColor(QRgb(0x1E88E5)) );    //
    palette.colors.append( QColor(QRgb(0x1976D2)) );    //
    palette.colors.append( QColor(QRgb(0x1565C0)) );    //
    palette.colors.append( QColor(QRgb(0x0D47A1)) );    //
    palette.colors.append( QColor(QRgb(0x82B1FF)) );    // 112 Blue Accent
    palette.colors.append( QColor(QRgb(0x448AFF)) );    //
    palette.colors.append( QColor(QRgb(0x2979FF)) );    //
    palette.colors.append( QColor(QRgb(0x2962FF)) );    // 115

    palette.colors.append( QColor(QRgb(0xE1F5FE)) );    // 116 Light Blue
    palette.colors.append( QColor(QRgb(0xB3E5FC)) );    //
    palette.colors.append( QColor(QRgb(0x81D4FA)) );    //
    palette.colors.append( QColor(QRgb(0x4FC3F7)) );    //
    palette.colors.append( QColor(QRgb(0x29B6F6)) );    //
    palette.colors.append( QColor(QRgb(0x03A9F4)) );    //
    palette.colors.append( QColor(QRgb(0x039BE5)) );    //
    palette.colors.append( QColor(QRgb(0x0288D1)) );    //
    palette.colors.append( QColor(QRgb(0x0277BD)) );    //
    palette.colors.append( QColor(QRgb(0x01579B)) );    //
    palette.colors.append( QColor(QRgb(0x80D8FF)) );    // 126 Light Blue Accent
    palette.colors.append( QColor(QRgb(0x40C4FF)) );    //
    palette.colors.append( QColor(QRgb(0x00B0FF)) );    //
    palette.colors.append( QColor(QRgb(0x0091EA)) );    // 129

    palette.colors.append( QColor(QRgb(0xE0F7FA)) );    // 130 Cyan
    palette.colors.append( QColor(QRgb(0xB2EBF2)) );    //
    palette.colors.append( QColor(QRgb(0x80DEEA)) );    //
    palette.colors.append( QColor(QRgb(0x4DD0E1)) );    //
    palette.colors.append( QColor(QRgb(0x26C6DA)) );    //
    palette.colors.append( QColor(QRgb(0x00BCD4)) );    //
    palette.colors.append( QColor(QRgb(0x00ACC1)) );    //
    palette.colors.append( QColor(QRgb(0x0097A7)) );    //
    palette.colors.append( QColor(QRgb(0x00838F)) );    //
    palette.colors.append( QColor(QRgb(0x006064)) );    //
    palette.colors.append( QColor(QRgb(0x84FFFF)) );    // 140 Cyan Accent
    palette.colors.append( QColor(QRgb(0x18FFFF)) );    //
    palette.colors.append( QColor(QRgb(0x00E5FF)) );    //
    palette.colors.append( QColor(QRgb(0x00B8D4)) );    // 143

    palette.colors.append( QColor(QRgb(0xE0F2F1)) );    // 144 Teal
    palette.colors.append( QColor(QRgb(0xB2DFDB)) );    //
    palette.colors.append( QColor(QRgb(0x80CBC4)) );    //
    palette.colors.append( QColor(QRgb(0x4DB6AC)) );    //
    palette.colors.append( QColor(QRgb(0x26A69A)) );    //
    palette.colors.append( QColor(QRgb(0x009688)) );    //
    palette.colors.append( QColor(QRgb(0x00897B)) );    //
    palette.colors.append( QColor(QRgb(0x00796B)) );    //
    palette.colors.append( QColor(QRgb(0x00695C)) );    //
    palette.colors.append( QColor(QRgb(0x004D40)) );    //
    palette.colors.append( QColor(QRgb(0xA7FFEB)) );    // 154 Teal Accent
    palette.colors.append( QColor(QRgb(0x64FFDA)) );    //
    palette.colors.append( QColor(QRgb(0x1DE9B6)) );    //
    palette.colors.append( QColor(QRgb(0x00BFA5)) );    // 157

    palette.colors.append( QColor(QRgb(0xE8F5E9)) );    // 158 Green
    palette.colors.append( QColor(QRgb(0xC8E6C9)) );    //
    palette.colors.append( QColor(QRgb(0xA5D6A7)) );    //
    palette.colors.append( QColor(QRgb(0x81C784)) );    //
    palette.colors.append( QColor(QRgb(0x66BB6A)) );    //
    palette.colors.append( QColor(QRgb(0x4CAF50)) );    //
    palette.colors.append( QColor(QRgb(0x43A047)) );    //
    palette.colors.append( QColor(QRgb(0x388E3C)) );    //
    palette.colors.append( QColor(QRgb(0x2E7D32)) );    //
    palette.colors.append( QColor(QRgb(0x1B5E20)) );    //
    palette.colors.append( QColor(QRgb(0xB9F6CA)) );    // 168 Green Accent
    palette.colors.append( QColor(QRgb(0x69F0AE)) );    //
    palette.colors.append( QColor(QRgb(0x00E676)) );    //
    palette.colors.append( QColor(QRgb(0x00C853)) );    // 171

    palette.colors.append( QColor(QRgb(0xF1F8E9)) );    // 172 Light Green
    palette.colors.append( QColor(QRgb(0xDCEDC8)) );    //
    palette.colors.append( QColor(QRgb(0xC5E1A5)) );    //
    palette.colors.append( QColor(QRgb(0xAED581)) );    //
    palette.colors.append( QColor(QRgb(0x9CCC65)) );    //
    palette.colors.append( QColor(QRgb(0x8BC34A)) );    //
    palette.colors.append( QColor(QRgb(0x7CB342)) );    //
    palette.colors.append( QColor(QRgb(0x689F38)) );    //
    palette.colors.append( QColor(QRgb(0x558B2F)) );    //
    palette.colors.append( QColor(QRgb(0x33691E)) );    //
    palette.colors.append( QColor(QRgb(0xCCFF90)) );    // 182 Light Green Accent
    palette.colors.append( QColor(QRgb(0xB2FF59)) );    //
    palette.colors.append( QColor(QRgb(0x76FF03)) );    //
    palette.colors.append( QColor(QRgb(0x64DD17)) );    // 185

    palette.colors.append( QColor(QRgb(0xF9FBE7)) );    // 186 Lime
    palette.colors.append( QColor(QRgb(0xF0F4C3)) );    //
    palette.colors.append( QColor(QRgb(0xE6EE9C)) );    //
    palette.colors.append( QColor(QRgb(0xDCE775)) );    //
    palette.colors.append( QColor(QRgb(0xD4E157)) );    //
    palette.colors.append( QColor(QRgb(0xCDDC39)) );    //
    palette.colors.append( QColor(QRgb(0xC0CA33)) );    //
    palette.colors.append( QColor(QRgb(0xAFB42B)) );    //
    palette.colors.append( QColor(QRgb(0x9E9D24)) );    //
    palette.colors.append( QColor(QRgb(0x827717)) );    //
    palette.colors.append( QColor(QRgb(0xF4FF81)) );    // 196 Lime Accent
    palette.colors.append( QColor(QRgb(0xEEFF41)) );    //
    palette.colors.append( QColor(QRgb(0xC6FF00)) );    //
    palette.colors.append( QColor(QRgb(0xAEEA00)) );    // 199

    palette.colors.append( QColor(QRgb(0xFFFDE7)) );    // 200 Yellow
    palette.colors.append( QColor(QRgb(0xFFF9C4)) );    //
    palette.colors.append( QColor(QRgb(0xFFF59D)) );    //
    palette.colors.append( QColor(QRgb(0xFFF176)) );    //
    palette.colors.append( QColor(QRgb(0xFFEE58)) );    //
    palette.colors.append( QColor(QRgb(0xFFEB3B)) );    //
    palette.colors.append( QColor(QRgb(0xFDD835)) );    //
    palette.colors.append( QColor(QRgb(0xFBC02D)) );    //
    palette.colors.append( QColor(QRgb(0xF9A825)) );    //
    palette.colors.append( QColor(QRgb(0xF57F17)) );    //
    palette.colors.append( QColor(QRgb(0xFFFF8D)) );    // 210 Yellow Accent
    palette.colors.append( QColor(QRgb(0xFFFF00)) );    //
    palette.colors.append( QColor(QRgb(0xFFEA00)) );    //
    palette.colors.append( QColor(QRgb(0xFFD600)) );    // 213

    palette.colors.append( QColor(QRgb(0xFFF8E1)) );    // 214 Amber
    palette.colors.append( QColor(QRgb(0xFFECB3)) );    //
    palette.colors.append( QColor(QRgb(0xFFE082)) );    //
    palette.colors.append( QColor(QRgb(0xFFD54F)) );    //
    palette.colors.append( QColor(QRgb(0xFFCA28)) );    //
    palette.colors.append( QColor(QRgb(0xFFC107)) );    //
    palette.colors.append( QColor(QRgb(0xFFB300)) );    //
    palette.colors.append( QColor(QRgb(0xFFA000)) );    //
    palette.colors.append( QColor(QRgb(0xFF8F00)) );    //
    palette.colors.append( QColor(QRgb(0xFF6F00)) );    //
    palette.colors.append( QColor(QRgb(0xFFE57F)) );    // 224 Amber Accent
    palette.colors.append( QColor(QRgb(0xFFD740)) );    //
    palette.colors.append( QColor(QRgb(0xFFC400)) );    //
    palette.colors.append( QColor(QRgb(0xFFAB00)) );    // 227

    palette.colors.append( QColor(QRgb(0xFFF3E0)) );    // 228 Orange
    palette.colors.append( QColor(QRgb(0xFFE0B2)) );    //
    palette.colors.append( QColor(QRgb(0xFFCC80)) );    //
    palette.colors.append( QColor(QRgb(0xFFB74D)) );    //
    palette.colors.append( QColor(QRgb(0xFFA726)) );    //
    palette.colors.append( QColor(QRgb(0xFF9800)) );    //
    palette.colors.append( QColor(QRgb(0xFB8C00)) );    //
    palette.colors.append( QColor(QRgb(0xF57C00)) );    //
    palette.colors.append( QColor(QRgb(0xEF6C00)) );    //
    palette.colors.append( QColor(QRgb(0xE65100)) );     //
    palette.colors.append( QColor(QRgb(0xFFD180)) );    // 238 Orange Accent
    palette.colors.append( QColor(QRgb(0xFFAB40)) );    //
    palette.colors.append( QColor(QRgb(0xFF9100)) );    //
    palette.colors.append( QColor(QRgb(0xFF6D00)) );    // 241

    palette.colors.append( QColor(QRgb(0xFBE9E7)) );    // 242 Deep Orange
    palette.colors.append( QColor(QRgb(0xFFCCBC)) );    //
    palette.colors.append( QColor(QRgb(0xFFAB91)) );    //
    palette.colors.append( QColor(QRgb(0xFF8A65)) );    //
    palette.colors.append( QColor(QRgb(0xFF7043)) );    //
    palette.colors.append( QColor(QRgb(0xFF5722)) );    //
    palette.colors.append( QColor(QRgb(0xF4511E)) );    //
    palette.colors.append( QColor(QRgb(0xE64A19)) );    //
    palette.colors.append( QColor(QRgb(0xD84315)) );    //
    palette.colors.append( QColor(QRgb(0xBF360C)) );    //
    palette.colors.append( QColor(QRgb(0xFF9E80)) );    // 252 Deep Orange Accent
    palette.colors.append( QColor(QRgb(0xFF6E40)) );    //
    palette.colors.append( QColor(QRgb(0xFF3D00)) );    //
    palette.colors.append( QColor(QRgb(0xDD2C00)) );    // 255

    palette.number_of_colors = palette.colors.count();
}


}   // namespace Dr




