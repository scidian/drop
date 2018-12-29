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
    color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Shadow_Dark, QColor(0, 0, 0)));
    color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Shadow_Light, QColor(16, 16, 16)));
    color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Background_Dark, QColor(24, 24, 24)));
    color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Background_Light, QColor(32, 32, 32)));
    color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Button_Dark, QColor(24, 24, 24)));
    color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Button_Light, QColor(48, 48, 48)));
    color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Midlight, QColor(64, 64, 64)));
    color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Text_Dark, QColor(195, 195, 195)));
    color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Text, QColor(205, 205, 205)));
    color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Text_Light, QColor(215, 215, 215)));
    color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Highlight, QColor(255, 255, 255)));
    color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Icon_Dark, QColor(0, 150, 145)));
    color_schemes[Color_Scheme::Dark].insert(std::make_pair(Window_Colors::Icon_Light, QColor(0, 180, 175)));

    color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Shadow_Dark, QColor(255, 255, 255)));
    color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Shadow_Light, QColor(240, 240, 240)));
    color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Background_Dark, QColor(200, 200, 200)));
    color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Background_Light, QColor(225, 225, 225)));
    color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Button_Dark, QColor(195, 195, 195)));
    color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Button_Light, QColor(215, 215, 215)));
    color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Midlight, QColor(128, 128, 128)));
    color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Text_Dark, QColor(45, 45, 45)));
    color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Text, QColor(55, 55, 55)));
    color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Text_Light, QColor(65, 65, 65)));
    color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Highlight, QColor(0, 0, 0)));
    color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Icon_Dark, QColor(0, 150, 145)));
    color_schemes[Color_Scheme::Light].insert(std::make_pair(Window_Colors::Icon_Light, QColor(0, 180, 175)));

    color_schemes[Color_Scheme::Blue].insert(std::make_pair(Window_Colors::Shadow_Dark, QColor(5, 16, 28)));
    color_schemes[Color_Scheme::Blue].insert(std::make_pair(Window_Colors::Shadow_Light, QColor(11, 32, 55)));
    color_schemes[Color_Scheme::Blue].insert(std::make_pair(Window_Colors::Background_Dark, QColor(20,55,103)));
    color_schemes[Color_Scheme::Blue].insert(std::make_pair(Window_Colors::Background_Light, QColor(29, 77, 144)));
    color_schemes[Color_Scheme::Blue].insert(std::make_pair(Window_Colors::Button_Dark, QColor(5, 16, 28)));
    color_schemes[Color_Scheme::Blue].insert(std::make_pair(Window_Colors::Button_Light, QColor(11, 32, 55)));
    color_schemes[Color_Scheme::Blue].insert(std::make_pair(Window_Colors::Midlight, QColor(29, 77, 144)));
    color_schemes[Color_Scheme::Blue].insert(std::make_pair(Window_Colors::Text_Dark, QColor(106, 160, 216)));
    color_schemes[Color_Scheme::Blue].insert(std::make_pair(Window_Colors::Text, QColor(112, 170, 229)));
    color_schemes[Color_Scheme::Blue].insert(std::make_pair(Window_Colors::Text_Light, QColor(118, 180, 242)));
    color_schemes[Color_Scheme::Blue].insert(std::make_pair(Window_Colors::Highlight, QColor(212, 234, 254)));
    color_schemes[Color_Scheme::Blue].insert(std::make_pair(Window_Colors::Icon_Dark, QColor(0, 150, 145)));
    color_schemes[Color_Scheme::Blue].insert(std::make_pair(Window_Colors::Icon_Light, QColor(0, 180, 175)));

    color_schemes[Color_Scheme::Autumn].insert(std::make_pair(Window_Colors::Shadow_Dark, QColor(39, 18, 11)));
    color_schemes[Color_Scheme::Autumn].insert(std::make_pair(Window_Colors::Shadow_Light, QColor(64, 30, 19)));
    color_schemes[Color_Scheme::Autumn].insert(std::make_pair(Window_Colors::Background_Dark, QColor(225, 167, 69)));
    color_schemes[Color_Scheme::Autumn].insert(std::make_pair(Window_Colors::Background_Light, QColor(166, 59, 31)));
    color_schemes[Color_Scheme::Autumn].insert(std::make_pair(Window_Colors::Button_Dark, QColor(233,125,49)));
    color_schemes[Color_Scheme::Autumn].insert(std::make_pair(Window_Colors::Button_Light, QColor(213,112,41)));
    color_schemes[Color_Scheme::Autumn].insert(std::make_pair(Window_Colors::Midlight, QColor(197, 79, 32)));
    color_schemes[Color_Scheme::Autumn].insert(std::make_pair(Window_Colors::Text_Dark, QColor(100, 41, 23)));
    color_schemes[Color_Scheme::Autumn].insert(std::make_pair(Window_Colors::Text, QColor(109, 51, 33)));
    color_schemes[Color_Scheme::Autumn].insert(std::make_pair(Window_Colors::Text_Light, QColor(120, 61, 43)));
    color_schemes[Color_Scheme::Autumn].insert(std::make_pair(Window_Colors::Highlight, QColor(218, 190, 98)));
    color_schemes[Color_Scheme::Autumn].insert(std::make_pair(Window_Colors::Icon_Dark,  QColor(131, 142, 41)));
    color_schemes[Color_Scheme::Autumn].insert(std::make_pair(Window_Colors::Icon_Light,  QColor(107, 116, 33)));
}













