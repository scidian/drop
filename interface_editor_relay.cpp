//
//      Created by Stephens Nunnally on 1/21/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      An interface class, used to transfer messages, etc around program between widgets / classes
//
//
#include "editor/tree_assets.h"

#include "globals.h"
#include "interface_editor_relay.h"


//####################################################################################
//##    Interface Destructors - Must include definition of a virtual destructor
//####################################################################################
IEditorRelay::~IEditorRelay() { }


// Updates Debug Labels with Active Widget
void IEditorRelay::setActiveWidget(Editor_Widgets widget) {
    m_active_widget = widget;
    getAssetTree()->update();
    Dr::SetLabelText(Label_Names::Label_2, "Active Widget: " + Dr::StringFromEditorWidget(widget));
}








