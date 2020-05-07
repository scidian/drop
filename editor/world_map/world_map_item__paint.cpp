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
#include "project/dr_project.h"
#include "project/settings/settings.h"

// Local Constants
const   double  c_transparent_not_selected =    0.60;
const   double  c_transparent_is_selected =     0.80;

//####################################################################################
//##    Custom Paint Handling
//####################################################################################
void WorldMapItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
    ///if (option->state & QStyle::State_Selected)  { fillColor = QColor(Qt::red); } //m_color.dark(150); }              // If selected
    ///if (option->state & QStyle::State_MouseOver) { fillColor = QColor(Qt::gray); } //fillColor.light(125); }          // If mouse is over

    // Turn off anti aliasing if necessary
    if (Dr::CheckDebugFlag(Debug_Flags::Turn_On_Antialiasing_in_Editor)) {
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->setRenderHint(QPainter::SmoothPixmapTransform, true);
    }

    // Check item is not super duper tiny, this seems to crash paint function
    QPolygonF   poly =   sceneTransform().map(boundingRect());
    QRectF      bounds = poly.boundingRect();
    if (bounds.width() < .5 || bounds.height() < .5) return;

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

    // ***** Draw Node
    painter->setFont(Dr::CustomFont());
    painter->setPen(Qt::NoPen);
    QColor back_color = Dr::ToQColor(Dr::GetColor(Window_Colors::Button_Light));
           back_color.setAlphaF(this->isSelected() ? c_transparent_is_selected : c_transparent_not_selected);
    painter->setBrush(back_color);
    painter->drawRoundedRect(boundingRect(), c_corner_radius, c_corner_radius);

    painter->setPen(Dr::ToQColor(Dr::GetColor(Window_Colors::Text)));
    painter->drawText(0, 0, m_width, m_height, Qt::AlignmentFlag::AlignCenter, QString::fromStdString(m_entity->getName()));

    // ***** Selection Highlight
    if (this->isSelected()) {
        QPen pen = QPen(Dr::ToQColor(Dr::GetColor(Window_Colors::Icon_Light)), 2, Qt::PenStyle::SolidLine, Qt::PenCapStyle::RoundCap, Qt::PenJoinStyle::BevelJoin );
        painter->setPen(pen);
        painter->setBrush(Qt::NoBrush);

        // Maps Pixmap shape as outline
        /**
        QPainterPath outline = this->shape();
        QPointF center = boundingRect().center();
        double transform_scale_x = 1.0 + (1.0 / this->m_width);         // Add 2 pixels of width
        double transform_scale_y = 1.0 + (1.0 / this->m_height);        // Add 2 pixels of height
        QTransform t = QTransform()
                .translate(center.x(), center.y())
                .scale(transform_scale_x, transform_scale_y)
                .translate(-center.x(), -center.y());
        outline = t.map(this->shape());
        painter->drawPath( outline );
        */

        painter->drawRoundedRect(boundingRect(), c_corner_radius, c_corner_radius);
    }

}












