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
    Label_FPS,                          // Show frames per second of SceneGraphicsView
    Label_Mouse_Coordinates,            // Show mouse position coordinates while moving over SceneGraphicsView

    Label_Object_Inspector_Build,       // Show info about item sent to Object Inspector Tree
    Label_Scene_Tree_Drag,              // Show info about dragging event of Scene Tree
    Label_Scene_Tree_Selection,         // Show info about item selected in Scene Tree

    Label_Rotation_Data,                // Show rotation data while in View_Mode::Rotating in SceneGraphicsView
    Label_Selection_Group_Data,         // Show info about SceneGraphicScene current QGraphicsItemGroup of selected items
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
