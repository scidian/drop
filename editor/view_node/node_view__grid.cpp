//
//      Created by Stephens Nunnally on 5/7/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QGraphicsItem>

#include "core/colors/colors.h"
#include "core/dr_math.h"
#include "editor/helper_library.h"
#include "editor/preferences.h"
#include "editor/view_node/node_item.h"
#include "editor/view_node/node_scene.h"
#include "editor/view_node/node_view.h"
#include "project/constants_comps_and_props.h"


//####################################################################################
//##    Updates grid based on current stage shown, recalculates points and lines
//####################################################################################
void NodeView::updateGrid() {
    if (scene() == nullptr) return;

    // Update Grid Properties
    ///m_grid_origin =     QPointF(0, 0);
    ///m_grid_size =       QPointF(16, 16);
    ///m_grid_scale =      QPointF(1.0, 1.0);
    ///m_grid_rotate =     0;
    QColor scheme_bg_color;
    if (Dr::GetColorScheme() == Color_Scheme::Light) {
        scheme_bg_color =   Dr::ToQColor(Dr::GetColor(Window_Colors::Background_Light));
        m_grid_color =      Dr::ToQColor(Dr::GetColor(Window_Colors::Button_Dark).darker(110));
    } else {
        scheme_bg_color =   Dr::ToQColor(Dr::GetColor(Window_Colors::Background_Dark));
        m_grid_color =      Dr::ToQColor(Dr::GetColor(Window_Colors::Button_Dark).darker(110));
    }
    ///m_grid_color =       QColor::fromRgba(stage->getComponentPropertyValue(Components::Stage_Grid, Properties::Stage_Grid_Color).toUInt());
    ///m_grid_style =       Grid_Style::Lines;

    m_grid_should_snap =    Dr::GetPreference(Preferences::Editor_Snap_To_Grid).toBool();

    if (m_back_color != scheme_bg_color) {
        m_back_color = scheme_bg_color;
        this->setStyleSheet(" QFrame { background-color: " + m_back_color.name() + "; } ");
    }

    scene()->update(this->sceneRect());         // Forces repaint of GraphicsView
    for (auto item : scene()->items()) {        // Forces repaint of all GraphicsItems
        item->update();
    }
}


//####################################################################################
//##    Rounds point in scene to nearest point aligned to grid
//####################################################################################
QPointF NodeView::roundToGrid(QPointF point_in_scene) {
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






















