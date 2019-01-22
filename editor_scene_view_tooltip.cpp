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
}


void SceneViewToolTip::startToolTip(View_Mode type, QPoint mouse_position, QVariant data)
{
    m_tip_type = type;
    this->setStyleSheet(" QWidget { background: qlineargradient(spread:pad, x1:0 y1:0, x2:0 y2:1, "
                        "                   stop:0 " + Dr::GetColor(Window_Colors::Button_Light).name() + ", "
                        "                   stop:1 " + Dr::GetColor(Window_Colors::Background_Light).name() + "); } " );
    clearMask();

    switch (m_tip_type)
    {
    case View_Mode::Resizing:
    case View_Mode::Translating:
        setFixedSize(65, 32);   m_offset = QPoint(30, -63);     Dr::ApplyRoundedCornerMask(this, 23, 50);   break;
    case View_Mode::Rotating:
    case View_Mode::Selecting:
    case View_Mode::Zooming:
    default:
        setFixedSize(60, 18);   m_offset = QPoint(30, -45);     Dr::ApplyRoundedCornerMask(this, 20, 75);   break;
    }
    Dr::ApplyDropShadowByType(this, Shadow_Types::Tool_Tip_Shadow);

    updateToolTipData(data);
    updateToolTipPosition(mouse_position);
    show();
}

void SceneViewToolTip::stopToolTip()
{
    hide();
    m_tip_type = View_Mode::None;
}


void SceneViewToolTip::updateToolTipData(QVariant data)
{
    switch (m_tip_type)
    {
    case View_Mode::Rotating:
        m_angle = data.toDouble();                              break;
    case View_Mode::Resizing:
    case View_Mode::Translating:
        m_x = data.toPointF().x(); m_y = data.toPointF().y();   break;
    case View_Mode::Selecting:
    case View_Mode::Zooming:
        m_int = data.toInt();                                   break;
    default: ;
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
    font.setPointSize ( 12 );
    painter.setFont(font);

    int w = this->geometry().width();
    int h = this->geometry().height();

    QString text_1 = "---";
    QString text_2 = "---";

    switch (m_tip_type)
    {
    case View_Mode::Rotating:
        text_1 = QString::number(m_angle, 'f', 1) + "°";
        painter.drawText( 0, 0, w, h, Qt::AlignmentFlag::AlignCenter, text_1);
        break;
    case View_Mode::Resizing:
        text_1 = "W: " + QString::number(m_x, 'f', 1);
        text_2 = "H: " + QString::number(m_y, 'f', 1);
        painter.drawText( 0,           0, w, h / 2, Qt::AlignmentFlag::AlignCenter, text_1);
        painter.drawText( 0, (h / 2) - 1, w, h / 2, Qt::AlignmentFlag::AlignCenter, text_2);
        break;
    case View_Mode::Translating:
        text_1 = "X: " + QString::number(m_x, 'f', 1);
        text_2 = "Y: " + QString::number(m_y, 'f', 1);
        painter.drawText( 0,           0, w, h / 2, Qt::AlignmentFlag::AlignCenter, text_1);
        painter.drawText( 0, (h / 2) - 1, w, h / 2, Qt::AlignmentFlag::AlignCenter, text_2);
        break;
    case View_Mode::Selecting:
        text_1 = "#: " + QString::number(m_int);
        painter.drawText( 0, 0, w, h, Qt::AlignmentFlag::AlignCenter, text_1);
        break;
    case View_Mode::Zooming:
        text_1 = QString::number(m_int) + "%";
        painter.drawText( 0, 0, w, h, Qt::AlignmentFlag::AlignCenter, text_1);
        break;
    default:
        painter.drawText( 0, 0, w, h, Qt::AlignmentFlag::AlignCenter, text_1);
    }


}












