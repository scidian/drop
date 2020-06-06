//
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
//##    Possible Headers and Descriptions for Items within editor
//############################
typedef std::vector<QString> HeaderBodyList;

namespace Advisor_Info {

    // ToolBar Buttons
    const HeaderBodyList Mode_World_Graph       { "World Graph", "This mode will allow you to see the layout of your Project and how the Worlds and "
                                                                 "User Interfaces in your Project connect to each other." };
    const HeaderBodyList Mode_World_Creator     { "World Creator", "This mode will allow you to edit the contents of each Physics World. It is where "
                                                                   "you can design each Stage and lay out your worlds / levels." };
    const HeaderBodyList Mode_UI_Creator        { "UI Creator", "This mode will allow you to edit the user interface layers of your Project." };
    const HeaderBodyList Mode_Sound_Creator     { "Sound Creator", "This mode will allow you mix / edit sounds for your Project." };

    const HeaderBodyList Settings_Image_Viewer  { "Image Viewer",       "View / manage Images used in this Project." };
    const HeaderBodyList Settings_Sound_Files   { "Sound Files",        "Create and edit sounds to use in this Project." };
    const HeaderBodyList Settings_Font_Builder  { "Font Builder",       "Create and edit fonts used for Text Boxes." };
    const HeaderBodyList Settings_Manager       { "Project Settings",   "View and edit Project settings." };

    const HeaderBodyList Send_to_Back           { "Send to Back",  "Send selected Thing(s) to the back." };
    const HeaderBodyList Send_to_Front          { "Send to Font",  "Send selected Thing(s) to the front." };
    const HeaderBodyList Send_Backward          { "Send Backward", "Send selected Thing(s) back by one." };
    const HeaderBodyList Send_Forward           { "Send Forward",  "Send selected Thing(s) forward by one." };

    const HeaderBodyList Add_Entity             { "Add Item",      "Adds an Item to the Project." };
    const HeaderBodyList Duplicate              { "Duplicate",     "Create a copy of selected Item(s)." };
    const HeaderBodyList Trash_Can              { "Delete",        "Remove selected Item(s)." };

    const HeaderBodyList Reset                  { "Reset Transform", "Resets selected Thing(s) transform to a Scale of X:1, Y:1 and Rotation of "
                                                                     "0 Degrees." };
    const HeaderBodyList Flip_H                 { "Flip Horizontal", "Flips the selection horizontally." };
    const HeaderBodyList Flip_V                 { "Flip Vertical",   "Flips the selection vertically." };
    const HeaderBodyList Rotate_L               { "Rotate Left",  "Rotates selection counter-clockwise by 90 degrees." };
    const HeaderBodyList Rotate_R               { "Rotate Right", "Rotates selection clockwise by 90 degrees." };

    const HeaderBodyList Grid_Show_On_Top       { "Grid on Top?",    "Draws grid lines on top of Things in editor." };
    const HeaderBodyList Resize_To_Grid         { "Resize to Grid?", "Snaps corners and edges of Things to grid while resizing. Only works if the angle "
                                                                     "of the Thing matches the angle of the grid, and the Grid Scale X and Y are the same. "
                                                                     "Selection handles will change to square shape to signify snapping is possible."};
    const HeaderBodyList Grid_Snap_To_Grid      { "Snap to Grid?",   "Snaps Items to grid lines when moving Items around with the mouse." };
    const HeaderBodyList Grid_Snap_Options      { "Snap Options",    "Choose how Things snap to grid while multiple Things are selected, and Things are moved "
                                                                     "with mouse. Either have the center of the selection group snap to the grid, or have each "
                                                                     "Thing within the group snap to grid individually." };

    const HeaderBodyList Play_Game              { "Play Game",  "Start playing game! Gameplay starts at the Start Stage of the World selected." };
    const HeaderBodyList Play_Stage             { "Play Stage", "Play <b>ONLY</b> the current Stage. Gameplay starts at the Start Stage of the current World "
                                                                "and only loads in the Stage currently selected." };

    const HeaderBodyList Mouse_Pointer          { "Selection Tool", "This is the main tool mode of the mouse cursor in the editor. Use this to select and move Things."};
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
    // Editor Widgets
    const HeaderBodyList Inspector_Window       { "Inspector", "Displays components / properties of currently selected Item." };
    const HeaderBodyList Advisor_Window         { "Advisor Window", "Shows a brief description of currently highlighted / selected item(s)." };
    const HeaderBodyList Asset_List             { "Asset List", "These are Things that can be dragged into your Project. Changing the properties "
                                                                "of these Things will affect all instances of those Things Project wide." };
    const HeaderBodyList Asset_Search           { "Asset Search", "Type the name of an asset here to help find it in the Asset List." };
    const HeaderBodyList World_Editor           { "World Editor", "Shows Things and layout of currently selected World / Stage. Drop assets into World "
                                                                  "Editor to add them to current Stage." };
    const HeaderBodyList Project_Tree           { "Project Tree", "Lists the Things contained within the currently displayed Stage. Select Things "
                                                                  "to view / adjust properties for each Thing." };
    const HeaderBodyList World_Graph            { "World Graph", "Shows project Worlds and User Interfaces. Allows you to control the flow of your game "
                                                                 "between Worlds and UIs." };

    // Form Main Widgets
    const HeaderBodyList ColorButton            { "Color Button", "Click this to open a color popup that has a convenient color selection." };
    const HeaderBodyList ColorPicker            { "Color Picker", "Press and hold this button to use a magnifying lens to pick a color off of the screen." };
    const HeaderBodyList ColorDialog            { "Color Dialog", "Opens the system color dialog toolbox." };

    // Project Types
    const HeaderBodyList Asset_Description      { "Asset", "This is an Thing that can be dragged into your Project. Changing the properties "
                                                           "of this Asset Thing will affect all occurences of the Asset within your game." };
    const HeaderBodyList World_Description      { "World", "A World is a container of Stages." };
    const HeaderBodyList Stage_Description      { "Stage", "A Stage is a container of Things." };
    const HeaderBodyList Camera_Thing           { "Camera", "This is a Camera. This will decide what the player sees." };
    const HeaderBodyList Character_Thing        { "Character", "This is a Character." };
    const HeaderBodyList Thing_Description      { "Thing", "This is a Thing in a Stage. Things are anything in a Stage (i.e. Character, Object, etc)." };
    const HeaderBodyList Things_Description     { "Things", "These are Things in a Stage. Things are anything in a Stage (i.e. Character, Object, etc)." };

    // Asset Types
    const HeaderBodyList Asset_Character        { "Character Asset", "This is a player character." };
    const HeaderBodyList Asset_Object           { "Object Asset", "This is an Object Thing that can have many copies of itself placed into your game. Any change to "
                                                                  "this Asset will change all occurences of the Asset within your game." };
    const HeaderBodyList Asset_Device           { "Device Asset", "This is a device that is used to add extra control to your game." };
    const HeaderBodyList Asset_Effect           { "Effect Asset", "Special effects provide an extra level of detail to make your game more visually appealing." };
    const HeaderBodyList Asset_Image            { "Image Asset", "This is an Image for use in your game." };
    const HeaderBodyList Asset_Item             { "Item Asset", "Specialized Items that serve to add depth and functionality to your game." };
    const HeaderBodyList Asset_Mix              { "Mix Asset", "This is a Mix Composition of Tracks created / editable in the Sound Creator." };
    const HeaderBodyList Asset_Prefab           { "Prefab Asset", "These are Physics Objects whose settings are already set to commmon settings for specific "
                                                                  "types of game objects." };
    const HeaderBodyList Asset_Sound            { "Sound Asset", "This is a Audio File or Sound Effect for use in your game or in the Sound Creator to mix into music." };
    const HeaderBodyList Asset_Text             { "Text Asset", "Use Text Assets to show text boxes in your game. You can use them as titles or for showing "
                                                                "values like hit points, coins or distance." };



    // Object Types
    const HeaderBodyList Object_Static          { "Static",     "Object can not move. Best performance, Dynamic Objects touching Static Objects can rest "
                                                                "when not moving which reduces physics calculations. Best for ground, walls, fixed enemies "
                                                                "(like spikes), etc." };
    const HeaderBodyList Object_Kinematic       { "Kinematic",  "Object can move at fixed speeds. Kinematic Objects are not affected by physical or "
                                                                "gravitational forces. Best for doors, moving platforms, some enemies, etc." };
    const HeaderBodyList Object_Dynamic         { "Dynamic",    "Object obeys the laws of physics. Dynamic Objects can be moved by forces and are affected by "
                                                                "gravity. Best for players, small enemies, balls, boxes, cars, wheels, etc."};

    const HeaderBodyList Damage_None            { "No Damage",      "This Object does not damage other Objects." };
    const HeaderBodyList Damage_Player          { "Damage Player",  "This is an enemy Object. It will damage player Objects that it comes into contact with." };
    const HeaderBodyList Damage_Enemy           { "Damage Enemy",   "This is a player Object. It will damage enemy Objects that it comes into contact with." };
    const HeaderBodyList Damage_All             { "Damage All",     "This Object can be damaged by players and enemies. It will damage all Objects it comes "
                                                                    "into contact with. "};

    const HeaderBodyList Light_Opaque           { "Opaque Light",   "Light is drawn as solid texture, does not provide actual lighting to scene, only shadows. "
                                                                    "However, light is able to be layered in a specific Z-Order." };
    const HeaderBodyList Light_Glow             { "Glow Light",     "Light provides diffuse lighting. Ambient light setting should be adjusted in World Settings "
                                                                    "to take advantage of Glow Lights. All Glow Lights are drawn at Z-Order specified in World Settings."};

    // Inspector Widgets
    const HeaderBodyList Variable_Widget        { "Variable Amount", "Plus or minus modifier to initial value, the following value allows for some "
                                                                     "variable amount to the initial value. For example, an initial value of 100 with "
                                                                     "a variable amount of 5, allows for values ranging from 95 to 105." };

    // View Modes
    const HeaderBodyList Camera_Edit            { "Camera Edit", "While Camera Boxes are On, cameras are interactive. Cameras can be dragged to desired position. "
                                                                 "Also possible to change zoom level of camera using Mouse Wheel (when available)." };


    // Other
    const HeaderBodyList Not_Set                { "Not Set", "Fix me!!!!!!" };
};



#endif // CONSTANTS_EDITOR_H















