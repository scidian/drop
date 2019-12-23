//
//      Created by Stephens Nunnally on 4/4/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "core/colors/colors.h"
#include "core/types/dr_color.h"

namespace Dr {


void LoadPaletteMaterial(Palette_Info &palette) {
    palette.name = "Material";
    palette.show_in_list = true;

    palette.colors.push_back( DrColor((0xFF000000)) );    //   0 Black
    palette.colors.push_back( DrColor((0xFFFFFFFF)) );    //   1 White

    palette.colors.push_back( DrColor((0xFFFAFAFA)) );    //   2 Gray
    palette.colors.push_back( DrColor((0xFFF5F5F5)) );    //   3
    palette.colors.push_back( DrColor((0xFFEEEEEE)) );    //   4
    palette.colors.push_back( DrColor((0xFFE0E0E0)) );    //   5
    palette.colors.push_back( DrColor((0xFFBDBDBD)) );    //   6
    palette.colors.push_back( DrColor((0xFF9E9E9E)) );    //   7
    palette.colors.push_back( DrColor((0xFF757575)) );    //   8
    palette.colors.push_back( DrColor((0xFF616161)) );    //   9
    palette.colors.push_back( DrColor((0xFF424242)) );    //  10
    palette.colors.push_back( DrColor((0xFF212121)) );    //  11

    palette.colors.push_back( DrColor((0xFFECEFF1)) );    //  12 Blue Gray
    palette.colors.push_back( DrColor((0xFFCFD8DC)) );    //  13
    palette.colors.push_back( DrColor((0xFFB0BEC5)) );    //  14
    palette.colors.push_back( DrColor((0xFF90A4AE)) );    //  15
    palette.colors.push_back( DrColor((0xFF78909C)) );    //  16
    palette.colors.push_back( DrColor((0xFF607D8B)) );    //  17
    palette.colors.push_back( DrColor((0xFF546E7A)) );    //  18
    palette.colors.push_back( DrColor((0xFF455A64)) );    //  19
    palette.colors.push_back( DrColor((0xFF37474F)) );    //  20
    palette.colors.push_back( DrColor((0xFF263238)) );    //  21

    palette.colors.push_back( DrColor((0xFFEFEBE9)) );    //  22 Brown
    palette.colors.push_back( DrColor((0xFFD7CCC8)) );    //  23
    palette.colors.push_back( DrColor((0xFFBCAAA4)) );    //  24
    palette.colors.push_back( DrColor((0xFFA1887F)) );    //  25
    palette.colors.push_back( DrColor((0xFF8D6E63)) );    //  26
    palette.colors.push_back( DrColor((0xFF795548)) );    //  27
    palette.colors.push_back( DrColor((0xFF6D4C41)) );    //  29
    palette.colors.push_back( DrColor((0xFF5D4037)) );    //  29
    palette.colors.push_back( DrColor((0xFF4E342E)) );    //  30
    palette.colors.push_back( DrColor((0xFF3E2723)) );    //  31

    palette.colors.push_back( DrColor((0xFFFFEBEE)) );    //  32 Red
    palette.colors.push_back( DrColor((0xFFFFCDD2)) );    //  33
    palette.colors.push_back( DrColor((0xFFEF9A9A)) );    //  34
    palette.colors.push_back( DrColor((0xFFE57373)) );    //  35
    palette.colors.push_back( DrColor((0xFFEF5350)) );    //  36
    palette.colors.push_back( DrColor((0xFFF44336)) );    //  37
    palette.colors.push_back( DrColor((0xFFE53935)) );    //  38
    palette.colors.push_back( DrColor((0xFFD32F2F)) );    //  39
    palette.colors.push_back( DrColor((0xFFC62828)) );    //  40
    palette.colors.push_back( DrColor((0xFFB71C1C)) );    //  41
    palette.colors.push_back( DrColor((0xFFFF8A80)) );    //  42 Red Accent
    palette.colors.push_back( DrColor((0xFFFF5252)) );    //  43
    palette.colors.push_back( DrColor((0xFFFF1744)) );    //  44
    palette.colors.push_back( DrColor((0xFFD50000)) );    //  45

    palette.colors.push_back( DrColor((0xFFFCE4EC)) );    //  46 Pink
    palette.colors.push_back( DrColor((0xFFF8BBD0)) );    //  47
    palette.colors.push_back( DrColor((0xFFF48FB1)) );    //  48
    palette.colors.push_back( DrColor((0xFFF06292)) );    //  49
    palette.colors.push_back( DrColor((0xFFEC407A)) );    //  50
    palette.colors.push_back( DrColor((0xFFE91E63)) );    //  51
    palette.colors.push_back( DrColor((0xFFD81B60)) );    //  52
    palette.colors.push_back( DrColor((0xFFC2185B)) );    //  53
    palette.colors.push_back( DrColor((0xFFAD1457)) );    //  54
    palette.colors.push_back( DrColor((0xFF880E4F)) );    //  55
    palette.colors.push_back( DrColor((0xFFFF80AB)) );    //  56 Pink Accent
    palette.colors.push_back( DrColor((0xFFFF4081)) );    //  57
    palette.colors.push_back( DrColor((0xFFF50057)) );    //  58
    palette.colors.push_back( DrColor((0xFFC51162)) );    //  59

    palette.colors.push_back( DrColor((0xFFF3E5F5)) );    //  60 Purple
    palette.colors.push_back( DrColor((0xFFE1BEE7)) );    //
    palette.colors.push_back( DrColor((0xFFCE93D8)) );    //
    palette.colors.push_back( DrColor((0xFFBA68C8)) );    //
    palette.colors.push_back( DrColor((0xFFAB47BC)) );    //
    palette.colors.push_back( DrColor((0xFF9C27B0)) );    //
    palette.colors.push_back( DrColor((0xFF8E24AA)) );    //
    palette.colors.push_back( DrColor((0xFF7B1FA2)) );    //
    palette.colors.push_back( DrColor((0xFF6A1B9A)) );    //
    palette.colors.push_back( DrColor((0xFF4A148C)) );    //
    palette.colors.push_back( DrColor((0xFFEA80FC)) );    //  70 Purple Accent
    palette.colors.push_back( DrColor((0xFFE040FB)) );    //
    palette.colors.push_back( DrColor((0xFFD500F9)) );    //
    palette.colors.push_back( DrColor((0xFFAA00FF)) );    //  73

    palette.colors.push_back( DrColor((0xFFEDE7F6)) );    //  74 Deep Purple
    palette.colors.push_back( DrColor((0xFFD1C4E9)) );    //
    palette.colors.push_back( DrColor((0xFFB39DDB)) );    //
    palette.colors.push_back( DrColor((0xFF9575CD)) );    //
    palette.colors.push_back( DrColor((0xFF7E57C2)) );    //
    palette.colors.push_back( DrColor((0xFF673AB7)) );    //
    palette.colors.push_back( DrColor((0xFF5E35B1)) );    //
    palette.colors.push_back( DrColor((0xFF512DA8)) );    //
    palette.colors.push_back( DrColor((0xFF4527A0)) );    //
    palette.colors.push_back( DrColor((0xFF311B92)) );    //
    palette.colors.push_back( DrColor((0xFFB388FF)) );    //  84 Deep Purple Accent
    palette.colors.push_back( DrColor((0xFF7C4DFF)) );    //
    palette.colors.push_back( DrColor((0xFF651FFF)) );    //
    palette.colors.push_back( DrColor((0xFF6200EA)) );    //  87

    palette.colors.push_back( DrColor((0xFFE8EAF6)) );    //  88 Indigo
    palette.colors.push_back( DrColor((0xFFC5CAE9)) );    //
    palette.colors.push_back( DrColor((0xFF9FA8DA)) );    //
    palette.colors.push_back( DrColor((0xFF7986CB)) );    //
    palette.colors.push_back( DrColor((0xFF5C6BC0)) );    //
    palette.colors.push_back( DrColor((0xFF3F51B5)) );    //
    palette.colors.push_back( DrColor((0xFF3949AB)) );    //
    palette.colors.push_back( DrColor((0xFF303F9F)) );    //
    palette.colors.push_back( DrColor((0xFF283593)) );    //
    palette.colors.push_back( DrColor((0xFF1A237E)) );    //
    palette.colors.push_back( DrColor((0xFF8C9EFF)) );    //  98 Indigo Accent
    palette.colors.push_back( DrColor((0xFF536DFE)) );    //
    palette.colors.push_back( DrColor((0xFF3D5AFE)) );    //
    palette.colors.push_back( DrColor((0xFF304FFE)) );    // 101

    palette.colors.push_back( DrColor((0xFFE3F2FD)) );    // 102 Blue
    palette.colors.push_back( DrColor((0xFFBBDEFB)) );    //
    palette.colors.push_back( DrColor((0xFF90CAF9)) );    //
    palette.colors.push_back( DrColor((0xFF64B5F6)) );    //
    palette.colors.push_back( DrColor((0xFF42A5F5)) );    //
    palette.colors.push_back( DrColor((0xFF2196F3)) );    //
    palette.colors.push_back( DrColor((0xFF1E88E5)) );    //
    palette.colors.push_back( DrColor((0xFF1976D2)) );    //
    palette.colors.push_back( DrColor((0xFF1565C0)) );    //
    palette.colors.push_back( DrColor((0xFF0D47A1)) );    //
    palette.colors.push_back( DrColor((0xFF82B1FF)) );    // 112 Blue Accent
    palette.colors.push_back( DrColor((0xFF448AFF)) );    //
    palette.colors.push_back( DrColor((0xFF2979FF)) );    //
    palette.colors.push_back( DrColor((0xFF2962FF)) );    // 115

    palette.colors.push_back( DrColor((0xFFE1F5FE)) );    // 116 Light Blue
    palette.colors.push_back( DrColor((0xFFB3E5FC)) );    //
    palette.colors.push_back( DrColor((0xFF81D4FA)) );    //
    palette.colors.push_back( DrColor((0xFF4FC3F7)) );    //
    palette.colors.push_back( DrColor((0xFF29B6F6)) );    //
    palette.colors.push_back( DrColor((0xFF03A9F4)) );    //
    palette.colors.push_back( DrColor((0xFF039BE5)) );    //
    palette.colors.push_back( DrColor((0xFF0288D1)) );    //
    palette.colors.push_back( DrColor((0xFF0277BD)) );    //
    palette.colors.push_back( DrColor((0xFF01579B)) );    //
    palette.colors.push_back( DrColor((0xFF80D8FF)) );    // 126 Light Blue Accent
    palette.colors.push_back( DrColor((0xFF40C4FF)) );    //
    palette.colors.push_back( DrColor((0xFF00B0FF)) );    //
    palette.colors.push_back( DrColor((0xFF0091EA)) );    // 129

    palette.colors.push_back( DrColor((0xFFE0F7FA)) );    // 130 Cyan
    palette.colors.push_back( DrColor((0xFFB2EBF2)) );    //
    palette.colors.push_back( DrColor((0xFF80DEEA)) );    //
    palette.colors.push_back( DrColor((0xFF4DD0E1)) );    //
    palette.colors.push_back( DrColor((0xFF26C6DA)) );    //
    palette.colors.push_back( DrColor((0xFF00BCD4)) );    //
    palette.colors.push_back( DrColor((0xFF00ACC1)) );    //
    palette.colors.push_back( DrColor((0xFF0097A7)) );    //
    palette.colors.push_back( DrColor((0xFF00838F)) );    //
    palette.colors.push_back( DrColor((0xFF006064)) );    //
    palette.colors.push_back( DrColor((0xFF84FFFF)) );    // 140 Cyan Accent
    palette.colors.push_back( DrColor((0xFF18FFFF)) );    //
    palette.colors.push_back( DrColor((0xFF00E5FF)) );    //
    palette.colors.push_back( DrColor((0xFF00B8D4)) );    // 143

    palette.colors.push_back( DrColor((0xFFE0F2F1)) );    // 144 Teal
    palette.colors.push_back( DrColor((0xFFB2DFDB)) );    //
    palette.colors.push_back( DrColor((0xFF80CBC4)) );    //
    palette.colors.push_back( DrColor((0xFF4DB6AC)) );    //
    palette.colors.push_back( DrColor((0xFF26A69A)) );    //
    palette.colors.push_back( DrColor((0xFF009688)) );    //
    palette.colors.push_back( DrColor((0xFF00897B)) );    //
    palette.colors.push_back( DrColor((0xFF00796B)) );    //
    palette.colors.push_back( DrColor((0xFF00695C)) );    //
    palette.colors.push_back( DrColor((0xFF004D40)) );    //
    palette.colors.push_back( DrColor((0xFFA7FFEB)) );    // 154 Teal Accent
    palette.colors.push_back( DrColor((0xFF64FFDA)) );    //
    palette.colors.push_back( DrColor((0xFF1DE9B6)) );    //
    palette.colors.push_back( DrColor((0xFF00BFA5)) );    // 157

    palette.colors.push_back( DrColor((0xFFE8F5E9)) );    // 158 Green
    palette.colors.push_back( DrColor((0xFFC8E6C9)) );    //
    palette.colors.push_back( DrColor((0xFFA5D6A7)) );    //
    palette.colors.push_back( DrColor((0xFF81C784)) );    //
    palette.colors.push_back( DrColor((0xFF66BB6A)) );    //
    palette.colors.push_back( DrColor((0xFF4CAF50)) );    //
    palette.colors.push_back( DrColor((0xFF43A047)) );    //
    palette.colors.push_back( DrColor((0xFF388E3C)) );    //
    palette.colors.push_back( DrColor((0xFF2E7D32)) );    //
    palette.colors.push_back( DrColor((0xFF1B5E20)) );    //
    palette.colors.push_back( DrColor((0xFFB9F6CA)) );    // 168 Green Accent
    palette.colors.push_back( DrColor((0xFF69F0AE)) );    //
    palette.colors.push_back( DrColor((0xFF00E676)) );    //
    palette.colors.push_back( DrColor((0xFF00C853)) );    // 171

    palette.colors.push_back( DrColor((0xFFF1F8E9)) );    // 172 Light Green
    palette.colors.push_back( DrColor((0xFFDCEDC8)) );    //
    palette.colors.push_back( DrColor((0xFFC5E1A5)) );    //
    palette.colors.push_back( DrColor((0xFFAED581)) );    //
    palette.colors.push_back( DrColor((0xFF9CCC65)) );    //
    palette.colors.push_back( DrColor((0xFF8BC34A)) );    //
    palette.colors.push_back( DrColor((0xFF7CB342)) );    //
    palette.colors.push_back( DrColor((0xFF689F38)) );    //
    palette.colors.push_back( DrColor((0xFF558B2F)) );    //
    palette.colors.push_back( DrColor((0xFF33691E)) );    //
    palette.colors.push_back( DrColor((0xFFCCFF90)) );    // 182 Light Green Accent
    palette.colors.push_back( DrColor((0xFFB2FF59)) );    //
    palette.colors.push_back( DrColor((0xFF76FF03)) );    //
    palette.colors.push_back( DrColor((0xFF64DD17)) );    // 185

    palette.colors.push_back( DrColor((0xFFF9FBE7)) );    // 186 Lime
    palette.colors.push_back( DrColor((0xFFF0F4C3)) );    //
    palette.colors.push_back( DrColor((0xFFE6EE9C)) );    //
    palette.colors.push_back( DrColor((0xFFDCE775)) );    //
    palette.colors.push_back( DrColor((0xFFD4E157)) );    //
    palette.colors.push_back( DrColor((0xFFCDDC39)) );    //
    palette.colors.push_back( DrColor((0xFFC0CA33)) );    //
    palette.colors.push_back( DrColor((0xFFAFB42B)) );    //
    palette.colors.push_back( DrColor((0xFF9E9D24)) );    //
    palette.colors.push_back( DrColor((0xFF827717)) );    //
    palette.colors.push_back( DrColor((0xFFF4FF81)) );    // 196 Lime Accent
    palette.colors.push_back( DrColor((0xFFEEFF41)) );    //
    palette.colors.push_back( DrColor((0xFFC6FF00)) );    //
    palette.colors.push_back( DrColor((0xFFAEEA00)) );    // 199

    palette.colors.push_back( DrColor((0xFFFFFDE7)) );    // 200 Yellow
    palette.colors.push_back( DrColor((0xFFFFF9C4)) );    //
    palette.colors.push_back( DrColor((0xFFFFF59D)) );    //
    palette.colors.push_back( DrColor((0xFFFFF176)) );    //
    palette.colors.push_back( DrColor((0xFFFFEE58)) );    //
    palette.colors.push_back( DrColor((0xFFFFEB3B)) );    //
    palette.colors.push_back( DrColor((0xFFFDD835)) );    //
    palette.colors.push_back( DrColor((0xFFFBC02D)) );    //
    palette.colors.push_back( DrColor((0xFFF9A825)) );    //
    palette.colors.push_back( DrColor((0xFFF57F17)) );    //
    palette.colors.push_back( DrColor((0xFFFFFF8D)) );    // 210 Yellow Accent
    palette.colors.push_back( DrColor((0xFFFFFF00)) );    //
    palette.colors.push_back( DrColor((0xFFFFEA00)) );    //
    palette.colors.push_back( DrColor((0xFFFFD600)) );    // 213

    palette.colors.push_back( DrColor((0xFFFFF8E1)) );    // 214 Amber
    palette.colors.push_back( DrColor((0xFFFFECB3)) );    //
    palette.colors.push_back( DrColor((0xFFFFE082)) );    //
    palette.colors.push_back( DrColor((0xFFFFD54F)) );    //
    palette.colors.push_back( DrColor((0xFFFFCA28)) );    //
    palette.colors.push_back( DrColor((0xFFFFC107)) );    //
    palette.colors.push_back( DrColor((0xFFFFB300)) );    //
    palette.colors.push_back( DrColor((0xFFFFA000)) );    //
    palette.colors.push_back( DrColor((0xFFFF8F00)) );    //
    palette.colors.push_back( DrColor((0xFFFF6F00)) );    //
    palette.colors.push_back( DrColor((0xFFFFE57F)) );    // 224 Amber Accent
    palette.colors.push_back( DrColor((0xFFFFD740)) );    //
    palette.colors.push_back( DrColor((0xFFFFC400)) );    //
    palette.colors.push_back( DrColor((0xFFFFAB00)) );    // 227

    palette.colors.push_back( DrColor((0xFFFFF3E0)) );    // 228 Orange
    palette.colors.push_back( DrColor((0xFFFFE0B2)) );    //
    palette.colors.push_back( DrColor((0xFFFFCC80)) );    //
    palette.colors.push_back( DrColor((0xFFFFB74D)) );    //
    palette.colors.push_back( DrColor((0xFFFFA726)) );    //
    palette.colors.push_back( DrColor((0xFFFF9800)) );    //
    palette.colors.push_back( DrColor((0xFFFB8C00)) );    //
    palette.colors.push_back( DrColor((0xFFF57C00)) );    //
    palette.colors.push_back( DrColor((0xFFEF6C00)) );    //
    palette.colors.push_back( DrColor((0xFFE65100)) );     //
    palette.colors.push_back( DrColor((0xFFFFD180)) );    // 238 Orange Accent
    palette.colors.push_back( DrColor((0xFFFFAB40)) );    //
    palette.colors.push_back( DrColor((0xFFFF9100)) );    //
    palette.colors.push_back( DrColor((0xFFFF6D00)) );    // 241

    palette.colors.push_back( DrColor((0xFFFBE9E7)) );    // 242 Deep Orange
    palette.colors.push_back( DrColor((0xFFFFCCBC)) );    //
    palette.colors.push_back( DrColor((0xFFFFAB91)) );    //
    palette.colors.push_back( DrColor((0xFFFF8A65)) );    //
    palette.colors.push_back( DrColor((0xFFFF7043)) );    //
    palette.colors.push_back( DrColor((0xFFFF5722)) );    //
    palette.colors.push_back( DrColor((0xFFF4511E)) );    //
    palette.colors.push_back( DrColor((0xFFE64A19)) );    //
    palette.colors.push_back( DrColor((0xFFD84315)) );    //
    palette.colors.push_back( DrColor((0xFFBF360C)) );    //
    palette.colors.push_back( DrColor((0xFFFF9E80)) );    // 252 Deep Orange Accent
    palette.colors.push_back( DrColor((0xFFFF6E40)) );    //
    palette.colors.push_back( DrColor((0xFFFF3D00)) );    //
    palette.colors.push_back( DrColor((0xFFDD2C00)) );    // 255

    palette.number_of_colors = static_cast<int>(palette.colors.size());
}


}   // namespace Dr




