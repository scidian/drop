//
//      Created by Stephens Nunnally on 1/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Tree Asset Definitions
//
//

#include "01_project.h"
#include "02_world.h"
#include "03_scene.h"
#include "04_object.h"

#include "30_settings.h"
#include "31_component.h"
#include "32_property.h"

#include "editor_tree_assets.h"
#include "interface_relay.h"



// Handles changing the Advisor on Mouse Enter
void TreeAssetList::enterEvent(QEvent *event)
{
    m_interface->setAdvisorInfo(Advisor_Info::Asset_List);
    QTreeWidget::enterEvent(event);
}

