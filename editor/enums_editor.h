//
//      Created by Stephens Nunnally on 12/5/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef ENUMS_EDITOR_H
#define ENUMS_EDITOR_H

#include <QString>


//####################################################################################
//##    Custom QStrings for storing data in QWidgets using setProperty
//############################
namespace User_Property {
    const char Integer[7] =         "dr_int";               // Stores an Integer in the User Property of a widget

    const char Key[7] =             "dr_key";               // Stores Object Key in User Property of widget

    const char CompKey[8] =         "dr_comp";              // Stores Component Key of a Property in a widget
    const char PropKey[8] =         "dr_prop";              // Stores Property  Key of a Property in a widget

    const char Color[9] =           "dr_color";             // Stores a color value (QColor.rgba())
    const char Order[9] =           "dr_order";             // Used for properties with mulitple values (like Size has X and Y),
                                                            //      the index of the single property we want (0, 1, 2, etc)

    const char Header[10] =         "dr_header";            // Used for Advisor Text
    const char Body[8] =            "dr_body";              // Used for Advisor Text

    const char Mouse_Over[14] =     "dr_mouse_over";        // Set to true by DrFilterHoverHandler when mouse is over widget
    const char Mouse_Pos[13] =      "dr_mouse_pos";         // Set to mouse position (QPoint) by DrFilterHoverHandler when mouse is moving over widget
    const char Mouse_Down[14] =     "dr_mouse_down";        // Set to true when mouse is down so we can prepare for a drag event (used for DrAssets)
    const char Mouse_Down_Pos[18] = "dr_mouse_down_pos";    // Set to mouse position when mouse button is first pressed (used for DrAssets)
}


//####################################################################################
//##    Custom Qt::UserRole 's for storing data in QWidgets using setData
//############################
enum User_Roles {
    Key = 0x0100,                                           // Stores Object Key in User Data of widget / graphicsitem
                                                            // From qnamespace.h: Qt::UserRole, starting number for our own UserRoles
    CompKey,                                                // Stores Component Key
    PropKey,                                                // Stores Property  Key

    Scale,      Pre_Resize_Scale,
    Rotation,   Pre_Rotate_Rotation,
    Z_Order,
    Opacity,

    Name,
    Type,

    Toy,
    Color,
    Selected,
};



//####################################################################################
//##    Modes for Interface Editor Form
//############################
enum class Editor_Mode {
    Clear                       = 0,
    Program_Loading             = 1,

    World_Map                   = 2,                        // "Map"
    World_Editor                = 3,                        // "Editor" (Physics)
    UI_Editor                   = 4,                        // "UI"

    Animation_Editor            = 5,
    Stage_Map                   = 6,
    // App_Editor               = 7,
    // Puzzle_World_Editor      = 8,

};



//####################################################################################
//##    Different widgets in an Editor window, used for cross updating widgets
//############################
enum class Editor_Widgets {
    // Universal Widgets
    ToolBar,
    Asset_Tree,
    Inspector_Tree,
    Project_Tree,

    // "World Editor" Widgets
    Stage_View,

    // "World Map" Widgets
    Map_View,
};


//####################################################################################
//##    QGraphicsView Enumerations
//############################
enum class Handle_Shapes {
    Circles,
    Squares,
};
enum class X_Axis {
    Left,
    Right,
    None
};
enum class Y_Axis {
    Top,
    Bottom,
    None
};
enum class Grid_Style {
    Lines,
    Dots,
};


//####################################################################################
//##    Possible handle rects - Order used to be vector / array index critical, but it shouldn't be now...
//##                            Probably still best to leave Top thru Bottom_Right as values 0 thru 7
//############################
enum class Position_Flags {                                 // !!!!! Order is important
    Top                 =  0,
    Bottom              =  1,
    Right               =  2,
    Left                =  3,
    Top_Left            =  4,
    Bottom_Left         =  5,
    Top_Right           =  6,
    Bottom_Right        =  7,
    Center              =  8,
    Rotate              =  9,
    Move_Item           = 10,
    Over_Camera         = 11,
    Stage_Size          = 12,
    No_Position         = 13,                               // When adding positions, can be changed to stay last number
};


//####################################################################################
//##    Stage View Mouse Modes
//############################
enum class Mouse_Mode {
    None                = 0,
    Pointer             = 1,
    Hand                = 2,
    Magnify             = 3,
};

//####################################################################################
//##    Sub Stage View Mouse Mode - Mouse_Mode::Pointer
//############################
enum class View_Mode {
    None,
    Disable_Update,                 // Special mode to disallow updates
    Selecting,                      // Rubber band selection
    Resizing,                       // Changing items size
    Rotating,                       // Rotating items
    Translating,                    // Moving item(s) around
    Dragging,                       // Moving scene with space bar
    Zooming,                        // Zooming in / out of view
    Holding_Keys,                   // Holding arrow keys to move or layering keys
    Moving_Camera,                  // Rotating a camera around its target
    Resizing_Stage,                 // Resizing Stage bounds
};


// Form Main enum for referencing debug labels
enum class Label_Names {
    Label_1,            Label_2,            Label_3,            Label_Mouse_1,      Label_Mouse_2,
    Label_Object_1,     Label_Object_2,     Label_Object_3,     Label_Object_4,     Label_Object_5,
    Label_Position,     Label_Center,       Label_Scale,        Label_Rotate,       Label_Z_Order,      Label_Pos_Flag,
    Label_Bottom,
};


//####################################################################################
//##    Form_Main Toolbar Button Groups
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


//####################################################################################
//##    Form_Main View Toolbar Button Groups
//############################
enum class Buttons_Toggle {
    CameraOnOff,                    // Turns Camera Box drawing on / off
    DebugOnOff,                     // Turns Collision Shape drawing on / off
};


//####################################################################################
//##    Some public forward function declarations for some enum functions
//############################
namespace Dr {
    QString     StringFromEditorWidget(Editor_Widgets widget);
    QString     StringFromPositionFlag(Position_Flags flag);
    QString     StringFromViewMode(View_Mode mode);
}




#endif // ENUMS_EDITOR_H















