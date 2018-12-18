//
//
//      Function declarations for Form_Styling.cpp
//
//

#ifndef FORM_COLORING_H
#define FORM_COLORING_H


#include "mainwindow.h"


static std::map<Color_Scheme, std::map<QPalette::ColorRole, QColor>> color_schemes;
static Color_Scheme current_color_scheme = Color_Scheme::Dark;


void loadPalettes();



#endif // FORM_COLORING_H



















