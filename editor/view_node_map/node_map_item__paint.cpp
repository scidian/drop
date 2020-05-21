//
//      Created by Stephens Nunnally on 5/6/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QDebug>
#include <QGraphicsPixmapItem>
#include <QStyleOptionGraphicsItem>
#include <QWidget>

#include "core/colors/colors.h"
#include "core/dr_debug.h"
#include "core/dr_math.h"
#include "editor/helper_library.h"
#include "editor/interface_editor_relay.h"
#include "editor/pixmap/pixmap.h"
#include "editor/preferences.h"
#include "editor/view_node_map/node_map_item.h"
#include "editor/view_node_map/node_map_scene.h"
#include "editor/view_node_map/node_map_view.h"
#include "engine/debug_flags.h"
#include "project/constants_component_info.h"
#include "project/dr_project.h"
#include "project/settings/settings.h"
#include "project/settings/settings_component.h"
#include "project/settings/settings_component_slot.h"

// Local Constants
const   double  c_transparent_not_selected =    0.70;
const   double  c_transparent_is_selected =     1.00;

const   double  c_seperator_height =            3;


//####################################################################################
//##    Custom Paint Handling
//####################################################################################
void NodeMapItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *) {
    if (m_component == nullptr) return;

    // ***** Turn on anti aliasing if necessary
    if (Dr::CheckDebugFlag(Debug_Flags::Turn_On_Antialiasing_in_Editor)) {
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->setRenderHint(QPainter::SmoothPixmapTransform, true);
    }

    // ***** Check item is not super duper tiny, this seems to crash paint function
    QRectF      box =    boundingRect();
    QPolygonF   poly =   sceneTransform().map(box);
    QRectF      bounds = poly.boundingRect();
    if (bounds.width() < .5 || bounds.height() < .5) return;

    // ***** Update Slot Circle Rects
    updateSlotRects();

    // ***** Decrease box size to allow for buffer
    box.adjust(c_node_buffer, c_node_buffer, -c_node_buffer, -c_node_buffer);

    // Set paint option to "not selected" or paint routine will draw dotted lines around item
    ///QStyleOptionGraphicsItem my_option(*option);
    ///my_option.state &= ~QStyle::State_Selected;
    // Custom pixmap painting
    ///painter->setRenderHint(QPainter::SmoothPixmapTransform, (this->transformationMode() == Qt::SmoothTransformation));
    ///QRectF dest = this->pixmap().rect();
    ///painter->drawPixmap(dest, this->pixmap(), this->pixmap().rect());
    // Original drawing using QtSource
    ///QGraphicsPixmapItem::paint(painter, my_option, widget);


    // ***** Alpha based on selected / mouse over
    double alpha = c_transparent_not_selected;
    if (option->state & QStyle::State_Selected)  alpha = c_transparent_is_selected;             // If selected
    if (option->state & QStyle::State_MouseOver) alpha = c_transparent_is_selected;             // If mouse is over

    // Node Colors
    QColor header_color_1 = Dr::ToQColor(m_component->getColor());
           header_color_1.setAlphaF(alpha);
    QColor header_color_2 = header_color_1.darker();
           header_color_2.setAlphaF(alpha);
    QColor back_color = Dr::ToQColor(Dr::GetColor(Window_Colors::Button_Light));
           back_color.setAlphaF(alpha);
    QColor dark_color = Dr::ToQColor(Dr::GetColor(Window_Colors::Background_Dark));
           dark_color.setAlphaF(alpha);
    QColor slot_color = Dr::ToQColor(Dr::GetColor(Window_Colors::Seperator));
           slot_color.setAlphaF(1.0);///alpha);
    QColor slot_border = Dr::ToQColor(this->isSelected() ? Dr::GetColor(Window_Colors::Icon_Light) : Dr::GetColor(Window_Colors::Button_Light));
           slot_border.setAlphaF(1.0);///alpha);
    QColor text_color;
    if (Dr::GetColorScheme() == Color_Scheme::Light) {
        text_color = Dr::ToQColor(Dr::GetColor(Window_Colors::Header_Text));
    } else {
        text_color = Dr::ToQColor(Dr::GetColor(Window_Colors::Text_Light));
    }


    // ********** Draw Top Row of Node
    QLinearGradient gradient(0, 0, 0, c_row_height);
                    gradient.setColorAt(0.00, header_color_1);
                    gradient.setColorAt(1.00, header_color_2);
    painter->setBrush(gradient);
    painter->setPen(Qt::NoPen);

    // Top Gradient
    QPainterPath top, top1, top2;
    top1.addRoundedRect(box.left(), box.top(), box.width(), c_row_height - c_seperator_height, c_corner_radius, c_corner_radius);
    top2.addRect(box.left(), box.top() + (c_row_height/2), box.width(), ((c_row_height/2) - c_seperator_height));
    top = top1.united(top2);
    painter->drawPath(top);

    // Seperator Line
    painter->setBrush(dark_color);
    painter->drawRect(box.left(), box.top() + (c_row_height - c_seperator_height), box.width(), c_seperator_height);

    // Component Icon
    QPixmap cat_icon(QString::fromStdString(m_component->getIcon()));
    if (cat_icon.isNull() == false && cat_icon.width() > 0 && cat_icon.height() > 0) {
        painter->setOpacity(0.4);
        // If we are zoomed in, allow QPainter to paint a larger version of the icon. Results is a smoother transformation to large scale.
        if (m_editor_relay && m_editor_relay->currentViewZoom() > 1.0) {
            double  hw_ratio = static_cast<double>(cat_icon.width()) / static_cast<double>(cat_icon.height());
            QRectF  dest_rect(box.left(), box.top() - (c_seperator_height/2), c_row_height * hw_ratio, c_row_height);
                    dest_rect.adjust(c_icon_reduce, c_icon_reduce / hw_ratio, -c_icon_reduce, -c_icon_reduce / hw_ratio);
            painter->drawPixmap(dest_rect, cat_icon, cat_icon.rect());
        // Else if we are zoomed out allow QPixmap.scaledToHeight to do the resizing. Results is a smoother transformation to small scale.
        } else {
            cat_icon = cat_icon.scaledToHeight(c_row_height - (c_icon_reduce*2), Qt::TransformationMode::SmoothTransformation);
            QPointF dest_point(box.left() + c_icon_reduce, box.top() - (c_seperator_height/2) + c_icon_reduce);
            painter->drawPixmap(dest_point, cat_icon);
        }
        painter->setOpacity(1.0);
    }

    // Header Text
    QString header_text = QString::fromStdString(m_component->getDisplayName());
    if (m_entity->getType() == DrType::World) header_text = QString::fromStdString(m_entity->getName());
    QFont header_font = Dr::CustomFont(2);
          ///header_font.setBold(true);
    painter->setFont(header_font);
    // Draw drop shadow for header
    ///painter->setPen(Qt::black);
    ///painter->drawText(box.left()-1, box.top()+1, box.width(), c_row_height - (c_seperator_height/2), Qt::AlignmentFlag::AlignCenter, QString::fromStdString(m_entity->getName()));
    // Draw Header
    painter->setPen(text_color);
    painter->drawText(box.left(), box.top(), box.width(), c_row_height - (c_seperator_height/2), Qt::AlignmentFlag::AlignCenter, QString::fromStdString(m_entity->getName()));


    // ********** Draw Bottom of Node
    painter->setBrush(back_color);
    painter->setPen(Qt::NoPen);

    QPainterPath mid, mid1, mid2;
    mid1.addRoundedRect(box.left(), box.top() + c_row_height, box.width(), box.height() - c_row_height, c_corner_radius, c_corner_radius);
    mid2.addRect(box.left(), box.top() + c_row_height, box.width(), c_row_height/2);
    mid = mid1.united(mid2);
    painter->drawPath(mid);


    // ********** Selection Highlight
    if (this->isSelected()) {
        QPen highlight_pen = QPen(Dr::ToQColor(Dr::GetColor(Window_Colors::Icon_Light)), 2, Qt::PenStyle::SolidLine, Qt::PenCapStyle::RoundCap, Qt::PenJoinStyle::BevelJoin);
        painter->setPen(highlight_pen);
        painter->setBrush(Qt::NoBrush);

        // The following maps pixmap() shape as outline
        /**
        QPainterPath outline = this->shape();
        QPointF center = box.center();
        double transform_scale_x = 1.0 + (1.0 / this->m_width);                 // Add 2 pixels of width
        double transform_scale_y = 1.0 + (1.0 / this->m_height);                // Add 2 pixels of height
        QTransform t = QTransform().translate(center.x(), center.y())
                                   .scale(transform_scale_x, transform_scale_y)
                                   .translate(-center.x(), -center.y());
        outline = t.map(this->shape());
        painter->drawPath( outline );
        */

        painter->drawRoundedRect(box, c_corner_radius, c_corner_radius);
    }


    // ********** Slots
    // Slot Circles
    QPen    slot_pen(slot_border, 2, Qt::PenStyle::SolidLine, Qt::PenCapStyle::RoundCap, Qt::PenJoinStyle::BevelJoin);
    painter->setPen( slot_pen );
    painter->setBrush( slot_color );
    for (auto &signal_rect : getSignalRects()) { painter->drawEllipse(signal_rect.adjusted(c_circle_reduce, c_circle_reduce, -c_circle_reduce, -c_circle_reduce)); }
    for (auto &output_rect : getOutputRects()) { painter->drawEllipse(output_rect.adjusted(c_circle_reduce, c_circle_reduce, -c_circle_reduce, -c_circle_reduce)); }

    // Slot Text
    painter->setFont(Dr::CustomFont(1));
    painter->setPen(text_color);
    int text_flags_in =  Qt::AlignVCenter | Qt::AlignLeft;
    int text_flags_out = Qt::AlignVCenter | Qt::AlignRight;

    // Signal Slot Text
    int signal_start_y = box.top() + (c_row_height * 1);
    for (auto &signal_pair : m_component->getSignalMap()) {
        QString slot_text = QString::fromStdString(signal_pair.second->getSlotName());
                slot_text.replace("_", "");
        painter->drawText(box.left()+(c_slot_size/1.5), signal_start_y, box.width()/2, c_row_height, text_flags_in, slot_text);
        signal_start_y += c_row_height;
    }

    // Output Slot Text
    int output_start_y = box.top() + (c_row_height * 1);
    for (auto &output_pair : m_component->getOutputMap()) {
        QString slot_text = QString::fromStdString(output_pair.second->getSlotName());
                slot_text.replace("_", "");
        painter->drawText(box.left()+(box.width()/2), output_start_y, (box.width()/2)-(c_slot_size/1.5), c_row_height, text_flags_out, slot_text);
        output_start_y += c_row_height;
    }


}   // End NodeMapItem::paint()



















