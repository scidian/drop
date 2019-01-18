//
//      Created by Stephens Nunnally on 1/8/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Graphics View Paint Overrides
//
//

#include "project.h"
#include "project_world.h"
#include "project_world_scene.h"
#include "project_world_scene_object.h"
#include "editor_scene_item.h"

#include "settings.h"
#include "settings_component.h"
#include "settings_component_property.h"

#include "editor_scene_scene.h"
#include "editor_scene_view.h"
#include "interface_relay.h"


//####################################################################################
//##        Draws grid lines
//####################################################################################
void SceneGraphicsView::drawGrid()
{
    QPainter painter(viewport());
    painter.setBrush(Qt::NoBrush);

    // Map viewport to scene rect
    QPointF topLeft = mapToScene(0, 0);
    QPointF bottomRight = mapToScene(this->width(), this->height());
    QRectF  scene_rect(topLeft, bottomRight);

    // ********** Draw Grid Lines
    if (m_grid_style == Grid_Style::Lines) {
        painter.setPen(QPen( m_relay->getColor(Window_Colors::Background_Dark), 1 ));
        QVector<QLine> lines;

        // Vertical lines to the right of scene zero
        for (qreal x = 0; x <= scene_rect.right(); x += m_grid_x)
            lines.append(QLine(mapFromScene(x, 0).x(), 0, mapFromScene(x, 0).x(), height()));
        // Vertical lines to the left of scene zero
        for (qreal x = 0; x >= scene_rect.left(); x -= m_grid_x)
            lines.append(QLine(mapFromScene(x, 0).x(), 0, mapFromScene(x, 0).x(), height()));

        // Horizontal lines below scene zero
        for (qreal y = 0; y <= scene_rect.bottom(); y += m_grid_y)
            lines.append(QLine(0, mapFromScene(0, y).y(), width(), mapFromScene(0, y).y()) );
        // Horizontal lines above scene zero
        for (qreal y = 0; y >= scene_rect.top(); y -= m_grid_y)
            lines.append(QLine(0, mapFromScene(0, y).y(), width(), mapFromScene(0, y).y()) );

        painter.drawLines(lines);

    // ********** Draw Grid Dots
    } else if (m_grid_style == Grid_Style::Dots && m_zoom_scale > .5) {
        qreal dot_size = m_zoom_scale;
        if (m_zoom_scale < 4) dot_size = 4;
        if (m_zoom_scale < 2) dot_size = 3;
        if (m_zoom_scale < 1) dot_size = 2;

        painter.setPen(QPen( m_relay->getColor(Window_Colors::Background_Dark), dot_size, Qt::PenStyle::SolidLine, Qt::PenCapStyle::RoundCap ));
        QVector<QPointF> points;

        // Bottom right
        for (qreal x = 0; x <= scene_rect.right(); x += m_grid_x) {
            for (qreal y = 0; y <= scene_rect.bottom(); y += m_grid_y)
                points.append( mapFromScene(x, y) );
        }
        // Bottom left
        for (qreal x = 0; x >= scene_rect.left(); x -= m_grid_x) {
            for (qreal y = 0; y <= scene_rect.bottom(); y += m_grid_y)
                points.append( mapFromScene(x, y) );
        }
        // Top right
        for (qreal x = 0; x <= scene_rect.right(); x += m_grid_x) {
            for (qreal y = 0; y >= scene_rect.top(); y -= m_grid_y)
                points.append( mapFromScene(x, y) );
        }
        // Top left
        for (qreal x = 0; x >= scene_rect.left(); x -= m_grid_x) {
            for (qreal y = 0; y >= scene_rect.top(); y -= m_grid_y)
                points.append( mapFromScene(x, y) );
        }

        painter.drawPoints(points.data(), points.size());
    }
}


bool SceneGraphicsView::eventFilter(QObject *obj, QEvent *event)
{
    //int t = event->type();
    //  1 = Timer
    // 11 = Leave
    // 12 = Paint
    // 13 = Move
    // 14 = Resize
    // 17 = Show
    // 24 = Window Activate
    // 74 = Polish Request
    // 78 = Update Later
    //128 = Hover Leave
    //129 = Hover Move
    //if (t != 1 && t != 11 && t != 12 && t != 13 && t != 14 && t != 17 && t != 24 && t != 74 && t != 78 && t != 128 && t != 129)
    //    m_relay->setLabelText(Label_Names::Label_2, QString::number(event->type()));
    return QGraphicsView::eventFilter(obj, event);
}


//####################################################################################
//##        Paints selection box, etc on top of items
//####################################################################################
void SceneGraphicsView::paintEvent(QPaintEvent *event)
{
    // ********** Check if scene that view is associated with has changed, if so re-connect signals from new scene
    if (scene() != m_scene) {
        if (scene() != nullptr) {
            m_scene = scene();
            connect(scene(), SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));
            connect(scene(), SIGNAL(changed(QList<QRectF>)), this, SLOT(sceneChanged(QList<QRectF>)));
            connect(dynamic_cast<SceneGraphicsScene*>(scene()), &SceneGraphicsScene::updateViews, [this]() { update(); });
        }
    }


    // ******************** Go ahead and draw grid first
    drawGrid();


    // ******************** At this point, if no selected item paint objects and get out of here
    SceneGraphicsScene    *my_scene = dynamic_cast<SceneGraphicsScene*>(scene());
    if (my_scene->getSelectionGroupCount() < 1) {
        QGraphicsView::paintEvent(event);
        return;
    }


    // ******************** Otherwise, break apart group to draw items in proper z-Order, then put back together
    QList<QGraphicsItem*>  my_items = my_scene->getSelectionGroupItems();
    for (auto item: my_items) my_scene->removeFromGroupNoUpdate(item);
    QGraphicsView::paintEvent(event);
    for (auto item: my_items) my_scene->addToGroupNoUpdate(item);
    ///QApplication::removePostedEvents(nullptr, 0);


    // ******************** Draw bounding box for each item
    QPainter painter(viewport());
    paintItemOutlines(painter);

    // ******************** Draw box around entire seleciton, with Size Grip handles
    paintBoundingBox(painter);

    // ******************** Draw angles if rotating
    if (m_view_mode == View_Mode::Rotating) {
        painter.setPen(QPen(m_relay->getColor(Window_Colors::Text_Light), 1));
        painter.setCompositionMode(QPainter::CompositionMode::RasterOp_NotDestination);
        painter.drawLine(mapFromScene(m_start_rotate_rect.center()), m_origin);
        painter.drawLine(mapFromScene(m_start_rotate_rect.center()), m_last_mouse_pos);
        painter.setCompositionMode(QPainter::CompositionMode::CompositionMode_SourceOver);
    }

    // ******************** If we have some objects selected and created some handles, draw them
    paintHandles(painter, m_handles_shape);


    // !!!!! #DEBUG:    Draw frames per second
    if (m_relay->debugFlag(Debug_Flags::FPS)) {
        m_relay->setLabelText(Label_Names::Label_3, "Draw Time: " + QString::number(m_debug_timer.elapsed()) );
        m_debug_fps++;
        if (m_debug_timer.elapsed() >= 1000) {
            m_relay->setLabelText(Label_Names::Label_1, "FPS: " + QString::number(m_debug_fps) );
            m_debug_timer.restart();
            m_debug_fps = 0;
        }
    }
    // !!!!! END
}

// Paint helper function, returns a RectF around center point with sides length of rect_size
QRectF SceneGraphicsView::rectAtCenterPoint(QPoint center, double rect_size)
{
    return QRectF(center.x() - rect_size / 2, center.y() - rect_size / 2, rect_size, rect_size);
}


// Paints outline around every selected item
void SceneGraphicsView::paintItemOutlines(QPainter &painter)
{
    QBrush pen_brush(m_relay->getColor(Window_Colors::Icon_Light));
    painter.setPen(QPen(pen_brush, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.setBrush(Qt::NoBrush);

    int first_time = 0;
    for (auto item: dynamic_cast<SceneGraphicsScene*>(scene())->getSelectionGroupItems()) {
        // Load in item bounding box
        QPolygonF polygon(item->boundingRect());

        // Apply item scaling / rotation to bounding box
        polygon = item->sceneTransform().map(polygon);

        // Convert bounding box to view coordinates and draw on screen
        QPolygon to_view = mapFromScene(polygon);
        painter.drawPolygon(to_view);


        // !!!!! #DEBUG:    Showing object data
        if (m_relay->debugFlag(Debug_Flags::Selected_Item_Data) && first_time == 0) {
            QPointF my_scale =  item->data(User_Roles::Scale).toPointF();
            double  my_angle =  item->data(User_Roles::Rotation).toDouble();
            QPointF my_center = item->sceneTransform().map( item->boundingRect().center() );
            m_relay->setLabelText(Label_Names::Label_Position, "Pos X: " +  QString::number(item->pos().x()) +
                                                             ", Pos Y: " +  QString::number(item->pos().y()) );
            m_relay->setLabelText(Label_Names::Label_Center, "Center X: " + QString::number(my_center.x()) +
                                                                  ", Y: " + QString::number(my_center.y()) );
            m_relay->setLabelText(Label_Names::Label_Scale, "Scale X: " +   QString::number(my_scale.x()) +
                                                          ", Scale Y: " +   QString::number(my_scale.y()) );
            m_relay->setLabelText(Label_Names::Label_Rotate, "Rotation: " + QString::number(my_angle));
            m_relay->setLabelText(Label_Names::Label_Z_Order, "Z Order: " + QString::number(item->zValue()) );
            first_time++;
        }
        // !!!!! END


        // !!!!! #DEBUG:    Shear Data
        if (m_relay->debugFlag(Debug_Flags::Shear_Matrix)) {
//        double   angle = item->data(User_Roles::Rotation).toDouble();
//        QPolygonF poly = item->boundingRect(); //item->sceneTransform().map(item->boundingRect());
//        QPointF center = item->boundingRect().center();

//        QTransform remove_rotation = QTransform().translate(center.x(), center.y()).rotate(-angle).translate(-center.x(), -center.y());
//        poly = remove_rotation.map(poly);


//        QTransform item_no_rotate = item->sceneTransform() * remove_rotation;
//        item_no_rotate.shear(1 - item_no_rotate.m21(), 1 - item_no_rotate.m12() );

//        poly = remove_rotation.map(poly);


//        //poly = no_shear.map(poly);


//        painter.setPen(QPen(QBrush(Qt::red), 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
//        painter.drawPolygon( mapFromScene(poly) );
//        painter.setPen(QPen(pen_brush, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));


//        QTransform t = item_no_rotate;
//        qreal m11 = t.m11(), m12 = t.m12(), m13 = t.m13();
//        qreal m21 = t.m21(), m22 = t.m22(), m23 = t.m23();
//        if (isCloseTo(0, m11, .00001)) m11 = 0;
//        if (isCloseTo(0, m12, .00001)) m12 = 0;
//        if (isCloseTo(0, m13, .00001)) m13 = 0;
//        if (isCloseTo(0, m21, .00001)) m21 = 0;
//        if (isCloseTo(0, m22, .00001)) m22 = 0;
//        if (isCloseTo(0, m23, .00001)) m23 = 0;
//        m_relay->setLabelText(Label_Names::Label_1, "11: " + QString::number(m11) + ", 12: " + QString::number(m12) + ", 13: " + QString::number(m13) );
//        m_relay->setLabelText(Label_Names::Label_2, "21: " + QString::number(m21) + ", 22: " + QString::number(m22) + ", 23: " + QString::number(m23) );
        }
        // !!!!! END

    }
}


// Paints bounding box onto view, calculates handle and side resize boxes
void SceneGraphicsView::paintBoundingBox(QPainter &painter)
{
    QGraphicsItem *item = dynamic_cast<SceneGraphicsScene*>(scene())->getSelectionGroupAsGraphicsItem();
    painter.setPen(QPen(m_relay->getColor(Window_Colors::Text_Light), 1));

    // Size of side handle boxes
    double side_size = 6;
    double s_half = side_size / 2;

    // Size of corner handle boxes
    double corner_size = 10;

    // Check if bounding box handles should be squares or circles
    double angle = item->data(User_Roles::Rotation).toDouble();
    if (isSquare(angle, ANGLE_TOLERANCE) == false)  m_handles_shape = Handle_Shapes::Circles;
    else                                            m_handles_shape = Handle_Shapes::Squares;


    // ******************** Set Size Grip rectangles
    // Map item bounding box to screen so we can draw it
    QPolygonF polygon(item->boundingRect());                                // Get bounding box of item as polygon
    QTransform transform = item->sceneTransform();                          // Get item bounding box to scene transform
    polygon = transform.map(polygon);                                       // Map bounding box to scene location
    QPolygon to_view = mapFromScene(polygon);                               // Convert bounding box to view coordinates

    // ***** Store corner handle polygons
    QPointF top_left =  transform.map(item->boundingRect().topLeft());
    QPointF top_right = transform.map(item->boundingRect().topRight());
    QPointF bot_left =  transform.map(item->boundingRect().bottomLeft());
    QPointF bot_right = transform.map(item->boundingRect().bottomRight());

    // Store view coodinate rectangles of corners for size grip handles
    m_handles[Position_Flags::Top_Left] =     rectAtCenterPoint( mapFromScene( top_left  ), corner_size);
    m_handles[Position_Flags::Top_Right] =    rectAtCenterPoint( mapFromScene( top_right ), corner_size);
    m_handles[Position_Flags::Bottom_Left] =  rectAtCenterPoint( mapFromScene( bot_left  ), corner_size);
    m_handles[Position_Flags::Bottom_Right] = rectAtCenterPoint( mapFromScene( bot_right ), corner_size);

    // ***** Store side box handle polygons
    QPointF center = item->boundingRect().center();
    center = transform.map(center);
    QTransform remove_rotation = QTransform().translate(center.x(), center.y()).rotate(-angle).translate(-center.x(), -center.y());

    // Remove rotation from bounding box in scene, map bounding box scene coordinates to view coordinates
    top_left =  mapFromScene( remove_rotation.map(top_left) );
    top_right=  mapFromScene( remove_rotation.map(top_right) );
    bot_left =  mapFromScene( remove_rotation.map(bot_left) );
    bot_right = mapFromScene( remove_rotation.map(bot_right) );

    // Create bounding box rectangles
    QRectF temp_top    ( QPointF(top_left.x() + s_half,  top_left.y() - s_half),
                         QPointF(top_right.x() - s_half, top_right.y() + s_half) );
    QRectF temp_bottom ( QPointF(bot_left.x() + s_half,  bot_left.y() - s_half),
                         QPointF(bot_right.x() - s_half, bot_right.y() + s_half));
    QRectF temp_left   ( QPointF(top_left.x() - s_half,  top_left.y() + s_half),
                         QPointF(bot_left.x() + s_half,  bot_left.y() + s_half));
    QRectF temp_right  ( QPointF(top_right.x() - s_half, top_right.y() + s_half),
                         QPointF(bot_right.x() + s_half, bot_right.y() - s_half));

    // Add rotation back onto side boxes
    center = QLineF(top_left, bot_right).pointAt(.5);
    QTransform add_rotation =    QTransform().translate(center.x(), center.y()).rotate( angle).translate(-center.x(), -center.y());
    m_handles[Position_Flags::Top] =    add_rotation.map(temp_top);
    m_handles[Position_Flags::Bottom] = add_rotation.map(temp_bottom);
    m_handles[Position_Flags::Left] =   add_rotation.map(temp_left);
    m_handles[Position_Flags::Right] =  add_rotation.map(temp_right);

    // Store polygon centers for use later in paintHandles
    for (auto h : m_handles) m_handles_centers[h.first] = h.second.boundingRect().center();

    // Draw bounding box onto view
    painter.drawPolygon(to_view);
}


// Paints handles into view
void SceneGraphicsView::paintHandles(QPainter &painter, Handle_Shapes shape_to_draw)
{
    painter.setBrush(m_relay->getColor(Window_Colors::Icon_Light));

    QVector<QPointF> handles;
    for (int i = 0; i < static_cast<int>(Position_Flags::Total); i++)
        handles.append(m_handles_centers[static_cast<Position_Flags>(i)]);

    double handle_size = 8;
    for (auto r : handles) {
        QRectF to_draw;
        to_draw.setX(r.x() - (handle_size / 2));
        to_draw.setY(r.y() - (handle_size / 2));
        to_draw.setWidth(handle_size);
        to_draw.setHeight(handle_size);
        switch (shape_to_draw)
        {
        case Handle_Shapes::Circles:    painter.drawPixmap(to_draw, p_circle, p_circle.rect());     break;
        case Handle_Shapes::Squares:    painter.drawPixmap(to_draw, p_square, p_square.rect());     break;
        }
    }
    ///// Draw polygons for all handles
    ///for (auto h : m_handles) painter.drawPolygon(h.second);
}




//####################################################################################
//##        Draw our Rubber Band selection box with custom colors
//####################################################################################
void SceneViewRubberBand::paintEvent(QPaintEvent *)
{
    QStylePainter painter(this);

    QColor bg = m_relay->getColor(Window_Colors::Icon_Light);
    bg.setAlpha(48);
    QBrush brush;
    brush.setStyle(Qt::BrushStyle::SolidPattern);
    brush.setColor(bg);

    painter.setBrush(brush);
    painter.setPen(QPen(QBrush(m_relay->getColor(Window_Colors::Icon_Light)), 2, Qt::PenStyle::SolidLine));
    painter.drawRect(this->rect());
}












