//
//      Created by Stephens Nunnally on 2/27/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrView Grid Functions
//
//

#include "colors.h"
#include "debug.h"
#include "editor_view.h"
#include "editor_scene.h"
#include "library.h"
#include "project.h"
#include "project_world_stage.h"


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

    Dr::SetLabelText(Label_Names::Label_1, "Updated Grid at: " + Dr::CurrentTimeAsString());
}

//####################################################################################
//##        Recalculates grid points into local QVectors, m_grid_points & m_grid_lines
//####################################################################################
void DrView::recalculateGrid()
{
    // Map viewport to scene rect
    QPointF topLeft = mapToScene(0, 0);
    QPointF bottomRight = mapToScene(this->width(), this->height());
    QRectF  scene_rect(topLeft, bottomRight);

    double grid_x =   m_grid_size.x();
    double grid_y =   m_grid_size.y();
    if (grid_x < 1) grid_x = 1;
    if (grid_y < 1) grid_y = 1;

    double origin_x = m_grid_origin.x();
    double origin_y = m_grid_origin.y();

    QTransform t = QTransform().rotate(m_grid_rotate);
    m_grid_points.clear();
    m_grid_lines.clear();

    // Bottom right
    for (double x = origin_x; x <= scene_rect.right() * 2; x += grid_x)
        for (double y = origin_y; y <= scene_rect.bottom(); y += grid_y)
            m_grid_points.append( QPointF(x, y) );

    // Bottom left
    for (double x = origin_x; x >= scene_rect.left() * 2; x -= grid_x)
        for (double y = origin_y; y <= scene_rect.bottom(); y += grid_y)
            m_grid_points.append( QPointF(x, y) );

    // Top right
    for (double x = origin_x; x <= scene_rect.right() * 2; x += grid_x)
        for (double y = origin_y; y >= scene_rect.top(); y -= grid_y)
            m_grid_points.append( QPointF(x, y) );

    // Top left
    for (double x = origin_x; x >= scene_rect.left() * 2; x -= grid_x)
        for (double y = origin_y; y >= scene_rect.top(); y -= grid_y)
            m_grid_points.append( QPointF(x, y) );

    for (auto point: m_grid_points) {
        QLineF line;
        line = QLineF( point, QPointF(point.x() + grid_x, point.y()) );
        line = t.map(line);
        m_grid_lines.append(line);
        line = QLineF( point, QPointF(point.x(), point.y() + grid_y) );
        line = t.map(line);
        m_grid_lines.append(line);
    }

    // Rotate grid points
    m_grid_points = t.map( m_grid_points );
}


//####################################################################################
//##        Finds closest Scene point in grid array to the Scene point passed in
//####################################################################################
QPointF DrView::closestGridPoint(QPointF check_point)
{
    QPointF closest = check_point;
    double length = 0;

    bool have_one = false;
    for (auto point : m_grid_points) {
        double check_length = QLineF(check_point, point).length();

        if (check_length < length || !have_one) {
            closest = point;
            length = check_length;
            have_one = true;
        }
    }
    return closest;
}













