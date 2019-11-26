//
//      Created by Stephens Nunnally on 11/25/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "docks/docks.h"
#include "editor_view/editor_scene.h"
#include "editor_view/editor_view.h"
#include "forms/form_animation.h"
#include "project/project.h"
#include "style/style.h"


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







