//
//      Created by Stephens Nunnally on 5/7/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "core/colors/colors.h"
#include "core/dr_math.h"
#include "editor/helper_library.h"
#include "editor/preferences.h"
#include "editor/world_map/world_map_view.h"
#include "project/constants_comps_and_props.h"


//####################################################################################
//##    Updates grid based on current stage shown, recalculates points and lines
//####################################################################################
void WorldMapView::updateGrid() {
    if (scene() == nullptr) return;

    // Update Grid Properties
    m_grid_origin =     QPointF(0, 0);
    m_grid_size =       QPointF(32, 32);
    m_grid_scale =      QPointF(1.0, 1.0);
    m_grid_rotate =     0;
    QColor scheme_bg_color;
    if (Dr::GetColorScheme() == Color_Scheme::Light) {
        scheme_bg_color =   Dr::ToQColor(Dr::GetColor(Window_Colors::Background_Light));
        m_grid_color =      Dr::ToQColor(Dr::GetColor(Window_Colors::Button_Dark).darker(110));
    } else {
        scheme_bg_color =   Dr::ToQColor(Dr::GetColor(Window_Colors::Background_Dark));
        m_grid_color =      Dr::ToQColor(Dr::GetColor(Window_Colors::Button_Dark).darker(110));
    }
    ///m_grid_color =       QColor::fromRgba(stage->getComponentPropertyValue(Components::Stage_Grid, Properties::Stage_Grid_Color).toUInt());
    m_back_color =          Qt::black;
    m_back_color_use =      false;

    m_grid_style =          Grid_Style::Lines;
    m_grid_should_snap =    Dr::GetPreference(Preferences::World_Editor_Snap_To_Grid).toBool();
    m_grid_resize_snap =    Dr::GetPreference(Preferences::World_Editor_Resize_To_Grid).toBool();
    m_grid_show_on_top =    Dr::GetPreference(Preferences::World_Editor_Grid_On_Top).toBool();

    QColor background = (m_back_color_use) ? m_back_color : scheme_bg_color;
    this->setStyleSheet(" QFrame { background-color: " + background.name() + "; } ");

    scene()->update(this->sceneRect());         // Forces repaint of view
}


//####################################################################################
//##    Rounds point in scene to nearest point aligned to grid
//####################################################################################
QPointF WorldMapView::roundToGrid(QPointF point_in_scene) {
    // ***** Align new desired center to grid
    QTransform  remove_angle =      QTransform().rotate(-m_grid_rotate);
    QTransform  add_angle =         QTransform().rotate( m_grid_rotate);
    QPointF     rounded_center =    point_in_scene;

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






















