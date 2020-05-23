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
        case Editor_Widgets::ToolBar:           return "ToolBar";
        case Editor_Widgets::View:              return "View";
        case Editor_Widgets::Advisor:           return "Advisor";
        case Editor_Widgets::Asset_Tree:        return "Asset Tree";
        case Editor_Widgets::Inspector_Tree:    return "Inspector Tree";
        case Editor_Widgets::Project_Tree:      return "Project Tree";
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
        case Position_Flags::Over_Camera:   return "Over Camera";
        case Position_Flags::Stage_Size:    return "Stage Resize";
        case Position_Flags::No_Position:   return "No Position";
    }
}

QString StringFromViewMode(View_Mode mode) {
    switch (mode) {
        case View_Mode::None:               return "None";
        case View_Mode::Disable_Update:     return "Disable Updates";
        case View_Mode::Selecting:          return "Selecting";
        case View_Mode::Resizing:           return "Resizing";
        case View_Mode::Rotating:           return "Rotating";
        case View_Mode::Translating:        return "Translating";
        case View_Mode::Dragging:           return "Dragging";
        case View_Mode::Zooming:            return "Zooming";
        case View_Mode::Holding_Keys:       return "Holding Keys";
        case View_Mode::Moving_Camera:      return "Moving Camera";
        case View_Mode::Resizing_Stage:     return "Resizing Stage";
        case View_Mode::Node_Connect:       return "Connecting_Node";
    }
    return "Mode not handled...";
}



}   // End namespace Dr









