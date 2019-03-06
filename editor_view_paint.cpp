//
//      Created by Stephens Nunnally on 1/8/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Graphics View Paint Overrides
//
//
#include <QPaintEngine>
#include <QPaintEvent>
#include <QStylePainter>
#include <QOpenGLFunctions>

#include "colors.h"
#include "debug.h"

#include "editor_item.h"
#include "editor_scene.h"
#include "editor_view.h"

#include "globals.h"
#include "interface_editor_relay.h"
#include "library.h"

#include "project.h"
#include "project_world.h"
#include "project_world_stage.h"
#include "project_world_stage_object.h"
#include "settings.h"
#include "settings_component.h"
#include "settings_component_property.h"


//####################################################################################
//##        DrawBackground / DrawForground, called before and after paintEvent
//####################################################################################
void DrView::drawBackground(QPainter *painter, const QRectF &rect)
{
    Q_UNUSED(rect);

    if (Dr::CheckDebugFlag(Debug_Flags::Turn_On_OpenGL)) {
        QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
        f->glClearColor(static_cast<float>(Dr::GetColor(Window_Colors::Background_Light).redF()),
                        static_cast<float>(Dr::GetColor(Window_Colors::Background_Light).greenF()),
                        static_cast<float>(Dr::GetColor(Window_Colors::Background_Light).blueF()),
                        1.0f);
        f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_ACCUM_BUFFER_BIT);
    }

    if (m_grid_show_on_top == false) {
        paintGrid(*painter);
    }
}

void DrView::drawForeground(QPainter *painter, const QRectF &rect)
{
    Q_UNUSED(rect);

    if (m_grid_show_on_top == true) {
        paintGrid(*painter);
    }
}


//####################################################################################
//##        PAINT: Main Paint Event for QGraphicsView (DrView)
//####################################################################################
void DrView::paintEvent(QPaintEvent *event)
{
    // ******************** Pass on event to parent class paint items into scene
    QGraphicsView::paintEvent(event);

    // ******************** At this point, if no selected items get out of here
    if (scene() == nullptr) return;

    // Store current center point of scene, so that if we go to a new scene and come back we stay in the same place
    if (my_scene->getCurrentStageShown()) {
        QRect view_rect = QRect(0, 0, this->width(), this->height());
        my_scene->getCurrentStageShown()->setViewCenterPoint( mapToScene( view_rect.center() ) );
    }

    // Initiate QPainter object
    QPainter painter(viewport());

    // If theres no selection we don't need to perform these paint routines
    if (my_scene->getSelectionCount() > 0) {

        paintItemOutlines(painter);                                     // Draw bounding box for each selected item

        if (m_hide_bounding == false) {
            paintBoundingBox(painter);                                  // Draw box around entire seleciton, with Size Grip handles
            paintHandles(painter, m_handles_shape);                     // Draw handles around selected item / bounding box
        }

        if (m_view_mode == View_Mode::Rotating)
            paintGroupAngle(painter, my_scene->getSelectionAngle());    // Draw angles if rotating

        if (m_view_mode == View_Mode::Translating)
            paintItemCenters(painter);
    }

    if (m_tool_tip->isHidden() == false && Dr::CheckDebugFlag(Debug_Flags::Turn_On_OpenGL))
        paintToolTip(painter);

    // !!!!! #DEBUG:    Draw frames per second
    if (Dr::CheckDebugFlag(Debug_Flags::Label_FPS)) {
        Dr::SetLabelText(Label_Names::Label_3, "Draw Time: " + QString::number(m_debug_timer.elapsed()) +
                                                   ", FPS: " + QString::number(m_debug_fps_last) );
        m_debug_fps++;
        if (m_debug_timer.elapsed() >= 1000) {
            m_debug_timer.restart();
            m_debug_fps_last = m_debug_fps;
            m_debug_fps = 0;
        }
    }
    // !!!!! END

}

// Paint helper function, returns a RectF around center point with sides length of rect_size
QRectF DrView::rectAtCenterPoint(QPoint center, double rect_size)
{
    return QRectF(center.x() - rect_size / 2, center.y() - rect_size / 2, rect_size, rect_size);
}


//####################################################################################
//##        PAINT: Draws grid lines
//####################################################################################
void DrView::paintGrid(QPainter &painter)
{
    double point_size = 4;
    if (m_zoom_scale <= .50) point_size = 3;
    if (m_zoom_scale >= 2.0) point_size = 6;
    if (m_zoom_scale >= 4.0) point_size = 8;
    if (m_zoom_scale >= 8.0) point_size = 12;
    QPen dot_pen =  QPen( Dr::GetColor(Window_Colors::Background_Dark), point_size, Qt::PenStyle::SolidLine, Qt::PenCapStyle::RoundCap );
    QPen line_pen = QPen( Dr::GetColor(Window_Colors::Background_Dark), 1);
    line_pen.setCosmetic(true);

    if (m_grid_needs_redraw) {

        QPointF top_left =  mapFromScene( m_grid_view_rect.topLeft() );
        QPointF bot_right = mapFromScene( m_grid_view_rect.bottomRight() );
        QTransform offset = QTransform().translate(-top_left.x(), -top_left.y());

        QRect grid_view = QRectF(top_left, bot_right).normalized().toRect();
        int new_width, new_height;
        if (abs(grid_view.width()) >= 1)  new_width  = abs(grid_view.width());  else new_width = 1;
        if (abs(grid_view.height()) >= 1) new_height = abs(grid_view.height()); else new_height = 1;
        m_grid_buffer = QPixmap(new_width, new_height);
        m_grid_buffer.fill(QColor(0, 0, 0, 0));

        QPainter *grid_painter = new QPainter(&m_grid_buffer);
        grid_painter->setBrush(Qt::NoBrush);

        // ***** Grid Lines
        if (m_grid_style == Grid_Style::Lines) {
            grid_painter->setPen(line_pen);

            QVector<QLineF> view_lines;
            for (auto &line : m_grid_lines) {
                QLineF new_line = QLineF( mapFromScene( line.p1() ) , mapFromScene( line.p2() ) );
                new_line = offset.map (new_line );
                view_lines.append( new_line );
            }
            grid_painter->drawLines( view_lines );

            ///painter.setPen(line_pen);
            ///painter.drawLines( m_grid_lines );

        // ***** Grid Dots
        } else {
            grid_painter->setPen(dot_pen);

            QVector<QPointF> view_points;
            for (auto &point : m_grid_points) {
                QPointF new_point = mapFromScene( point );
                new_point = offset.map( new_point );
                view_points.append( new_point );
            }
            grid_painter->drawPoints( view_points );
        }

        grid_painter->end();
        delete grid_painter;

        m_grid_needs_redraw = false;
    }

    // Draw buffer holding grid onto screen
    QRectF target_rect = m_grid_view_rect;
    painter.drawPixmap(target_rect, m_grid_buffer, QRectF(m_grid_buffer.rect()) );

    // Draws an outline around grid
    painter.setBrush(Qt::NoBrush);
    painter.setPen(line_pen);
    painter.drawRect(m_grid_view_rect);
}



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
        Dr::SetLabelText(Label_Names::Label_Object_3, "Group Size X: " + QString::number(my_scene->getSelectionBox().width()) +
                                                               ", Y: " + QString::number(my_scene->getSelectionBox().height()) );
        Dr::SetLabelText(Label_Names::Label_Object_4, "Scale X: " +      QString::number(my_scene->getSelectionScale().x()) +
                                                          ", Y: " +      QString::number(my_scene->getSelectionScale().y()) +
                                                      ", Angle: " +      QString::number(my_scene->getSelectionAngle()));
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
    double handle_size = 8;
    double rotate_size = 10;
    for (int i = 0; i < handles.count(); i++) {
        if (i < (handles.count() - 1)) {
            to_draw.setX(handles[i].x() - (handle_size / 2));    to_draw.setWidth(handle_size);
            to_draw.setY(handles[i].y() - (handle_size / 2));    to_draw.setHeight(handle_size);

            if (shape_to_draw == Handle_Shapes::Circles) {
                painter.drawPixmap(to_draw, p_circle, p_circle.rect());
            } else if (shape_to_draw == Handle_Shapes::Squares) {
                painter.translate(to_draw.center());
                painter.rotate( m_grid_rotate );
                painter.scale(.25, .25);
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


//####################################################################################
//##        PAINT: Paints tool tip when using OpenGL
//####################################################################################
void DrView::paintToolTip(QPainter &painter)
{
    int w = m_tool_tip->geometry().width();
    int h = m_tool_tip->geometry().height();
    int left_offset = m_tool_tip->pos().x();
    int top_offset =  m_tool_tip->pos().y();

    painter.setPen(Qt::NoPen);
    painter.setBrush(Dr::GetColor(Window_Colors::Shadow));
    painter.drawRoundedRect(left_offset, top_offset + 2, w, h, m_tool_tip->getXRadius(), m_tool_tip->getYRadius(), Qt::SizeMode::RelativeSize);

    QLinearGradient gradient( left_offset, top_offset, left_offset, top_offset + h);
    gradient.setColorAt(0.0, Dr::GetColor(Window_Colors::Button_Light));
    gradient.setColorAt(1.0, Dr::GetColor(Window_Colors::Button_Dark));
    painter.setBrush(gradient);
    painter.drawRoundedRect(left_offset, top_offset, w, h, m_tool_tip->getXRadius(), m_tool_tip->getYRadius(), Qt::SizeMode::RelativeSize);

    // External call to paint routine of tool tip, but since we pass a painter we can pass the QGraphicsView to be painted on when OpenGL is turned on,
    // this way the tool tip text is drawn directly onto the OpenGL surface and not a seperate QWidget tool tip window
    m_tool_tip->drawText(painter, left_offset, top_offset);
}


//####################################################################################
//##        PAINT: Draw our Rubber Band selection box with custom colors
//####################################################################################
void DrViewRubberBand::paintEvent(QPaintEvent *)
{
    QStylePainter painter(this);

    QColor bg = Dr::GetColor(Window_Colors::Icon_Light);
    bg.setAlpha(48);
    QBrush brush;
    brush.setStyle(Qt::BrushStyle::SolidPattern);
    brush.setColor(bg);

    painter.setBrush(brush);
    painter.setPen(QPen(QBrush(Dr::GetColor(Window_Colors::Icon_Light)), 2, Qt::PenStyle::SolidLine));
    painter.drawRect(this->rect());
}












