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


// ***** Possible debug options
enum class Debug_Flags {
    Secret_Menu,                    // Add special menu to menu bar containing some hidden test functions
    FPS,                            // Show frames per second of SceneGraphicsView
    Mouse_Coordinates,              // Show mouse position coordinates while moving over SceneGraphicsView

    Object_Inspector_Build,         // Show info about item sent to Object Inspector Tree
    Scene_Tree_Drag,                // Show info about dragging event of Scene Tree
    Scene_Tree_Selection,           // Show info about item selected in Scene Tree

    Rotation_Data,                  // Show rotation data while in View_Mode::Rotating in SceneGraphicsView
    Selection_Box_Group_Data,       // Show info about SceneGraphicScene current QGraphicsItemGroup of selected items
    Selected_Item_Data,             // Show info about first item in Selection Group
    Shear_Matrix,                   // Show matrix for selected item's transform()
    Total,
};


namespace Dr {

    bool CheckDebugFlag(Debug_Flags flag);
    void SetDebugFlag(Debug_Flags flag);

    void InitializeFlags();

}

#endif // DEBUG_H
