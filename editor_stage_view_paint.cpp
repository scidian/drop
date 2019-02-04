//
//      Created by Stephens Nunnally on 1/8/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Graphics View Paint Overrides
//
//

#include "library.h"

#include "project.h"
#include "project_world.h"
#include "project_world_stage.h"
#include "project_world_stage_object.h"
#include "editor_stage_item.h"

#include "settings.h"
#include "settings_component.h"
#include "settings_component_property.h"

#include "editor_stage_scene.h"
#include "editor_stage_view.h"
#include "interface_relay.h"


//####################################################################################
//##        Event Filter, can monitor events being recieved
//####################################################################################
bool StageGraphicsView::eventFilter(QObject *obj, QEvent *event)
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



//####################################################################################
//##        PAINT: Main Paint Event for QGraphicsView (StageGraphicsView)
//####################################################################################
void StageGraphicsView::paintEvent(QPaintEvent *event)
{
    // ********** Check if scene that view is associated with has changed, if so re-connect signals from new scene
    if (scene() != m_scene) {
        if (scene() != nullptr) {
            m_scene = scene();
            StageGraphicsScene *my_scene = dynamic_cast<StageGraphicsScene*>(scene());

            connect(scene(), SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));
            connect(scene(), SIGNAL(changed(QList<QRectF>)), this, SLOT(sceneChanged(QList<QRectF>)));

            connect(my_scene, &StageGraphicsScene::updateViews, [this]() { update(); });

            connect(this,   SIGNAL(selectionGroupMoved(StageGraphicsScene*, QPointF)),
                    my_scene, SLOT(selectionGroupMoved(StageGraphicsScene*, QPointF)));

            connect(this,     SIGNAL(selectionGroupNewGroup(StageGraphicsScene*, QList<DrObject*>, QList<DrObject*>, DrObject*, DrObject*)),
                    my_scene,   SLOT(selectionGroupNewGroup(StageGraphicsScene*, QList<DrObject*>, QList<DrObject*>, DrObject*, DrObject*)) );
        }
    }


    // ******************** Go ahead and draw grid first
    paintGrid();


    // ******************** At this point, if no selected item paint objects and get out of here
    if (scene() == nullptr) return;
    StageGraphicsScene    *my_scene = dynamic_cast<StageGraphicsScene *>(scene());

    // Store current center point of scene, so that if we go to a new scene and come back we stay in the same place
    if (my_scene->getCurrentStageShown())
        my_scene->getCurrentStageShown()->setViewCenterPoint( mapToScene( this->viewport()->rect().center() ) );

    // If theres no selection we don't need to perform rest of paint routine
    if (my_scene->getSelectionGroupCount() < 1) {
        QGraphicsView::paintEvent(event);
        return;
    }


    // #################### Locking here so we can break apart selection group for layered drawing
    if (my_scene->scene_mutex.tryLock(10) == false) return;


    // ******************** Otherwise, break apart group to draw items in proper z-Order, then put back together
    QList<QGraphicsItem*>  my_items = my_scene->getSelectionGroupItems();
    for (auto item: my_items) my_scene->removeFromGroupNoUpdate(item);
    QGraphicsView::paintEvent(event);
    for (auto item: my_items) my_scene->addToGroupNoUpdate(item);
    ///QApplication::removePostedEvents(nullptr, 0);


    // ******************** Draw bounding box for each item
    QPainter painter(viewport());
    paintItemOutlines(painter);


    // #################### We aren't messing with anymore scene data after here
    my_scene->scene_mutex.unlock();


    // ******************** Draw box around entire seleciton, with Size Grip handles
    paintBoundingBox(painter);

    // ******************** Draw angles if rotating
    double group_angle = my_scene->getSelectionGroupAsGraphicsItem()->data(User_Roles::Rotation).toDouble();
    paintGroupAngle(painter, group_angle);

    // ******************** If we have some objects selected and created some handles, draw them
    paintHandles(painter, m_handles_shape);


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
QRectF StageGraphicsView::rectAtCenterPoint(QPoint center, double rect_size)
{
    return QRectF(center.x() - rect_size / 2, center.y() - rect_size / 2, rect_size, rect_size);
}


//####################################################################################
//##        PAINT: Draws grid lines
//####################################################################################
void StageGraphicsView::paintGrid()
{
    QPainter painter(viewport());
    painter.setBrush(Qt::NoBrush);

    // Map viewport to scene rect
    QPointF topLeft = mapToScene(0, 0);
    QPointF bottomRight = mapToScene(this->width(), this->height());
    QRectF  scene_rect(topLeft, bottomRight);

    // ********** Draw Grid Lines
    if (m_grid_style == Grid_Style::Lines) {
        painter.setPen(QPen( Dr::GetColor(Window_Colors::Background_Dark), 1 ));
        QVector<QLine> lines;

        // Vertical lines right of scene zero, followed by Vertical lines left of scene zero
        for (qreal x = 0; x <= scene_rect.right(); x += m_grid_x)
            lines.append(QLine(mapFromScene(x, 0).x(), 0, mapFromScene(x, 0).x(), height()));
        for (qreal x = 0; x >= scene_rect.left(); x -= m_grid_x)
            lines.append(QLine(mapFromScene(x, 0).x(), 0, mapFromScene(x, 0).x(), height()));

        // Horizontal lines below scene zero, followed by Horizontal lines above scene zero
        for (qreal y = 0; y <= scene_rect.bottom(); y += m_grid_y)
            lines.append(QLine(0, mapFromScene(0, y).y(), width(), mapFromScene(0, y).y()) );
        for (qreal y = 0; y >= scene_rect.top(); y -= m_grid_y)
            lines.append(QLine(0, mapFromScene(0, y).y(), width(), mapFromScene(0, y).y()) );

        painter.drawLines(lines);

    // ********** Draw Grid Dots
    } else if (m_grid_style == Grid_Style::Dots && m_zoom_scale > .5) {
        qreal dot_size = m_zoom_scale;
        if (m_zoom_scale < 4) dot_size = 4;
        if (m_zoom_scale < 2) dot_size = 3;
        if (m_zoom_scale < 1) dot_size = 2;

        painter.setPen(QPen( Dr::GetColor(Window_Colors::Background_Dark), dot_size, Qt::PenStyle::SolidLine, Qt::PenCapStyle::RoundCap ));
        QVector<QPointF> points;

        // Bottom right
        for (qreal x = 0; x <= scene_rect.right(); x += m_grid_x)
            for (qreal y = 0; y <= scene_rect.bottom(); y += m_grid_y)
                points.append( mapFromScene(x, y) );
        // Bottom left
        for (qreal x = 0; x >= scene_rect.left(); x -= m_grid_x)
            for (qreal y = 0; y <= scene_rect.bottom(); y += m_grid_y)
                points.append( mapFromScene(x, y) );
        // Top right
        for (qreal x = 0; x <= scene_rect.right(); x += m_grid_x)
            for (qreal y = 0; y >= scene_rect.top(); y -= m_grid_y)
                points.append( mapFromScene(x, y) );
        // Top left
        for (qreal x = 0; x >= scene_rect.left(); x -= m_grid_x)
            for (qreal y = 0; y >= scene_rect.top(); y -= m_grid_y)
                points.append( mapFromScene(x, y) );

        painter.drawPoints(points.data(), points.size());
    }
}


//####################################################################################
//##        PAINT: Paints outline around every selected item
//####################################################################################
void StageGraphicsView::paintItemOutlines(QPainter &painter)
{
    StageGraphicsScene *my_scene = dynamic_cast<StageGraphicsScene*>(scene());
    QList<QGraphicsItem*>  my_items = my_scene->getSelectionGroupItems();

    QBrush pen_brush(Dr::GetColor(Window_Colors::Icon_Light));
    painter.setPen(QPen(pen_brush, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.setBrush(Qt::NoBrush);

    for (auto item: my_items) {

        // Apply item scaling / rotation to bounding box
        QPolygonF polygon = item->sceneTransform().map( item->boundingRect() );

        // Convert bounding box to view coordinates and draw on screen
        QPolygon to_view = mapFromScene(polygon);
        painter.drawPolygon(to_view);


        // !!!!! #DEBUG:    Shear Data
        if (Dr::CheckDebugFlag(Debug_Flags::Paint_Shear_Matrix)) {
            double  angle = item->data(User_Roles::Rotation).toDouble();
            QPointF origin = item->mapToScene( item->boundingRect().center() );
            QTransform remove_rotation = QTransform().translate(origin.x(), origin.y()).rotate(-angle).translate(-origin.x(), -origin.y());

            // Draws an unrotated version of item
            QPolygonF poly = item->sceneTransform().map( item->boundingRect() );
            poly = remove_rotation.map(poly);
            painter.setPen(QPen(QBrush(Qt::red), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            painter.drawPolygon( mapFromScene(poly) );
            painter.setPen(QPen(pen_brush, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

            // Draws an unrotated, unskewed version of item
            QTransform item_no_rotate = item->sceneTransform() * remove_rotation;
            poly = item->boundingRect();
            origin = item->boundingRect().center();
            QPointF in_scene = item->sceneTransform().map( item->boundingRect().topLeft() );
            QTransform more = QTransform().rotate(angle).scale(item_no_rotate.m11(), item_no_rotate.m22());
            poly = more.map(poly);
            more = QTransform().translate(in_scene.x(), in_scene.y());
            poly = more.map(poly);
            painter.setPen(QPen(QBrush(Qt::GlobalColor::cyan), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            painter.drawPolygon( mapFromScene(poly) );
            painter.setPen(QPen(pen_brush, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

            // Prints out unrotated item transform
            QTransform t = item_no_rotate;
            qreal m11 = t.m11(), m12 = t.m12();
            qreal m21 = t.m21(), m22 = t.m22();
            if (Dr::IsCloseTo(0, m11, .00001)) m11 = 0;
            if (Dr::IsCloseTo(0, m12, .00001)) m12 = 0;
            if (Dr::IsCloseTo(0, m21, .00001)) m21 = 0;
            if (Dr::IsCloseTo(0, m22, .00001)) m22 = 0;
            m_relay->setLabelText(Label_Names::Label_1, "11: " + QString::number(m11, 'g', 3) + QString("\t\t") +
                                                        "12: " + QString::number(m12, 'g', 3) + QString("\t\t"));
            m_relay->setLabelText(Label_Names::Label_2, "21: " + QString::number(m21, 'g', 3) + QString("\t\t") +
                                                        "22: " + QString::number(m22, 'g', 3) + QString("\t\t"));
        }
        // !!!!! END

    }


    // !!!!! #DEBUG:    Show selection group info
    if (Dr::CheckDebugFlag(Debug_Flags::Label_Selection_Group_Data)) {
        QGraphicsItem *sgroup = my_scene->getSelectionGroupAsGraphicsItem();
        m_relay->setLabelText(Label_Names::Label_Object_1, "Group Pos  X: " +   QString::number(sgroup->sceneBoundingRect().x()) +
                                                                    ", Y: " +   QString::number(sgroup->sceneBoundingRect().y()) );
        m_relay->setLabelText(Label_Names::Label_Object_2, "Group Size X: " +   QString::number(sgroup->sceneBoundingRect().width()) +
                                                                    ", Y: " +   QString::number(sgroup->sceneBoundingRect().height()) );
        m_relay->setLabelText(Label_Names::Label_Object_3, "Group Scale X: " +  QString::number(sgroup->data(User_Roles::Scale).toPointF().x()) +
                                                                     ", Y: " +  QString::number(sgroup->data(User_Roles::Scale).toPointF().y()) );
        m_relay->setLabelText(Label_Names::Label_Object_4, "Group Rotation: " + QString::number(sgroup->data(User_Roles::Rotation).toDouble()) );
        m_relay->setLabelText(Label_Names::Label_Object_5, "Group Z: " +        QString::number(sgroup->zValue()) + QString("\t") +
                                                           "# Items: " +        QString::number(sgroup->childItems().count()) );
    }
    // !!!!! END

}


//####################################################################################
//##        PAINT: Paints bounding box onto view
//####################################################################################
void StageGraphicsView::paintBoundingBox(QPainter &painter)
{
    QGraphicsItem *item = dynamic_cast<StageGraphicsScene*>(scene())->getSelectionGroupAsGraphicsItem();
    painter.setPen(QPen(Dr::GetColor(Window_Colors::Text_Light), 1));

    // ***** Map item bounding box to screen so we can draw it
    QPolygonF polygon(item->boundingRect());                                // Get bounding box of item as polygon
    QTransform transform = item->sceneTransform();                          // Get item bounding box to scene transform
    polygon = transform.map(polygon);                                       // Map bounding box to scene location
    QPolygon to_view = mapFromScene(polygon);                               // Convert bounding box to view coordinates

    // ***** Draw bounding box onto view
    painter.drawLine( QLineF( m_handles_centers[Position_Flags::Rotate], m_handles_centers[Position_Flags::Top]) );
    painter.drawPolygon(to_view);
}


//####################################################################################
//##        PAINT: Paints lines showing rotation while rotating
//####################################################################################
void StageGraphicsView::paintGroupAngle(QPainter &painter, double angle)
{
    if (m_view_mode == View_Mode::Rotating) {
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
}


//####################################################################################
//##        PAINT: Paints handles onto view
//####################################################################################
void StageGraphicsView::paintHandles(QPainter &painter, Handle_Shapes shape_to_draw)
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
//##        PAINT: Draw our Rubber Band selection box with custom colors
//####################################################################################
void StageViewRubberBand::paintEvent(QPaintEvent *)
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












