//
//      Created by Stephens Nunnally on 3/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#ifndef GLOBALS_H
#define GLOBALS_H

#include "enums.h"
#include "enums_form_main.h"

// Forward declarations
class DrProject;
class FormMain;
class IEditorRelay;

//####################################################################################
//##    Program wide options
//############################
enum class Preferences
{

    World_Editor_Lock_Backgrounds,                  // bool
    World_Editor_Show_Collision_Shapes,             // bool
    World_Editor_Show_Connections,                  // bool
    World_Editor_Show_Game_Frame,                   // bool

    World_Editor_Snap_To_Grid,                      // bool
    World_Editor_Resize_To_Grid,                    // bool
    World_Editor_Grid_On_Top,                       // bool

    World_Editor_Snap_To_Center_Of_Selection_Box,   // bool
};


//####################################################################################
//##    Global function declarations
//############################
namespace Dr {

    void        LoadPreferences();

    QVariant    GetPreference(Preferences preference_to_get);
    void        SetPreference(Preferences preference_to_set, QVariant new_value);

    void        SetActiveEditorRelay(IEditorRelay *new_editor_relay);
    void        SetActiveFormMain(FormMain *new_form_main);

    void        SetLabelText(Label_Names label, QString text);

    void        ClearCursor();
    void        HideCursor();
    void        SetCursor(QCursor &cursor);

}   // namespace Dr


#endif // GLOBALS_H
















