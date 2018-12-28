//
//
//      Handles dealing with the Advisor
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
void TreeAdvisorList::enterEvent(QEvent *event)
{
    getMainWindow()->setAdvisorInfo(Advisor_Info::Advisor_Window);
    QTreeWidget::enterEvent(event);
}
