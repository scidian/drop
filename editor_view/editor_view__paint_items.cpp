//
//      Created by Stephens Nunnally on 3/9/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrView Paint Functions that paint on / around QGraphicsItems
//
//
#include <QDebug>

#include "colors/colors.h"
#include "debug.h"
#include "editor_scene.h"
#include "editor_view.h"
#include "enums.h"
#include "globals.h"
#include "helper.h"


//####################################################################################
//##    PAINT: Paints outline around every selected item
//####################################################################################
void DrView::paintItemOutlines(QPainter &painter) {
    QList<QGraphicsItem*>  my_items = my_scene->getSelectionItems();

    QBrush pen_brush(Dr::GetColor(Window_Colors::Icon_Light));
    painter.setPen(QPen(pen_brush, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.setBrush(Qt::NoBrush);

    bool antialiasing_before = painter.testRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::Antialiasing, false);

    for (auto item: my_items) {

        // Apply item scaling / rotation to bounding box
        QPolygonF polygon = item->sceneTransform().map( item->boundingRect() );

        // Convert bounding box to view coordinates and draw on screen
        QPolygon to_view = mapFromScene(polygon);
        painter.drawPolygon(to_view);
    }

    painter.setRenderHint(QPainter::Antialiasing, antialiasing_before);

    // !!!!! #DEBUG:    Show selection group info
    if (Dr::CheckDebugFlag(Debug_Flags::Label_Selection_Group_Data)) {
        Dr::SetLabelText(Label_Names::Label_Object_3, "Group W: " + QString::number(my_scene->getSelectionBox().width()) +
                                                          ", H: " + QString::number(my_scene->getSelectionBox().height()) );
        Dr::SetLabelText(Label_Names::Label_Object_4, "Scale X: " + QString::number(my_scene->getSelectionScale().x()) +
                                                          ", Y: " + QString::number(my_scene->getSelectionScale().y()) +
                                                      ", Angle: " + QString::number(my_scene->getSelectionAngle()) );
    }
    // !!!!! END

}


//####################################################################################
//##    PAINT: Paints bounding box onto view
//####################################################################################
void DrView::paintBoundingBox(QPainter &painter) {
    bool antialiasing_before = painter.testRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setPen(QPen(Dr::GetColor(Window_Colors::Text_Light), 1));

    // ***** Map item bounding box to screen so we can draw it
    QPolygonF polygon = my_scene->getSelectionBox();
    QTransform transform = my_scene->getSelectionTransform();
    polygon = transform.map(polygon);                                       // Map bounding box to scene location
    QPolygon to_view = mapFromScene(polygon);                               // Convert bounding box to view coordinates

    // ***** Draw bounding box onto view
    painter.drawLine( QLineF( m_handles_centers[Position_Flags::Rotate], m_handles_centers[Position_Flags::Top]) );
    painter.drawPolygon(to_view);
    painter.setRenderHint(QPainter::Antialiasing, antialiasing_before);
}



//####################################################################################
//##    PAINT: Paints handles onto view
//####################################################################################
void DrView::paintHandles(QPainter &painter, Handle_Shapes shape_to_draw) {
    painter.setBrush(Dr::GetColor(Window_Colors::Icon_Light));
    painter.resetTransform();

    QList<Position_Flags> handles {
        Position_Flags::Top,        Position_Flags::Bottom,     Position_Flags::Left,           Position_Flags::Right,
        Position_Flags::Top_Left,   Position_Flags::Top_Right,  Position_Flags::Bottom_Left,    Position_Flags::Bottom_Right,
        Position_Flags::Rotate
    };

    double rotate_size  =   10;                         // Determines size of rotate handle
    double handle_size  =    8;                         // Determines size of circular corner and side handles
    double square_scale = .225;                         // Determines size of square   corner and side handles
    for (auto handle : handles) {

        // Draw Cornder and Side Handles
        if (handle != Position_Flags::Rotate) {
            QPointF draw_center = mapFromScene(m_selection_points[handle]);
            QRectF  to_draw( draw_center.x() - (handle_size / 2), draw_center.y() - (handle_size / 2), handle_size, handle_size);

            if (shape_to_draw == Handle_Shapes::Circles) {
                painter.drawPixmap(to_draw, p_circle, p_circle.rect());
            } else if (shape_to_draw == Handle_Shapes::Squares) {
                painter.translate(to_draw.center());
                painter.rotate( m_grid_rotate );
                painter.scale( square_scale, square_scale );
                painter.drawPixmap(-p_square.rect().center(), p_square);
                painter.resetTransform();
            }

        // Draw rotate handle
        } else {
            QPointF draw_center = m_handles_centers[Position_Flags::Rotate];
            QRectF  to_draw( draw_center.x() - (rotate_size / 2), draw_center.y() - (rotate_size / 2), rotate_size, rotate_size);

            painter.drawPixmap(to_draw, p_rotate, p_rotate.rect());
        }
    }

    // !!!!! #DEBUG:    Paint rects of all mouse resizing grip handles
    if (Dr::CheckDebugFlag(Debug_Flags::Paint_Size_Grip_Handles)) {
        for (auto h : m_handles)
            painter.drawPolygon(h.second);
    }
    // !!!!! END
}


//####################################################################################
//##    PAINT: Paints lines showing rotation while rotating
//####################################################################################
void DrView::paintGroupAngle(QPainter &painter, double angle) {
    bool antialiasing_before = painter.testRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::Antialiasing, false);

    painter.setPen(QPen(Dr::GetColor(Window_Colors::Text_Light), 1));

    // !!!!! #DEBUG:    Draws from center to origin point (mouse down), and center to last position (mouse move)
    if (Dr::CheckDebugFlag(Debug_Flags::Paint_Rotating_Angles)) {
        painter.drawLine( m_handles_centers[Position_Flags::Center], m_origin);
        painter.drawLine( m_handles_centers[Position_Flags::Center], m_last_mouse_pos);
    }
    // !!!!! END

    // Map center point to view, create a line going straight up at zero degrees
    QPointF center_point = m_handles_centers[Position_Flags::Center];
    QLineF     line_zero = QLineF(center_point, QPointF(center_point.x(), center_point.y() - 30) );

    // Make a copy of line rotated at the current Selection Group angle (passed in)
    QPointF origin = center_point;
    QTransform rotate = QTransform().translate(origin.x(), origin.y()).rotate(angle).translate(-origin.x(), -origin.y());
    QLineF  line_rotated = rotate.map(line_zero);
    line_zero.setP1( QPointF(line_zero.x1(), line_zero.y1() - 1) );

    // Draw lines
    painter.drawLine( line_zero );
    painter.drawLine( line_rotated );
    painter.setRenderHint(QPainter::Antialiasing, antialiasing_before);
}


//####################################################################################
//##    PAINT: Paints cross in the center of item / selection box while translating
//####################################################################################
void DrView::paintItemCenters(QPainter &painter) {
    // Don't draw if snap to grid is off
    if (m_grid_should_snap == false) return;
    if (m_allow_movement == false) return;

    ///// Paints in NOT mode, doesnt work in OpenGL
    ///if (Dr::CheckDebugFlag(Debug_Flags::Turn_On_OpenGL))
    ///    painter.setCompositionMode(QPainter::CompositionMode::CompositionMode_Source);
    ///else
    ///    painter.setCompositionMode(QPainter::CompositionMode::RasterOp_NotDestination);

    // Draw crosshairs on SelectionBox center
    if (Dr::GetPreference(Preferences::World_Editor_Snap_To_Center_Of_Selection_Box).toBool()) {
        QPoint center = mapFromScene(my_scene->getSelectionTransform().map( my_scene->getSelectionBox().center()) );
        paintCrossHairs(painter, center);

    // Loop through selected items and draw crosshairs on each one
    } else {
        for (auto item: my_scene->getSelectionItems()) {
            QPoint center = mapFromScene( item->sceneTransform().map( item->boundingRect().center() ) );
            paintCrossHairs(painter, center);
        }
    }

    ///// Restore normal painting mode
    ///painter.setCompositionMode(QPainter::CompositionMode::CompositionMode_SourceOver);
}


//####################################################################################
//##    PAINT: Paints the actual crosshairs for function paintItemCenters
//####################################################################################
void DrView::paintCrossHairs(QPainter &painter, QPoint center) {
    QTransform t = QTransform()
            .translate(center.x(), center.y())
            .scale(m_grid_scale.x(), m_grid_scale.y())
            .rotate(m_grid_rotate)
            .translate(-center.x(), -center.y());

    QVector<QLine> lines;
    QLine line;

    int line_size = 15;
    line = QLine(center.x(), center.y() + line_size, center.x(), center.y() - line_size);
    lines.append( t.map (line) );

    line = QLine(center.x() + line_size, center.y(), center.x() - line_size, center.y());
    lines.append( t.map (line) );

    painter.setBrush(Qt::NoBrush);
    painter.setPen(QPen(Dr::GetColor(Window_Colors::Text_Light), 4));
    painter.drawLines(lines);
    painter.setPen(QPen(Dr::GetColor(Window_Colors::Shadow), 2));
    painter.drawLines(lines);

}













