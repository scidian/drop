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
    QRectF scene_rect(topLeft, bottomRight);

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

        m_interface->setLabelText(Label_Names::Label1, "Test: Fail");

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
    // Go ahead and have base class paint scene (draw items)
    drawGrid();
    QGraphicsView::paintEvent(event);

    // ********** Check if scene view is associated has changed, if so re-connect signals from scene
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

    // If no selected item gets out of here
    if (scene()->selectedItems().count() < 1) return;

    // ********** Draw selection boxes around all selected items
    QPainter painter(viewport());
    QBrush pen_brush(m_interface->getColor(Window_Colors::Icon_Light));
    painter.setPen(QPen(pen_brush, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.setBrush(Qt::NoBrush);

    // Draw bounding box for each item
    for (auto item: scene()->selectedItems()) {
        // Load in item bounding box
        QPolygonF polygon(item->boundingRect());

        // Apply item scaling / rotation to bounding box
        QTransform t = item->transform();
        polygon = t.map(polygon);

        // Move bounding box to item position in the scene
        t.reset();
        t.translate(item->pos().x(), item->pos().y());
        polygon = t.map(polygon);

        // Convert bounding box to view coordinates and draw on screen
        QPolygon to_view = mapFromScene(polygon);
        painter.drawPolygon(to_view);


        // TEMP: Showing object data
        QVariant get_data;
        QPointF my_scale, my_pos;
        double  my_angle;
        get_data = item->data(User_Roles::Position);    my_pos =   get_data.toPointF();
        get_data = item->data(User_Roles::Scale);       my_scale = get_data.toPointF();
        get_data = item->data(User_Roles::Rotation);    my_angle = get_data.toDouble();
        m_interface->setLabelText(Label_Names::LabelObject5, "Pos X: " + QString::number(my_pos.x()) +
                                                             "Pos Y: " + QString::number(my_pos.y()));
        m_interface->setLabelText(Label_Names::LabelObject6, "Scale X: " + QString::number(my_scale.x()) +
                                                             "Scale Y: " + QString::number(my_scale.y()));
        m_interface->setLabelText(Label_Names::LabelObject7, "Rotation: " + QString::number(my_angle));
    }


    // ********** Draw box around entire seleciton, with Size Grip squares
    QRectF bigger = m_selection_rect;
    QPolygon to_view = mapFromScene(bigger);
    QPoint top_left = mapFromScene(bigger.topLeft());
    QPoint bot_right = mapFromScene(bigger.bottomRight());

    ///painter.setCompositionMode(QPainter::CompositionMode::RasterOp_NotDestination);      // Draws with pixels using NOT operation
    painter.setPen(QPen(m_interface->getColor(Window_Colors::Text_Light), 1));
    painter.drawPolygon(to_view);

    int r_half = 3;
    int r_size = r_half * 2;
    ///QLinearGradient linear_grad( QPointF(top_left.x() - r_size, top_left.y() - r_size), QPointF(top_left.x() + r_size, top_left.y() + r_size) );
    ///linear_grad.setColorAt(0, m_interface->getColor(Window_Colors::Icon_Light));
    ///linear_grad.setColorAt(1, m_interface->getColor(Window_Colors::Icon_Dark));
    ///painter.setBrush(QBrush(linear_grad));

    // Set Size Grip rectangles, draw on screen
    painter.setBrush(m_interface->getColor(Window_Colors::Icon_Light));
    m_handles[static_cast<int>(Position_Flags::Top_Left)] =     QRectF(top_left.x() - r_half,  top_left.y() - r_half,  r_size, r_size);
    m_handles[static_cast<int>(Position_Flags::Top_Right)] =    QRectF(bot_right.x() - r_half, top_left.y() - r_half,  r_size, r_size);
    m_handles[static_cast<int>(Position_Flags::Bottom_Left)] =  QRectF(top_left.x() - r_half,  bot_right.y() - r_half, r_size, r_size);
    m_handles[static_cast<int>(Position_Flags::Bottom_Right)] = QRectF(bot_right.x() - r_half, bot_right.y() - r_half, r_size, r_size);
    m_handles[static_cast<int>(Position_Flags::Top)] =    QRectF(top_left.x() + r_half,  top_left.y() - r_half,  bot_right.x() - r_size, r_size);
    m_handles[static_cast<int>(Position_Flags::Bottom)] = QRectF(top_left.x() + r_half,  bot_right.y() - r_half, bot_right.x() - r_size, r_size);
    m_handles[static_cast<int>(Position_Flags::Left)] =   QRectF(top_left.x() - r_half,  top_left.y() + r_half,  r_size, bot_right.y() - r_size);
    m_handles[static_cast<int>(Position_Flags::Right)] =  QRectF(bot_right.x() - r_half,  top_left.y() + r_half,  r_size, bot_right.y() - r_size);
    QVector<QRectF> handles;
    handles.append(m_handles[static_cast<int>(Position_Flags::Top_Left)]);
    handles.append(m_handles[static_cast<int>(Position_Flags::Top_Right)]);
    handles.append(m_handles[static_cast<int>(Position_Flags::Bottom_Left)]);
    handles.append(m_handles[static_cast<int>(Position_Flags::Bottom_Right)]);
    painter.drawRects(handles);

    // Draw angles if rotating
    if (m_view_mode == View_Mode::Rotating) {
        painter.setPen(QPen(m_interface->getColor(Window_Colors::Text_Light), 1));
        painter.drawLine(m_selection_center, m_origin);
        painter.drawLine(m_selection_center, m_last_mouse_pos);
    }

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







