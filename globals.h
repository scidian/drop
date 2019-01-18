//
//      Created by Stephens Nunnally on 12/7/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Globals - Holds some global variables for use across the project
//
//

#ifndef GLOBALS_H
#define GLOBALS_H

#include <bitset>
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

// ***** Possible debug options
enum class Debug_Flags {
    Secret_Menu,                    // Add special menu to menu bar containing some hidden test functions
    FPS,                            // Show frames per second of SceneGraphicsView
    Mouse_Coordinates,              // Show mouse position coordinates while moving over SceneGraphicsView

    Object_Inspector_Build,         // Show info about item sent to Object Inspector Tree
    Scene_Tree_Drag,                // Show info about dragging event of Scene Tree
    Scene_Tree_Selection,           // Show info about item selected in Scene Tree

    Rotation_Data,                  // Show rotation data while in View_Mode::Rotating in SceneGraphicsView
    Selection_Box_Group_Data,       // Show info about SceneGraphicScene current QGraphicsItemGroup of selected items
    Selected_Item_Data,             // Show info about first item in Selection Group
    Shear_Matrix,                   // Show matrix for selected item's transform()
    Total,
};


class Globals
{
private:
    void    loadPalettes();                         // Called during Constructor to load data into color_schemes

public:
    // Constructor
    Globals();

    // Variables
    std::bitset<static_cast<int>(Debug_Flags::Total)>   debug_flags;

    std::map<Color_Scheme, std::map<Window_Colors, QColor>> color_schemes;
    Color_Scheme current_color_scheme;

    // Variable Functions
    QColor  getColor(Window_Colors color_role) { return color_schemes[current_color_scheme][color_role]; }

    // Helper functions
    void    showMessageBox(QString new_message);

};

#endif // GLOBALS_H










