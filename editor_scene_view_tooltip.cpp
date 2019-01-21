//
//      Created by Stephens Nunnally on 1/21/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//


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
SceneViewToolTip::SceneViewToolTip(QWidget *parent) : QWidget (parent)
{
    this->setObjectName(QStringLiteral("formTooltip"));
    this->setWindowModality(Qt::WindowModality::NonModal);
    this->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    this->setWindowFlag(Qt::WindowType::ToolTip);

    //QRegion region()

    QBitmap bitmap(100, 80);

    //QPixmap pix(500,500);

    QPainter *paint = new QPainter(&bitmap);
    paint->setPen(QColor(255,34,255,255));
    paint->drawRect(15,15,100,100);
    delete paint;

    //this->setMask(region);

    this->resize(100, 80);

    //this->setMinimumSize(QSize(100, 80));
    //this->setMouseTracking(false);
    //this->setAcceptDrops(false);
}



void SceneViewToolTip::paintEvent(QPaintEvent *)
{

    QPainter painter;

    painter.drawText( QPointF(0, 0), QString("Blah"));

}














