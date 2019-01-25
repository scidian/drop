//
//      Created by Stephens Nunnally on 1/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Tree Asset Definitions
//
//

#include "project.h"
#include "project_world.h"
#include "project_world_scene.h"
#include "project_world_scene_object.h"

#include "settings.h"
#include "settings_component.h"
#include "settings_component_property.h"

#include "editor_tree_assets.h"
#include "interface_relay.h"



//####################################################################################
//##        EVENT, Handles changing the Advisor on Mouse Enter
//####################################################################################
void TreeAssetList::enterEvent(QEvent *event)
{
    m_relay->setAdvisorInfo(Advisor_Info::Asset_List);
    QTreeWidget::enterEvent(event);
}

