//
//
//      Functions for our Globals class
//
//

#include <QMessageBox>

#include "globals.h"

Globals::Globals()
{
    loadPalettes();
}


//####################################################################################
//##        Used to show a modal messagebox
//####################################################################################
void Globals::showMessageBox(QString new_message)
{
    QMessageBox *msgBox = new QMessageBox(nullptr);
    msgBox->setText(new_message);
    msgBox->exec();
}


//####################################################################################
//##        Store custom window palette colors
//####################################################################################
void Globals::loadPalettes()
{
    color_schemes[Color_Scheme::Dark].insert(std::make_pair(QPalette::ColorRole::Window, QColor(32, 32, 32)));
    color_schemes[Color_Scheme::Dark].insert(std::make_pair(QPalette::ColorRole::Background, QColor(32, 32, 32)));
    color_schemes[Color_Scheme::Dark].insert(std::make_pair(QPalette::ColorRole::WindowText, QColor(205, 205, 205)));
    color_schemes[Color_Scheme::Dark].insert(std::make_pair(QPalette::ColorRole::Foreground, QColor(205, 205, 205)));
    color_schemes[Color_Scheme::Dark].insert(std::make_pair(QPalette::ColorRole::Base, QColor(24, 24, 24)));
    color_schemes[Color_Scheme::Dark].insert(std::make_pair(QPalette::ColorRole::AlternateBase, QColor(32, 32, 32)));
    color_schemes[Color_Scheme::Dark].insert(std::make_pair(QPalette::ColorRole::Text, QColor(205, 205 ,205)));
    color_schemes[Color_Scheme::Dark].insert(std::make_pair(QPalette::ColorRole::Button, QColor(16, 16, 16)));
    color_schemes[Color_Scheme::Dark].insert(std::make_pair(QPalette::ColorRole::Dark, QColor(0, 180, 175)));
    color_schemes[Color_Scheme::Dark].insert(std::make_pair(QPalette::ColorRole::Highlight, QColor(64, 64, 64)));
    color_schemes[Color_Scheme::Dark].insert(std::make_pair(QPalette::ColorRole::HighlightedText, QColor(0, 150, 145)));
    color_schemes[Color_Scheme::Dark].insert(std::make_pair(QPalette::ColorRole::ButtonText, QColor(0, 0 ,0)));
    color_schemes[Color_Scheme::Dark].insert(std::make_pair(QPalette::ColorRole::ToolTipBase, QColor(98, 98, 98)));
    color_schemes[Color_Scheme::Dark].insert(std::make_pair(QPalette::ColorRole::ToolTipText, QColor(205, 205 , 205)));
    color_schemes[Color_Scheme::Dark].insert(std::make_pair(QPalette::ColorRole::BrightText, QColor(255, 255 , 255)));
    color_schemes[Color_Scheme::Dark].insert(std::make_pair(QPalette::ColorRole::Mid, QColor(215, 215 , 215)));
    color_schemes[Color_Scheme::Dark].insert(std::make_pair(QPalette::ColorRole::Midlight, QColor(48, 48, 48)));
    color_schemes[Color_Scheme::Dark].insert(std::make_pair(QPalette::ColorRole::Shadow, QColor(0, 0, 0)));

    color_schemes[Color_Scheme::Light].insert(std::make_pair(QPalette::ColorRole::Window, QColor(200, 200, 200)));
    color_schemes[Color_Scheme::Light].insert(std::make_pair(QPalette::ColorRole::Background, QColor(234, 234, 234)));
    color_schemes[Color_Scheme::Light].insert(std::make_pair(QPalette::ColorRole::WindowText, QColor(55, 55, 55)));
    color_schemes[Color_Scheme::Light].insert(std::make_pair(QPalette::ColorRole::Foreground, QColor(55, 55, 55)));
    color_schemes[Color_Scheme::Light].insert(std::make_pair(QPalette::ColorRole::Base, QColor(225, 225, 225)));
    color_schemes[Color_Scheme::Light].insert(std::make_pair(QPalette::ColorRole::AlternateBase, QColor(240, 240, 240)));
    color_schemes[Color_Scheme::Light].insert(std::make_pair(QPalette::ColorRole::Text, QColor(55, 55, 55)));
    color_schemes[Color_Scheme::Light].insert(std::make_pair(QPalette::ColorRole::Button, QColor(220, 220, 220)));
    color_schemes[Color_Scheme::Light].insert(std::make_pair(QPalette::ColorRole::Dark, QColor(190, 190, 190)));
    color_schemes[Color_Scheme::Light].insert(std::make_pair(QPalette::ColorRole::Highlight, QColor(64, 64, 64)));
    color_schemes[Color_Scheme::Light].insert(std::make_pair(QPalette::ColorRole::HighlightedText, QColor(240, 240, 240)));
    color_schemes[Color_Scheme::Light].insert(std::make_pair(QPalette::ColorRole::ButtonText, QColor(255, 255,255)));
    color_schemes[Color_Scheme::Light].insert(std::make_pair(QPalette::ColorRole::ToolTipBase, QColor(205, 205, 205)));
    color_schemes[Color_Scheme::Light].insert(std::make_pair(QPalette::ColorRole::ToolTipText, QColor(98, 98, 98)));
    color_schemes[Color_Scheme::Light].insert(std::make_pair(QPalette::ColorRole::BrightText, QColor(0, 0, 0)));
    color_schemes[Color_Scheme::Light].insert(std::make_pair(QPalette::ColorRole::Mid, QColor(48, 48 , 48)));
    color_schemes[Color_Scheme::Light].insert(std::make_pair(QPalette::ColorRole::Midlight, QColor(200, 200, 200)));
    color_schemes[Color_Scheme::Light].insert(std::make_pair(QPalette::ColorRole::Shadow, QColor(130, 130, 130)));

    color_schemes[Color_Scheme::Blue].insert(std::make_pair(QPalette::ColorRole::Window, QColor(20,55,103)));
    color_schemes[Color_Scheme::Blue].insert(std::make_pair(QPalette::ColorRole::Background, QColor(20,55,103)));
    color_schemes[Color_Scheme::Blue].insert(std::make_pair(QPalette::ColorRole::WindowText, QColor(112, 170, 229)));
    color_schemes[Color_Scheme::Blue].insert(std::make_pair(QPalette::ColorRole::Foreground, QColor(112, 170, 229)));
    color_schemes[Color_Scheme::Blue].insert(std::make_pair(QPalette::ColorRole::Base, QColor(11, 32, 55)));
    color_schemes[Color_Scheme::Blue].insert(std::make_pair(QPalette::ColorRole::AlternateBase, QColor(212, 234, 254)));
    color_schemes[Color_Scheme::Blue].insert(std::make_pair(QPalette::ColorRole::Text, QColor(112, 170, 229)));
    color_schemes[Color_Scheme::Blue].insert(std::make_pair(QPalette::ColorRole::Button, QColor(11, 32, 55)));
    color_schemes[Color_Scheme::Blue].insert(std::make_pair(QPalette::ColorRole::Dark, QColor(0,105,200)));
    color_schemes[Color_Scheme::Blue].insert(std::make_pair(QPalette::ColorRole::Highlight, QColor(29, 77, 144)));
    color_schemes[Color_Scheme::Blue].insert(std::make_pair(QPalette::ColorRole::HighlightedText, QColor(0, 150, 145)));
    color_schemes[Color_Scheme::Blue].insert(std::make_pair(QPalette::ColorRole::ButtonText, QColor(212, 234 ,254)));
    color_schemes[Color_Scheme::Blue].insert(std::make_pair(QPalette::ColorRole::ToolTipBase, QColor(20, 55, 103)));
    color_schemes[Color_Scheme::Blue].insert(std::make_pair(QPalette::ColorRole::ToolTipText, QColor(112, 234 , 254)));
    color_schemes[Color_Scheme::Blue].insert(std::make_pair(QPalette::ColorRole::BrightText, QColor(212, 234, 254)));
    color_schemes[Color_Scheme::Blue].insert(std::make_pair(QPalette::ColorRole::Mid, QColor(112, 170, 229)));
    color_schemes[Color_Scheme::Blue].insert(std::make_pair(QPalette::ColorRole::Midlight, QColor(20,55,103)));
    color_schemes[Color_Scheme::Blue].insert(std::make_pair(QPalette::ColorRole::Shadow, QColor(5, 16, 28)));
}

