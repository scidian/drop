//
//      Created by Stephens Nunnally on 2/27/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrView Grid Functions
//
//
#include <QtMath>

#include "colors.h"
#include "debug.h"
#include "editor_view.h"
#include "editor_scene.h"
#include "library.h"
#include "project.h"
#include "project_world_stage.h"


//####################################################################################
//##        Updates grid based on current stage shown, recalculates points and lines
//####################################################################################
void DrView::updateGrid()
{
    if (!scene()) return;
    if (!my_scene->getCurrentStageShown()) return;

    m_grid_origin =  my_scene->getCurrentStageShown()->getComponentPropertyValue(Components::Stage_Grid, Properties::Stage_Grid_Origin_Point).toPointF();
    m_grid_size =    my_scene->getCurrentStageShown()->getComponentPropertyValue(Components::Stage_Grid, Properties::Stage_Grid_Size).toPointF();
    m_grid_rotate =  my_scene->getCurrentStageShown()->getComponentPropertyValue(Components::Stage_Grid, Properties::Stage_Grid_Rotation).toDouble();
    int style =      my_scene->getCurrentStageShown()->getComponentPropertyValue(Components::Stage_Grid, Properties::Stage_Grid_Style).toInt();
    m_grid_style =   static_cast<Grid_Style>(style);
    m_grid_should_snap =  my_scene->getCurrentStageShown()->getComponentPropertyValue(Components::Stage_Grid, Properties::Stage_Grid_Should_Snap).toBool();
    m_grid_show_on_top =  my_scene->getCurrentStageShown()->getComponentPropertyValue(Components::Stage_Grid, Properties::Stage_Grid_Show_On_Top).toBool();

    recalculateGrid();

    update();

    ///Dr::SetLabelText(Label_Names::Label_1, "Updated Grid at: " + Dr::CurrentTimeAsString());
}

//####################################################################################
//##        Recalculates grid points into local QVectors, m_grid_points & m_grid_lines
//####################################################################################
void DrView::recalculateGrid()
{
    // Map viewport to scene rect
    QPointF topLeft = mapToScene(0, 0);
    QPointF bottomRight = mapToScene(this->width(), this->height());
    QRectF  viewport_rect(topLeft, bottomRight);

    QRectF  scene_rect = this->scene()->sceneRect();

    double grid_x = m_grid_size.x();
    double grid_y = m_grid_size.y();

    double min_x = (scene_rect.left()   > viewport_rect.left())   ? scene_rect.left()   : viewport_rect.left();
    double max_x = (scene_rect.right()  < viewport_rect.right())  ? scene_rect.right()  : viewport_rect.right();
    double min_y = (scene_rect.top()    > viewport_rect.top())    ? scene_rect.top()    : viewport_rect.top();
    double max_y = (scene_rect.bottom() < viewport_rect.bottom()) ? scene_rect.bottom() : viewport_rect.bottom();

    // !!!!!!!!!! Figure out how to start drawing when scene barely is in view in one corner

    double origin_x = round((viewport_rect.center().x() - m_grid_origin.x()) / grid_x) * grid_x + m_grid_origin.x();
    double origin_y = round((viewport_rect.center().y() - m_grid_origin.y()) / grid_y) * grid_y + m_grid_origin.y();

    bool allow_dots = true;
    if ((m_zoom_scale <= .50) && (grid_x < 10 || grid_y < 10)) allow_dots = false;
    if ((m_zoom_scale <= .25) && (grid_x < 25 || grid_y < 25)) allow_dots = false;
    if (m_grid_style != Grid_Style::Dots) allow_dots = false;

    QTransform t = QTransform().rotate(m_grid_rotate);
    QVector<QPointF> new_points;
    QVector<QLineF>  new_lines;

    // Bottom right
    for (double x = origin_x; x <= max_x; x += grid_x) {
        // Right side vertical lines
        new_lines.append( QLineF(x, min_y, x, max_y) );

        if (!allow_dots) continue;
        for (double y = origin_y; y <= max_y; y += grid_y)
            new_points.append( QPointF(x, y) );
    }

    // Bottom left
    for (double y = origin_y; y <= max_y; y += grid_y) {
        // Bottom horizontal lines
        new_lines.append( QLineF(min_x, y, max_x, y) );

        if (!allow_dots) continue;
        for (double x = origin_x; x >= min_x; x -= grid_x)
            new_points.append( QPointF(x, y) );
    }

    // Top right
    for (double y = origin_y; y >= min_y; y -= grid_y) {
        // Top horizontal lines
        new_lines.append( QLineF(min_x, y, max_x, y) );
        if (!allow_dots) continue;

        for (double x = origin_x; x <= max_x; x += grid_x)
            new_points.append( QPointF(x, y) );
    }

    // Top left
    for (double x = origin_x; x >= min_x; x -= grid_x) {
        // Left side vertical lines
        new_lines.append( QLineF(x, min_y, x, max_y) );
        if (!allow_dots) continue;

        for (double y = origin_y; y >= min_y; y -= grid_y)
            new_points.append( QPointF(x, y) );
    }


    // ***** Rotate grid lines
    QLineF diag1 { viewport_rect.topLeft(),    viewport_rect.bottomRight() };
    QLineF diag2 { viewport_rect.bottomLeft(), viewport_rect.topRight() };
    QPointF *ipoint = new QPointF();
    m_grid_lines.clear();
    for (auto &line: new_lines) {
        QLineF  rotated_line = t.map( line );

        //if ( rotated_line.intersect(diag1, ipoint) == QLineF::BoundedIntersection ||
        //     rotated_line.intersect(diag2, ipoint) == QLineF::BoundedIntersection) {
            m_grid_lines.append( rotated_line );
        //}
    }
    delete ipoint;

    // ***** Rotate grid points and add points inside view to QVector used to paint grid dots
    m_grid_points.clear();
    if (allow_dots) {
        new_points = t.map( new_points );
        for (auto point : new_points) {
            if (viewport_rect.contains( point ))
                m_grid_points.append( point );
        }
    }

}














