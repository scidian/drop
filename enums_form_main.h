//
//      Created by Stephens Nunnally on 3/15/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef ENUMS_FORM_MAIN_H
#define ENUMS_FORM_MAIN_H


//####################################################################################
//##    Types of modes for Form Main
//############################
enum class Form_Main_Mode {
    Program_Loading,

    World_Map,                  // "Map"
    World_Editor,               // "Editor" (Physics)
    UI_Editor,                  // "UI"

    Stage_Map,
    // App_Editor,
    // Puzzle_World_Editor,

    Clear,
};



//####################################################################################
//##    Different widgets in an editor window, used for cross updating widgets
//############################
enum class Editor_Widgets {
    Asset_Tree,
    Inspector_Tree,
    Project_Tree,
    Stage_View,

    ToolBar,
};


//####################################################################################
//##    Interactive mouse modes
//############################
enum class View_Mode {
    None,
    Disable_Update,     // Special mode to disallow updates
    Selecting,          // Rubber band selection
    Resizing,           // Changing items size
    Rotating,           // Rotating items
    Translating,        // Moving item(s) around
    Dragging,           // Moving scene with space bar
    Zooming,            // Zooming in / out of view
    Holding_Keys,       // Holding arrow keys to move or layering keys
};


// Form Main enum for referencing debug labels
enum class Label_Names {
    Label_1,            Label_2,            Label_3,            Label_Mouse_1,      Label_Mouse_2,
    Label_Object_1,     Label_Object_2,     Label_Object_3,     Label_Object_4,     Label_Object_5,
    Label_Position,     Label_Center,       Label_Scale,        Label_Rotate,       Label_Z_Order,      Label_Pos_Flag,
    Label_Bottom,
};


//####################################################################################
//##    Toolbar Button Groups
//############################
enum class Buttons_Layering {
    Send_To_Back,
    Send_Backward,
    Send_To_Front,
    Send_Forward,
};

enum class Buttons_Edit {
    Add,
    Duplicate,
    Delete,
};

enum class Buttons_Transform {
    Reset_Object,
    Flip_H,
    Flip_V,
    Rotate_L,
    Rotate_R,
};

enum class Buttons_Grid {
    Snap_To_Grid,
    Snap_Options,
    Resize_To_Grid,
    Grid_On_Top,
};

enum class Buttons_Play {
    Play,
    Play_Stage,
};









#endif // ENUMS_FORM_MAIN_H






