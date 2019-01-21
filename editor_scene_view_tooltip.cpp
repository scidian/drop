//
//      Created by Stephens Nunnally on 1/21/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//

#include "library.h"

#include "project.h"
#include "project_world.h"
#include "project_world_scene.h"
#include "project_world_scene_object.h"
#include "editor_scene_item.h"

#include "settings.h"
#include "settings_component.h"
#include "settings_component_property.h"

#include "editor_scene_scene.h"
#include "editor_scene_view.h"
#include "interface_relay.h"


//####################################################################################
//##        Constructor / Destructor
//####################################################################################
SceneViewToolTip::SceneViewToolTip(InterfaceRelay *relay, QWidget *parent) : QWidget(parent), m_relay(relay)
{
    this->setObjectName(QStringLiteral("formTooltip"));

    this->setWindowModality(Qt::WindowModality::NonModal);
    this->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    this->setWindowFlags(Qt::WindowType::ToolTip |
                         Qt::FramelessWindowHint |
                         Qt::WindowSystemMenuHint |
                         Qt::WindowStaysOnTopHint);
    this->setAcceptDrops(false);
    this->setMinimumSize(QSize(60, 20));
    this->resize(60, 20);

    Dr::WidgetMaskRoundedCorners(this, 25, 75);




}



void SceneViewToolTip::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawText( QPointF(0, 0), QString("Blah"));


}












