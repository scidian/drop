//
//      Created by Stephens Nunnally on 3/9/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QGraphicsPixmapItem>
#include <QStyleOptionGraphicsItem>

#include "editor/colors/colors.h"
#include "editor/debug.h"
#include "editor/interface_editor_relay.h"
#include "editor/view/editor_item.h"
#include "editor/view/editor_view.h"
#include "library/dr_math.h"
#include "model/enums_model_types.h"
#include "model/project/project_thing.h"


//####################################################################################
//##    Custom Paint Handling
//####################################################################################
void DrItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
    ///if (option->state & QStyle::State_Selected)  { fillColor = QColor(Qt::red); } //m_color.dark(150); }              // If selected
    ///if (option->state & QStyle::State_MouseOver) { fillColor = QColor(Qt::gray); } //fillColor.light(125); }          // If mouse is over

    // Turn off anti aliasing if necessary
    if (Dr::CheckDebugFlag(Debug_Flags::Turn_On_Antialiasing)) {
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->setRenderHint(QPainter::SmoothPixmapTransform, true);
    }

    // Check item is not super duper tiny, this seems to crash paint function
    QPolygonF poly = sceneTransform().map(boundingRect());
    QRectF bounds = poly.boundingRect();
    if (bounds.width() < .5 || bounds.height() < .5) return;

    // Set paint option to "not selected" or paint routine will draw dotted lines around item
    ///QStyleOptionGraphicsItem my_option(*option);
    ///my_option.state &= ~QStyle::State_Selected;

    // Check opacity of current item
    double transparency = 0.01;
    if (m_thing != nullptr) {
        transparency = m_thing->getComponentPropertyValue(Components::Thing_Layering, Properties::Thing_Opacity).toDouble() / 100;
        transparency = Dr::Clamp(transparency, 0.0, 1.0);
    }

    // ***** Apply the proper opacity to this item and either paint the pixmap, or paint a pattern representation of the item
    if (transparency > 0) {
        if (Dr::FuzzyCompare(painter->opacity(), transparency) == false) painter->setOpacity(transparency);

        // Our custom pixmap painting to draw slightly larger and reduce black lines between images
        painter->setRenderHint(QPainter::SmoothPixmapTransform, (this->transformationMode() == Qt::SmoothTransformation));
        QPointF scale = data(User_Roles::Scale).toPointF();
        if (Dr::IsCloseTo(0.0, scale.x(), 0.001)) scale.setX(0.001);
        if (Dr::IsCloseTo(0.0, scale.y(), 0.001)) scale.setY(0.001);
        double adjust_x = 0.60 / (m_editor_relay->getStageView()->currentZoomLevel() * scale.x());      // Enlarge by x.xx pixels
        double adjust_y = 0.60 / (m_editor_relay->getStageView()->currentZoomLevel() * scale.y());
        QRectF dest = this->pixmap().rect();
               dest.adjust(-adjust_x, -adjust_y, adjust_x, adjust_y);
        painter->drawPixmap(dest, this->pixmap(), this->pixmap().rect());

        // Original drawing using QtSource
        ///QGraphicsPixmapItem::paint(painter, &my_option, widget);

    // ***** Draw crooss hatch image mask for transparent items
    } else {
        if (Dr::FuzzyCompare(painter->opacity(), 1.0) == false) painter->setOpacity(1);

        QPen comestic_pen = QPen(Dr::GetColor(Window_Colors::Icon_Dark), 0, Qt::PenStyle::SolidLine, Qt::PenCapStyle::FlatCap, Qt::PenJoinStyle::MiterJoin );
        comestic_pen.setCosmetic(true);
        painter->setPen( comestic_pen );

        QColor brush_color = Dr::GetColor(Window_Colors::Icon_Dark);
        brush_color.setAlpha(96);
        QBrush scaled_brush = QBrush(brush_color, Qt::BrushStyle::DiagCrossPattern );   //Qt::BrushStyle::SolidPattern);
        scaled_brush.setTransform(QTransform(painter->worldTransform().inverted()));
        painter->setBrush(scaled_brush);

        painter->drawPath( this->shape() );
    }
}


QColor DrItem::getColorAtPoint(QPointF at_local_point) {
    QImage image = this->pixmap().toImage();

    if (image.rect().contains(at_local_point.toPoint()))
        return image.pixelColor(at_local_point.toPoint());
    else
        return QColor(0, 0, 0, 0);
}

QColor DrItem::getColorAtPoint(QPointF at_view_point, QGraphicsView *mouse_over_view) {
    QPointF in_scene = mouse_over_view->mapToScene(at_view_point.toPoint());
    QPointF in_object = this->mapFromScene(in_scene);
    return this->getColorAtPoint(in_object);
}









