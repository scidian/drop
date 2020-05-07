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
#include "editor/world_map/world_map_scene.h"
#include "editor/world_map/world_map_view.h"


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


}



//####################################################################################
//##    PAINT: Draws grid lines
//####################################################################################
void WorldMapView::paintGrid(QPainter &painter) {
    updateGrid();

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

}













