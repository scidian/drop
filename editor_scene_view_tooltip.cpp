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
SceneViewToolTip::SceneViewToolTip(QWidget *parent) : QWidget(parent)
{
    this->setObjectName(QStringLiteral("formTooltip"));

    this->setWindowModality(Qt::WindowModality::NonModal);
    this->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowStaysOnTopHint);
    this->setAcceptDrops(false);
    this->setMinimumSize(QSize(60, 20));
    this->resize(60, 20);

    Dr::ApplyRoundedCornerMask(this, 25, 75);

    Dr::ApplyDropShadowByType(this, Shadow_Types::Button_Shadow);
}


void SceneViewToolTip::setupToolTip(View_Mode type, QPoint mouse_position, QVariant data)
{
    this->setStyleSheet(" QWidget { background: qlineargradient(spread:pad, x1:0 y1:0, x2:0 y2:1, "
                        "                   stop:0 " + Dr::GetColor(Window_Colors::Button_Light).name() + ", "
                        "                   stop:1 " + Dr::GetColor(Window_Colors::Background_Light).name() + "); } " );
    setToolTipType(type);
    updateToolTipData(data);
    updateToolTipPosition(mouse_position);
    show();
}


void SceneViewToolTip::updateToolTipData(QVariant data)
{
    switch (m_tip_type)
    {
    case View_Mode::Rotating:   m_angle = data.toDouble();
    }
}

void SceneViewToolTip::updateToolTipPosition(QPoint mouse_position)
{
    move(mouse_position + getOffset());
}


void SceneViewToolTip::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(Dr::GetColor(Window_Colors::Text_Light));

    QFont font = painter.font();
    font.setPointSize ( 14 );
    painter.setFont(font);

    int w = this->geometry().width();
    int h = this->geometry().height();

    QString text = "---";

    switch (m_tip_type)
    {
    case View_Mode::Rotating:   text = QString::number(m_angle, 'f', 1) + "°";  break;


    }
    painter.drawText( 0, 0, w, h, Qt::AlignmentFlag::AlignCenter, text);

}












