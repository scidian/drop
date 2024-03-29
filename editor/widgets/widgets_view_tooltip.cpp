//
//      Created by Stephens Nunnally on 5/22/2020, (c) 2020 Scidian Studios, All Rights Reserved
//
//  File:
//
//
//
#include "core/colors/colors.h"
#include "editor/helper_library.h"
#include "editor/interface_editor_relay.h"
#include "editor/style/style.h"
#include "editor/view_editor/editor_item.h"
#include "editor/view_editor/editor_scene.h"
#include "editor/view_editor/editor_view.h"
#include "editor/widgets/widgets_view.h"
#include "engine/debug_flags.h"
#include "project/dr_project.h"
#include "project/entities/dr_world.h"
#include "project/entities/dr_stage.h"
#include "project/entities/dr_thing.h"
#include "project/settings/settings.h"
#include "project/settings/settings_component.h"
#include "project/settings/settings_component_property.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
ViewToolTip::ViewToolTip(QWidget *parent) : QWidget(parent) {
    this->setObjectName(QStringLiteral("formTooltip"));

    this->setWindowModality(Qt::WindowModality::NonModal);
    this->setFocusPolicy(Qt::FocusPolicy::NoFocus);

    this->setWindowFlag(Qt::WindowType::FramelessWindowHint);
    this->setWindowFlag(Qt::WindowType::WindowSystemMenuHint);
    this->setWindowFlag(Qt::WindowType::WindowStaysOnTopHint);

    //this->setWindowFlag(Qt::WindowType::

    this->setAcceptDrops(false);
}


void ViewToolTip::startToolTip(View_Mode type, QPoint mouse_position, QVariant data) {
    m_tip_type = type;
    this->setStyleSheet(QString::fromStdString(
                        " QWidget { background: qlineargradient(spread:pad, x1:0 y1:0, x2:0 y2:1, "
                        "                   stop:0 " + Dr::GetColor(Window_Colors::Button_Light).name() + ", "
                        "                   stop:1 " + Dr::GetColor(Window_Colors::Background_Light).name() + "); } " ));
    clearMask();

    switch (m_tip_type) {
        case View_Mode::Resizing:
        case View_Mode::Translating:
            setFixedSize(Dr::Scale(66), Dr::Scale(32));   m_offset = QPoint(30, -63);
            m_x_radius = 8;
            m_y_radius = 8;
            break;
        case View_Mode::Rotating:
        case View_Mode::Zooming:
        default:
            setFixedSize(Dr::Scale(60), Dr::Scale(18));   m_offset = QPoint(30, -45);
            m_x_radius = 8;
            m_y_radius = 8;
            break;
    }

    if (Dr::CheckDebugFlag(Debug_Flags::Turn_On_OpenGL_in_Editor) == false) {
        Dr::ApplyRoundedCornerMask(this, m_x_radius, m_y_radius);
        Dr::ApplyDropShadowByType(this, Shadow_Types::Tool_Tip_Shadow);
    }

    updateToolTipData(data);
    updateToolTipPosition(mouse_position);
    show();
}

void ViewToolTip::stopToolTip() {
    hide();
    m_tip_type = View_Mode::None;
}


void ViewToolTip::updateToolTipData(QVariant data) {
    switch (m_tip_type) {
        case View_Mode::Rotating:       m_angle = data.toDouble();                                          break;
        case View_Mode::Resizing:       m_x = data.toPointF().x();      m_y = data.toPointF().y();          break;
        case View_Mode::Translating:    m_x = data.toPointF().x();      m_y = data.toPointF().y() * -1;     break;
        case View_Mode::Zooming:        m_int = data.toInt();                                               break;
        default: ;
    }
}

void ViewToolTip::updateToolTipPosition(QPoint mouse_position) {
    move(mouse_position + getOffset());
}


void ViewToolTip::paintEvent(QPaintEvent *) {
    QPainter painter(this);

    drawText(painter);
}


void ViewToolTip::drawText(QPainter &painter, int left_offset, int top_offset) {

    QFont font = Dr::CustomFont(1);
    painter.setFont(font);
    painter.setPen(Dr::ToQColor(Dr::GetColor(Window_Colors::Text_Light)));

    int w = this->geometry().width();
    int h = this->geometry().height();
    int left;
    QPixmap  pic(w * 2, h);
    QPainter paint_pic(&pic);
             paint_pic.setFont(font);
    QRect    bounding_1, bounding_2;
    QString  text_1, text_2;

    switch (m_tip_type) {
        case View_Mode::Zooming:     text_1 = QString::number(m_int) + "%";             break;
        case View_Mode::Rotating:    text_1 = QString::number(m_angle, 'f', 1) + "°";   break;
        case View_Mode::Resizing:    text_1 = "W: " + QString::number(m_x, 'f', 1);     text_2 = "H: " + QString::number(m_y, 'f', 1);  break;
        case View_Mode::Translating: text_1 = "X: " + QString::number(m_x, 'f', 1);     text_2 = "Y: " + QString::number(m_y, 'f', 1);  break;
        default:                     text_1 = "---";                                    text_2 = "---";
    }

    switch (m_tip_type) {
        case View_Mode::Resizing:
        case View_Mode::Translating:
            ///// Simple text centering
            ///painter.drawText( 0,           0, w, h / 2, Qt::AlignmentFlag::AlignCenter, text_1);
            ///painter.drawText( 0, (h / 2) - 1, w, h / 2, Qt::AlignmentFlag::AlignCenter, text_2);

            // Draw text off screen to calculate proper starting x coord
            paint_pic.drawText( 0, 0, w, h, Qt::AlignmentFlag::AlignLeft, text_1, &bounding_1);
            paint_pic.drawText( 0, 0, w, h, Qt::AlignmentFlag::AlignLeft, text_2, &bounding_2);
            left = (bounding_1.width() > bounding_2.width()) ? (w - bounding_1.width() + 4) / 2 : (w - bounding_2.width() + 4) / 2;
            painter.drawText(left_offset + left,     top_offset,               w, h / 2, Qt::AlignmentFlag::AlignVCenter, text_1);
            painter.drawText(left_offset + left + 1, top_offset + (h / 2) - 1, w, h / 2, Qt::AlignmentFlag::AlignVCenter, text_2);

            break;
        case View_Mode::Rotating:
        case View_Mode::Zooming:
        default:
            painter.drawText(left_offset, top_offset, w, h, Qt::AlignmentFlag::AlignCenter, text_1);
    }
}
















