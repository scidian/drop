//
//      Created by Stephens Nunnally on 1/21/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Handles debug flags for use in debugging / troubleshooting
//
//
#ifndef DEBUG_H
#define DEBUG_H


//####################################################################################
//##    Possible debug options
//############################
enum class Debug_Flags {
    Show_Secret_Menu,                   // Add special menu to menu bar containing some hidden test functions
    Show_Undo_View,                     // Shows a window with all undo / redo commands from current DrScene
    Show_Bottom_Debug_Labels,           // Shows Scene View Debug labels
    Show_Hidden_Component,              // Shows hidden custom Entity Advisor component / properties

    Turn_On_Antialiasing,               // Turns on antialiasing in the World Editor GraphicsView
    Turn_On_OpenGL,                     // Turns on opengl optimizations for World Editor GraphicsView

    Label_FPS,                          // Show frames per second of DrView
    Label_Mouse_Coordinates,            // Show mouse position coordinates while moving over DrView
    Label_Top_Item_RGBA,                // Show red, green, blue, alpha of pixel on item at top of mouse position

    Label_Inspector_Build,              // Show info about item sent to Inspector Tree
    Label_Stage_Tree_Drag,              // Show info about dragging event of Stage Tree

    Label_Rotation_Data,                // Show rotation data while in View_Mode::Rotating in DrView
    Label_Selection_Group_Data,         // Show info about DrScene current QGraphicsItemGroup of selected items
    Label_Selected_Item_Data,           // Show info about first item in Selection Group
    Label_Where_Update_Box_From,        // Shows where the call to the UpdateSelectionBoxData function originated from

    Paint_Rotating_Angles,              // Paints mouse origin and mouse position lines during Rotating
    Paint_Size_Grip_Handles,            // Paints rects of resize grip handles
    Total,
};


//####################################################################################
//##    Debug Function Declarations
//############################
namespace Dr {

    bool CheckDebugFlag(Debug_Flags flag);
    void FlipDebugFlag(Debug_Flags flag);
    void SetDebugFlag(Debug_Flags flag);

    bool CheckDoneLoading();
    void SetDoneLoading(bool done);

    void InitializeFlags();

}   // namespace Dr


#endif // DEBUG_H












