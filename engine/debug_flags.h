//
//      Created by Stephens Nunnally on 1/30/2020, (c) 2020 Scidian Studios, All Rights Reserved
//
//  File:
//      Handles debug flags for use in debugging / troubleshooting
//
//
#ifndef DEBUG_FLAGS_H
#define DEBUG_FLAGS_H


//####################################################################################
//##    Possible debug options
//############################
enum class Debug_Flags {

    // ***** ENGINE
    Render_Soft_Body_Shapes,            // When true, debug render of soft bodies shows all objects and joints, otherwise just soft body outline


    // ***** EDITOR
    Load_Test_Project,                  // Loads the Test Project into editor when program starts

    Show_Secret_Menu,                   // Add special menu to menu bar containing some hidden test functions
    Show_Undo_View,                     // Shows a window with all undo / redo commands from current EditorScene
    Show_Bottom_Debug_Labels,           // Shows QGraphicsScene World Editor View Debug labels
    Show_Hidden_Component,              // Shows hidden custom Entity Advisor component / properties

    Turn_On_Antialiasing_in_Editor,     // Turns on antialiasing in the World Editor GraphicsView
    Turn_On_OpenGL_in_Editor,           // Turns on opengl optimizations for World Editor GraphicsView

    Label_FPS,                          // Show frames per second of EditorView
    Label_Mouse_Coordinates,            // Show mouse position coordinates while moving over EditorView
    Label_Top_Item_RGBA,                // Show red, green, blue, alpha of pixel on item at top of mouse position

    Label_Inspector_Items,              // Shows which items are selected at top of Object Inspector
    Label_Inspector_Build,              // Show info about item sent to Inspector Tree
    Label_Stage_Tree_Drag,              // Show info about dragging event of Stage Tree

    Label_Rotation_Data,                // Show rotation data while in View_Mode::Rotating in EditorView
    Label_Selection_Group_Data,         // Show info about EditorScene current QGraphicsItemGroup of selected items
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


    // ***** Debug Functions
    bool CheckDebugFlag(Debug_Flags flag);
    void FlipDebugFlag(Debug_Flags flag);
    void SetDebugFlag(Debug_Flags flag);

    void LoadDebugFlags();


}   // namespace Dr


#endif // DEBUG_FLAGS_H


























