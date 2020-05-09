//
//      Created by Stephens Nunnally on 5/6/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QGraphicsPixmapItem>
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
void WorldMapItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
    ///if (option->state & QStyle::State_Selected)  { fillColor = QColor(Qt::red); } //m_color.dark(150); }              // If selected
    ///if (option->state & QStyle::State_MouseOver) { fillColor = QColor(Qt::gray); } //fillColor.light(125); }          // If mouse is over

    // ***** Turn off anti aliasing if necessary
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


    // ***** Apply the proper opacity to this item and either paint the pixmap, or paint a pattern representation of the item
    double transparency = 1.0;
    if (Dr::FuzzyCompare(painter->opacity(), transparency) == false) painter->setOpacity(transparency);

    // Our custom pixmap painting to draw slightly larger and reduce black lines between images
    ///painter->setRenderHint(QPainter::SmoothPixmapTransform, (this->transformationMode() == Qt::SmoothTransformation));
    ///QRectF dest = this->pixmap().rect();
    ///painter->drawPixmap(dest, this->pixmap(), this->pixmap().rect());
    // Original drawing using QtSource
    ///QGraphicsPixmapItem::paint(painter, my_option, widget);


    // ***** Node Colors
    double alpha = this->isSelected() ? c_transparent_is_selected : c_transparent_not_selected;
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
    painter->setPen(Dr::ToQColor(this->isSelected() ? Dr::GetColor(Window_Colors::Text_Light) : Dr::GetColor(Window_Colors::Text)));
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

        // Input Slot Circles
        int start_y = box.top() + (c_row_height * 2) - (c_row_height/2);
        for (size_t count = 0; count < node->getInputSlots().size(); ++count) {
            painter->drawEllipse(box.left()-(c_slot_size*0.5), start_y - (c_slot_size/2), c_slot_size, c_slot_size);
            start_y += c_row_height;
        }

        // Output Slot Circles
        start_y = box.top() + (c_row_height * 2) - (c_row_height/2);
        for (size_t count = 0; count < node->getOutputSlots().size(); ++count) {
            painter->drawEllipse(box.left() + box.width() - (c_slot_size*0.5), start_y - (c_slot_size/2), c_slot_size, c_slot_size);
            start_y += c_row_height;
        }

        // Slot Text
        painter->setPen(Dr::ToQColor(this->isSelected() ? Dr::GetColor(Window_Colors::Text_Light) : Dr::GetColor(Window_Colors::Text)));
        int text_flags_in =  Qt::AlignVCenter | Qt::AlignLeft;
        int text_flags_out = Qt::AlignVCenter | Qt::AlignRight;

        // Input Slot Text
        start_y = box.top() + (c_row_height * 1);
        for (auto &slot : node->getInputSlots()) {
            QString slot_text = QString::fromStdString(slot.name);
                    slot_text.replace("_", "");
            painter->drawText(box.left()+c_slot_size, start_y, (box.width()/2)-(c_slot_size*2), c_row_height, text_flags_in, slot_text);
            start_y += c_row_height;
        }

        // Output Slot Text
        start_y = box.top() + (c_row_height * 1);
        for (auto &slot : node->getOutputSlots()) {
            QString slot_text = QString::fromStdString(slot.name);
                    slot_text.replace("_", "");
            painter->drawText(box.left()+(box.width()/2)+c_slot_size, start_y, (box.width()/2)-(c_slot_size*2), c_row_height, text_flags_out, slot_text);
            start_y += c_row_height;
        }
    }




}












