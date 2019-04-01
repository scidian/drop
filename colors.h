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


//####################################################################################
//##    Palette options for windows
//############################
enum class Color_Scheme
{
    Dark,       Light,      Blue,       Autumn
};

enum class Window_Colors
{
    Shadow,
    Background_Dark,    Background_Light,
    Button_Dark,        Button_Light,
    Midlight,           Highlight,
    Text_Dark,          Text_Light,             Text,
    Icon_Dark,          Icon_Light,
    Seperator,
};


namespace Dr {

    void            LoadPalettes();
    void            LoadPaletteMaterial();

    QString         BorderWidth();                              // Project wide border width for Style Sheets, as QString
    int             BorderWidthAsInt();                         // Project wide border width for Style Sheets, as Int
    int             FontSize();                                 // Project wide font size

    QColor          GetColor(Window_Colors color_role);
    QColor          GetColorMaterial(int color);
    Color_Scheme    GetColorScheme();

    QColor          GenerateRandomColor();

    /**
     * Doxygen comments, type slash star star enter, brief shows a mouse mover popup for the described function in the editor
     *
     * @brief SetColorScheme is a setter to a global veriable that keeps track of which color scheme to use for the open editor
     * @param new_scheme
     */
    void            SetColorScheme(Color_Scheme new_scheme);

}   // namespace Dr




#endif // COLORS_H



