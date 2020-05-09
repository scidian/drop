//
//      Created by Stephens Nunnally on 5/1/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QDebug>

#include "core/colors/colors.h"
#include "editor/helper_library.h"
#include "editor/interface_editor_relay.h"
#include "editor/preferences.h"
#include "editor/world_map/world_map_item.h"
#include "editor/world_map/world_map_scene.h"
#include "editor/world_map/world_map_view.h"
#include "engine/debug_flags.h"
#include "project/dr_project.h"
#include "project/entities/dr_node.h"


//####################################################################################
//##    DrawBackground / DrawForground, called before and after paintEvent
//####################################################################################
void WorldMapView::drawBackground(QPainter *painter, const QRectF &rect) {
    (void) rect;

    paintGrid(*painter);
}

void WorldMapView::drawForeground(QPainter *painter, const QRectF &rect) {
    (void) painter;
    (void) rect;

}



//####################################################################################
//##    PAINT: Main Paint Event for QGraphicsView (WorldMapView)
//####################################################################################
void WorldMapView::paintEvent(QPaintEvent *event) {
    // ***** Pass on event to parent class paint items into scene
    QGraphicsView::paintEvent(event);

    // ***** Make sure we have a scene to paint
    if (scene() == nullptr) return;

    // Initiate QPainter object
    QPainter painter(viewport());

    // Paint Node Lines
    paintNodeLines(painter);
}



//####################################################################################
//##    PAINT: Draws grid lines
//####################################################################################
void WorldMapView::paintGrid(QPainter &painter) {
    // Save current WorldMapView settings
    if (my_scene && my_scene->needRebuild() == false) {
        if (m_editor_relay->getEditorMode() == Editor_Mode::World_Map && Dr::CheckDoneLoading()) {
            QRect   viewport_rect(0, 0, this->viewport()->width(), this->viewport()->height());
            QRectF  visible_scene_rect = this->mapToScene(viewport_rect).boundingRect();
            QPointF center_point = visible_scene_rect.center();
            m_project->setOption(Project_Options::World_Map_Center,   DrPointF(center_point.x(), center_point.y()));
            m_project->setOption(Project_Options::World_Map_Zoom,     m_zoom_scale);
        }
    }

    // Turn off anti aliasing
    bool aliasing = painter.renderHints() & QPainter::Antialiasing;
    painter.setRenderHint(QPainter::Antialiasing, false);

    // Set up QPainter
    QColor  line_color = Dr::ToQColor(Dr::GetColor(Window_Colors::Button_Dark).darker(110));
    QPen    line_pen = QPen(line_color, 1);
            line_pen.setCosmetic(true);
    painter.setBrush(Qt::NoBrush);
    painter.setPen(line_pen);

    // Paint Grid
    QRectF  scene_rect = mapToScene(this->geometry()).boundingRect();

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
//##    Cubic Bezier Curve Control Points
//##        Calculations are designed for left to right node lines
//####################################################################################
std::pair<QPointF, QPointF> WorldMapView::pointsC1C2(QPointF in, QPointF out) {

    const double c_min_offset = 200 * m_zoom_scale;

    QRectF rect = QRectF(out, in);
    double x1, x2;
    double y1 = out.y();
    double y2 = in.y();
    double percent_a = abs(rect.width() / c_min_offset);
    double percent_b = 1.0 - percent_a;

    // Smooth out to input from left to right
    if (in.x() > out.x() + c_min_offset) {
        double x1_a = ((in.x() * 1.0) + (out.x() * 4.0)) / 5.0; ///((in.x() * 1.0) + (out.x() * 1.0)) / 2.0
        double x2_a = ((in.x() * 4.0) + (out.x() * 1.0)) / 5.0; ///((in.x() * 1.0) + (out.x() * 1.0)) / 2.0
        x1 = x1_a;
        x2 = x2_a;

    // Combo from front, ease into it
    } else if (in.x() > out.x()) {
        double x1_a = ((in.x() * 1.0) + (out.x() * 4.0)) / 5.0; ///((in.x() * 1.0) + (out.x() * 1.0)) / 2.0
        double x2_a = ((in.x() * 4.0) + (out.x() * 1.0)) / 5.0; ///((in.x() * 1.0) + (out.x() * 1.0)) / 2.0

        double x1_b = out.x() + abs(rect.width() * 0.2) + (c_min_offset * percent_a);
        double x2_b = in.x()  - abs(rect.width() * 0.2) - (c_min_offset * percent_a);
        x1 = (x1_a * percent_a) + (x1_b * percent_b);
        x2 = (x2_a * percent_a) + (x2_b * percent_b);

    // Combo going back, ease into it
    } else {
        if (percent_a > 1.0) percent_a = 1.0;
        double x1_b = out.x() + abs(rect.width() * 0.2) + (c_min_offset * percent_a);
        double x2_b = in.x()  - abs(rect.width() * 0.2) - (c_min_offset * percent_a);
        x1 = x1_b;
        x2 = x2_b;
    }

    // Returns cubic bezier points C1, C2 in order x1, y2 = upper right, x2,y2 = lower left
    return std::make_pair(QPointF(x1, y1), QPointF(x2, y2));
}


//####################################################################################
//##    PAINT: Draws Node Lines
//####################################################################################
void WorldMapView::paintNodeLines(QPainter &painter) {
    // ***** Turn on anti aliasing if necessary
    if (Dr::CheckDebugFlag(Debug_Flags::Turn_On_Antialiasing_in_Editor)) {
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    }

    // ***** Set up QPainter
    QColor  line_color = Dr::ToQColor(Dr::GetColor(Window_Colors::Icon_Light));
    QPen    line_pen =   QPen(line_color, 2 * m_zoom_scale);
    painter.setPen(line_pen);

    // ***** Get WorldScene
    WorldMapScene *world_scene = dynamic_cast<WorldMapScene*>(scene());
    if (world_scene == nullptr) return;

    // ***** Go through each Node and draw output lines
    QList<WorldMapItem*> map_items = world_scene->worldMapItems();
    for (auto map_item : map_items) {
        DrNode *node = dynamic_cast<DrNode*>(map_item->getEntity());
        if (node == nullptr) continue;

        int slot_number = 0;
        for (auto &node : node->getOutputSlots()) {
            WorldMapItem *connected = WorldMapScene::worldMapItemWithKey(map_items, node.connected_key);
            if (connected == nullptr) continue;

            QRectF  rect_out =  map_item->slotRect(DrSlotType::Output, slot_number);
            QRectF  rect_in =   connected->slotRect(DrSlotType::Input, slot_number);
            QPointF point_out = mapFromScene(map_item->pos()  + rect_out.center());
            QPointF point_in =  mapFromScene(connected->pos() + rect_in.center());

            painter.setBrush(line_color);
            double radius = c_slot_size * m_zoom_scale * 0.35;
            painter.drawEllipse(point_out, radius, radius);
            painter.drawEllipse(point_in,  radius, radius);

            // Cubic Bezier Curve
            painter.setBrush(Qt::NoBrush);
            auto c1c2 = pointsC1C2(point_in, point_out);
            QPainterPath cubic(point_out);
            cubic.cubicTo(c1c2.first, c1c2.second, point_in);
            painter.drawPath(cubic);

            slot_number++;
        }
    }


}   // End paintNodeLines()
























