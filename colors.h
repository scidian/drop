//
//      Created by Stephens Nunnally on 1/21/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Holds all of our custom colors
//
//
#ifndef COLORS_H
#define COLORS_H

#include <map>
#include <QColor>


// ***** Palette options for windows
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
};


namespace Dr {

    void            LoadPalettes();

    QColor          GetColor(Window_Colors color_role);
    Color_Scheme    GetColorScheme();
    void            SetColorScheme(Color_Scheme new_scheme);

}




#endif // COLORS_H



