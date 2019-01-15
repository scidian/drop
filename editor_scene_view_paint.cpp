//
//      Created by Stephens Nunnally on 1/8/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Graphics View Paint Overrides
//
//

#include "01_project.h"
#include "02_world.h"
#include "03_scene.h"
#include "04_object.h"
#include "05_item.h"

#include "30_settings.h"
#include "31_component.h"
#include "32_property.h"

#include "editor_scene_view.h"
#include "interface_relay.h"


//####################################################################################
//##        Draws grid lines
void SceneGraphicsView::drawBackground(QPainter *painter, const QRectF &rect)
{
    Q_UNUSED(painter);
    Q_UNUSED(rect);
}

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
        painter.setPen(QPen( m_interface->getColor(Window_Colors::Background_Dark), 1 ));
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

        painter.setPen(QPen( m_interface->getColor(Window_Colors::Background_Dark), dot_size, Qt::PenStyle::SolidLine, Qt::PenCapStyle::RoundCap ));
        QVector<QPointF> points;

        // !!!!! TEMP:
        m_interface->setLabelText(Label_Names::Label_1, "Test: Fail");
        // !!!!! END

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



//####################################################################################
//##        Paints selection box, etc on top of items
void SceneGraphicsView::paintEvent(QPaintEvent *event)
{
    // Go ahead and draw grid and then have base class paint scene (draw items)
    drawGrid();
    QGraphicsView::paintEvent(event);

    // ********** Check if scene that view is associated with has changed, if so re-connect signals from new scene
    if (scene() != m_scene) {
        if (scene() != nullptr) {
            m_scene = scene();
            connect(scene(), SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));
            connect(scene(), SIGNAL(changed(QList<QRectF>)), this, SLOT(sceneChanged(QList<QRectF>)));

            ///// Convert pointer to QString, display in label
            ///QString ptrStr = QString("0x%1").arg((quintptr)m_scene, QT_POINTER_SIZE * 2, 16, QChar('0'));
            ///m_interface->setLabelText(Label_Names::LabelObject3, "Changed: " + ptrStr);
        }
    }

    // At this point, if no selected item gets out of here
    if (scene()->selectedItems().count() < 1) return;

    // Set up painter object
    QPainter painter(viewport());
    QBrush pen_brush(m_interface->getColor(Window_Colors::Icon_Light));
    painter.setPen(QPen(pen_brush, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.setBrush(Qt::NoBrush);

    // ******************** Draw bounding box for each item
    for (auto item: scene()->selectedItems()) {
        // Load in item bounding box
        QPolygonF polygon(item->boundingRect());

        // Apply item scaling / rotation to bounding box
        QTransform t = item->sceneTransform();
        polygon = t.map(polygon);

        // Convert bounding box to view coordinates and draw on screen
        QPolygon to_view = mapFromScene(polygon);
        painter.drawPolygon(to_view);



        // !!!!! TEMP: Showing object data
        QVariant get_data;
        QPointF my_scale, my_pos, my_center;
        double  my_angle;
        get_data = item->data(User_Roles::Position);    my_pos =   QPointF(item->pos().x(), item->pos().y()); //get_data.toPointF();
        get_data = item->data(User_Roles::Scale);       my_scale = get_data.toPointF();
        get_data = item->data(User_Roles::Rotation);    my_angle = get_data.toDouble();
        my_center = item->boundingRect().center();
        t = item->sceneTransform();
        my_center = t.map(my_center);

        m_interface->setLabelText(Label_Names::Label_Position, "Pos X: " + QString::number(my_pos.x()) +
                                                             ", Pos Y: " + QString::number(my_pos.y()));
        m_interface->setLabelText(Label_Names::Label_Center, "Center X: " + QString::number(my_center.x()) +
                                                                  ", Y: " + QString::number(my_center.y()));
        m_interface->setLabelText(Label_Names::Label_Scale, "Scale X: " + QString::number(my_scale.x()) +
                                                          ", Scale Y: " + QString::number(my_scale.y()));
        m_interface->setLabelText(Label_Names::Label_Rotate, "Rotation: " + QString::number(my_angle));
        m_interface->setLabelText(Label_Names::Label_Z_Order, "Z Order: " + QString::number(item->zValue()));
        // !!!!! END
    }


    // ******************** Draw box around entire seleciton, with Size Grip squares
    painter.setPen(QPen(m_interface->getColor(Window_Colors::Text_Light), 1));

    double r_half = 3;                  // Radius of square handle
    double r_size = r_half * 2;
    double c_size = 4;                  // Radius of circle handle

    // Check if we should draw bounding box, and if it should be squares or circles
    bool draw_box = false;
    bool do_squares = true;
    bool more_than_one = false;

    if (scene()->selectedItems().count() > 0) draw_box = true;
    if (scene()->selectedItems().count() == 1) {
         double angle = scene()->selectedItems().first()->data(User_Roles::Rotation).toDouble();
         if (isSquare(angle, ANGLE_TOLERANCE) == false) do_squares = false;
    }
    if (scene()->selectedItems().count() > 1) more_than_one = true;

    // ***** Set Size Grip rectangles
    QPolygon to_view;

    // If multiple items use one big rectangle
    if (draw_box && more_than_one) {
        QRectF bigger = m_selection_rect;
        to_view = mapFromScene(bigger);

        QPoint top_left = mapFromScene(bigger.topLeft());
        QPoint bot_right = mapFromScene(bigger.bottomRight());
        double select_width =  abs(bot_right.x() - top_left.x());
        double select_height = abs(bot_right.y() - top_left.y());

        m_handles[Position_Flags::Top_Left] =     QRectF(top_left.x() - r_half,  top_left.y() - r_half,  r_size, r_size);
        m_handles[Position_Flags::Top_Right] =    QRectF(bot_right.x() - r_half, top_left.y() - r_half,  r_size, r_size);
        m_handles[Position_Flags::Bottom_Left] =  QRectF(top_left.x() - r_half,  bot_right.y() - r_half, r_size, r_size);
        m_handles[Position_Flags::Bottom_Right] = QRectF(bot_right.x() - r_half, bot_right.y() - r_half, r_size, r_size);

        m_handles[Position_Flags::Top] =    QRectF(top_left.x() + r_half,  top_left.y() - r_half,  select_width - r_size, r_size);
        m_handles[Position_Flags::Bottom] = QRectF(top_left.x() + r_half,  bot_right.y() - r_half, select_width - r_size, r_size);
        m_handles[Position_Flags::Left] =   QRectF(top_left.x() - r_half,  top_left.y() + r_half,  r_size, select_height - r_size);
        m_handles[Position_Flags::Right] =  QRectF(bot_right.x() - r_half,  top_left.y() + r_half,  r_size, select_height- r_size);
    }

    // If one item, map item shape
    else if (draw_box) {
        // Map item bounding box to screen and draw it
        QGraphicsItem *item = scene()->selectedItems().first();     // Grab only selected item
        QPolygonF polygon(item->boundingRect());                    // Get bounding box of item as polygon

        QTransform transform = item->sceneTransform();              // Get item bounding box to scene transform
        polygon = transform.map(polygon);                           // Map bounding box to scene location
        to_view = mapFromScene(polygon);                            // Convert bounding box to view coordinates

        // Store item bounding box points
        QPointF top_left =  item->boundingRect().topLeft();
        QPointF top_right = item->boundingRect().topRight();
        QPointF bot_left =  item->boundingRect().bottomLeft();
        QPointF bot_right = item->boundingRect().bottomRight();

        // Store view coodinate rectangles of corners for size grip handles
        m_handles[Position_Flags::Top_Left] =     rectAtCenterPoint(mapFromScene( transform.map(top_left)  ), r_size);
        m_handles[Position_Flags::Top_Right] =    rectAtCenterPoint(mapFromScene( transform.map(top_right) ), r_size);
        m_handles[Position_Flags::Bottom_Left] =  rectAtCenterPoint(mapFromScene( transform.map(bot_left)  ), r_size);
        m_handles[Position_Flags::Bottom_Right] = rectAtCenterPoint(mapFromScene( transform.map(bot_right) ), r_size);

        // Store sides as rects
        QRectF temp_top    (top_left.x() + r_half,  top_left.y() - r_half,  item->boundingRect().width() - r_size, r_size);
        QRectF temp_bottom (top_left.x() + r_half,  bot_right.y() - r_half, item->boundingRect().width() - r_size, r_size);
        QRectF temp_left   (top_left.x() - r_half,  top_left.y() + r_half,  r_size, item->boundingRect().height() - r_size);
        QRectF temp_right  (bot_right.x() - r_half,  top_left.y() + r_half,  r_size, item->boundingRect().height() - r_size);

        // Change rects into polygons, then apply item scene transform, and then map to view and store
        QPolygonF top(temp_top), bottom(temp_bottom), left(temp_left), right(temp_right);
        m_handles[Position_Flags::Top] = mapFromScene(transform.map(top));
        m_handles[Position_Flags::Bottom] = mapFromScene(transform.map(bottom));
        m_handles[Position_Flags::Left] = mapFromScene(transform.map(left));
        m_handles[Position_Flags::Right] = mapFromScene(transform.map(right));
    }

    if (draw_box){
        // Store polygon centers
        for (auto h : m_handles) m_handles_centers[h.first] = h.second.boundingRect().center();

        // Draw bounding box
        painter.drawPolygon(to_view);
    }

    // ********** Draw angles if rotating
    if (m_view_mode == View_Mode::Rotating) {
        painter.setPen(QPen(m_interface->getColor(Window_Colors::Text_Light), 1));
        painter.setCompositionMode(QPainter::CompositionMode::RasterOp_NotDestination);
        painter.drawLine(mapFromScene(m_start_resize_rect.center()), m_origin);
        painter.drawLine(mapFromScene(m_start_resize_rect.center()), m_last_mouse_pos);
        painter.setCompositionMode(QPainter::CompositionMode::CompositionMode_SourceOver);
    }


    // ********** If we have some objects selected and created some handles, draw them
    if (draw_box) {
        painter.setBrush(m_interface->getColor(Window_Colors::Icon_Light));

        QVector<QPointF> handles;
        for (int i = 0; i < static_cast<int>(Position_Flags::Total); i++)
            handles.append(m_handles_centers[static_cast<Position_Flags>(i)]);

        QPixmap p;
        if (do_squares == false)
            p = QPixmap(":/gui_misc/handle_circle.png");
        else
            p = QPixmap(":/gui_misc/handle_square.png");

//        for (auto r : handles) {
//            QRectF to_draw;
//           to_draw.setX(r.x() - c_size);
//            to_draw.setY(r.y() - c_size);
//            to_draw.setWidth(c_size * 2);
//            to_draw.setHeight(c_size * 2);
//            painter.drawPixmap(to_draw, p, p.rect());
//            //painter.drawEllipse(r.center(), c_size, c_size);              // Circles
//            //painter.drawPolygons(handles);                                // Squares
//        }

        for (auto h : m_handles) {

            painter.drawPolygon(h.second);

        }

    }



    // !!!!! TEMP:
    painter.setPen(QPen(Qt::red, 3));
    painter.drawPolygon(m_temp_polygon);

    painter.setPen(QPen(Qt::blue, 3));
    painter.drawPolygon(m_temp_polygon2);
    // !!!!! END

}

QRectF SceneGraphicsView::rectAtCenterPoint(QPoint center, double rect_size)
{
    return QRectF(center.x() - rect_size / 2, center.y() - rect_size / 2, rect_size, rect_size);
}





//####################################################################################
//##        Draw our Rubber Band selection box with custom colors
void SceneViewRubberBand::paintEvent(QPaintEvent *)
{
    QColor bg = m_interface->getColor(Window_Colors::Icon_Light);
    QStylePainter painter(this);

    QPen pen;
    pen.setStyle(Qt::PenStyle::SolidLine); //DashLine);
    pen.setWidth(2);
    pen.setColor(bg);
    QBrush brush;
    brush.setStyle(Qt::BrushStyle::SolidPattern);
    bg.setAlpha(64);
    brush.setColor(bg);

    painter.setPen(pen);
    painter.setBrush(brush);
    painter.drawRect(this->rect());
}







