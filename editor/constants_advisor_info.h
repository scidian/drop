﻿//
//      Created by Stephens Nunnally on 2/15/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Some Constant Project Data
//
//
#ifndef CONSTANTS_EDITOR_H
#define CONSTANTS_EDITOR_H

#include <QString>
#include <vector>


//####################################################################################
//##    Possible Headers and Descriptions for items within editor
//############################
typedef std::vector<QString> HeaderBodyList;

namespace Advisor_Info {

    // Toolbar Buttons
    const HeaderBodyList Mode_Map               { "World Map", "This mode will allow you to see the layout of your Project and how the worlds and "
                                                               "user interfaces in your Project connect to each other." };
    const HeaderBodyList Mode_Editor            { "World Editor Mode", "This mode will allow you to edit the contents of each physics world. It is where "
                                                                       "you can design each Stage and lay out your worlds / levels." };
    const HeaderBodyList Mode_UI                { "UI Editor Mode",     "This mode will allow you to edit the user interface layer of your Project." };
    const HeaderBodyList Settings_Playground    { "Physics Playground", "Play with the physics engine to learn more about physics properties." };
    const HeaderBodyList Settings_Image_Viewer  { "Image Viewer",       "View / manage Images used in this Project." };
    const HeaderBodyList Settings_Font_Builder  { "Font Builder",       "Create and edit fonts used for Text Boxes." };
    const HeaderBodyList Settings_Manager       { "Settings Manager",   "View and edit Project settings." };

    const HeaderBodyList Send_to_Back           { "Send to Back",  "Send selected item(s) to the back." };
    const HeaderBodyList Send_to_Front          { "Send to Font",  "Send selected item(s) to the front." };
    const HeaderBodyList Send_Backward          { "Send Backward", "Send selected item(s) back by one." };
    const HeaderBodyList Send_Forward           { "Send Forward",  "Send selected item(s) forward by one." };

    const HeaderBodyList Add_Entity             { "Add Item",      "Adds an item to the Project." };
    const HeaderBodyList Duplicate              { "Duplicate",     "Create a copy of selected item(s)." };
    const HeaderBodyList Trash_Can              { "Delete",        "Remove selected item(s)." };

    const HeaderBodyList Reset                  { "Reset Transform", "Resets selected objects' transforms to a Scale of X:1, Y:1 and Rotation of "
                                                                     "0 Degrees." };
    const HeaderBodyList Flip_H                 { "Flip Horizontal", "Flips the selection horizontally." };
    const HeaderBodyList Flip_V                 { "Flip Vertical",   "Flips the selection vertically." };
    const HeaderBodyList Rotate_L               { "Rotate Left",  "Rotates selection counter-clockwise by 90 degrees." };
    const HeaderBodyList Rotate_R               { "Rotate Right", "Rotates selection clockwise by 90 degrees." };

    const HeaderBodyList Grid_Show_On_Top       { "Grid on Top?",    "Draws grid lines on top of objects in editor." };
    const HeaderBodyList Resize_To_Grid         { "Resize to Grid?", "Snaps corners and edges of objects to grid while resizing. Only works if the angle "
                                                                     "of the object matches the angle of the grid, and the Grid Scale X and Y are the same. "
                                                                     "Selection handles will change to square shape to signify snapping is possible."};
    const HeaderBodyList Grid_Snap_To_Grid      { "Snap to Grid?",   "Snaps objects to grid lines when moving objects around with the mouse." };
    const HeaderBodyList Grid_Snap_Options      { "Snap Options",    "Choose how objects snap to grid while multiple objects are selected, and objects are moved "
                                                                     "with mouse. Either have the center of the selection group snap to the grid, or have each "
                                                                     "object within the group snap to grid individually." };

    const HeaderBodyList Play_Game              { "Play Game",  "Start playing game! Gameplay starts at the Start Stage of the World selected." };
    const HeaderBodyList Play_Stage             { "Play Stage", "Play <b>ONLY</b> the current Stage. Gameplay starts at the Start Stage of the current World "
                                                                "and only loads in the Stage currently selected." };

    const HeaderBodyList Mouse_Pointer          { "Selection Tool", "This is the main tool mode of the mouse cursor in the editor. Use this to select and move objects."};
    const HeaderBodyList Mouse_Hand             { "Hand Tool",      "Use this tool to move the viewable game area in the editor. It is also possible to activate the "
                                                                    "<b>Hand Tool</b> while using the <b>Selection Tool</b> by holding down the <b>Space Bar</b>." };
    const HeaderBodyList Mouse_Magnify          { "Zoom Tool",      "Use this tool to change the zoom level of the editor. It is also possible to change the zoom level "
                                                                    "while using the <b>Selection Tool</b> by using the <b>Mouse Wheel</b> (when available)."};

    const HeaderBodyList Camera_On_Off          { "Camera Boxes On / Off", "Toggles drawing of Camera Boxes On / Off. These boxes show "
                                                                           "the lag areas of Character Cameras." };
    const HeaderBodyList Debug_On_Off           { "Collision Shapes On / Off", "Toggles drawing of Physics Collision Shapes On / Off. These shapes show "
                                                                               "physics / event collisions within your game. "
                                                                               "<b>Blue:</b> Player, <b>Red:</b> Enemy, "
                                                                               "<b>Purple:</b> Damage All, <b>Green:</b> Damage None" };

    // Form Main Widgets
    const HeaderBodyList Advisor_Window         { "Advisor Window", "Shows a brief description of editor items." };
    const HeaderBodyList Inspector_Window       { "Inspector", "Displays components / properties of currently selected item." };

    const HeaderBodyList ColorButton            { "Color Button", "Click this to open a color popup that has a convenient color selection." };
    const HeaderBodyList ColorPicker            { "Color Picker", "Press and hold this button to use a magnifying lens to pick a color off of the screen." };
    const HeaderBodyList ColorDialog            { "Color Dialog", "Opens the system color dialog toolbox." };

    // Editor Widgets
    const HeaderBodyList Asset_List             { "Asset List", "These are items that can be dragged into your Project. Changing the properties "
                                                                "of these items will affect all instances of those items Project wide." };
    const HeaderBodyList Asset_Search           { "Asset Search", "Type the name of an asset here to help find it in the Asset List." };
    const HeaderBodyList Stage_View             { "Stage View", "Shows objects and layout of currently selected Stage. Drop assets into Stage "
                                                                "View to add to Stage." };
    const HeaderBodyList Project_Tree           { "Project Tree", "Lists the items contained within the currently displayed Stage. Select items "
                                                                  "to view / adjust properties for each item." };

    // Project Types
    const HeaderBodyList Asset_Description      { "Asset", "This is an item that can be dragged into your Project. Changing the properties "
                                                           "of this item will affect all instances of this item Project wide." };
    const HeaderBodyList World_Description      { "World", "A World is a container of Stages." };
    const HeaderBodyList Stage_Description      { "Stage", "A Stage is a container of Things." };
    const HeaderBodyList Camera_Thing           { "Camera", "This is a Camera. This will decide what the player sees." };
    const HeaderBodyList Character_Thing        { "Character", "This is a Character." };
    const HeaderBodyList Thing_Description      { "Thing", "This is a Thing in a Stage. Things are anything in a Stage (i.e. Character, Object, etc)." };

    // Asset Types
    const HeaderBodyList Asset_Character        { "Character Asset", "This is a player character." };
    const HeaderBodyList Asset_Object           { "Object Asset", "This is an object that can have many copies of itself placed into your game. Any change to "
                                                                  "this Asset will change all occurences of the Asset within your game." };
    const HeaderBodyList Asset_Device           { "Device Asset", "This is a device that is used to add extra control to your game." };
    const HeaderBodyList Asset_Effect           { "Effect Asset", "Special effects provide an extra level of detail to make your game more visually appealing." };
    const HeaderBodyList Asset_Image            { "Image Asset", "This is an Image for use in your game." };
    const HeaderBodyList Asset_Image_Built_In   { "Buil In Image Asset", "This is a simple, easy to use built-in Image for use in your game." };
    const HeaderBodyList Asset_Text             { "Text Asset", "Use Text Assets to show text boxes in your game. You can use them as titles or for showing "
                                                                "values like hit points, coins or distance." };



    // Object Types
    const HeaderBodyList Object_Static          { "Static",     "Object can not move. Best performance, dynamic objects touching static objects can rest "
                                                                "when not moving which reduces physics calculations. Best for ground, walls, fixed enemies "
                                                                "(like spikes), etc." };
    const HeaderBodyList Object_Kinematic       { "Kinematic",  "Object can move at fixed speeds. Kinematic objects are not affected by physical or "
                                                                "gravitational forces. Best for doors, moving platforms, some enemies, etc." };
    const HeaderBodyList Object_Dynamic         { "Dynamic",    "Object obeys the laws of physics. Dynamic objects can be moved by forces and are affected by "
                                                                "gravity. Best for players, small enemies, balls, boxes, cars, wheels, etc."};

    const HeaderBodyList Damage_None            { "No Damage",      "This object does not damage other objects." };
    const HeaderBodyList Damage_Player          { "Damage Player",  "This is an enemy object. It will damage player objects that it comes into contact with." };
    const HeaderBodyList Damage_Enemy           { "Damage Enemy",   "This is a player object. It will damage enemy objects that it comes into contact with." };
    const HeaderBodyList Damage_All             { "Damage All",     "This object can be damaged by players and enemies. It will damage all objects it comes "
                                                                    "into contact with. "};

    const HeaderBodyList Light_Opaque           { "Opaque Light",   "Light is drawn as solid texture, does not provide actual lighting to scene, only shadows. "
                                                                    "However, light is able to be layered in a specific Z-Order." };
    const HeaderBodyList Light_Glow             { "Glow Light",     "Light provides diffuse lighting. Ambient light setting should be adjusted in World Settings "
                                                                    "to take advantage of Glow Lights. All Glow Lights are drawn at Z-Order specified in World Settings."};

    // Inspector Widgets
    const HeaderBodyList Variable_Widget        { "Variable Amount", "Plus or minus modifier to initial value, the following value allows for some "
                                                                     "variable amount to the initial value. For example, an initial value of 100 with "
                                                                     "a variable amount of 5, allows for values ranging from 95 to 105." };

    // Other
    const HeaderBodyList Not_Set                { "Not Set", "Fix me!!!!!!" };
};



#endif // CONSTANTS_EDITOR_H














