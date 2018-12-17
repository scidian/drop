//
//
//      Function declarations for Form_Styling.cpp
//
//

#ifndef FORM_COLORING_H
#define FORM_COLORING_H


#include "mainwindow.h"
#include "ui_mainwindow.h"

static std::map<Color_Scheme, std::map<QPalette::ColorRole, QColor>> color_schemes;
static Color_Scheme current_color_scheme = Color_Scheme::Dark;

void adjustMainWindowUi(MainWindow *main_window);
void loadPalettes();
void applyMainWindowPalette(MainWindow *main_window, Color_Scheme new_color);


#endif // FORM_COLORING_H



















