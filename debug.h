//
//      Created by Stephens Nunnally on 1/21/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Handles debug flags for use in debugging / troubleshooting
//
//
#ifndef DEBUG_H
#define DEBUG_H


#include <bitset>


//####################################################################################
//##    Possible debug options
//############################
enum class Debug_Flags {
    Show_Secret_Menu,                   // Add special menu to menu bar containing some hidden test functions
    Show_Undo_View,                     // Shows a window with all undo / redo commands from current StageGraphicsScene

    Turn_Off_Antialiasing,              // Turns off antialiasing in the GraphicsView

    Label_FPS,                          // Show frames per second of StageGraphicsView
    Label_Mouse_Coordinates,            // Show mouse position coordinates while moving over StageGraphicsView
    Label_Top_Item_RGBA,                // Show red, green, blue, alpha of pixel on item at top of mouse position

    Label_Object_Inspector_Build,       // Show info about item sent to Object Inspector Tree
    Label_Stage_Tree_Drag,              // Show info about dragging event of Stage Tree
    Label_Stage_Tree_Selection,         // Show info about item selected in Stage Tree
    Label_Selection_Change_Stage_Tree,  // Show info about how many items in the Stage Tree matched the items selected in the GraphicsScene

    Label_Rotation_Data,                // Show rotation data while in View_Mode::Rotating in StageGraphicsView
    Label_Selection_Group_Data,         // Show info about StageGraphicsScene current QGraphicsItemGroup of selected items
    Label_Selected_Item_Data,           // Show info about first item in Selection Group
    Paint_Shear_Matrix,                 // Paints un sheared / unrotated boxes and shows matrix data for selected item's transform()
    Paint_Rotating_Angles,              // Paints mouse origin and mouse position lines during Rotating
    Paint_Size_Grip_Handles,            // Paints rects of resize grip handles
    Total,
};


namespace Dr {

    bool CheckDebugFlag(Debug_Flags flag);
    void SetDebugFlag(Debug_Flags flag);

    void InitializeFlags();

}

#endif // DEBUG_H
