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


enum class Options
{
    Form_Main_Mode,

    World_Editor_Current_World,
    World_Editor_Lock_Backgrounds,
    World_Editor_Show_Collision_Shapes,
    World_Editor_Show_Connections,
    World_Editor_Show_Game_Frame,
};


// External functions, can be called Globally without a local IEditorRelay reference in calling class
namespace Dr {

    void        LoadPreferences();

    QVariant    GetOption(Options option_to_get);
    void        SetOption(Options option_to_set, QVariant new_value);

    void        SetActiveEditorRelay(IEditorRelay *new_editor_relay);
    void        SetActiveFormMain(FormMain *new_form_main);
    void        SetLabelText(Label_Names label, QString text);

}



#endif // GLOBALS_H
