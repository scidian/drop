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
    QPen p = painter->pen();
    QBrush b = painter->brush();

    painter->setPen(Qt::black);
    painter->setBrush(Qt::NoBrush);

    if (m_grid_style == Grid_Style::Lines) {
        QRect int_rect = rect.toRect();
        for (int i = 0; i <= rect.right(); i += m_grid_x) {      painter->drawLine(i, int_rect.top(), i, int_rect.bottom() + 1);     }
        for (int i = 0; i >= rect.left(); i -= m_grid_x) {       painter->drawLine(i, int_rect.top(), i, int_rect.bottom() + 1);     }
        for (int i = 0; i <= rect.bottom(); i += m_grid_y) {     painter->drawLine(int_rect.left(), i, int_rect.right() + 1, i);     }
        for (int i = 0; i >= rect.top(); i -= m_grid_y) {        painter->drawLine(int_rect.left(), i, int_rect.right() + 1, i);     }
    } else {
        qreal left = int(rect.left()) - (int(rect.left()) % static_cast<int>(m_grid_x));
        qreal top = int(rect.top()) - (int(rect.top()) % static_cast<int>(m_grid_y));
        QVector<QPointF> points;
        for (qreal x = left; x < rect.right(); x += m_grid_x){
            for (qreal y = top; y < rect.bottom(); y += m_grid_y){
                points.append(QPointF(x,y));
            }
        }
        painter->drawPoints(points.data(), points.size());
    }

    painter->setPen(p);
    painter->setBrush(b);
}


//####################################################################################
//##        Paints selection box, etc on top of items
void SceneGraphicsView::paintEvent(QPaintEvent *event)
{
    // Go ahead and have base class paint scene (draw items)
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

    for (auto item: scene()->selectedItems()) {
        QRectF rect = item->sceneBoundingRect();                            // Get item bounding box
        //rect.setWidth(rect.width() * item->transform().m11());            // Transform to item scale
        //rect.setHeight(rect.height() * item->transform().m22());
        //rect.translate(item->pos().x(), item->pos().y());                 // Translate bounding box to proper scene location
        QPolygon to_view = mapFromScene(rect);                              // Convert scene location to view location
        painter.drawPolygon(to_view);                                       // Draw polygon
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







