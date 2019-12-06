//
//      Created by Stephens Nunnally on 2/27/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrView Grid Functions
//
//
#include <QtMath>
#include <QScrollBar>

#include "editor/colors/colors.h"
#include "editor/debug.h"
#include "editor/globals_editor.h"
#include "editor/view/editor_view.h"
#include "editor/view/editor_scene.h"
#include "library/helper.h"
#include "model/project/project.h"
#include "model/project/project_world.h"
#include "model/project/project_stage.h"


//####################################################################################
//##    Updates grid based on current stage shown, recalculates points and lines
//####################################################################################
void DrView::updateGrid() {
    if (scene() == nullptr) return;
    if (my_scene->getCurrentStageShown() == nullptr) return;

    DrStage *stage = my_scene->getCurrentStageShown();
    DrWorld *world = stage->getParentWorld();

    // Increase Scene Rect if Stage Rect changed
    double h2;
    QRectF stage_rect = DrView::stageBoundingRect(m_project, stage, h2).adjusted(-500, -500, 500, 500);
    scene()->setSceneRect( scene()->sceneRect().united(stage_rect) );

    // Update Grid Properties
    m_grid_origin =     stage->getComponentPropertyValue(Components::Stage_Grid, Properties::Stage_Grid_Origin_Point).toPointF();
    m_grid_size =       stage->getComponentPropertyValue(Components::Stage_Grid, Properties::Stage_Grid_Size).toPointF();
    m_grid_scale =      stage->getComponentPropertyValue(Components::Stage_Grid, Properties::Stage_Grid_Scale).toPointF();
    m_grid_rotate =     stage->getComponentPropertyValue(Components::Stage_Grid, Properties::Stage_Grid_Rotation).toDouble();
    m_grid_color =      Dr::GetColor(Window_Colors::Background_Light);
    ///m_grid_color =   QColor::fromRgba(stage->getComponentPropertyValue(Components::Stage_Grid, Properties::Stage_Grid_Color).toUInt());
    m_back_color =      QColor::fromRgba(world->getComponentPropertyValue(Components::World_Settings, Properties::World_Background_Color).toUInt());
    m_back_color_use =  world->getComponentPropertyValue(Components::World_Settings, Properties::World_Use_Background_Color).toBool();

    int style =         stage->getComponentPropertyValue(Components::Stage_Grid, Properties::Stage_Grid_Style).toInt();
    m_grid_style =      static_cast<Grid_Style>(style);
    m_grid_should_snap =  Dr::GetPreference(Preferences::World_Editor_Snap_To_Grid).toBool();
    m_grid_resize_snap =  Dr::GetPreference(Preferences::World_Editor_Resize_To_Grid).toBool();
    m_grid_show_on_top =  Dr::GetPreference(Preferences::World_Editor_Grid_On_Top).toBool();

    QColor background = (m_back_color_use) ? m_back_color : Dr::GetColor(Window_Colors::Background_Dark);
    this->setStyleSheet("background: " + background.name() + ";");

    recalculateGrid();
    m_grid_needs_redraw = true;                 // Flag grid for redraw during next paintEvent
    scene()->update(this->sceneRect());         // Forces repaint of view

    ///Dr::SetLabelText(Label_Names::Label_1, "Updated Grid at: " + Dr::CurrentTimeAsString());
}


//####################################################################################
//##    Rounds point in scene to nearest point aligned to grid
//####################################################################################
QPointF DrView::roundToGrid(QPointF point_in_scene) {
    // ***** Align new desired center to grid
    QTransform remove_angle = QTransform().rotate(-m_grid_rotate);
    QTransform add_angle =    QTransform().rotate( m_grid_rotate);
    QPointF rounded_center = point_in_scene;

    // Divide by scale to equalize mouse position to scaling being applied after rounding
    rounded_center.setX( rounded_center.x() / m_grid_scale.x() );
    rounded_center.setY( rounded_center.y() / m_grid_scale.y() );

    // Remove rotation, round new position to nearest grid step, and add rotation back
    rounded_center = remove_angle.map ( rounded_center );
    rounded_center.setX( round((rounded_center.x() - m_grid_origin.x()) / m_grid_size.x()) * m_grid_size.x() + m_grid_origin.x());
    rounded_center.setY( round((rounded_center.y() - m_grid_origin.y()) / m_grid_size.y()) * m_grid_size.y() + m_grid_origin.y());
    rounded_center = add_angle.map ( rounded_center );

    // Apply grid scale to new position
    rounded_center.setX( rounded_center.x() * m_grid_scale.x() );
    rounded_center.setY( rounded_center.y() * m_grid_scale.y() );

    return rounded_center;
}


//####################################################################################
//##    Calculate Game Frame
//####################################################################################
QRectF DrView::gameFrame(DrProject *project) {
    if (project == nullptr) return QRectF();
    QRectF game_frame;
    int width =  project->getOption(Project_Options::Width).toInt();
    int height = project->getOption(Project_Options::Height).toInt();
    double w2 = width /  2.0;
    double h2 = height / 2.0;
    switch (project->getOptionOrientation()) {
        case Orientation::Portrait:     game_frame = QRectF(-w2, -h2, width, height);     break;
        case Orientation::Landscape:    game_frame = QRectF(-h2, -w2, height,  width);    break;
    }
    return game_frame;
}

//####################################################################################
//##    Returns: - Bounding Box Rect of DrStage (rotated + size)
//##             - Half height of bracket side of Stage Rect as half_height
//####################################################################################
QRectF DrView::stageBoundingRect(DrProject *project, DrStage *stage, double &half_height) {
    if (project == nullptr) return QRectF();
    if (stage == nullptr)   return QRectF();

    // Calculate Game Frame, Get Stage Size, Game Direction
    QRectF game_frame = gameFrame(project);
    int    stage_size = stage->getComponentPropertyValue(Components::Stage_Settings, Properties::Stage_Size).toInt();
    double game_direction = stage->getParentWorld()->getComponentPropertyValue(Components::World_Settings, Properties::World_Game_Direction).toDouble();
    half_height = game_frame.height() / 2.0;
    QTransform t_scene = QTransform().rotate( game_direction );

    // Find width of rotated game frame
    QLineF line_down = t_scene.map( QLineF(0, 0, 0, 5000) );
    QLineF frame_top(       game_frame.topLeft(),     game_frame.topRight()     );
    QLineF frame_bottom(    game_frame.bottomLeft(),  game_frame.bottomRight()  );
    QLineF frame_left(      game_frame.topLeft(),     game_frame.bottomLeft()   );
    QLineF frame_right(     game_frame.topRight(),    game_frame.bottomRight()  );
    QPointF border_intersect;
    if (       line_down.intersect(frame_top, &border_intersect) ==     QLineF::BoundedIntersection) {
        half_height = QLineF(QPointF(0, 0), border_intersect).length();
    } else if (line_down.intersect(frame_bottom, &border_intersect) ==  QLineF::BoundedIntersection) {
        half_height = QLineF(QPointF(0, 0), border_intersect).length();
    } else if (line_down.intersect(frame_left, &border_intersect) ==    QLineF::BoundedIntersection) {
        half_height = QLineF(QPointF(0, 0), border_intersect).length();
    } else if (line_down.intersect(frame_right, &border_intersect) ==   QLineF::BoundedIntersection) {
        half_height = QLineF(QPointF(0, 0), border_intersect).length();
    }

    // Create start bracket (in Scene coordinates)
    QPainterPath rect;
    rect.lineTo(          0,  half_height );
    rect.lineTo(          0, -half_height );
    rect.lineTo( stage_size, -half_height );
    rect.lineTo( stage_size,  half_height );
    rect = t_scene.map(rect);

    return rect.boundingRect();
}


//####################################################################################
//##    Recalculates grid points into local QVectors, m_grid_points & m_grid_lines
//####################################################################################
void DrView::recalculateGrid() {
    // Map viewport to scene rect
    QRectF  scene_rect = this->scene()->sceneRect();
    QPointF top_left = mapToScene(0, 0);
    QPointF bottom_right = mapToScene(this->viewport()->width(), this->viewport()->height());

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
    double view_min_x = min_x, view_min_y = min_y;
    double view_max_x = max_x, view_max_y = max_y;
    if (view_min_x < scene_rect.left())   view_min_x = scene_rect.left();
    if (view_min_y < scene_rect.top())    view_min_y = scene_rect.top();
    if (view_max_x > scene_rect.right())  view_max_x = scene_rect.right();
    if (view_max_y > scene_rect.bottom()) view_max_y = scene_rect.bottom();

    // ***** Create viewport bounding rect, calculate grid drawing origin point
    QRectF  viewport_rect = QRectF( QPointF(view_min_x, view_min_y), QPointF(view_max_x, view_max_y) );
    double origin_x = round((viewport_rect.center().x() - m_grid_origin.x()) / m_grid_size.x()) * m_grid_size.x() + m_grid_origin.x();
    double origin_y = round((viewport_rect.center().y() - m_grid_origin.y()) / m_grid_size.y()) * m_grid_size.y() + m_grid_origin.y();
    m_grid_view_rect = viewport_rect;


    // Hide dots if too zoomed out
    bool allow_dots = (m_grid_style == Grid_Style::Dots);
    ///if ((m_zoom_scale <= .50) && (step_x < 10 || step_y < 10)) allow_dots = false;
    ///if ((m_zoom_scale <= .25) && (step_x < 25 || step_y < 25)) allow_dots = false;
    ///if  (m_zoom_scale <= .10) allow_dots = false;

    // Step sizes (affected by zoom)
    double step_x = m_grid_size.x();
    double step_y = m_grid_size.y();
    double min = (m_grid_style == Grid_Style::Dots) ? 10.0 : 4.0;
    if (step_x * m_zoom_scale * m_grid_scale.x() < min)    step_x = min / (m_zoom_scale * m_grid_scale.x());
    if (step_y * m_zoom_scale * m_grid_scale.y() < min)    step_y = min / (m_zoom_scale * m_grid_scale.y());


    // ********** Calculate lines and dots
    QVector<QPointF> new_points;
    QVector<QLineF>  new_lines;
    double adjust = 2;///1.44;

    // Add in some buffering for rotation, and negate the effects of the scaling
    double smaller_scale = (m_grid_scale.x() < m_grid_scale.y()) ? m_grid_scale.x() : m_grid_scale.y();
    max_x += abs(max_x * adjust / smaller_scale) - abs(max_x);
    max_y += abs(max_y * adjust / smaller_scale) - abs(max_y);
    min_x -= abs(min_x * adjust / smaller_scale) - abs(min_x);
    min_y -= abs(min_y * adjust / smaller_scale) - abs(min_y);

    // Square max and min values so that if grid is rotated it will still fill in the viewport rect
    if (max_x < max_y) max_x = max_y;
    if (max_y < max_x) max_y = max_x;
    if (min_x > min_y) min_x = min_y;
    if (min_y > min_x) min_y = min_x;

    ///Dr::SetLabelText(Label_Names::Label_1, "Min X: " + QString::number(min_x) + ", Max X: " + QString::number(max_x));
    ///Dr::SetLabelText(Label_Names::Label_2, "Min Y: " + QString::number(min_y) + ", Max Y: " + QString::number(max_y));

    // Bottom right -- Right side vertical lines
    for (double x = origin_x; x <= max_x; x += step_x) {
        new_lines.append( QLineF(x, min_y, x, max_y) );

        if (allow_dots)     for (double y = origin_y; y <= max_y; y += step_y)  new_points.append( QPointF(x, y) );
    }

    // Bottom left -- Bottom horizontal lines
    for (double y = origin_y; y <= max_y; y += step_y) {
        new_lines.append( QLineF(min_x, y, max_x, y) );

        if (allow_dots)     for (double x = origin_x; x >= min_x; x -= step_x)  new_points.append( QPointF(x, y) );
    }

    // Top right -- Top horizontal lines
    for (double y = origin_y; y >= min_y; y -= step_y) {
        new_lines.append( QLineF(min_x, y, max_x, y) );

        if (allow_dots)     for (double x = origin_x; x <= max_x; x += step_x)  new_points.append( QPointF(x, y) );
    }

    // Top left -- Left side vertical lines
    for (double x = origin_x; x >= min_x; x -= step_x) {
        new_lines.append( QLineF(x, min_y, x, max_y) );

        if (allow_dots)     for (double y = origin_y; y >= min_y; y -= step_y)  new_points.append( QPointF(x, y) );
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
    center1.setX(round((center1.x() - m_grid_origin.x()) / m_grid_size.x()) * m_grid_size.x() + m_grid_origin.x());
    center1.setY(round((center1.y() - m_grid_origin.y()) / m_grid_size.y()) * m_grid_size.y() + m_grid_origin.y());

    // Fit center of bounding box to grid, then rotate bounding box
    QPointF center3 = bounding_box.center();
    center3.setX(round((center3.x() - m_grid_origin.x()) / m_grid_size.x()) * m_grid_size.x() + m_grid_origin.x());
    center3.setY(round((center3.y() - m_grid_origin.y()) / m_grid_size.y()) * m_grid_size.y() + m_grid_origin.y());
    QPointF center2 = add_angle.map ( center3 );

    // Calculate difference between start center and new center
    QPointF diff = center2 - center1;

    // Align new desired center to grid
    QPointF rounded_center = remove_angle.map ( QPointF(diff.x(), diff.y()) );
    rounded_center.setX( round((rounded_center.x()) / m_grid_size.x()) * m_grid_size.x() );
    rounded_center.setY( round((rounded_center.y()) / m_grid_size.y()) * m_grid_size.y() );
    rounded_center = add_angle.map ( rounded_center );
    QTransform slide = QTransform().translate( -rounded_center.x(), -rounded_center.y() );
    QTransform scale = QTransform().scale(m_grid_scale.x(), m_grid_scale.y());

    // Apply the rotation and translation transforms to the new lines
    for (auto &line: new_lines) {
        line = add_angle.map( line );
        line = slide.map( line );
        line = scale.map( line );
        m_grid_lines.append( line );
    }


    // If too many dots, don't bother, too slow
    ///Dr::SetLabelText(Label_Names::Label_1, "Dots: " + QString::number(new_points.count()));
    if (allow_dots && new_points.count() > 500000) allow_dots = false;

    // ********** Rotate grid points and add points inside view to QVector used to paint grid dots
    m_grid_points.clear();
    if (allow_dots) {
        new_points = add_angle.map( new_points );
        new_points = slide.map( new_points );
        new_points = scale.map( new_points );

        for (auto point : new_points) {
            if (viewport_rect.contains( point ))
                m_grid_points.append( point );
        }
    }

}














