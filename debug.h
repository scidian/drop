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
    Show_Undo_View,                     // Shows a window with all undo / redo commands from current DrScene

    Turn_Off_Antialiasing,              // Turns off antialiasing in the GraphicsView

    Label_FPS,                          // Show frames per second of DrView
    Label_Mouse_Coordinates,            // Show mouse position coordinates while moving over DrView
    Label_Top_Item_RGBA,                // Show red, green, blue, alpha of pixel on item at top of mouse position

    Label_Object_Inspector_Build,       // Show info about item sent to Object Inspector Tree
    Label_Stage_Tree_Drag,              // Show info about dragging event of Stage Tree
    Label_Stage_Tree_Selection,         // Show info about item selected in Stage Tree
    Label_Selection_Change_Stage_Tree,  // Show info about how many items in the Stage Tree matched the items selected in the GraphicsScene

    Label_Rotation_Data,                // Show rotation data while in View_Mode::Rotating in DrView
    Label_Selection_Group_Data,         // Show info about DrScene current QGraphicsItemGroup of selected items
    Label_Selected_Item_Data,           // Show info about first item in Selection Group
    Label_Where_Update_Box_From,        // Shows where the call to the UpdateSelectionBoxData function originated from

    Paint_Resize_Calculations,          // Paints unrotated selection box with distance point used for calculating scale
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
