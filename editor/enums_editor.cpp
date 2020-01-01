//
//      Created by Stephens Nunnally on 12/5/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "editor/enums_editor.h"


namespace Dr {


//####################################################################################
//##    Returns a QString respresentaiton of the passed Editor Enum
//####################################################################################
QString StringFromEditorWidget(Editor_Widgets widget) {
    switch (widget) {
        case Editor_Widgets::ToolBar:           return "Toolbar";
        case Editor_Widgets::Asset_Tree:        return "Asset Tree";
        case Editor_Widgets::Stage_View:        return "Stage View";
        case Editor_Widgets::Project_Tree:      return "Project Tree";
        case Editor_Widgets::Inspector_Tree:    return "Inspector Tree";
    }
}

QString StringFromPositionFlag(Position_Flags flag) {
    switch (flag) {
        case Position_Flags::Top:           return "Top";
        case Position_Flags::Top_Right:     return "Top Right";
        case Position_Flags::Top_Left:      return "Top Left";
        case Position_Flags::Bottom:        return "Bottom";
        case Position_Flags::Bottom_Left:   return "Bottom Left";
        case Position_Flags::Bottom_Right:  return "Bottom Right";
        case Position_Flags::Left:          return "Left";
        case Position_Flags::Right:         return "Right";
        case Position_Flags::Center:        return "Center";
        case Position_Flags::Rotate:        return "Rotate";
        case Position_Flags::Move_Item:     return "Move Item";
        case Position_Flags::No_Position:   return "No Position";
    }
}




}   // End namespace Dr









