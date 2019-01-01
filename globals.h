//
//      Created by Stephens Nunnally on 12/7/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Globals - Holds some global variables for use across the project
//
//

#ifndef GLOBALS_H
#define GLOBALS_H

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

enum class Shadow_Types
{
    Button_Shadow,
};

class Globals
{
private:
    void    loadPalettes();                         // Called during Constructor to load data into color_schemes

public:
    // Constructor
    Globals();

    // Variables
    bool    show_debug;

    std::map<Color_Scheme, std::map<Window_Colors, QColor>> color_schemes;
    Color_Scheme current_color_scheme;

    // Variable Functions
    QColor  getColor(Window_Colors color_role) { return color_schemes[current_color_scheme][color_role]; }

    // Helper functions
    void    showMessageBox(QString new_message);

};

#endif // GLOBALS_H










