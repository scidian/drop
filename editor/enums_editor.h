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
//##    Modes for Interface Editor Form
//##        These modes are modes that use a QGraphicsView and interface with some
//##        combination of the Asset Tree, Advisor, Object Inspector, Toolbar, etc
//############################
enum class Editor_Mode {
    Clear                       = 0,
    Program_Loading             = 1,

    World_Graph                 = 2,                        // World Node Graph
    World_Creator               = 3,                        // 2D Physics World Editor
    UI_Creator                  = 4,                        // UI Editor
    Sound_Creator               = 5,                        // Sound Mixer

    Animation_Creator           = 6,                        // Animation Editor
    Component_Graph             = 7,                        // Visual Scripting

    // Stage_Graph              = ,

    // App_Creator              = ,
    // Puzzle_World_Creator     = ,
    // Tower_Defense_Creator    = ,
    // Card_Game_Creator        = ,
};



//####################################################################################
//##    Different widgets in an Editor window, used for cross updating widgets
//############################
enum class Editor_Widgets {
    // Global Widgets
    ToolBar,
    View,

    // Docks
    Advisor,
    Asset_Tree,
    Inspector_Tree,
    Project_Tree,
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
//##    World Editor View Mouse Modes
//############################
enum class Mouse_Mode {
    None                = 0,
    Pointer             = 1,
    Hand                = 2,
    Magnify             = 3,
};

//####################################################################################
//##    Sub World Editor View Mouse Mode - Mouse_Mode::Pointer
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
    Node_Connect,                   // For drawing of lines between node slots
};


// Form Main enum for referencing debug labels
enum class Label_Names {
    Label_1,            Label_2,            Label_3,            Label_Mouse_1,      Label_Mouse_2,
    Label_Object_1,     Label_Object_2,     Label_Object_3,     Label_Object_4,     Label_Object_5,
    Label_Position,     Label_Center,       Label_Scale,        Label_Rotate,       Label_Z_Order,      Label_Pos_Flag,
    Label_Bottom,
};


//####################################################################################
//##    Form_Main ToolBar Button Groups
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
//##    Form_Main View ToolBar Button Groups
//############################
enum class Buttons_Toggle {
    CameraOnOff,                    // Turns Camera Box drawing on / off
    DebugOnOff,                     // Turns Collision Shape drawing on / off
};



//####################################################################################
//##    Custom QStrings for storing data in QWidgets using setProperty
//############################
namespace User_Property {
    const char Integer[7] =         "dr_int";               // int      Stores an Integer in the User Property of a widget
    const char Key[7] =             "dr_key";               // long     Stores Object Key in User Property of widget

    const char External[12] =       "dr_external";          // bool     Set to true if DrEntity (DrImage) is in external image DrProject

    const char CompKey[12] =        "dr_comp_key";          // long     Stores Component Key (sub key assigned to unique Component from owning DrSettings entity)
    const char CompName[13] =       "dr_comp_name";         // QString  Stores Component Name of a Property in a widget
    const char PropKey[12] =        "dr_prop_key";          // long     Stores Property  Key (sub key assigned to unique Property from owning DrComponent)
    const char PropName[13] =       "dr_prop_name";         // QString  Stores Property  Name of a Property in a widget

    const char Size[8] =            "dr_size";              // QSize    Stores a size value
    const char Color[9] =           "dr_color";             // QColor   Stores a color value (QColor.rgba())
    const char Order[9] =           "dr_order";             // int      Used for properties with mulitple values (like Size has X and Y),
                                                            //              the index of the single property we want (0, 1, 2, etc)

    const char Header[10] =         "dr_header";            // QString  Used for Advisor Text
    const char Body[8] =            "dr_body";              // QString  Used for Advisor Text

    const char Mouse_Over[14] =     "dr_mouse_over";        // bool     Set to true by DrFilterHoverHandler when mouse is over widget
    const char Mouse_Pos[13] =      "dr_mouse_pos";         // QPoint   Set to mouse position (QPoint) by DrFilterHoverHandler when mouse is moving over widget
    const char Mouse_Down[14] =     "dr_mouse_down";        // bool     Set to true when mouse is down so we can prepare for a drag event (used for DrAssets)
    const char Mouse_Down_Pos[18] = "dr_mouse_down_pos";    // QPoint   Set to mouse position when mouse button is first pressed (used for DrAssets)
}


//####################################################################################
//##    Custom Qt::UserRole 's for storing data in QWidgets using setData
//############################
enum User_Roles {
    Key = 0x0100,                                           // long     Stores Object Key in User Data of Widget / QGraphicsItem
                                                            //              From qnamespace.h: Qt::UserRole, starting number for our own UserRoles
    CompKey,                                                // long     Stores Component Key
    CompName,                                               // QString  Stores Component Name
    PropKey,                                                // long     Stores Property  Key
    PropName,                                               // QString  Stores Property  Name

    Scale,                                                  // QPointF  Stores an items scale
    Pre_Resize_Scale,                                       // QPointF  Stores an items scale at the start of a transformation (usually by Mouse_Mode)
    Rotation,                                               // double   Stores an items rotation
    Pre_Rotate_Rotation,                                    // double   Stores an items rotation at the start of a transformation (usually by Mouse_Mode)
    Z_Order,                                                // double   Stores an items z order
    Opacity,                                                // double   Opacity stored between 0.0 and 100.0

    Name,                                                   // QString  Stores an items name
    Category_Name,                                          // QString
    Description,                                            // QString
    Type,                                                   // int      static_cast from DrType
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



















