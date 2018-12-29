//
//
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
    Shadow_Dark,        Shadow_Light,
    Background_Dark,    Background_Light,
    Button_Dark,        Button_Light,
    Midlight,           Highlight,
    Text_Dark,          Text_Light,             Text,
    Icon_Dark,          Icon_Light,
};


class Globals
{
public:
    // Constructor
    Globals();

    // Variables
    std::map<Color_Scheme, std::map<Window_Colors, QColor>> color_schemes;
    Color_Scheme current_color_scheme;

    // Variable Functions
    void    loadPalettes();
    QColor  getColor(Window_Colors color_role) { return color_schemes[current_color_scheme][color_role]; }


    // Helper functions
    void    showMessageBox(QString new_message);

};

#endif // GLOBALS_H










