//
//      Created by Stephens Nunnally on 2/15/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Some Constatnt Project Data
//
//
#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QColor>
#include <QCursor>
#include <QPixmap>
#include <QString>


//####################################################################################
//##    Possible background colors for Component Categories
//####################################################################################
namespace Component_Colors
{
    const QColor White_Snow     { QColor(255, 252, 249, 255) };
    const QColor Silver_Sonic   { QColor(117, 112, 131, 255) };

    const QColor Orange_Medium  { QColor(221, 110,  66, 255) };

    const QColor Pink_Pearl     { QColor(228, 180, 194, 255) };
    const QColor Red_Faded      { QColor(255, 105, 120, 255) };

    const QColor Red_Tuscan     { QColor(104,  71,  86, 255) };
    const QColor Purple_Pastel  { QColor(176, 161, 186, 255) };
    const QColor Blue_Yonder    { QColor(165, 181, 191, 255) };

    const QColor Green_SeaGrass { QColor(154, 225, 157, 255) };

    const QColor Mustard_Yellow { QColor(255, 200,  87, 255) };
    const QColor Mellow_Yellow  { QColor(250, 223, 127, 255) };

    const QColor Brown_Sugar    { QColor(165, 117,  72, 255) };
    const QColor Beige_Apricot  { QColor(252, 215, 173, 255) };


};

//####################################################################################
//##    Possible icons for Component Categories
//####################################################################################
namespace Component_Icons
{
    const QString Settings      { QString(":/inspector_icons/comp_settings.png") };
    const QString Physics       { QString(":/inspector_icons/comp_physics.png") };
    const QString Transform     { QString(":/inspector_icons/comp_transform.png") };
    const QString Layering      { QString(":/inspector_icons/comp_layering.png") };
    const QString Movement      { QString(":/inspector_icons/comp_movement.png") };
    const QString Appearance    { QString(":/inspector_icons/comp_appearance.png") };
    const QString Camera        { QString(":/inspector_icons/comp_camera.png") };
    const QString Character     { QString(":/inspector_icons/comp_character.png") };
    const QString Animation     { QString(":/inspector_icons/comp_animation.png") };
    const QString Font          { QString(":/inspector_icons/comp_font.png") };
    const QString None          { QString("") };
};



//####################################################################################
//##    Possible Headers and Descriptions for items within editor
//####################################################################################
typedef QList<QString> HeaderBodyList;

namespace Advisor_Info
{
    // Toolbar Buttons
    const HeaderBodyList Mode_Map               { "World Map", "This mode will allow you to see the layout of your project and how the worlds and "
                                                              "user interfaces in you project connect to each other. " };
    const HeaderBodyList Mode_Editor            { "World Editor Mode", "This mode will allow you to edit the contents of each world. It is where "
                                                                      "you can design each Stage and lay out your levels." };
    const HeaderBodyList Mode_UI                { "UI Editor Mode", "This mode will allow you to edit the user interfaces of your project. " };
    const HeaderBodyList Settings_Font_Editor   { "Font Editor", "Create and edit fonts usued for Text objects." };
    const HeaderBodyList Settings_Editor        { "Settings Editor", "View and edit project settings." };

    const HeaderBodyList Grid_Snap_To_Grid      { "Snap to Grid?", "Snaps objects to grid lines when moving objects around with the mouse." };
    const HeaderBodyList Resize_To_Grid         { "Resize to Grid?", "Snaps objects to grid lines while resizing. This only works if the angle of the object "
                                                                     "matches the angle of the grid. Resize handles will turn from round to square "
                                                                     "to signify snapping is possible. "};
    const HeaderBodyList Grid_Show_On_Top       { "Grid on Top?", "Draws grid lines on top of objects in editor." };


    // Form Main Widgets
    const HeaderBodyList Advisor_Window         { "Advisor Window", "Shows a brief description of editor objects." };
    const HeaderBodyList Object_Inspector       { "Object Inspector", "Displays editable properties of currently selected item." };

    // Editor Widgets
    const HeaderBodyList Asset_List             { "Asset List", "These are items that can be dragged into your project. Changing the properties "
                                                                "of these items will affect all instances of those items project wide." };
    const HeaderBodyList Stage_View             { "Stage View", "Shows objects and layout of currently selected Stage. Drop assets into Stage "
                                                                "View to add to Stage." };
    const HeaderBodyList Project_Tree           { "Project Tree", "Lists the items contained within the currently displayed Stage. Select items "
                                                                  "to view / adjust properties for each item." };

    // Project Types
    const HeaderBodyList World_Object           { "World Object", "A World is a container of Stages." };
    const HeaderBodyList Stage_Object           { "Stage Object", "A Stage is a container of Objects." };
    const HeaderBodyList Camera_Object          { "Camera Object", "This is a camera object. This object will decide what the player sees." };
    const HeaderBodyList Character_Object       { "Character Object", "This is a character object" };
    const HeaderBodyList Object_Object          { "Object", "This is an object in a Stage." };

    // Asset Types
    const HeaderBodyList Asset_Object           { "Object Asset", "This is an object that can have many copies of itself placed into your game. Any change to this "
                                                          "asset will change all occurences of the asset within the game." };
    const HeaderBodyList Asset_Text             { "Text Asset", "Use text assets to show labels in your game. You can use the as titles or for showing "
                                                                "values like hit points, coins or distance. " };

    // Inspector Widgets
    const HeaderBodyList Variable_Widget        { "Variable Amount", "Plus or minus modifier to initial value, the following value allows for some "
                                                                     "variable amount to the initial value. For example, an initial value of 100 with "
                                                                     "a variable amount of 5, allows for values ranging from 95 to 105." };

    // Other
    const HeaderBodyList Not_Set                { "Not Set", "Fix me!!!!!!" };
};


//####################################################################################
//##    Mouse cursors to use throughout project, defined in constants.cpp
//####################################################################################
namespace Mouse_Cursors {
    QCursor sizeVertical();
    QCursor size022();
    QCursor size045();
    QCursor size067();
    QCursor sizeHorizontal();
    QCursor size112();
    QCursor size135();
    QCursor size157();
    QCursor rotateAll();
};



#endif // CONSTANTS_H















