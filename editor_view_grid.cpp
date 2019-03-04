//
//      Created by Stephens Nunnally on 2/27/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrView Grid Functions
//
//
#include <QtMath>
#include <QScrollBar>

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
    m_grid_needs_redraw = true;         // Flag grid for redraw during next paintEvent
    update();

    ///Dr::SetLabelText(Label_Names::Label_1, "Updated Grid at: " + Dr::CurrentTimeAsString());
}

//####################################################################################
//##        Recalculates grid points into local QVectors, m_grid_points & m_grid_lines
//####################################################################################
void DrView::recalculateGrid()
{
    // Map viewport to scene rect
    QRectF  scene_rect = this->scene()->sceneRect();
    QPointF top_left = mapToScene(0, 0);
    QPointF bottom_right = mapToScene(this->width(), this->height());

    // Variables used to create bounding box in view of where we need to draw grid lines
    double min_x = top_left.x();
    double min_y = top_left.y();
    double max_x = bottom_right.x();
    double max_y = bottom_right.y();

    // If zoom is out all the way on x or y, set bounds to scene().sceneRect() bounds
    if (this->horizontalScrollBar()->value() == 0) {
        min_x = scene_rect.left();
        max_x = scene_rect.right();
    }
    if (this->verticalScrollBar()->value() == 0) {
        min_y = scene_rect.top();
        max_y = scene_rect.bottom();
    }
    // If view bounding box is less than scene_rect bounding box, fit bounding box to scene_rect
    if (min_x < scene_rect.left()) min_x = scene_rect.left();
    if (min_y < scene_rect.top())  min_y = scene_rect.top();
    if (max_x > scene_rect.right())  max_x = scene_rect.right();
    if (max_y > scene_rect.bottom()) max_y = scene_rect.bottom();


    // ***** Create viewport bounding rect, calculate grid drawing origin point
    double grid_x = m_grid_size.x();
    double grid_y = m_grid_size.y();

    QRectF  viewport_rect = QRectF( QPointF(min_x, min_y), QPointF(max_x, max_y) );
    double origin_x = round((viewport_rect.center().x() - m_grid_origin.x()) / grid_x) * grid_x + m_grid_origin.x();
    double origin_y = round((viewport_rect.center().y() - m_grid_origin.y()) / grid_y) * grid_y + m_grid_origin.y();
    m_grid_view_rect = viewport_rect;

    // Square max and min values so that if grid is rotated it will still fill in the viewport rect
    if (max_x < max_y) max_x = max_y;
    if (max_y < max_x) max_y = max_x;
    if (min_x > min_y) min_x = min_y;
    if (min_y > min_x) min_y = min_x;

    // Hide dots if too zoomed out
    bool allow_dots = true;
    if ((m_zoom_scale <= .50) && (grid_x < 10 || grid_y < 10)) allow_dots = false;
    if ((m_zoom_scale <= .25) && (grid_x < 25 || grid_y < 25)) allow_dots = false;
    if  (m_zoom_scale <= .10) allow_dots = false;
    if  (m_grid_style != Grid_Style::Dots) allow_dots = false;


    // ********** Calculate lines and dots
    QVector<QPointF> new_points;
    QVector<QLineF>  new_lines;
    double adjust = 1.44;

    max_x *= adjust;
    max_y *= adjust;
    min_x *= adjust;
    min_y *= adjust;

    // Bottom right -- Right side vertical lines
    for (double x = origin_x; x <= max_x; x += grid_x) {
        new_lines.append( QLineF(x, min_y, x, max_y) );

        if (allow_dots)     for (double y = origin_y; y <= max_y; y += grid_y)  new_points.append( QPointF(x, y) );
    }

    // Bottom left -- Bottom horizontal lines
    for (double y = origin_y; y <= max_y; y += grid_y) {
        new_lines.append( QLineF(min_x, y, max_x, y) );

        if (allow_dots)     for (double x = origin_x; x >= min_x; x -= grid_x)  new_points.append( QPointF(x, y) );
    }

    // Top right -- Top horizontal lines
    for (double y = origin_y; y >= min_y; y -= grid_y) {
        new_lines.append( QLineF(min_x, y, max_x, y) );

        if (allow_dots)     for (double x = origin_x; x <= max_x; x += grid_x)  new_points.append( QPointF(x, y) );
    }

    // Top left -- Left side vertical lines
    for (double x = origin_x; x >= min_x; x -= grid_x) {
        new_lines.append( QLineF(x, min_y, x, max_y) );

        if (allow_dots)     for (double y = origin_y; y >= min_y; y -= grid_y)  new_points.append( QPointF(x, y) );
    }



    // ********** Rotate grid lines and calculate transform that centers items to be drawn within the viewport rect
    m_grid_lines.clear();

    // Create some rotation transforms
    QTransform remove_angle = QTransform().rotate(-m_grid_rotate);
    QTransform add_angle =    QTransform().rotate( m_grid_rotate);

    // Calculates total bounding box of all potential grid lines
    QRectF bounding_box = QRectF(0, 0, 0, 0);
    for (auto &line : new_lines) {
        bounding_box = bounding_box.united( QRectF(line.p1(), line.p2()).normalized() );
    }

    // Fit center of viewport to grid
    QPointF center1 = viewport_rect.center();
    center1.setX(round((center1.x() - m_grid_origin.x()) / grid_x) * grid_x + m_grid_origin.x());
    center1.setY(round((center1.y() - m_grid_origin.y()) / grid_y) * grid_y + m_grid_origin.y());

    // Fit center of bounding box to grid, then rotate bounding box
    QPointF center3 = bounding_box.center();
    center3.setX(round((center3.x() - m_grid_origin.x()) / grid_x) * grid_x + m_grid_origin.x());
    center3.setY(round((center3.y() - m_grid_origin.y()) / grid_y) * grid_y + m_grid_origin.y());
    QPointF center2 = add_angle.map ( center3 );

    // Calculate difference between start center and new center
    QPointF diff = center2 - center1;

    // Align new desired center to grid
    QPointF rounded_center = remove_angle.map ( QPointF(diff.x(), diff.y()) );
    rounded_center.setX( round((rounded_center.x() - m_grid_origin.x()) / grid_x) * grid_x + m_grid_origin.x());
    rounded_center.setY( round((rounded_center.y() - m_grid_origin.y()) / grid_y) * grid_y + m_grid_origin.y());
    rounded_center = add_angle.map ( rounded_center );

    // Create transform to move center lines within the bounding box, move the lines
    QTransform slide = QTransform().translate( -rounded_center.x(), -rounded_center.y() );
    for (auto &line: new_lines) {
        line = add_angle.map( line );
        line = slide.map( line );
        m_grid_lines.append( line );
    }


    // ********** Rotate grid points and add points inside view to QVector used to paint grid dots
    m_grid_points.clear();
    if (allow_dots) {
        new_points = add_angle.map( new_points );
        new_points = slide.map( new_points );
        for (auto point : new_points) {
            if (viewport_rect.contains( point ))
                m_grid_points.append( point );
        }
    }

}














