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
#include "editor/world_map/world_map_item.h"
#include "editor/world_map/world_map_view.h"
#include "engine/debug_flags.h"
#include "project/constants_component_info.h"
#include "project/dr_project.h"
#include "project/entities/dr_node.h"
#include "project/settings/settings.h"

// Local Constants
const   double  c_transparent_not_selected =    0.70;
const   double  c_transparent_is_selected =     1.00;


//####################################################################################
//##    Custom Paint Handling
//####################################################################################
void WorldMapItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *) {

    // ***** Turn on anti aliasing if necessary
    if (Dr::CheckDebugFlag(Debug_Flags::Turn_On_Antialiasing_in_Editor)) {
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->setRenderHint(QPainter::SmoothPixmapTransform, true);
    }

    // Check item is not super duper tiny, this seems to crash paint function
    QRectF      box =    boundingRect();
    QPolygonF   poly =   sceneTransform().map(box);
    QRectF      bounds = poly.boundingRect();
    if (bounds.width() < .5 || bounds.height() < .5) return;

    // Decrease box size to allow for buffer
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
    QColor header_color_1 = Dr::ToQColor(Component_Colors::RGB_07_LightBlue);
           header_color_1.setAlphaF(alpha);
    QColor header_color_2 = Dr::ToQColor(Component_Colors::RGB_07_LightBlue).darker();
           header_color_2.setAlphaF(alpha);
    QColor back_color = Dr::ToQColor(Dr::GetColor(Window_Colors::Button_Light));
           back_color.setAlphaF(alpha);
    QColor slot_color = Dr::ToQColor(Dr::GetColor(Window_Colors::Seperator));
           slot_color.setAlphaF(1.0);///alpha);
    QColor slot_border = this->isSelected() ? Dr::ToQColor(Dr::GetColor(Window_Colors::Icon_Light)) : Dr::ToQColor(Dr::GetColor(Window_Colors::Button_Light));
           slot_border.setAlphaF(1.0);///alpha);


    // ***** Draw Top Row of Node
    QLinearGradient gradient(0, 0, 0, c_row_height);
                    gradient.setColorAt(0.00, header_color_1);
                    gradient.setColorAt(1.00, header_color_2);
    painter->setBrush(gradient);
    painter->setPen(Qt::NoPen);

    QPainterPath top, top1, top2;
    top1.addRoundedRect(box.left(), box.top(), box.width(), c_row_height, c_corner_radius, c_corner_radius);
    top2.addRect(box.left(), box.top()+c_row_height/2, box.width(), c_row_height/2);
    top = top1.united(top2);
    painter->drawPath(top);

    painter->setFont(Dr::CustomFontLarger());
    if (Dr::GetColorScheme() == Color_Scheme::Light) {
        painter->setPen(Dr::ToQColor(this->isSelected() ? Dr::GetColor(Window_Colors::Header_Text) : Dr::GetColor(Window_Colors::Header_Text)));
    } else {
        painter->setPen(Dr::ToQColor(this->isSelected() ? Dr::GetColor(Window_Colors::Text_Light) : Dr::GetColor(Window_Colors::Text)));
    }
    painter->drawText(box.left(), box.top(), box.width(), c_row_height, Qt::AlignmentFlag::AlignCenter, QString::fromStdString(m_entity->getName()));


    // ***** Draw Bottom of Node
    painter->setBrush(back_color);
    painter->setPen(Qt::NoPen);

    QPainterPath mid, mid1, mid2;
    mid1.addRoundedRect(box.left(), box.top()+c_row_height, box.width(), box.height()-c_row_height, c_corner_radius, c_corner_radius);
    mid2.addRect(box.left(), box.top()+c_row_height, box.width(), c_row_height/2);
    mid = mid1.united(mid2);
    painter->drawPath(mid);


    // ***** Selection Highlight
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
    DrNode* node = dynamic_cast<DrNode*>(m_entity);
    if (node != nullptr) {
        // Slot Circles
        QPen slot_pen(slot_border, 2, Qt::PenStyle::SolidLine, Qt::PenCapStyle::RoundCap, Qt::PenJoinStyle::BevelJoin);
        painter->setPen( slot_pen );
        painter->setBrush( slot_color );

        // Input / Output Slot Circles
        for (auto &input_rect :  getInputRects())  { painter->drawEllipse(input_rect); }
        for (auto &output_rect : getOutputRects()) { painter->drawEllipse(output_rect); }

        // Slot Text
        painter->setPen(Dr::ToQColor(this->isSelected() ? Dr::GetColor(Window_Colors::Text_Light) : Dr::GetColor(Window_Colors::Text)));
        int text_flags_in =  Qt::AlignVCenter | Qt::AlignLeft;
        int text_flags_out = Qt::AlignVCenter | Qt::AlignRight;

        // Input Slot Text
        int input_start_y = box.top() + (c_row_height * 1);
        for (auto &slot : node->getInputSlots()) {
            QString slot_text = QString::fromStdString(slot.owner_slot_name);
                    slot_text.replace("_", "");
            painter->drawText(box.left()+c_slot_size, input_start_y, (box.width()/2)-(c_slot_size*2), c_row_height, text_flags_in, slot_text);
            input_start_y += c_row_height;
        }

        // Output Slot Text
        int output_start_y = box.top() + (c_row_height * 1);
        for (auto &slot : node->getOutputSlots()) {
            QString slot_text = QString::fromStdString(slot.owner_slot_name);
                    slot_text.replace("_", "");
            painter->drawText(box.left()+(box.width()/2)+c_slot_size, output_start_y, (box.width()/2)-(c_slot_size*2), c_row_height, text_flags_out, slot_text);
            output_start_y += c_row_height;
        }
    }

}   // End WorldMapItem::paint()


//####################################################################################
//##    Returns Slot Circle Rect for a particular slot, slot_number should start at 0
//####################################################################################
QRectF WorldMapItem::slotRect(DrSlotType slot_type, int slot_number) {
    int left = 0;
    int top  = (boundingRect().top() + c_node_buffer) + (c_row_height * (1.5 + static_cast<double>(slot_number)));
        top -= (c_slot_size/2);
    switch (slot_type) {
        case DrSlotType::Input:     left = (boundingRect().top()   + c_node_buffer) - (c_slot_size*0.5);    break;
        case DrSlotType::Output:    left = (boundingRect().width() - c_node_buffer) - (c_slot_size*0.5);    break;
    }
    return QRectF(left, top, c_slot_size, c_slot_size);
}

// Returns DrSlot under scene_point
OldSlot WorldMapItem::slotAtPoint(QPointF scene_point) {
    OldSlot over_slot;
    DrNode* node = dynamic_cast<DrNode*>(m_entity);
    if (node == nullptr) return over_slot;

    for (size_t i = 0; i < this->getInputRects().size(); ++i) {
        QPolygonF scene_rect = this->mapToScene(this->getInputRects()[i]);
        if (scene_rect.containsPoint(scene_point, Qt::FillRule::OddEvenFill)) {
            auto it = node->getInputSlots().begin();
            std::advance(it, i);
            over_slot = (*it);
            over_slot.scene_position = Dr::FromQPointF(scene_rect.boundingRect().center());
            return over_slot;
        }
    }

    for (size_t i = 0; i < this->getOutputRects().size(); ++i) {
        QPolygonF scene_rect = this->mapToScene(this->getOutputRects()[i]);
        if (scene_rect.containsPoint(scene_point, Qt::FillRule::OddEvenFill)) {
            auto it = node->getOutputSlots().begin();
            std::advance(it, i);
            over_slot = (*it);
            over_slot.scene_position = Dr::FromQPointF(scene_rect.boundingRect().center());
            return over_slot;
        }
    }
    return over_slot;
}





















