//
//      Created by Stephens Nunnally on 1/21/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Handles debug flags for use in debugging / troubleshooting
//
//

#include "debug.h"

namespace Dr {

//####################################################################################
//##        Local Static Variables
//####################################################################################
static std::bitset<static_cast<int>(Debug_Flags::Total)>   g_debug_flags;


//####################################################################################
//##        Functions to initialize, set and check flags
//####################################################################################
bool CheckDebugFlag(Debug_Flags flag)
{
    return g_debug_flags.test(static_cast<size_t>(flag));
}

void SetDebugFlag(Debug_Flags flag)
{
    g_debug_flags.set(static_cast<size_t>(flag));
}

void InitializeFlags()
{
    SetDebugFlag( Debug_Flags::Show_Secret_Menu );
    SetDebugFlag( Debug_Flags::Show_Undo_View );

    SetDebugFlag( Debug_Flags::Turn_Off_Antialiasing );

    SetDebugFlag( Debug_Flags::Label_FPS );
    SetDebugFlag( Debug_Flags::Label_Mouse_Coordinates );
    //SetDebugFlag( Debug_Flags::Label_Top_Item_RGBA);

    SetDebugFlag( Debug_Flags::Label_Object_Inspector_Build );
    SetDebugFlag( Debug_Flags::Label_Scene_Tree_Drag );
    SetDebugFlag( Debug_Flags::Label_Scene_Tree_Selection );
    //SetDebugFlag( Debug_Flags::Label_Selection_Change_Scene_Tree );

    //SetDebugFlag( Debug_Flags::Paint_Rotating_Angles );
    //SetDebugFlag( Debug_Flags::Label_Rotation_Data );
    SetDebugFlag( Debug_Flags::Label_Selection_Group_Data );
    SetDebugFlag( Debug_Flags::Label_Selected_Item_Data );
    //SetDebugFlag( Debug_Flags::Paint_Shear_Matrix );
    //SetDebugFlag( Debug_Flags::Paint_Size_Grip_Handles );
}


}





