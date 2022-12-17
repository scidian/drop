//
//      Created by Stephens Nunnally on 1/21/2019, (c) 2019 Scidian Studios, All Rights Reserved
//
//  File:
//      An interface class, used to transfer messages, etc around program between widgets / classes
//
//
#include "editor/interface_editor_relay.h"
#include "editor/preferences.h"
#include "editor/trees/tree_assets.h"


//####################################################################################
//##    Interface Destructors - Must include definition of a virtual destructor
//####################################################################################
IEditorRelay::~IEditorRelay() { }


//####################################################################################
//##    Setter for Active Widget
//##        Updates Debug Labels with Active Widget
//####################################################################################
void IEditorRelay::setActiveWidget(Editor_Widgets widget) {
    m_active_widget = widget;
    getAssetTree()->update();
    Dr::SetLabelText(Label_Names::Label_2, "Active Widget: " + Dr::StringFromEditorWidget(widget));
}


//####################################################################################
//##    Get Dock by Editor_Widget if exists
//####################################################################################
QDockWidget* IEditorRelay::getDock(Editor_Widgets editor_widget) {
    auto it = m_dock_widigets.find(editor_widget);
    if (it == m_dock_widigets.end()) return nullptr; else return it->second;
}





