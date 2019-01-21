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
static std::bitset<static_cast<int>(Debug_Flags::Total)>   debug_flags;


//####################################################################################
//##        Functions to initialize, set and check flags
//####################################################################################
bool CheckDebugFlag(Debug_Flags flag)
{
    return debug_flags.test(static_cast<size_t>(flag));
}

void SetDebugFlag(Debug_Flags flag)
{
    debug_flags.set(static_cast<size_t>(flag));
}

void InitializeFlags()
{
    SetDebugFlag( Debug_Flags::Secret_Menu );
    //SetDebugFlag( Debug_Flags::FPS );
    SetDebugFlag( Debug_Flags::Mouse_Coordinates );

    SetDebugFlag( Debug_Flags::Object_Inspector_Build );
    SetDebugFlag( Debug_Flags::Scene_Tree_Drag );
    SetDebugFlag( Debug_Flags::Scene_Tree_Selection );

    //SetDebugFlag( Debug_Flags::Rotation_Data );
    SetDebugFlag( Debug_Flags::Selection_Box_Group_Data );
    SetDebugFlag( Debug_Flags::Selected_Item_Data );
    //SetDebugFlag( Debug_Flags::Shear_Matrix );
}


}





