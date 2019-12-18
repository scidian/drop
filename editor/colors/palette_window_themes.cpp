//
//      Created by Stephens Nunnally on 4/4/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "editor/colors/colors.h"


namespace Dr {

void LoadPaletteWindowThemes(Palette_Info &palette) {
    palette.name = "Window Themes";
    palette.show_in_list = true;

    palette.colors.push_back( DrColor( 0xFFFFFFFF ) );    //   0 Whites   (255, 255, 255)
    palette.colors.push_back( DrColor( 0x00000000 ) );    //   1 Blank
    palette.colors.push_back( DrColor( 0xFFF2F2F2 ) );    //   2          (242, 242, 242)
    palette.colors.push_back( DrColor( 0xFFD9D9D9 ) );    //   3          (217, 217, 217)
    palette.colors.push_back( DrColor( 0xFFBFBFBF ) );    //   4          (191, 191, 191)
    palette.colors.push_back( DrColor( 0xFFA6A6A6 ) );    //   5          (166, 166, 166)
    palette.colors.push_back( DrColor( 0xFF7F7F7F ) );    //   6          (127, 127, 127)
    palette.colors.push_back( DrColor( 0x00000000 ) );    //   7
    palette.colors.push_back( DrColor( 0x00000000 ) );    //   8
    palette.colors.push_back( DrColor( 0x00000000 ) );    //   9
    palette.colors.push_back( DrColor( 0x00000000 ) );    //  10
    palette.colors.push_back( DrColor( 0x00000000 ) );    //  11
    palette.colors.push_back( DrColor( 0x00000000 ) );    //  12
    palette.colors.push_back( DrColor( 0x00000000 ) );    //  13
    palette.colors.push_back( DrColor( 0x00000000 ) );    //  14
    palette.colors.push_back( DrColor( 0x00000000 ) );    //  15

    palette.colors.push_back( DrColor( 0xFF000000 ) );    //  16 Blacks
    palette.colors.push_back( DrColor( 0x00000000 ) );    //  17
    palette.colors.push_back( DrColor( 0xFF7F7F7F ) );    //  18          (127, 127, 127)
    palette.colors.push_back( DrColor( 0xFF595959 ) );    //  19          ( 89,  89,  89)
    palette.colors.push_back( DrColor( 0xFF404040 ) );    //  20          ( 64,  64,  64)
    palette.colors.push_back( DrColor( 0xFF262626 ) );    //  21          ( 38,  38,  38)
    palette.colors.push_back( DrColor( 0xFF0D0D0D ) );    //  22          ( 13,  13,  13)
    palette.colors.push_back( DrColor( 0x00000000 ) );    //  23
    palette.colors.push_back( DrColor( 0x00000000 ) );    //  24
    palette.colors.push_back( DrColor( 0x00000000 ) );    //  25
    palette.colors.push_back( DrColor( 0x00000000 ) );    //  26
    palette.colors.push_back( DrColor( 0x00000000 ) );    //  27
    palette.colors.push_back( DrColor( 0x00000000 ) );    //  29
    palette.colors.push_back( DrColor( 0x00000000 ) );    //  29
    palette.colors.push_back( DrColor( 0x00000000 ) );    //  30
    palette.colors.push_back( DrColor( 0x00000000 ) );    //  31

    palette.colors.push_back( DrColor( 0xFFEEECE1 ) );    //  32 Browns   (238, 236, 225)
    palette.colors.push_back( DrColor( 0x00000000 ) );    //  33
    palette.colors.push_back( DrColor( 0xFFDDD9C3 ) );    //  34          (221, 217, 195)
    palette.colors.push_back( DrColor( 0xFFC4BD97 ) );    //  35          (196, 189, 151)
    palette.colors.push_back( DrColor( 0xFF948A54 ) );    //  36          (148, 138,  84)
    palette.colors.push_back( DrColor( 0xFF4A452A ) );    //  37          ( 74,  69,  42)
    palette.colors.push_back( DrColor( 0xFF1E1C11 ) );    //  38          ( 30,  28,  17)
    palette.colors.push_back( DrColor( 0x00000000 ) );    //  39
    palette.colors.push_back( DrColor( 0x00000000 ) );    //  40
    palette.colors.push_back( DrColor( 0x00000000 ) );    //  41
    palette.colors.push_back( DrColor( 0x00000000 ) );    //  42
    palette.colors.push_back( DrColor( 0x00000000 ) );    //  43
    palette.colors.push_back( DrColor( 0x00000000 ) );    //  44
    palette.colors.push_back( DrColor( 0x00000000 ) );    //  45
    palette.colors.push_back( DrColor( 0x00000000 ) );    //  46
    palette.colors.push_back( DrColor( 0x00000000 ) );    //  47

    palette.colors.push_back( DrColor( 0xFF1F497D ) );    //  48 Blues    ( 31,  73, 125)
    palette.colors.push_back( DrColor( 0x00000000 ) );    //  49
    palette.colors.push_back( DrColor( 0xFFC6D9F1 ) );    //  50          (198, 217, 241)
    palette.colors.push_back( DrColor( 0xFF8EB4E3 ) );    //  51          (142, 180, 227)
    palette.colors.push_back( DrColor( 0xFF558ED5 ) );    //  52          ( 85, 142, 213)
    palette.colors.push_back( DrColor( 0xFF17375D ) );    //  53          ( 23,  55,  93)
    palette.colors.push_back( DrColor( 0xFF0F243F ) );    //  54          ( 15,  36,  63)
    palette.colors.push_back( DrColor( 0x00000000 ) );    //  55
    palette.colors.push_back( DrColor( 0x00000000 ) );    //  56
    palette.colors.push_back( DrColor( 0x00000000 ) );    //  57
    palette.colors.push_back( DrColor( 0x00000000 ) );    //  58
    palette.colors.push_back( DrColor( 0x00000000 ) );    //  59
    palette.colors.push_back( DrColor( 0x00000000 ) );    //  60
    palette.colors.push_back( DrColor( 0x00000000 ) );    //  61
    palette.colors.push_back( DrColor( 0x00000000 ) );    //  62
    palette.colors.push_back( DrColor( 0x00000000 ) );    //  63

    palette.colors.push_back( DrColor( 0xFF4F81BD ) );    //  64 Gray Blues ( 79, 129, 189)
    palette.colors.push_back( DrColor( 0x00000000 ) );    //  65
    palette.colors.push_back( DrColor( 0xFFDCE6F2 ) );    //  66          (220, 230, 242)
    palette.colors.push_back( DrColor( 0xFFB9CDE5 ) );    //  67          (185, 205, 229)
    palette.colors.push_back( DrColor( 0xFF95B3D7 ) );    //  68          (149, 179, 215)
    palette.colors.push_back( DrColor( 0xFF376192 ) );    //  69          ( 55,  97, 146)
    palette.colors.push_back( DrColor( 0xFF244061 ) );    //  70          ( 36,  64,  97)
    palette.colors.push_back( DrColor( 0x00000000 ) );    //  71
    palette.colors.push_back( DrColor( 0x00000000 ) );    //  72
    palette.colors.push_back( DrColor( 0x00000000 ) );    //  73
    palette.colors.push_back( DrColor( 0x00000000 ) );    //  74
    palette.colors.push_back( DrColor( 0x00000000 ) );    //  75
    palette.colors.push_back( DrColor( 0x00000000 ) );    //  76
    palette.colors.push_back( DrColor( 0x00000000 ) );    //  77
    palette.colors.push_back( DrColor( 0x00000000 ) );    //  78
    palette.colors.push_back( DrColor( 0x00000000 ) );    //  79

    palette.colors.push_back( DrColor( 0xFFC0504D ) );    //  80 Burnt Oranges (192,  80,  77)
    palette.colors.push_back( DrColor( 0x00000000 ) );    //  81
    palette.colors.push_back( DrColor( 0xFFF2DCDB ) );    //  82          (242, 220, 219)
    palette.colors.push_back( DrColor( 0xFFE6B9B8 ) );    //  83          (230, 185, 184)
    palette.colors.push_back( DrColor( 0xFFD99694 ) );    //  84          (217, 150, 148)
    palette.colors.push_back( DrColor( 0xFF953736 ) );    //  85          (149,  55,  54)
    palette.colors.push_back( DrColor( 0xFF632523 ) );    //  86          ( 99,  37,  35)
    palette.colors.push_back( DrColor( 0x00000000 ) );    //  87
    palette.colors.push_back( DrColor( 0x00000000 ) );    //  88
    palette.colors.push_back( DrColor( 0x00000000 ) );    //  89
    palette.colors.push_back( DrColor( 0x00000000 ) );    //  90
    palette.colors.push_back( DrColor( 0x00000000 ) );    //  91
    palette.colors.push_back( DrColor( 0x00000000 ) );    //  92
    palette.colors.push_back( DrColor( 0x00000000 ) );    //  93
    palette.colors.push_back( DrColor( 0x00000000 ) );    //  94
    palette.colors.push_back( DrColor( 0x00000000 ) );    //  95

    palette.colors.push_back( DrColor( 0xFF9BBB59 ) );    //  96 Olive Drabs (155, 187,  89)
    palette.colors.push_back( DrColor( 0x00000000 ) );    //  97
    palette.colors.push_back( DrColor( 0xFFEBF1DE ) );    //  98          (235, 241, 222)
    palette.colors.push_back( DrColor( 0xFFD7E4BD ) );    //  99          (215, 228, 189)
    palette.colors.push_back( DrColor( 0xFFC3D69B ) );    // 100          (195, 214, 155)
    palette.colors.push_back( DrColor( 0xFF77933C ) );    // 101          (119, 147,  60)
    palette.colors.push_back( DrColor( 0xFF4F6228 ) );    // 102          ( 79,  98,  40)
    palette.colors.push_back( DrColor( 0x00000000 ) );    // 103
    palette.colors.push_back( DrColor( 0x00000000 ) );    // 104
    palette.colors.push_back( DrColor( 0x00000000 ) );    // 105
    palette.colors.push_back( DrColor( 0x00000000 ) );    // 106
    palette.colors.push_back( DrColor( 0x00000000 ) );    // 107
    palette.colors.push_back( DrColor( 0x00000000 ) );    // 108
    palette.colors.push_back( DrColor( 0x00000000 ) );    // 109
    palette.colors.push_back( DrColor( 0x00000000 ) );    // 110
    palette.colors.push_back( DrColor( 0x00000000 ) );    // 111

    palette.colors.push_back( DrColor( 0xFF4F6228 ) );    // 112 Grayish Purples ( 79,  98,  40)
    palette.colors.push_back( DrColor( 0x00000000 ) );    // 113
    palette.colors.push_back( DrColor( 0xFFE6E0EC ) );    // 114          (230, 224, 236)
    palette.colors.push_back( DrColor( 0xFFCCC1DA ) );    // 115          (204, 193, 218)
    palette.colors.push_back( DrColor( 0xFFB3A2C7 ) );    // 116          (179, 162, 199)
    palette.colors.push_back( DrColor( 0xFF604A7B ) );    // 117          ( 96,  74, 123)
    palette.colors.push_back( DrColor( 0xFF403152 ) );    // 118          ( 64,  49,  82)
    palette.colors.push_back( DrColor( 0x00000000 ) );    // 119
    palette.colors.push_back( DrColor( 0x00000000 ) );    // 120
    palette.colors.push_back( DrColor( 0x00000000 ) );    // 121
    palette.colors.push_back( DrColor( 0x00000000 ) );    // 122
    palette.colors.push_back( DrColor( 0x00000000 ) );    // 123
    palette.colors.push_back( DrColor( 0x00000000 ) );    // 124
    palette.colors.push_back( DrColor( 0x00000000 ) );    // 125
    palette.colors.push_back( DrColor( 0x00000000 ) );    // 126
    palette.colors.push_back( DrColor( 0x00000000 ) );    // 127

    palette.colors.push_back( DrColor( 0xFF4BACC6 ) );    // 128 Light Blues ( 75, 172, 198)
    palette.colors.push_back( DrColor( 0x00000000 ) );    // 129
    palette.colors.push_back( DrColor( 0xFFDBEEF4 ) );    // 130          (219, 238, 244)
    palette.colors.push_back( DrColor( 0xFFB7DEE8 ) );    // 131          (183, 222, 232)
    palette.colors.push_back( DrColor( 0xFF93CDDD ) );    // 132          (147, 205, 221)
    palette.colors.push_back( DrColor( 0xFF31859C ) );    // 133          ( 49, 133, 156)
    palette.colors.push_back( DrColor( 0xFF215968 ) );    // 134          ( 33,  89, 104)
    palette.colors.push_back( DrColor( 0x00000000 ) );    // 135
    palette.colors.push_back( DrColor( 0x00000000 ) );    // 136
    palette.colors.push_back( DrColor( 0x00000000 ) );    // 137
    palette.colors.push_back( DrColor( 0x00000000 ) );    // 138
    palette.colors.push_back( DrColor( 0x00000000 ) );    // 139
    palette.colors.push_back( DrColor( 0x00000000 ) );    // 140
    palette.colors.push_back( DrColor( 0x00000000 ) );    // 141
    palette.colors.push_back( DrColor( 0x00000000 ) );    // 142
    palette.colors.push_back( DrColor( 0x00000000 ) );    // 143

    palette.colors.push_back( DrColor( 0xFFF79646 ) );    // 144 Oranges  (247, 150,  70)
    palette.colors.push_back( DrColor( 0x00000000 ) );    // 145
    palette.colors.push_back( DrColor( 0xFFFDEADA ) );    // 146          (253, 234, 218)
    palette.colors.push_back( DrColor( 0xFFFCD5B5 ) );    // 147          (252, 213, 181)
    palette.colors.push_back( DrColor( 0xFFFAC090 ) );    // 148          (250, 192, 144)
    palette.colors.push_back( DrColor( 0xFFE46B0A ) );    // 149          (228, 107,  10)
    palette.colors.push_back( DrColor( 0xFF984807 ) );    // 150          (152,  72,   7)
    palette.colors.push_back( DrColor( 0x00000000 ) );    // 151
    palette.colors.push_back( DrColor( 0x00000000 ) );    // 152
    palette.colors.push_back( DrColor( 0x00000000 ) );    // 153
    palette.colors.push_back( DrColor( 0x00000000 ) );    // 154
    palette.colors.push_back( DrColor( 0x00000000 ) );    // 155
    palette.colors.push_back( DrColor( 0x00000000 ) );    // 156
    palette.colors.push_back( DrColor( 0x00000000 ) );    // 157
    palette.colors.push_back( DrColor( 0x00000000 ) );    // 158
    palette.colors.push_back( DrColor( 0x00000000 ) );    // 159

    palette.number_of_colors = static_cast<int>(palette.colors.size());
}


}   // namespace Dr




