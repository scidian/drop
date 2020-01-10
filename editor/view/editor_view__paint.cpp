//
//      Created by Stephens Nunnally on 1/8/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Graphics View Paint Overrides
//
//
#include <QDebug>
#include <QPaintEngine>
#include <QPaintEvent>
#include <QStylePainter>
#include <QOpenGLFunctions>

#include "core/colors/colors.h"
#include "editor/debug.h"
#include "editor/globals_editor.h"
#include "editor/helper_library.h"
#include "editor/interface_editor_relay.h"
#include "editor/view/editor_item.h"
#include "editor/view/editor_scene.h"
#include "editor/view/editor_view.h"
#include "project/dr_project.h"
#include "project/entities/dr_world.h"
#include "project/entities/dr_stage.h"
#include "project/entities/dr_thing.h"
#include "project/settings/settings.h"
#include "project/settings/settings_component.h"
#include "project/settings/settings_component_property.h"


//####################################################################################
//##    DrawBackground / DrawForground, called before and after paintEvent
//####################################################################################
void DrView::drawBackground(QPainter *painter, const QRectF &rect) {
    Q_UNUSED(rect)

    if (Dr::CheckDebugFlag(Debug_Flags::Turn_On_OpenGL_in_Editor)) {
        QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
        f->glClearColor(static_cast<float>(Dr::GetColor(Window_Colors::Background_Dark).redF()),
                        static_cast<float>(Dr::GetColor(Window_Colors::Background_Dark).greenF()),
                        static_cast<float>(Dr::GetColor(Window_Colors::Background_Dark).blueF()),
                        1.0f);
        f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }   

    if (m_grid_show_on_top == false) {
        paintGrid(*painter);
    }
}

void DrView::drawForeground(QPainter *painter, const QRectF &rect) {
    Q_UNUSED(rect)

    if (m_grid_show_on_top == true) {
        paintGrid(*painter);
    }

    // Draw Game Frame
    DrStage *stage = my_scene->getCurrentStageShown();
    if (stage) {
        if (stage->isStartStage()) {
            paintGameFrame(*painter);
        }
    }
}


//####################################################################################
//##    PAINT: Main Paint Event for QGraphicsView (DrView)
//####################################################################################
void DrView::paintEvent(QPaintEvent *event) {
    // ******************** Pass on event to parent class paint items into scene
    QGraphicsView::paintEvent(event);

    // ******************** At this point, if no selected items get out of here
    if (scene() == nullptr) return;

    // Store current center point of scene, so that if we go to a new scene and come back we stay in the same place
    DrStage *stage = my_scene->getCurrentStageShown();
    if (stage != nullptr && Dr::CheckDoneLoading()) {
        QRect   viewport_rect(0, 0, this->viewport()->width(), this->viewport()->height());
        QRectF  visible_scene_rect = this->mapToScene(viewport_rect).boundingRect();
        QPointF center_point = visible_scene_rect.center();
        stage->setViewCenterPoint( DrPointF(center_point.x(), center_point.y()) );
        stage->setViewZoomLevel( m_zoom_scale );
    }

    // Initiate QPainter object
    QPainter painter(viewport());

    if (Dr::CheckDebugFlag(Debug_Flags::Turn_On_Antialiasing)) {
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    }

    // Draw collision shapes if we have a Stage
    if (Dr::GetPreference(Preferences::World_Editor_Show_Collision_Shapes).toBool() && stage != nullptr) {
        paintCollisionShapes(painter, stage);
        paintDebugHealth(painter, stage);
    }

    // If theres no selection we don't need to perform these paint routines
    if (my_scene->getSelectionCount() > 0) {

        paintItemOutlines(painter);                                                 // Draw bounding box for each selected item

        if (m_hide_bounding == false) {
            paintBoundingBox(painter);                                              // Draw box around entire seleciton, with Size Grip handles
            paintHandles(painter, m_handles_shape);                                 // Draw handles around selected item / bounding box
        }

        if (m_view_mode == View_Mode::Rotating)
            paintGroupAngle(painter, my_scene->getSelectionAngle());                // Draw angles if rotating

        if (m_view_mode == View_Mode::Translating)                                  // Draw crosshairs if translating
            paintItemCenters(painter);
    }

    // Draw Stage Boundary
    if (stage != nullptr) {
        paintStageBounds(painter, stage);
    }

    // Draw Crosshairs under potential drag and drop
    if (Dr::GetPreference(Preferences::World_Editor_Snap_To_Grid).toBool()) {
        if (m_drop_might_happen)
            paintCrossHairs(painter, mapFromScene(m_drop_location));
    }

    // Draw ToolTip
    if (m_tool_tip->isHidden() == false && Dr::CheckDebugFlag(Debug_Flags::Turn_On_OpenGL_in_Editor)) {
        paintToolTip(painter);
    }

    // !!!!! #DEBUG:    Draw frames per second
    if (Dr::CheckDebugFlag(Debug_Flags::Label_FPS)) {
        Dr::SetLabelText(Label_Names::Label_3, "Draw Time: " + QString::number(m_debug_timer.elapsed()) +
                                                   ", FPS: " + QString::number(m_debug_fps_last) );
        m_debug_fps++;
        if (m_debug_timer.elapsed() >= 1000) {
            m_debug_timer.restart();
            m_debug_fps_last = m_debug_fps;
            m_debug_fps = 0;
        }
    }
    // !!!!! END


    QPointF center_point = this->mapToScene(this->viewport()->geometry().center());
    emit updateCenterPointX( center_point.x());
    emit updateCenterPointY(-center_point.y());
}


//####################################################################################
//##    PAINT: Draws grid lines
//####################################################################################
void DrView::paintGrid(QPainter &painter) {
    double point_size = 4;
    if (m_zoom_scale <=  .5) point_size = 3;
    if (m_zoom_scale <= 1.0) point_size = 4;
    if (m_zoom_scale >= 2.0) point_size = 5;
    if (m_zoom_scale >= 3.0) point_size = 6;
    if (m_zoom_scale >= 4.0) point_size = 7;
    if (m_zoom_scale >= 8.0) point_size = 8;
    if (m_zoom_scale >= 16.0) point_size = 10;
    QPen dot_pen =  QPen( m_grid_color, point_size, Qt::PenStyle::SolidLine, Qt::PenCapStyle::RoundCap );
    QPen line_pen = QPen( m_grid_color, 1);
    line_pen.setCosmetic(true);

    if (m_grid_needs_redraw) {

        QPointF top_left =  mapFromScene( m_grid_view_rect.topLeft() );
        QPointF bot_right = mapFromScene( m_grid_view_rect.bottomRight() );
        QTransform offset = QTransform().translate(-top_left.x(), -top_left.y());

        QRect grid_view = QRectF(top_left, bot_right).normalized().toRect();
        int new_width, new_height;
        if (abs(grid_view.width()) >= 1)  new_width  = abs(grid_view.width());  else new_width = 1;
        if (abs(grid_view.height()) >= 1) new_height = abs(grid_view.height()); else new_height = 1;
        m_grid_buffer = QPixmap(new_width, new_height);
        m_grid_buffer.fill( QColor(0, 0, 0, 0) );

        QPainter *grid_painter = new QPainter(&m_grid_buffer);
        grid_painter->setBrush(Qt::NoBrush);

        // ***** Grid Lines
        if (m_grid_style == Grid_Style::Lines) {
            grid_painter->setPen(line_pen);

            QVector<QLineF> view_lines;
            for (auto &line : m_grid_lines) {
                QLineF new_line = QLineF( mapFromScene( line.p1() ) , mapFromScene( line.p2() ) );
                new_line = offset.map (new_line );
                view_lines.append( new_line );
            }
            grid_painter->drawLines( view_lines );

            ///painter.setPen(line_pen);
            ///painter.drawLines( m_grid_lines );

        // ***** Grid Dots
        } else {
            grid_painter->setPen(dot_pen);

            QVector<QPointF> view_points;
            for (auto &point : m_grid_points) {
                QPointF new_point = mapFromScene( point );
                new_point = offset.map( new_point );
                view_points.append( new_point );
            }
            grid_painter->drawPoints( view_points );
        }

        grid_painter->end();
        delete grid_painter;

        m_grid_needs_redraw = false;
    }

    // Draw buffer holding grid onto screen
    QRectF target_rect = m_grid_view_rect;
    if (target_rect.width() > 0 && target_rect.height() > 0) {
        painter.drawPixmap(target_rect, m_grid_buffer, QRectF(m_grid_buffer.rect()) );

        // Draws an outline around grid
        painter.setBrush(Qt::NoBrush);
        painter.setPen(line_pen);
        painter.drawRect(m_grid_view_rect);
    }
}


//####################################################################################
//##    PAINT: Paints Game Frame in Scene Coordinates
//####################################################################################
void DrView::paintGameFrame(QPainter &painter) {
    painter.setBrush(Qt::NoBrush);
    QColor frame_color = Dr::ToQColor(Dr::GetColor(Window_Colors::Button_Light));
    frame_color.setAlpha(128);

    QPen frame_pen_outline = QPen(frame_color, 4);
    QPen frame_pen_inside  = QPen(frame_color.lighter(150), 2);
    frame_pen_outline.setCosmetic( true );
    frame_pen_inside.setCosmetic(  true );

    // Calculate Game Frame
    QRectF   game_frame = gameFrame(m_project);
    DrStage *stage = my_scene->getCurrentStageShown();
    QRectF   stage_rect;
    if (stage != nullptr && Dr::CheckDoneLoading()) {
        double h2;
        stage_rect = stageBoundingRect(m_project, stage, h2);

        // Get equalized game direction
        double game_direction = stage->getParentWorld()->getComponentPropertyValue(Components::World_Settings, Properties::World_Game_Direction).toDouble();
        while (game_direction < 0)   game_direction += 360.0;
        while (game_direction > 360) game_direction -= 360.0;

        // Move game frame as needed
        double side_frame_diff = 0, top_frame_diff = 0;
        if (game_direction < 270.0 && game_direction > 90.0) {
            side_frame_diff = stage_rect.right() - game_frame.right();
        } else {
            side_frame_diff = stage_rect.left() -  game_frame.left();
        }

        if (game_direction > 0.0 && game_direction < 180.0) {
            top_frame_diff  = stage_rect.top() -    game_frame.top();
        } else {
            top_frame_diff  = stage_rect.bottom() - game_frame.bottom();
        }

        game_frame = game_frame.adjusted(side_frame_diff, top_frame_diff, side_frame_diff, top_frame_diff);
    }

    painter.setPen( frame_pen_outline );
    painter.drawRoundedRect(game_frame, 25, 25);

    painter.setPen( frame_pen_inside );
    painter.drawRoundedRect(game_frame, 25, 25);
}


//####################################################################################
//##    PAINT: Paints Stage Bounds in Scene Coordinates
//####################################################################################
void DrView::paintStageBounds(QPainter &painter, DrStage *stage) {
    if (stage == nullptr) return;

    // Get Stage Size, Game Direction, figure out rotation QGraphicsScene transform
    long   stage_size = stage->getComponentPropertyValue(Components::Stage_Settings, Properties::Stage_Size).toInt();
    double game_direction = stage->getParentWorld()->getComponentPropertyValue(Components::World_Settings, Properties::World_Game_Direction).toDouble();
    QTransform t_scene = QTransform().rotate( game_direction );

    // Find width of rotated game frame
    double h2;
    stageBoundingRect(m_project, stage, h2);

    // Draw start bracket (in Scene coordinates)
    QPainterPath left, right, direction, arrow;
    left.moveTo(  30,  h2 );
    left.lineTo(   0,  h2 );
    left.lineTo(   0, -h2 );
    left.lineTo(  30, -h2 );
    left = t_scene.map(left);

    // End Bracket (in Scene Coordinates)
    right.moveTo(-30,  h2 );
    right.lineTo(  0,  h2 );
    right.lineTo(  0, -h2 );
    right.lineTo(-30, -h2 );
    right.translate( stage_size, 0 );
    right = t_scene.map(right);

    // Direction Box (in View Coordinates)
    QPoint middle_right = mapFromScene( t_scene.map(QPointF(stage_size, 0)) );
    direction.addRect( middle_right.x() - 5, middle_right.y() - 5, 10, 10);

    // Figure out rotation QGraphicsView transform
    QTransform t_view = QTransform().translate( middle_right.x(),  middle_right.y())
                                    .rotate(game_direction)
                                    .translate(-middle_right.x(), -middle_right.y());
    direction = t_view.map(direction);

    // Directional Arrow (in View Coordinates)
    arrow.moveTo( middle_right.x() + 11, middle_right.y());
    arrow.lineTo( middle_right.x() + 15, middle_right.y());
    arrow.lineTo( middle_right.x() + 15, middle_right.y() - 5);
    arrow.lineTo( middle_right.x() + 23, middle_right.y());
    arrow.lineTo( middle_right.x() + 15, middle_right.y() + 5);
    arrow.lineTo( middle_right.x() + 15, middle_right.y());
    arrow = t_view.map(arrow);

    painter.setBrush( Qt::NoBrush );
    QColor line_color = Dr::ToQColor(Dr::GetColor(Window_Colors::Background_Dark));
    QColor fill_color = Dr::ToQColor(Dr::GetColor(Window_Colors::Icon_Dark));
    QPen frame_pen_outline = QPen(line_color, 4);
    QPen frame_pen_inside  = QPen(fill_color, 2);

    painter.setPen( frame_pen_outline );    painter.drawPath( mapFromScene(left) );     painter.drawPath( mapFromScene(right) );
    painter.setPen( frame_pen_inside );     painter.drawPath( mapFromScene(left) );     painter.drawPath( mapFromScene(right) );

    painter.setBrush( Dr::ToQColor(Dr::GetColor(Window_Colors::Button_Dark)) );
    painter.setPen( frame_pen_outline );
    painter.drawPath( direction );
    painter.drawPath( arrow );

    painter.setPen( frame_pen_inside );
    painter.drawPath( direction );

    painter.setBrush( Dr::ToQColor(Dr::GetColor(Window_Colors::Icon_Light)) );
    painter.drawPath( arrow );
}


//####################################################################################
//##    PAINT: Paints tool tip when using OpenGL
//####################################################################################
void DrView::paintToolTip(QPainter &painter) {
    int w = m_tool_tip->geometry().width();
    int h = m_tool_tip->geometry().height();
    int left_offset = m_tool_tip->pos().x();
    int top_offset =  m_tool_tip->pos().y();

    painter.setPen(Qt::NoPen);
    painter.setBrush(Dr::ToQColor(Dr::GetColor(Window_Colors::Shadow)));
    painter.drawRoundedRect(left_offset, top_offset + 2, w, h, m_tool_tip->getXRadius(), m_tool_tip->getYRadius(), Qt::SizeMode::AbsoluteSize);

    QLinearGradient gradient( left_offset, top_offset, left_offset, top_offset + h);
    gradient.setColorAt(0.0, Dr::ToQColor(Dr::GetColor(Window_Colors::Button_Light)));
    gradient.setColorAt(1.0, Dr::ToQColor(Dr::GetColor(Window_Colors::Button_Dark)));
    painter.setBrush(gradient);
    painter.drawRoundedRect(left_offset, top_offset, w, h, m_tool_tip->getXRadius(), m_tool_tip->getYRadius(), Qt::SizeMode::AbsoluteSize);

    // External call to paint routine of tool tip, but since we pass a painter we can pass the QGraphicsView to be painted on when OpenGL is turned on,
    // this way the tool tip text is drawn directly onto the OpenGL surface and not a seperate QWidget tool tip window
    m_tool_tip->drawText(painter, left_offset, top_offset);
}


//####################################################################################
//##    PAINT: Draw our Rubber Band selection box with custom colors
//####################################################################################
void DrViewRubberBand::paintEvent(QPaintEvent *) {
    QStylePainter painter(this);

    QColor bg = Dr::ToQColor(Dr::GetColor(Window_Colors::Icon_Light));
    bg.setAlpha(48);
    QBrush brush;
    brush.setStyle(Qt::BrushStyle::SolidPattern);
    brush.setColor(bg);

    painter.setBrush(brush);
    painter.setPen(QPen(QBrush(Dr::ToQColor(Dr::GetColor(Window_Colors::Icon_Light))), 2, Qt::PenStyle::SolidLine));
    painter.drawRect(this->rect());
}












