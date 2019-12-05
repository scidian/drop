//
//      Created by Stephens Nunnally on 1/21/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Holds all of our custom colors
//
//
#ifndef COLORS_H
#define COLORS_H

#include <QColor>
#include <QVector>

//####################################################################################
//##    Palette options for windows
//############################
enum class Color_Scheme {
    Dark,
    Light,

    Navy,
    Grape,
    Rust,
    Coffee,
    Emerald,
};

enum class Window_Colors {
    Shadow,
    Background_Dark,    Background_Light,
    Button_Dark,        Button_Light,
    Midlight,           Highlight,
    Text_Dark,          Text_Light,             Text,
    Icon_Dark,          Icon_Light,
    Seperator,
    Header_Text,        Header_Fade,
};

enum class Color_Palettes {
    Basic,
    Blank,
    Material,
    Rocky_Rover,
    Window_Themes,
};

struct Palette_Info {
    QString         name =              "Name Me";          // Name of color palette
    bool            show_in_list =      false;              // Show to user?
    int             number_of_colors =  0;                  // Number of colors in palette
    QVector<QColor> colors;
};

namespace Dr {

    // Color Helper Functions
    bool            IsSameColor(const QColor &color1, const QColor &color2, double tolerance);
    QColor          RandomColor();

    // Style Functions
    QString         BorderWidth();                              // Project wide border width for Style Sheets, as QString
    int             BorderWidthAsInt();                         // Project wide border width for Style Sheets, as Int

    // Custom Palette Functions
    void            LoadPalettes();
    void            LoadPaletteBasic(Palette_Info &palette);
    void            LoadPaletteBlank(Palette_Info &palette);
    void            LoadPaletteMaterial(Palette_Info &palette);
    void            LoadPaletteRockyRover(Palette_Info &palette);
    void            LoadPaletteWindowThemes(Palette_Info &palette);

    Color_Scheme    GetColorScheme();
    QColor          GetColor(Window_Colors color_role);    
    QColor          GetColorFromPalette(Color_Palettes palette, int color_index);

    int             GetPaletteColorCount(Color_Palettes palette);
    QString         GetPaletteName(Color_Palettes palette);
    bool            GetPaletteShowInList(Color_Palettes palette);

    void            AddToColorHistory(QColor color);

    /**
     * Doxygen comments, type slash star star enter, brief shows a mouse mover popup for the described function in the editor
     *
     * @brief SetColorScheme is a setter to a global veriable that keeps track of which color scheme to use for the open editor
     * @param new_scheme
     * @note
     */
    void            SetColorScheme(Color_Scheme new_scheme);

}   // namespace Dr




#endif // COLORS_H



