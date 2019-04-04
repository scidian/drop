//
//      Created by Stephens Nunnally on 4/4/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QColor>

#include "colors.h"
#include "library.h"

namespace Dr {

void LoadPaletteWindowThemes(Palette_Info &palette)
{
    palette.name = "Window Themes";
    palette.show_in_list = true;

    palette.colors.append( QColor::fromRgba( 0xFFFFFF   ) );    //   0 Whites   (255, 255, 255)
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    //   1 Blank
    palette.colors.append( QColor::fromRgba( 0xF2F2F2   ) );    //   2          (242, 242, 242)
    palette.colors.append( QColor::fromRgba( 0xD9D9D9   ) );    //   3          (217, 217, 217)
    palette.colors.append( QColor::fromRgba( 0xBFBFBF   ) );    //   4          (191, 191, 191)
    palette.colors.append( QColor::fromRgba( 0xA6A6A6   ) );    //   5          (166, 166, 166)
    palette.colors.append( QColor::fromRgba( 0x7F7F7F   ) );    //   6          (127, 127, 127)
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    //   7
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    //   8
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    //   9
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    //  10
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    //  11
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    //  12
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    //  13
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    //  14
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    //  15

    palette.colors.append( QColor::fromRgba( 0xFF000000 ) );    //  16 Blacks
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    //  17
    palette.colors.append( QColor::fromRgba( 0x7F7F7F   ) );    //  18          (127, 127, 127)
    palette.colors.append( QColor::fromRgba( 0x595959   ) );    //  19          ( 89,  89,  89)
    palette.colors.append( QColor::fromRgba( 0x404040   ) );    //  20          ( 64,  64,  64)
    palette.colors.append( QColor::fromRgba( 0x262626   ) );    //  21          ( 38,  38,  38)
    palette.colors.append( QColor::fromRgba( 0x0D0D0D   ) );    //  22          ( 13,  13,  13)
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    //  23
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    //  24
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    //  25
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    //  26
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    //  27
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    //  29
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    //  29
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    //  30
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    //  31

    palette.colors.append( QColor::fromRgba( 0xEEECE1   ) );    //  32 Browns   (238, 236, 225)
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    //  33
    palette.colors.append( QColor::fromRgba( 0xDDD9C3   ) );    //  34          (221, 217, 195)
    palette.colors.append( QColor::fromRgba( 0xC4BD97   ) );    //  35          (196, 189, 151)
    palette.colors.append( QColor::fromRgba( 0x948A54   ) );    //  36          (148, 138,  84)
    palette.colors.append( QColor::fromRgba( 0x4A452A   ) );    //  37          ( 74,  69,  42)
    palette.colors.append( QColor::fromRgba( 0x1E1C11   ) );    //  38          ( 30,  28,  17)
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    //  39
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    //  40
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    //  41
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    //  42
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    //  43
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    //  44
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    //  45
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    //  46
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    //  47

    palette.colors.append( QColor::fromRgba( 0x1F497D   ) );    //  48 Blues    ( 31,  73, 125)
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    //  49
    palette.colors.append( QColor::fromRgba( 0xC6D9F1   ) );    //  50          (198, 217, 241)
    palette.colors.append( QColor::fromRgba( 0x8EB4E3   ) );    //  51          (142, 180, 227)
    palette.colors.append( QColor::fromRgba( 0x558ED5   ) );    //  52          ( 85, 142, 213)
    palette.colors.append( QColor::fromRgba( 0x17375D   ) );    //  53          ( 23,  55,  93)
    palette.colors.append( QColor::fromRgba( 0x0F243F   ) );    //  54          ( 15,  36,  63)
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    //  55
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    //  56
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    //  57
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    //  58
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    //  59
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    //  60
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    //  61
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    //  62
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    //  63

    palette.colors.append( QColor::fromRgba( 0x4F81BD   ) );    //  64 Gray Blues ( 79, 129, 189)
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    //  65
    palette.colors.append( QColor::fromRgba( 0xDCE6F2   ) );    //  66          (220, 230, 242)
    palette.colors.append( QColor::fromRgba( 0xB9CDE5   ) );    //  67          (185, 205, 229)
    palette.colors.append( QColor::fromRgba( 0x95B3D7   ) );    //  68          (149, 179, 215)
    palette.colors.append( QColor::fromRgba( 0x376192   ) );    //  69          ( 55,  97, 146)
    palette.colors.append( QColor::fromRgba( 0x244061   ) );    //  70          ( 36,  64,  97)
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    //  71
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    //  72
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    //  73
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    //  74
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    //  75
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    //  76
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    //  77
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    //  78
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    //  79

    palette.colors.append( QColor::fromRgba( 0xC0504D   ) );    //  80 Burnt Oranges (192,  80,  77)
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    //  81
    palette.colors.append( QColor::fromRgba( 0xF2DCDB   ) );    //  82          (242, 220, 219)
    palette.colors.append( QColor::fromRgba( 0xE6B9B8   ) );    //  83          (230, 185, 184)
    palette.colors.append( QColor::fromRgba( 0xD99694   ) );    //  84          (217, 150, 148)
    palette.colors.append( QColor::fromRgba( 0x953736   ) );    //  85          (149,  55,  54)
    palette.colors.append( QColor::fromRgba( 0x632523   ) );    //  86          ( 99,  37,  35)
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    //  87
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    //  88
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    //  89
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    //  90
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    //  91
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    //  92
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    //  93
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    //  94
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    //  95

    palette.colors.append( QColor::fromRgba( 0x9BBB59   ) );    //  96 Olive Drabs (155, 187,  89)
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    //  97
    palette.colors.append( QColor::fromRgba( 0xEBF1DE   ) );    //  98          (235, 241, 222)
    palette.colors.append( QColor::fromRgba( 0xD7E4BD   ) );    //  99          (215, 228, 189)
    palette.colors.append( QColor::fromRgba( 0xC3D69B   ) );    // 100          (195, 214, 155)
    palette.colors.append( QColor::fromRgba( 0x77933C   ) );    // 101          (119, 147,  60)
    palette.colors.append( QColor::fromRgba( 0x4F6228   ) );    // 102          ( 79,  98,  40)
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 103
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 104
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 105
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 106
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 107
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 108
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 109
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 110
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 111

    palette.colors.append( QColor::fromRgba( 0x4F6228   ) );    // 112 Grayish Purples ( 79,  98,  40)
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 113
    palette.colors.append( QColor::fromRgba( 0xE6E0EC   ) );    // 114          (230, 224, 236)
    palette.colors.append( QColor::fromRgba( 0xCCC1DA   ) );    // 115          (204, 193, 218)
    palette.colors.append( QColor::fromRgba( 0xB3A2C7   ) );    // 116          (179, 162, 199)
    palette.colors.append( QColor::fromRgba( 0x604A7B   ) );    // 117          ( 96,  74, 123)
    palette.colors.append( QColor::fromRgba( 0x403152   ) );    // 118          ( 64,  49,  82)
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 119
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 120
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 121
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 122
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 123
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 124
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 125
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 126
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 127

    palette.colors.append( QColor::fromRgba( 0x4BACC6   ) );    // 128 Light Blues ( 75, 172, 198)
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 129
    palette.colors.append( QColor::fromRgba( 0xDBEEF4   ) );    // 130          (219, 238, 244)
    palette.colors.append( QColor::fromRgba( 0xB7DEE8   ) );    // 131          (183, 222, 232)
    palette.colors.append( QColor::fromRgba( 0x93CDDD   ) );    // 132          (147, 205, 221)
    palette.colors.append( QColor::fromRgba( 0x31859C   ) );    // 133          ( 49, 133, 156)
    palette.colors.append( QColor::fromRgba( 0x215968   ) );    // 134          ( 33,  89, 104)
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 135
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 136
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 137
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 138
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 139
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 140
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 141
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 142
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 143

    palette.colors.append( QColor::fromRgba( 0xF79646   ) );    // 144 Oranges  (247, 150,  70)
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 145
    palette.colors.append( QColor::fromRgba( 0xFDEADA   ) );    // 146          (253, 234, 218)
    palette.colors.append( QColor::fromRgba( 0xFCD5B5   ) );    // 147          (252, 213, 181)
    palette.colors.append( QColor::fromRgba( 0xFAC090   ) );    // 148          (250, 192, 144)
    palette.colors.append( QColor::fromRgba( 0xE46B0A   ) );    // 149          (228, 107,  10)
    palette.colors.append( QColor::fromRgba( 0x984807   ) );    // 150          (152,  72,   7)
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 151
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 152
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 153
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 154
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 155
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 156
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 157
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 158
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 159

    palette.colors.append( QColor::fromRgba( 0x000000   ) );    // 160
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 161
    palette.colors.append( QColor::fromRgba( 0x000000   ) );    // 162
    palette.colors.append( QColor::fromRgba( 0x000000   ) );    // 163
    palette.colors.append( QColor::fromRgba( 0x000000   ) );    // 164
    palette.colors.append( QColor::fromRgba( 0x000000   ) );    // 165
    palette.colors.append( QColor::fromRgba( 0x000000   ) );    // 166
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 167
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 168
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 169
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 170
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 171
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 172
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 173
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 174
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 175

    palette.colors.append( QColor::fromRgba( 0x000000   ) );    // 176
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 177
    palette.colors.append( QColor::fromRgba( 0x000000   ) );    // 178
    palette.colors.append( QColor::fromRgba( 0x000000   ) );    // 179
    palette.colors.append( QColor::fromRgba( 0x000000   ) );    // 180
    palette.colors.append( QColor::fromRgba( 0x000000   ) );    // 181
    palette.colors.append( QColor::fromRgba( 0x000000   ) );    // 182
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 183
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 184
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 185
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 186
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 187
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 188
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 189
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 190
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 191

    palette.colors.append( QColor::fromRgba( 0x000000   ) );    // 192
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 193
    palette.colors.append( QColor::fromRgba( 0x000000   ) );    // 194
    palette.colors.append( QColor::fromRgba( 0x000000   ) );    // 195
    palette.colors.append( QColor::fromRgba( 0x000000   ) );    // 196
    palette.colors.append( QColor::fromRgba( 0x000000   ) );    // 197
    palette.colors.append( QColor::fromRgba( 0x000000   ) );    // 198
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 199
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 200
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 201
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 202
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 203
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 204
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 205
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 206
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 207

    palette.colors.append( QColor::fromRgba( 0x000000   ) );    // 208
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 209
    palette.colors.append( QColor::fromRgba( 0x000000   ) );    // 210
    palette.colors.append( QColor::fromRgba( 0x000000   ) );    // 211
    palette.colors.append( QColor::fromRgba( 0x000000   ) );    // 212
    palette.colors.append( QColor::fromRgba( 0x000000   ) );    // 213
    palette.colors.append( QColor::fromRgba( 0x000000   ) );    // 214
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 215
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 216
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 217
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 218
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 219
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 220
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 221
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 222
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 223

    palette.colors.append( QColor::fromRgba( 0x000000   ) );    // 224
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 225
    palette.colors.append( QColor::fromRgba( 0x000000   ) );    // 226
    palette.colors.append( QColor::fromRgba( 0x000000   ) );    // 227
    palette.colors.append( QColor::fromRgba( 0x000000   ) );    // 228
    palette.colors.append( QColor::fromRgba( 0x000000   ) );    // 229
    palette.colors.append( QColor::fromRgba( 0x000000   ) );    // 230
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 231
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 232
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 233
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 234
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 235
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 236
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 237
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 238
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 239

    palette.colors.append( QColor::fromRgba( 0x000000   ) );    // 240
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 241
    palette.colors.append( QColor::fromRgba( 0x000000   ) );    // 242
    palette.colors.append( QColor::fromRgba( 0x000000   ) );    // 243
    palette.colors.append( QColor::fromRgba( 0x000000   ) );    // 244
    palette.colors.append( QColor::fromRgba( 0x000000   ) );    // 245
    palette.colors.append( QColor::fromRgba( 0x000000   ) );    // 246
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 247
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 248
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 249
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 250
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 251
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 252
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 253
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 254
    palette.colors.append( QColor::fromRgba( 0x00000000 ) );    // 255

    palette.number_of_colors = palette.colors.count();
}


}   // namespace Dr




