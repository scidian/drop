//
//      Created by Stephens Nunnally on 1/21/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QGraphicsItemGroup>
#include <QMouseEvent>

#include "editor/interface_editor_relay.h"
#include "editor/view/editor_item.h"
#include "editor/view/editor_scene.h"
#include "editor/view/editor_view.h"
#include "project/entities/dr_thing.h"


//####################################################################################
//##    Extracts a list of DrSettings pointers from a list of
//##    DrItems (QGraphicsItems) representing DrThings in a QGraphicsScene
//####################################################################################
QList<DrSettings*> ConvertItemListToSettings(QList<QGraphicsItem*> list) {
    QList<DrSettings*> new_list;
    for (auto item : list) {
        DrItem* as_item = dynamic_cast<DrItem*>(item);
        DrThing* as_thing = as_item->getThing();
        new_list.append(as_thing);
    }
    return new_list;
}


//####################################################################################
//##
//##    Mouse Released
//##
//####################################################################################
void DrView::mouseReleaseEvent(QMouseEvent *event) {
    // Test for scene, convert to our custom class
    if (scene() == nullptr) return;

    // Process left mouse button released
    if (event->button() & Qt::LeftButton) {
        m_hide_bounding = false;
        m_rubber_band->hide();
        m_tool_tip->stopToolTip();

        // We have it so that Inspector only completely changes itself based on selection from scene view on mouse up
        // (i.e. after rubber band box mode is over so it's not trying to change a ton and slow things down)
        if (m_view_mode == View_Mode::Selecting) {
            m_view_mode = View_Mode::None;
            if (my_scene->getSelectionCount() > 0) {
                m_editor_relay->buildInspector( { static_cast<long>(my_scene->getSelectionItems().first()->data(User_Roles::Key).toLongLong()) } );
                m_editor_relay->updateItemSelection(Editor_Widgets::Stage_View);
            } else {
                if (itemAt(event->pos()) == nullptr) {
                    m_editor_relay->buildInspector( { } );
                    m_editor_relay->updateItemSelection(Editor_Widgets::Stage_View);
                }
            }
        }

        // Clean up temporary item group used for resize routine
        if (m_view_mode == View_Mode::Resizing) {
            for (auto item : m_group->childItems()) {
                m_group->removeFromGroup(item);
                delete item;
            }
            my_scene->destroyItemGroup(m_group);
            m_resize_started = false;
        }

        // Inspector ignores changes during Translating and Resizing and Rotating, it's much, much faster this way...
        // Now that mousue has been released, update Inspector property boxes
        if (m_view_mode == View_Mode::Translating || m_view_mode == View_Mode::Resizing || m_view_mode == View_Mode::Rotating) {
            m_view_mode = View_Mode::None;
            QList<DrSettings*> selected_entities = ConvertItemListToSettings(my_scene->getSelectionItems());
            m_editor_relay->updateEditorWidgetsAfterItemChange(
                        Editor_Widgets::Stage_View, { selected_entities.begin(), selected_entities.end() },
                        {   std::make_pair(Comps::Thing_Transform, Props::Thing_Position),
                            std::make_pair(Comps::Thing_Transform, Props::Thing_Size),
                            std::make_pair(Comps::Thing_Transform, Props::Thing_Scale),
                            std::make_pair(Comps::Thing_Transform, Props::Thing_Rotation) });
        }

        // Release camera
        if (m_view_mode == View_Mode::Moving_Camera) {
            m_view_mode = View_Mode::None;
            m_cam_selected = nullptr;
        }

        updateSelectionBoundingBox(6);

        m_view_mode = View_Mode::None;
    }


    // Pass on event, update
    QGraphicsView::mouseReleaseEvent(event);
    this->viewport()->repaint(this->viewport()->rect());
    update();
}





















