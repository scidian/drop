//
//      Created by Stephens Nunnally on 1/15/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Graphics View Handling - Mouse Events
//
//
#include <QtMath>
#include <QMouseEvent>
#include <QScrollBar>
#include <QTimer>

#include "colors/colors.h"
#include "debug.h"
#include "editor_item.h"
#include "editor_scene.h"
#include "editor_view.h"
#include "globals.h"
#include "helper.h"
#include "helper_qt.h"
#include "interface_editor_relay.h"
#include "project/project.h"
#include "project/project_world.h"
#include "project/project_world_stage.h"
#include "project/project_world_stage_thing.h"
#include "settings/settings.h"
#include "settings/settings_component.h"
#include "settings/settings_component_property.h"


//####################################################################################
//##    Mouse Pressed
//####################################################################################
// Forwards a double click as just another mouse down
void DrView::mouseDoubleClickEvent(QMouseEvent *event) { mousePressEvent(event); }

void DrView::mousePressEvent(QMouseEvent *event) {
    // Test for scene, convert to our custom class and lock the scene
    if (scene() == nullptr) return;
    if (my_scene->scene_mutex.tryLock(100) == false) return;

    // On initial mouse down, store mouse origin point
    m_origin =          event->pos();
    m_origin_in_scene = mapToScene(m_origin);

    // Get top most unlocked item
    long        origin_item_key = 0;
    DrSettings *origin_item_settings = nullptr;
    m_origin_item =  itemAt(event->pos());
    for (auto item : items(event->pos())) {
        origin_item_key = item->data(User_Roles::Key).toLongLong();
        origin_item_settings = m_project->findSettingsFromKey(origin_item_key);
        if (origin_item_settings != nullptr) {
            if (origin_item_settings->isLocked() == false) {
                m_origin_item = item;
                break;
            }
        }
    }

    // If space bar isn't down, process mouse down
    if (dragMode() == QGraphicsView::DragMode::NoDrag) {

        // If left mouse button down
        if (event->button() & Qt::LeftButton) {
            // Restart mouse select movement buffer timer
            m_origin_timer.restart();
            m_allow_movement = false;

            if (my_scene->getSelectionCount() > 0) {
                // ******************* If clicked while holding Alt key start rotating
                if (event->modifiers() & Qt::KeyboardModifier::AltModifier || m_over_handle == Position_Flags::Rotate) {
                    m_view_mode = View_Mode::Rotating;
                    startRotate(m_origin);
                    my_scene->scene_mutex.unlock();
                    return;
                }

                // ******************* If clicked while in a Size Grip Handle start resizing
                if (m_over_handle != Position_Flags::No_Position && m_over_handle != Position_Flags::Move_Item) {
                    m_view_mode = View_Mode::Resizing;
                    startResize(m_origin);
                    my_scene->scene_mutex.unlock();
                    return;
                }
            }


            // ******************* If no modifier keys are down, only select item under mouse. Also updates the Inspector
            //                           if the item clicked was already selected (but the Inspector was showing something
            //                           else like an asset or something)
            //                    #NOTE: If object was not already selected the Inspector will be updated when the
            //                           DrScene::selectionChanged slot fires
            if (event->modifiers() == Qt::KeyboardModifier::NoModifier) {
                if (m_origin_item != nullptr && origin_item_settings != nullptr) {

                    // ***** If we clicked clicked a new item, set selection group to that
                    if (my_scene->getSelectionItems().contains(m_origin_item) == false) {
                        my_scene->clearSelection();
                        m_origin_item->setSelected(true);
                    }

                    // ***** Process press event for item movement (Translation)
                    if (origin_item_settings->isLocked() == false) {
                        // Make sure item is on top before firing QGraphicsView event so we start translating properly
                        double original_z = m_origin_item->zValue();
                        m_origin_item->setZValue(std::numeric_limits<double>::max());
                        QGraphicsView::mousePressEvent(event);
                        m_origin_item->setZValue(original_z);

                        // Prep Translating start
                        viewport()->setCursor(Qt::CursorShape::SizeAllCursor);
                        QTimer::singleShot(500, this, SLOT(checkTranslateToolTipStarted()));

                        m_hide_bounding = true;
                        m_view_mode = View_Mode::Translating;

                        for (auto item : my_scene->getSelectionItems()) {
                            item->moveBy(0, 0);
                        }
                    }

                    if (my_scene->getSelectionItems().count() <= 1) {
                        m_editor_relay->buildInspector( { origin_item_key } );
                        m_editor_relay->updateItemSelection(Editor_Widgets::Scene_View, { origin_item_key } );
                    }
                }

            // ******************** If clicked while control is down, add to selection group, or take out
            } else if (event->modifiers() & Qt::KeyboardModifier::ControlModifier && m_origin_item != nullptr) {

                if (m_origin_item != nullptr) {
                     m_origin_item->setSelected(!m_origin_item->isSelected());
                }

            }


            // ******************* If theres no item under mouse, start selection box
            if (m_origin_item == nullptr) {
                m_view_mode = View_Mode::Selecting;
                startSelect(event);
                processSelection(event->pos());
                my_scene->scene_mutex.unlock();
                return;
            }

        }


    // ******************** Process mouse press event for hand drag
    } else {
        QGraphicsView::mousePressEvent(event);
        m_view_mode = View_Mode::Dragging;
    }

    update();
    my_scene->scene_mutex.unlock();
}


// SLOT: Fired from single shot timer when mouse is down, starts tooltip after x milliseconds if user pressed mouse but hasn't started moving it yet
void DrView::checkTranslateToolTipStarted() {
    if (m_view_mode == View_Mode::Translating) {
        if (m_tool_tip->getTipType() != View_Mode::Translating) {
            my_scene->updateSelectionBox();
            m_tool_tip->startToolTip(View_Mode::Translating, m_origin, my_scene->getSelectionTransform().map(my_scene->getSelectionBox().center()) );
            m_allow_movement = true;
            update();
        }
    }
}




//####################################################################################
//##    Mouse Released
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
        // (i.e. after rubber band box mode is over so it's not trying to change a ton and slow thigs down)
        if (m_view_mode == View_Mode::Selecting) {
            m_view_mode = View_Mode::None;
            if (my_scene->getSelectionCount() > 0)
                m_editor_relay->buildInspector( { static_cast<long>(my_scene->getSelectionItems().first()->data(User_Roles::Key).toLongLong()) } );
            else
                m_editor_relay->buildInspector( { } );
            m_editor_relay->updateItemSelection(Editor_Widgets::Scene_View);
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
            m_editor_relay->updateEditorWidgetsAfterItemChange(
                        Editor_Widgets::Scene_View, Dr::ConvertItemListToSettings(my_scene->getSelectionItems()),
                        { Properties::Thing_Position, Properties::Thing_Size, Properties::Thing_Scale, Properties::Thing_Rotation });
        }

        updateSelectionBoundingBox(6);

        m_view_mode = View_Mode::None;
    }


    // Pass on event, update
    QGraphicsView::mouseReleaseEvent(event);
    this->viewport()->repaint(this->viewport()->rect());
    update();
}



//####################################################################################
//##    Wheel Event / Zoom Functions
//####################################################################################
// Handles zooming in / out of view with mouse wheel
#if QT_CONFIG(wheelevent)
void DrView::wheelEvent(QWheelEvent *event) {
    // Allow for scene scrolling if ctrl (cmd) is down
    if (event->modifiers() & Qt::KeyboardModifier::ControlModifier) {
        QGraphicsView::wheelEvent(event);
        return;
    }

    if (event->delta() > 0)
        zoomInOut(10);
    else
        zoomInOut(-10);
    event->accept();

    // Show tool tip with zoom percentage, if first time start tooltip, otherwise update it
    if (m_tool_tip->getTipType() != View_Mode::Zooming)
        m_tool_tip->startToolTip(View_Mode::Zooming, event->pos(), static_cast<int>(m_zoom_scale * 100) );
    else
        m_tool_tip->updateToolTipData( static_cast<int>(m_zoom_scale * 100) );

    // Reset tool tip timeout, post a single shot timer to eventually hide tool tip
    m_zoom_timer.restart();
    QTimer::singleShot(300, this, SLOT(stoppedZooming()));
}
#endif

// SLOT: Handles hiding tool tip after done zooming
void DrView::stoppedZooming() {
    // If over 1.2 seconds have passed since last time mouse wheel was activated, stop tool tip
    if (m_tool_tip->getTipType() == View_Mode::Zooming) {
        if (m_zoom_timer.elapsed() > 1200) {
            m_tool_tip->stopToolTip();
            this->viewport()->repaint(this->viewport()->rect());
        } else {
            QTimer::singleShot(300, this, SLOT(stoppedZooming()));
        }
    }
}

void DrView::zoomInOut(int level) {
    m_zoom += level;
    if (m_zoom > 500) m_zoom = 500;
    if (m_zoom < -40) m_zoom = -40;
    m_zoom_scale = qPow(qreal(2), (m_zoom - 250) / qreal(50));

    ///Dr::SetLabelText(Label_Names::Label_1, "Zoom: " + QString::number(m_zoom));

    QMatrix matrix;
    matrix.scale(m_zoom_scale, m_zoom_scale);
    matrix.rotate(m_rotate);
    this->setMatrix(matrix);

    updateSelectionBoundingBox(5);
    if (horizontalScrollBar()->value() == 0 && verticalScrollBar()->value() == 0)
        updateGrid();
}














