//
//      Created by Stephens Nunnally on 5/27/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "editor/docks/docks.h"
#include "editor/form_animation/form_animation.h"
#include "editor/style/style.h"
#include "editor/view_editor/editor_scene.h"
#include "editor/view_editor/editor_view.h"
#include "project/dr_project.h"


//####################################################################################
//##    Constructor / Destructor for Animation Editor Window
//####################################################################################
FormAnimation::FormAnimation(DrProject *project, long animation_key, QWidget *parent) : QMainWindow(parent) {
    m_project = project;
    m_animation_key = animation_key;

    // ***** Initialize FormAnimation
    this->setStyleSheet( Dr::CustomStyleSheetFormatting() );
    initializeFormAnimation();
}

FormAnimation::~FormAnimation() { }





