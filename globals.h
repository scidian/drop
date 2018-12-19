//
//
//      Globals - Holds some global variables for use across the project
//
//

#ifndef GLOBALS_H
#define GLOBALS_H

#include "enums.h"


class Globals
{
public:
    Globals();

    std::map<Color_Scheme, std::map<QPalette::ColorRole, QColor>> color_schemes;
    Color_Scheme current_color_scheme = Color_Scheme::Dark;

    void    loadPalettes();

    void    showMessageBox(QString new_message);

};

#endif // GLOBALS_H
