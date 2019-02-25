//
//      Created by Stephens Nunnally on 1/8/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Graphics View Paint Overrides
//
//
#include <QPaintEngine>
#include <QStylePainter>
#include <QOpenGLFunctions>

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
//##        Event Filter, can monitor events being recieved
//####################################################################################
bool DrView::eventFilter(QObject *obj, QEvent *event)
{
    //int t = event->type();
    //   1 = Timer
    //  11 = Leave            12 = Paint
    //  13 = Move             14 = Resize
    //  17 = Show             24 = Window Activate
    //  74 = Polish Request   78 = Update Later
    // 128 = Hover Leave     129 = Hover Move
    //
    //if (t != 1 && t != 11 && t != 12 && t != 13 && t != 14 && t != 17 && t != 24 && t != 74 && t != 78 && t != 128 && t != 129)
    //    m_relay->setLabelText(Label_Names::Label_2, QString::number(event->type()));
    return QGraphicsView::eventFilter(obj, event);
}


void DrView::drawBackground(QPainter *painter, const QRectF &rect)
{
    Q_UNUSED(painter);
    Q_UNUSED(rect);

    ///QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    ///f->glClearColor(static_cast<float>(Dr::GetColor(Window_Colors::Background_Light).redF()),
    ///                static_cast<float>(Dr::GetColor(Window_Colors::Background_Light).greenF()),
    ///                static_cast<float>(Dr::GetColor(Window_Colors::Background_Light).blueF()),
    ///                1.0f);
    ///f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_ACCUM_BUFFER_BIT);
}


//####################################################################################
//##        PAINT: Main Paint Event for QGraphicsView (DrView)
//####################################################################################
void DrView::paintEvent(QPaintEvent *event)
{
    // ******************** Clears the sceen
    if (Dr::CheckDebugFlag(Debug_Flags::Turn_On_OpenGL)) {
        QPainter clear_painter(this->viewport());
        clear_painter.eraseRect(this->viewport()->rect());
        clear_painter.end();
    }

    // ******************** Go ahead and draw grid first then pass on event to paint items
    paintGrid();
    QGraphicsView::paintEvent(event);


    // ******************** At this point, if no selected items get out of here
    if (scene() == nullptr) return;

    // Store current center point of scene, so that if we go to a new scene and come back we stay in the same place
    if (my_scene->getCurrentStageShown())
        my_scene->getCurrentStageShown()->setViewCenterPoint( mapToScene( this->viewport()->rect().center() ) );

    // If theres no selection we don't need to perform rest of paint routine
    if (my_scene->getSelectionCount() < 1) return;

    // Initiate QPainter object
    QPainter painter(viewport());

    paintItemOutlines(painter);                                     // Draw bounding box for each selected item

    if (m_hide_bounding == false)
        paintBoundingBox(painter);                                  // Draw box around entire seleciton, with Size Grip handles

    if (m_view_mode == View_Mode::Rotating)
        paintGroupAngle(painter, my_scene->getSelectionAngle());    // Draw angles if rotating

    if (m_hide_bounding == false)
        paintHandles(painter, m_handles_shape);                     // Draw handles around selected item / bounding box

    if (m_tool_tip->isHidden() == false &&
            Dr::CheckDebugFlag(Debug_Flags::Turn_On_OpenGL))
        paintToolTip(painter);


    // !!!!! #DEBUG:    Draw frames per second
    if (Dr::CheckDebugFlag(Debug_Flags::Label_FPS)) {
        m_relay->setLabelText(Label_Names::Label_3, "Draw Time: " + QString::number(m_debug_timer.elapsed()) +
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
void DrView::paintGrid()
{
    QPainter painter(viewport());
    painter.setBrush(Qt::NoBrush);

    // Map viewport to scene rect
    QPointF topLeft = mapToScene(0, 0);
    QPointF bottomRight = mapToScene(this->width(), this->height());
    QRectF  scene_rect(topLeft, bottomRight);

    double grid_x = m_grid_size.x();
    double grid_y = m_grid_size.y();

    // Bounds check
    if (grid_x < 1) grid_x = 1;
    if (grid_y < 1) grid_y = 1;

    // ********** Draw Grid Lines
    if (m_grid_style == Grid_Style::Lines) {
        painter.setPen(QPen( Dr::GetColor(Window_Colors::Background_Dark), 1 ));
        QVector<QLine> lines;

        // Vertical lines right of scene zero, followed by Vertical lines left of scene zero
        for (double x = 0; x <= scene_rect.right(); x += grid_x)
            lines.append(QLine(mapFromScene(x, 0).x(), 0, mapFromScene(x, 0).x(), height()));
        for (double x = 0; x >= scene_rect.left(); x -= grid_x)
            lines.append(QLine(mapFromScene(x, 0).x(), 0, mapFromScene(x, 0).x(), height()));

        // Horizontal lines below scene zero, followed by Horizontal lines above scene zero
        for (double y = 0; y <= scene_rect.bottom(); y += grid_y)
            lines.append(QLine(0, mapFromScene(0, y).y(), width(), mapFromScene(0, y).y()) );
        for (double y = 0; y >= scene_rect.top(); y -= grid_y)
            lines.append(QLine(0, mapFromScene(0, y).y(), width(), mapFromScene(0, y).y()) );

        painter.drawLines(lines);

    // ********** Draw Grid Dots
    } else if (m_grid_style == Grid_Style::Dots && m_zoom_scale > .1) {
        double dot_size = m_zoom_scale;
        if (m_zoom_scale < 8)   dot_size = 9;
        if (m_zoom_scale < 4)   dot_size = 7;
        if (m_zoom_scale < 2)   dot_size = 5.4;
        if (m_zoom_scale < 1.5) dot_size = 4.7;
        if (m_zoom_scale < 1)   dot_size = 4.1;
        if (m_zoom_scale < .80) dot_size = 3.5;
        if (m_zoom_scale < .60) dot_size = 3.3;
        if (m_zoom_scale < .45) dot_size = 3.25;
        if (m_zoom_scale < .35) dot_size = 3.2;
        if (m_zoom_scale < .25) dot_size = 3.15;
        if (m_zoom_scale < .2)  dot_size = 3.1;

        painter.setPen(QPen( Dr::GetColor(Window_Colors::Background_Dark), dot_size, Qt::PenStyle::SolidLine, Qt::PenCapStyle::RoundCap ));
        QVector<QPointF> points;

        // Bottom right
        for (double x = 0; x <= scene_rect.right(); x += grid_x)
            for (double y = 0; y <= scene_rect.bottom(); y += grid_y)
                points.append( mapFromScene(x, y) );
        // Bottom left
        for (double x = 0; x >= scene_rect.left(); x -= grid_x)
            for (double y = 0; y <= scene_rect.bottom(); y += grid_y)
                points.append( mapFromScene(x, y) );
        // Top right
        for (double x = 0; x <= scene_rect.right(); x += grid_x)
            for (double y = 0; y >= scene_rect.top(); y -= grid_y)
                points.append( mapFromScene(x, y) );
        // Top left
        for (double x = 0; x >= scene_rect.left(); x -= grid_x)
            for (double y = 0; y >= scene_rect.top(); y -= grid_y)
                points.append( mapFromScene(x, y) );

        painter.drawPoints(points.data(), points.size());
    }
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
        m_relay->setLabelText(Label_Names::Label_Object_3, "Group Size X: " + QString::number(my_scene->getSelectionBox().width()) +
                                                                    ", Y: " + QString::number(my_scene->getSelectionBox().height()) );
        m_relay->setLabelText(Label_Names::Label_Object_4, "Scale X: " +      QString::number(my_scene->getSelectionScale().x()) +
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
//##        PAINT: Paints lines showing rotation while rotating
//####################################################################################
void DrView::paintGroupAngle(QPainter &painter, double angle)
{
    ///// Set pen to draw as NOT operation
    ///painter.setCompositionMode(QPainter::CompositionMode::RasterOp_NotDestination);
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

    ///// Reset pen to draw normally
    ///painter.setCompositionMode(QPainter::CompositionMode::CompositionMode_SourceOver);
}


//####################################################################################
//##        PAINT: Paints handles onto view
//####################################################################################
void DrView::paintHandles(QPainter &painter, Handle_Shapes shape_to_draw)
{
    painter.setBrush(Dr::GetColor(Window_Colors::Icon_Light));

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

            switch (shape_to_draw)
            {
            case Handle_Shapes::Circles:    painter.drawPixmap(to_draw, p_circle, p_circle.rect());     break;
            case Handle_Shapes::Squares:    painter.drawPixmap(to_draw, p_square, p_square.rect());     break;
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












