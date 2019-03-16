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
//####################################################################################
enum class Form_Main_Mode {
    None,
    World_Map,          // "Map"
    World_Editor,       // "Editor"
    UI_Editor,          // "UI"
    Stage_Map,
    Clear,
};



//####################################################################################
//##    Different widgets in an editor window, used for cross updating widgets
//####################################################################################
enum class Editor_Widgets {
    Asset_Tree,
    Object_Inspector,
    Project_Tree,
    Scene_View,
};


//####################################################################################
//##    Interactive mouse modes
//####################################################################################
enum class View_Mode {
    None,
    Selecting,          // Rubber band selection
    Resizing,           // Changing items size
    Rotating,           // Rotating items
    Translating,        // Moving item(s) around
    Dragging,           // Moving scene with space bar
    Zooming,            // Zooming in / out of view
};


// Form Main enum for referencing debug labels
enum class Label_Names
{
    Label_1,            Label_2,            Label_3,            Label_Mouse_1,      Label_Mouse_2,
    Label_Object_1,     Label_Object_2,     Label_Object_3,     Label_Object_4,     Label_Object_5,
    Label_Position,     Label_Center,       Label_Scale,        Label_Rotate,       Label_Z_Order,      Label_Pos_Flag,
    Label_Bottom,
};


//####################################################################################
//##    Toolbar Button Groups
//####################################################################################
enum class Buttons_Layering {
    Send_To_Back,
    Send_Backward,
    Send_To_Front,
    Send_Forward,
};

enum class Buttons_Reset {
    Reset_Object,
};













#endif // ENUMS_FORM_MAIN_H






