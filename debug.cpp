//
//      Created by Stephens Nunnally on 1/21/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Handles debug flags for use in debugging / troubleshooting
//
//
#include <bitset>

#include "debug.h"

namespace Dr {

//####################################################################################
//##    Local Static Variables
//####################################################################################
static std::bitset<static_cast<int>(Debug_Flags::Total)>   g_debug_flags;
static bool                                                done_loading = false;    // True after initial startup of FormMain,
                                                                                    // makes sure done loading before any calls to SetLabelText

//####################################################################################
//##    Functions to initialize, set and check flags
//####################################################################################
bool CheckDoneLoading() { return done_loading; }
void SetDoneLoading(bool done) { done_loading = done; }

bool CheckDebugFlag(Debug_Flags flag) {
    return g_debug_flags.test(static_cast<size_t>(flag));
}

void FlipDebugFlag(Debug_Flags flag) {
    g_debug_flags.flip(static_cast<size_t>(flag));
}

void SetDebugFlag(Debug_Flags flag) {
    g_debug_flags.set(static_cast<size_t>(flag));
}

void LoadDebugFlags() {
    ///SetDebugFlag( Debug_Flags::Show_Secret_Menu );

    //SetDebugFlag( Debug_Flags::Show_Undo_View );
    //SetDebugFlag( Debug_Flags::Show_Bottom_Debug_Labels );
    //SetDebugFlag( Debug_Flags::Show_Hidden_Component );

    SetDebugFlag( Debug_Flags::Turn_On_Antialiasing );
    //SetDebugFlag( Debug_Flags::Turn_On_OpenGL_in_Editor );

    SetDebugFlag( Debug_Flags::Label_FPS );
    SetDebugFlag( Debug_Flags::Label_Mouse_Coordinates );
    //SetDebugFlag( Debug_Flags::Label_Top_Item_RGBA);

    SetDebugFlag( Debug_Flags::Label_Inspector_Build );
    SetDebugFlag( Debug_Flags::Label_Stage_Tree_Drag );

    //SetDebugFlag( Debug_Flags::Label_Rotation_Data );
    SetDebugFlag( Debug_Flags::Label_Selection_Group_Data );
    SetDebugFlag( Debug_Flags::Label_Selected_Item_Data );
    //SetDebugFlag( Debug_Flags::Label_Where_Update_Box_From );

    //SetDebugFlag( Debug_Flags::Paint_Rotating_Angles );
    //SetDebugFlag( Debug_Flags::Paint_Size_Grip_Handles );
}


}   // namespace Dr











