//
//      Created by Stephens Nunnally on 5/6/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QMouseEvent>
#include <QTimer>

#include "core/dr_math.h"
#include "editor/interface_editor_relay.h"
#include "editor/world_map/world_map_item.h"
#include "editor/world_map/world_map_view.h"
#include "project/dr_project.h"


//####################################################################################
//##
//##    Mouse Released
//##
//####################################################################################
void WorldMapView::mouseReleaseEvent(QMouseEvent *event) {
    // Test for scene, convert to our custom class
    if (scene() == nullptr) return;

    // Store starting View_Mode in case we need it later
    View_Mode mode_at_start_of_function = m_view_mode;

    // Process left mouse button released
    if (event->button() & Qt::LeftButton) {
        m_hide_bounding = false;
///        m_rubber_band->hide();


        // Inspector ignores changes during Translating and Resizing and Rotating, it's much, much faster this way...
        // Now that mousue has been released, update Inspector property boxes
        if (m_view_mode == View_Mode::Translating || m_view_mode == View_Mode::Resizing || m_view_mode == View_Mode::Rotating) {
            m_view_mode = View_Mode::None;
//            QList<DrSettings*> selected_entities = ConvertItemListToSettings(my_scene->getSelectionItems());
//            m_editor_relay->updateEditorWidgetsAfterItemChange(
//                        Editor_Widgets::Map_View, { selected_entities.begin(), selected_entities.end() },
//                        {   std::make_pair(Comps::Thing_Transform, Props::Thing_Position),
//                            std::make_pair(Comps::Thing_Transform, Props::Thing_Size),
//                            std::make_pair(Comps::Thing_Transform, Props::Thing_Scale),
//                            std::make_pair(Comps::Thing_Transform, Props::Thing_Rotation) });
        }

        m_view_mode = View_Mode::None;
    }


    // ***** Don't pass event to base class
    if (mode_at_start_of_function == View_Mode::Translating &&
            Dr::IsCloseTo(m_origin_item->pos().x(), m_origin_item_start_pos.x(), 0.001) &&
            Dr::IsCloseTo(m_origin_item->pos().y(), m_origin_item_start_pos.y(), 0.001) &&
            m_origin_timer.elapsed() > 500) {
        // This is when multiple items are selected and the user starts to click on one of the items selected to select it by itself, but then
        // realizes they want to keep the multi selection they have. Standard practice dictates if you continue to hold the mouse button for a
        // longer time (in this case over 500 milliseconds) before letting go, you get to keep the multi selction. This case is handled here by
        // not calling the base class event that clears the selection.

        QList<long> item_keys { };
//        for (auto &item : my_scene->getSelectionItems())
//            item_keys.append(item->data(User_Roles::Key).toLongLong());
        m_editor_relay->buildInspector( item_keys );
        m_editor_relay->updateItemSelection(Editor_Widgets::Stage_View);

    // ***** Pass event to base class
    } else {
        QGraphicsView::mouseReleaseEvent(event);
    }

    // ***** Update
    this->viewport()->repaint(this->viewport()->rect());
    update();
}






