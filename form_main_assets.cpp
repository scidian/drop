//
//      Created by Stephens Nunnally on 12/7/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Handles dealing with the Asset List
//
//

#include "01_project.h"
#include "02_world.h"
#include "03_scene.h"
#include "04_object.h"

#include "30_settings.h"
#include "31_component.h"
#include "32_property.h"

#include "form_main.h"


// Handles changing the Advisor on Mouse Enter
void TreeAssetList::enterEvent(QEvent *event)
{
    getMainWindow()->setAdvisorInfo(Advisor_Info::Asset_List);
    QTreeWidget::enterEvent(event);
}



