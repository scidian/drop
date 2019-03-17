//
//      Created by Stephens Nunnally on 3/9/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrView Paint Functions that paint on / around QGraphicsItems
//
//
#include "colors.h"
#include "debug.h"
#include "editor_scene.h"
#include "editor_view.h"
#include "enums.h"
#include "globals.h"
#include "library.h"


//####################################################################################
//##        PAINT: Paints outline around every selected item
//####################################################################################
void DrView::paintItemOutlines(QPainter &painter)
{
    QList<QGraphicsItem*>  my_items = my_scene->getSelectionItems();

    QBrush pen_brush(Dr::GetColor(Window_Colors::Icon_Light));
    painter.setPen(QPen(pen_brush, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.setBrush(Qt::NoBrush);

    for (auto item: my_items) {

        // Apply item scaling / rotation to bounding box
        QPolygonF polygon = item->sceneTransform().map( item->boundingRect() );

        // Convert bounding box to view coordinates and draw on screen
        QPolygon to_view = mapFromScene(polygon);
        painter.drawPolygon(to_view);
    }

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
//##        PAINT: Paints bounding box onto view
//####################################################################################
void DrView::paintBoundingBox(QPainter &painter)
{
    painter.setPen(QPen(Dr::GetColor(Window_Colors::Text_Light), 1));

    // ***** Map item bounding box to screen so we can draw it
    QPolygonF polygon = my_scene->getSelectionBox();
    QTransform transform = my_scene->getSelectionTransform();
    polygon = transform.map(polygon);                                       // Map bounding box to scene location
    QPolygon to_view = mapFromScene(polygon);                               // Convert bounding box to view coordinates

    // ***** Draw bounding box onto view
    painter.drawLine( QLineF( m_handles_centers[Position_Flags::Rotate], m_handles_centers[Position_Flags::Top]) );
    painter.drawPolygon(to_view);


    // !!!!! #DEBUG:    Paints unrotated selection box with distance point used for calculating scale
    if (Dr::CheckDebugFlag(Debug_Flags::Paint_Resize_Calculations)) {
        painter.setPen(QPen(QBrush(Qt::red), 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter.drawPolygon(m_debug_points);
    }
    // !!!!! END

    // !!!!! #DEBUG:    Paints un rotated, sheared boxes
    if (Dr::CheckDebugFlag(Debug_Flags::Paint_Shear_Remove)) {
        painter.setPen(QPen(QBrush(Qt::red), 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        QPolygonF shear_to_view = mapFromScene(m_debug_shear);
        painter.drawPolygon(shear_to_view);

        painter.setPen(QPen(QBrush(Qt::green), 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        QPolygonF shear_to_view2 = mapFromScene(m_debug_shear2);
        painter.drawPolygon(shear_to_view2);

        painter.setPen(QPen(QBrush(Qt::blue), 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        QPolygonF shear_to_view3 = mapFromScene(m_debug_shear3);
        painter.drawPolygon(shear_to_view3);
    }
    // !!!!! END
}



//####################################################################################
//##        PAINT: Paints handles onto view
//####################################################################################
void DrView::paintHandles(QPainter &painter, Handle_Shapes shape_to_draw)
{
    painter.setBrush(Dr::GetColor(Window_Colors::Icon_Light));
    painter.resetTransform();

    QVector<QPointF> handles;
    for (int i = 0; i < static_cast<int>(Position_Flags::Total); i++)
        handles.append(m_handles_centers[static_cast<Position_Flags>(i)]);
    handles.append(m_handles_centers[Position_Flags::Rotate] );

    QRectF to_draw;
    double handle_size  =    8;
    double rotate_size  =   10;
    double square_scale = .225;
    for (int i = 0; i < handles.count(); i++) {
        if (i < (handles.count() - 1)) {
            to_draw.setX(handles[i].x() - (handle_size / 2));    to_draw.setWidth(handle_size);
            to_draw.setY(handles[i].y() - (handle_size / 2));    to_draw.setHeight(handle_size);

            if (shape_to_draw == Handle_Shapes::Circles) {
                painter.drawPixmap(to_draw, p_circle, p_circle.rect());
            } else if (shape_to_draw == Handle_Shapes::Squares) {
                painter.translate(to_draw.center());
                painter.rotate( m_grid_rotate );
                painter.scale( square_scale, square_scale );
                painter.drawPixmap(-p_square.rect().center(), p_square);
                painter.resetTransform();
            }

        } else {
            to_draw.setX(handles[i].x() - (rotate_size / 2));    to_draw.setWidth(rotate_size);
            to_draw.setY(handles[i].y() - (rotate_size / 2));    to_draw.setHeight(rotate_size);

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
//##        PAINT: Paints lines showing rotation while rotating
//####################################################################################
void DrView::paintGroupAngle(QPainter &painter, double angle)
{
    painter.setPen(QPen(Dr::GetColor(Window_Colors::Text_Light), 1));

    // !!!!! #DEBUG:    Draws from center to origin point (mouse down), and center to last position (mouse move)
    if (Dr::CheckDebugFlag(Debug_Flags::Paint_Rotating_Angles)) {
        painter.drawLine(mapFromScene(m_rotate_start_rect.center()), m_origin);
        painter.drawLine(mapFromScene(m_rotate_start_rect.center()), m_last_mouse_pos);
    }
    // !!!!! END

    // Map center point to view, create a line going straight up at zero degrees
    QPointF center_point = mapFromScene(m_rotate_start_rect.center());
    QLineF     line_zero = QLineF(center_point, QPointF(center_point.x(), center_point.y() - 30) );

    // Make a copy of line rotated at the current Selection Group angle (passed in)
    QPointF origin = center_point;
    QTransform rotate = QTransform().translate(origin.x(), origin.y()).rotate(angle).translate(-origin.x(), -origin.y());
    QLineF  line_rotated = rotate.map(line_zero);
    line_zero.setP1( QPointF(line_zero.x1(), line_zero.y1() - 1) );

    // Draw lines
    painter.drawLine( line_zero );
    painter.drawLine( line_rotated );
}


//####################################################################################
//##        PAINT: Paints cross in the center of item while translating
//####################################################################################
void DrView::paintItemCenters(QPainter &painter)
{
    // Don't draw if snap to grid is off
    if (m_grid_should_snap == false) return;
    if (m_allow_movement == false) return;

    int line_size = 15;

    if (Dr::CheckDebugFlag(Debug_Flags::Turn_On_OpenGL) == false)
        painter.setCompositionMode(QPainter::CompositionMode::RasterOp_NotDestination);
    else
        painter.setCompositionMode(QPainter::CompositionMode::CompositionMode_Source);

    // Loop through selected items and draw crosshairs on each one
    for (auto item: my_scene->getSelectionItems()) {
        QPoint center = mapFromScene( item->sceneTransform().map( item->boundingRect().center() ) );

        QTransform t = QTransform()
                .translate(center.x(), center.y())
                .scale(m_grid_scale.x(), m_grid_scale.y())
                .rotate(m_grid_rotate)
                .translate(-center.x(), -center.y());

        QVector<QLine> lines;
        QLine line;

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

    painter.setCompositionMode(QPainter::CompositionMode::CompositionMode_SourceOver);
}














