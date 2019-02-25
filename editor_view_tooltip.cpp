//
//      Created by Stephens Nunnally on 1/21/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "colors.h"
#include "debug.h"

#include "editor_item.h"
#include "editor_scene.h"
#include "editor_view.h"

#include "interface_relay.h"
#include "library.h"

#include "project.h"
#include "project_world.h"
#include "project_world_stage.h"
#include "project_world_stage_object.h"
#include "settings.h"
#include "settings_component.h"
#include "settings_component_property.h"



//####################################################################################
//##        Constructor / Destructor
//####################################################################################
DrViewToolTip::DrViewToolTip(QWidget *parent) : QWidget(parent)
{
    this->setObjectName(QStringLiteral("formTooltip"));

    this->setWindowModality(Qt::WindowModality::NonModal);
    this->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowStaysOnTopHint);
    this->setAcceptDrops(false);
}


void DrViewToolTip::startToolTip(View_Mode type, QPoint mouse_position, QVariant data)
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
        setFixedSize(66, 32);   m_offset = QPoint(30, -63);
        m_x_radius = 23;
        m_y_radius = 50;
        break;
    case View_Mode::Rotating:
    case View_Mode::Zooming:
    default:
        setFixedSize(60, 18);   m_offset = QPoint(30, -45);
        m_x_radius = 20;
        m_y_radius = 75;
        break;
    }

    if (Dr::CheckDebugFlag(Debug_Flags::Turn_On_OpenGL) == false) {
        Dr::ApplyRoundedCornerMask(this, m_x_radius, m_y_radius);
        Dr::ApplyDropShadowByType(this, Shadow_Types::Tool_Tip_Shadow);
    }

    updateToolTipData(data);
    updateToolTipPosition(mouse_position);
    show();
}

void DrViewToolTip::stopToolTip()
{
    hide();
    m_tip_type = View_Mode::None;
}


void DrViewToolTip::updateToolTipData(QVariant data)
{
    switch (m_tip_type)
    {
    case View_Mode::Rotating:
        m_angle = data.toDouble();                              break;
    case View_Mode::Resizing:
    case View_Mode::Translating:
        m_x = data.toPointF().x(); m_y = data.toPointF().y();   break;
    case View_Mode::Zooming:
        m_int = data.toInt();                                   break;
    default: ;
    }
}

void DrViewToolTip::updateToolTipPosition(QPoint mouse_position)
{
    move(mouse_position + getOffset());
}


void DrViewToolTip::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    drawText(painter);
}


void DrViewToolTip::drawText(QPainter &painter, int left_offset, int top_offset)
{
    QFont font = painter.font();
    font.setPointSize ( Dr::FontSize() + 1 );
    painter.setFont(font);
    painter.setPen(Dr::GetColor(Window_Colors::Text_Light));

    int w = this->geometry().width();
    int h = this->geometry().height();
    int left;
    QPixmap  pic(w * 2, h);
    QPainter paint_pic(&pic);
    QRect    bounding_1, bounding_2;
    QString  text_1, text_2;

    switch (m_tip_type)
    {
    case View_Mode::Zooming:     text_1 = QString::number(m_int) + "%";             break;
    case View_Mode::Rotating:    text_1 = QString::number(m_angle, 'f', 1) + "°";   break;
    case View_Mode::Resizing:    text_1 = "W: " + QString::number(m_x, 'f', 1);     text_2 = "H: " + QString::number(m_y, 'f', 1);  break;
    case View_Mode::Translating: text_1 = "X: " + QString::number(m_x, 'f', 1);     text_2 = "Y: " + QString::number(m_y, 'f', 1);  break;
    default:                     text_1 = "---";                                    text_2 = "---";
    }

    switch (m_tip_type)
    {
    case View_Mode::Resizing:
    case View_Mode::Translating:
        ///// Simple text centering
        ///painter.drawText( 0,           0, w, h / 2, Qt::AlignmentFlag::AlignCenter, text_1);
        ///painter.drawText( 0, (h / 2) - 1, w, h / 2, Qt::AlignmentFlag::AlignCenter, text_2);

        // Draw text off screen to calculate proper starting x coord
        paint_pic.drawText( 0, 0, w, h, Qt::AlignmentFlag::AlignLeft, text_1, &bounding_1);
        paint_pic.drawText( 0, 0, w, h, Qt::AlignmentFlag::AlignLeft, text_2, &bounding_2);
        left = (bounding_1.width() > bounding_2.width()) ? (w - bounding_1.width() + 4) / 2 : (w - bounding_2.width() + 4) / 2;
        painter.drawText( left_offset + left,     top_offset,               w, h / 2, Qt::AlignmentFlag::AlignVCenter, text_1);
        painter.drawText( left_offset + left + 1, top_offset + (h / 2) - 1, w, h / 2, Qt::AlignmentFlag::AlignVCenter, text_2);

        break;
    case View_Mode::Rotating:
    case View_Mode::Zooming:
    default:
        painter.drawText( left_offset, top_offset, w, h, Qt::AlignmentFlag::AlignCenter, text_1);
    }
}









