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

class FormMain;
class IEditorRelay;

enum class Preferences
{
    Form_Main_Mode,                         // int (Form_Main_Mode)


    World_Editor_Lock_Backgrounds,          // bool
    World_Editor_Show_Collision_Shapes,     // bool
    World_Editor_Show_Connections,          // bool
    World_Editor_Show_Game_Frame,           // bool
};


// External functions, can be called Globally without a local IEditorRelay reference in calling class
namespace Dr {

    void        LoadPreferences();

    QVariant    GetPreference(Preferences preference_to_get);
    void        SetPreference(Preferences preference_to_set, QVariant new_value);

    void        SetActiveEditorRelay(IEditorRelay *new_editor_relay);
    void        SetActiveFormMain(FormMain *new_form_main);
    void        SetLabelText(Label_Names label, QString text);

}


#endif // GLOBALS_H

