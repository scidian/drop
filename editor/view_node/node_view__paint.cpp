//
//      Created by Stephens Nunnally on 5/1/2020, (c) 2020 Scidian Studios, All Rights Reserved
//
//  File:
//
//
//
#include <QDebug>

#include "core/colors/colors.h"
#include "core/dr_math.h"
#include "editor/helper_library.h"
#include "editor/interface_editor_relay.h"
#include "editor/preferences.h"
#include "editor/view_node/node_item.h"
#include "editor/view_node/node_scene.h"
#include "editor/view_node/node_view.h"
#include "engine/debug_flags.h"
#include "project/dr_project.h"
#include "project/settings/settings_component.h"
#include "project/settings/settings_component_slot.h"


//####################################################################################
//##    DrawBackground / DrawForground, called before and after paintEvent
//####################################################################################
void NodeView::drawBackground(QPainter *painter, const QRectF &rect) {
    (void) rect;

    paintGrid(*painter);
}

void NodeView::drawForeground(QPainter *painter, const QRectF &rect) {
    (void) painter;
    (void) rect;

}


//####################################################################################
//##    PAINT: Main Paint Event for QGraphicsView (NodeView)
//####################################################################################
void NodeView::paintEvent(QPaintEvent *event) {
    // ***** Pass on event to parent class paint items into scene
    QGraphicsView::paintEvent(event);

    // ***** Make sure we have a scene to paint
    if (scene() == nullptr) return;

    // Initiate QPainter object
    QPainter painter(viewport());

    // Paint Node Lines
    paintNodeLines(painter);

    QPointF center_point = this->mapToScene(this->viewport()->geometry().center());
    emit updateCenterPointX( center_point.x());
    emit updateCenterPointY(-center_point.y());
}


//####################################################################################
//##    PAINT: Draws grid lines
//####################################################################################
void NodeView::paintGrid(QPainter &painter) {
    // Save current World Graph view settings
    if (my_scene && my_scene->needRebuild() == false) {
        if (m_editor_relay->getEditorMode() == Editor_Mode::World_Graph && Dr::CheckDoneLoading()) {
            QRect   viewport_rect(0, 0, this->viewport()->width(), this->viewport()->height());
            QRectF  visible_scene_rect = this->mapToScene(viewport_rect).boundingRect();
            QPointF center_point = visible_scene_rect.center();
            m_project->setOption(Project_Options::World_Graph_Center,   DrPointF(center_point.x(), center_point.y()));
            m_project->setOption(Project_Options::World_Graph_Zoom,     m_zoom_scale);
        }
    }

    // Turn off anti aliasing
    bool aliasing = painter.renderHints() & QPainter::Antialiasing;
    painter.setRenderHint(QPainter::Antialiasing, false);

    // Set up QPainter
    QColor  line_color = Dr::ToQColor(Dr::GetColor(Window_Colors::Button_Dark).darker(110));
    QPen    line_pen = QPen(line_color, Dr::Scale(1.0));
            line_pen.setCosmetic(true);
    painter.setBrush(Qt::NoBrush);
    painter.setPen(line_pen);

    // Paint Grid
    QRectF  scene_rect = mapToScene(this->viewport()->rect()).boundingRect();

    // Right Lines
    for (int x = 0; x < scene_rect.right(); x += m_grid_size.x()) {
        painter.drawLine(x, scene_rect.top(), x, scene_rect.bottom());
    }
    // Left Lines
    for (int x = -m_grid_size.x(); x > scene_rect.left(); x -= m_grid_size.x()) {
        painter.drawLine(x, scene_rect.top(), x, scene_rect.bottom());
    }

    // Bottom Lines
    for (int y = 0; y < scene_rect.bottom(); y += m_grid_size.y()) {
        painter.drawLine(scene_rect.left(), y, scene_rect.right(), y);
    }
    // Top Lines
    for (int y = -m_grid_size.y(); y > scene_rect.top(); y -= m_grid_size.y()) {
        painter.drawLine(scene_rect.left(), y, scene_rect.right(), y);
    }

    painter.setRenderHint(QPainter::Antialiasing, aliasing);
}   // End paintGrid()


//####################################################################################
//##    PAINT: Draws Node Lines
//####################################################################################
void NodeView::paintNodeLines(QPainter &painter) {
    // ***** Turn on anti aliasing if necessary
    if (Dr::CheckDebugFlag(Debug_Flags::Turn_On_Antialiasing_in_Editor)) {
        painter.setRenderHint(QPainter::Antialiasing, true);
    }

    // ***** Get WorldScene
    if (my_scene == nullptr) return;

    // **** Get Items, make sure they have DrComponents
    QList<NodeItem*> map_items;
    for (auto map_item : my_scene->nodeItems()) {
        if (map_item->getComponent() == nullptr) continue;
        map_items.push_back(map_item);
    }

    // ***** Reset Signal Slot connection counts
    for (auto map_item : map_items) {
        for (auto &signal_pair : map_item->getComponent()->getSignalMap()) {
            signal_pair.second->clearRecentLineSlots();
        }
    }


    // ***** Highlight Mouse Slot
    QColor  text_color = Dr::ToQColor(Dr::GetColor(Window_Colors::Text));
    QPen    slot_pen(text_color, 2.5 * m_zoom_scale, Qt::PenStyle::SolidLine, Qt::PenCapStyle::RoundCap, Qt::PenJoinStyle::BevelJoin);
    painter.setPen( slot_pen );
    painter.setBrush( Qt::NoBrush );
    if (m_last_mouse_slot != nullptr) {
        // If m_view_mode == View_Mode::Node_Connect, make sure highlight Slot is compatible with mouse down slot
        bool should_draw = true;
        if (m_slot_start != nullptr) {
            if (m_slot_start->getParentComponent() == m_last_mouse_slot->getParentComponent()) should_draw = false;
            if (m_slot_start->getSlotType() == m_last_mouse_slot->getSlotType()) should_draw = false;
            if (m_slot_start == m_last_mouse_slot) should_draw = true;
        }
        // Highlight mouse over slot
        if (should_draw) {
            NodeItem *mouse_item = NodeScene::nodeItemWithKey(map_items, m_last_mouse_slot->getParentSettings()->getKey());
            if (mouse_item != nullptr) {
                QRectF scene_rect = mouse_item->slotSceneRect(m_last_mouse_slot);
                       scene_rect.adjust(c_circle_reduce, c_circle_reduce, -c_circle_reduce, -c_circle_reduce);
                painter.drawEllipse( mapFromScene(scene_rect).boundingRect() );
            }
        }
    }


    // ***** Set up QPainter for Node Lines
    QColor  line_color = Dr::ToQColor(Dr::GetColor(Window_Colors::Icon_Light));
    QPen    line_pen =   QPen(line_color, 2 * m_zoom_scale);

    // ***** Go through each GraphicsItem and draw output lines
    for (auto map_item : map_items) {
        // Go through each Output Slot
        int output_slot_number = 0;
        for (auto &output_pair : map_item->getComponent()->getOutputMap()) {
            DrSlot *slot = output_pair.second;
                    slot->clearRecentLineSlots();

            // Go through each connection of Output Slot
            for (auto connection : slot->connections()) {

                // Find signal (input) slot to connect to
                NodeItem *connected = NodeScene::nodeItemWithKey(map_items, connection.connected_entity_key);
                           if (connected == nullptr) continue;
                DrComponent   *connected_component = connected->getComponent();
                           if (connected_component == nullptr) continue;
                DrSlot        *connected_slot = nullptr;
                int slot_number_to_connect_to = 0;
                for (auto &signal_pair : connected_component->getSignalMap()) {
                    if (signal_pair.second->getSlotKey() == connection.connected_signal_key) {
                        connected_slot = signal_pair.second;
                        break;
                    }
                    ++slot_number_to_connect_to;
                }
                if (connected_slot == nullptr) continue;

                // Get Slot Locations
                QRectF  rect_out =  map_item->slotRect(DrSlotType::Output,  output_slot_number);
                QRectF  rect_in =   connected->slotRect(DrSlotType::Signal, slot_number_to_connect_to);
                QPointF point_out = mapFromScene(map_item->pos()  + rect_out.center());
                QPointF point_in =  mapFromScene(connected->pos() + rect_in.center());

                // Paint Curve
                paintCubicCurve(painter, line_pen, point_in, point_out, true);

                // Each slot has a list of other slots it was drawn to each frame for reference elsewhere
                slot->addRecentLineSlot(connected_slot);
                connected_slot->addRecentLineSlot(slot);
            }
            ++output_slot_number;
        }
    }


    // ***** If mouse is dragging a node line, paint it
    if (m_view_mode == View_Mode::Node_Connect) {
        // Find start slot scene position
        QPointF slot_point(0, 0);
        NodeItem *connected = NodeScene::nodeItemWithKey(map_items, m_slot_start->getParentSettings()->getKey());
        if (connected != nullptr) {
            slot_point = mapFromScene( connected->slotSceneRect(m_slot_start).center() );

            // Highlight mouse down Slot
            painter.setPen( slot_pen );
            painter.setBrush( Qt::NoBrush );
            QRectF scene_rect = connected->slotSceneRect(m_slot_start);
                   scene_rect.adjust(c_circle_reduce, c_circle_reduce, -c_circle_reduce, -c_circle_reduce);
            painter.drawEllipse( mapFromScene(scene_rect).boundingRect() );

            // Draw curve from mouse down Slot to mouse position
            if (m_slot_start->getSlotType() == DrSlotType::Signal) {
                paintCubicCurve(painter, line_pen, slot_point, m_last_mouse_pos, true);
            } else {
                paintCubicCurve(painter, line_pen, m_last_mouse_pos, slot_point, true);
            }
        }
    }

}   // End paintNodeLines()


//####################################################################################
//##    Cubic Bezier Curve Control Points
//##        Calculations are designed for left to right node lines
//####################################################################################
std::pair<QPointF, QPointF> NodeView::pointsC1C2(QPointF in, QPointF out) {

    const double c_min_offset = 150 * m_zoom_scale;

    QRectF rect = QRectF(out, in);
    double x1, x2;
    double y1 = out.y();
    double y2 = in.y();
    double percent_a =  abs(rect.width() / c_min_offset);
    double percent_b =  1.0 - percent_a;

    // Calculate control point x values based on midpoint
    double x1_a = (in.x() + out.x()) / 2.0; ///((in.x() * 1.0) + (out.x() * 5.0)) / 6.0;
    double x2_a = (in.x() + out.x()) / 2.0; ///((in.x() * 5.0) + (out.x() * 1.0)) / 6.0;

    // When wrapping behind, calculate control point x values with a wrap around curve width included
    double curve_percent = Dr::Clamp(percent_a * 0.5, 0.2, 1.0);
    double curve = abs(rect.width() * 0.15) + (c_min_offset * curve_percent);
    double x1_b = out.x() + curve;
    double x2_b = in.x()  - curve;

    // INPUT FAR RIGHT - Smooth out to input from left to right
    if (in.x() > out.x() + c_min_offset) {
        x1 = x1_a;
        x2 = x2_a;

    // INPUT TO THE RIGHT AT ALL - Combo from front, ease into it
    } else if (in.x() > out.x()) {
        x1 = (x1_a * percent_a) + (x1_b * percent_b);
        x2 = (x2_a * percent_a) + (x2_b * percent_b);

    // INPUT TO THE LEFT - Combo going back, ease into it
    } else {
        x1 = x1_b;
        x2 = x2_b;
    }

    // Returns cubic bezier points C1, C2 in order x1, y2 = upper right, x2,y2 = lower left
    return std::make_pair(QPointF(x1, y1), QPointF(x2, y2));
}


//####################################################################################
//##    PAINT: Draws Node Lines
//####################################################################################
void NodeView::paintCubicCurve(QPainter &painter, QPen &line_pen, QPointF point_in, QPointF point_out, bool paint_dots) {
    // Calculate Cubic Bezier Curve
    painter.setPen(line_pen);
    painter.setBrush(Qt::NoBrush);
    auto c1c2 = pointsC1C2(point_in, point_out);

    QPainterPath cubic(point_out);
    cubic.cubicTo(c1c2.first, c1c2.second, point_in);

    // Draw Horizontal line if y values are the same
    if (Dr::IsCloseTo(point_out.y(), point_in.y(), 0.001)) {
        painter.drawLine(cubic.boundingRect().topLeft(), cubic.boundingRect().topRight());
    // Draw Cubic Bezier Curve
    } else {
        painter.drawPath(cubic);
    }

    // Draw curve end point dots
    if (paint_dots) {
        painter.setBrush(line_pen.color());
        double radius = c_slot_size * m_zoom_scale * 0.1;
        painter.drawEllipse(point_out, radius, radius);
        painter.drawEllipse(point_in,  radius, radius);
    }
}























