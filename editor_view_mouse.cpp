//
//      Created by Stephens Nunnally on 1/15/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Graphics View Handling - Mouse Events
//
//

#include "library.h"

#include "project.h"
#include "project_world.h"
#include "project_world_stage.h"
#include "project_world_stage_object.h"
#include "editor_item.h"

#include "settings.h"
#include "settings_component.h"
#include "settings_component_property.h"

#include "editor_scene.h"
#include "editor_view.h"
#include "interface_relay.h"



//####################################################################################
//##        Mouse Pressed
//####################################################################################
void DrView::mousePressEvent(QMouseEvent *event)
{
    // Test for scene, convert to our custom class and lock the scene
    if (scene() == nullptr) return;
    DrScene *my_scene = dynamic_cast<DrScene*>(scene());
    if (my_scene->scene_mutex.tryLock(100) == false) return;

    // On initial mouse down, store mouse origin point
    m_origin =          event->pos();
    m_origin_in_scene = mapToScene(m_origin);
    m_origin_item =     itemOnTopAtPosition(event->pos());

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


            // ******************* If no keys are down, only select item under mouse
            if (event->modifiers() == Qt::KeyboardModifier::NoModifier) {

                if (m_origin_item != nullptr) {

                    // ***** If we clicked clicked a new item, set selection group to that
                    if (my_scene->getSelectionItems().contains(m_origin_item) == false) {
                        my_scene->clearSelection();
                        m_origin_item->setSelected(true);
                    }

                    // ***** Process press event for item movement (Translation)
                    QGraphicsView::mousePressEvent(event);
                    viewport()->setCursor(Qt::CursorShape::SizeAllCursor);
                    QTimer::singleShot(500, this, SLOT(checkTranslateToolTipStarted()));

                    m_view_mode = View_Mode::Translating;
                }

            // ******************** If clicked while control is down, add to selection group, or take out
            } else if (event->modifiers() & Qt::KeyboardModifier::ControlModifier && m_origin_item != nullptr) {

                if (m_origin_item != nullptr)
                    m_origin_item->setSelected(!m_origin_item->isSelected());

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
void DrView::checkTranslateToolTipStarted()
{
    if (m_view_mode == View_Mode::Translating) {
        if (m_tool_tip->getTipType() != View_Mode::Translating)
            m_tool_tip->startToolTip(View_Mode::Translating, m_origin, mapToScene( m_handles_centers[Position_Flags::Center].toPoint()) );
    }
}


//####################################################################################
//##        Finds item on top of scene at point in View, ignoring selection group
//####################################################################################
QGraphicsItem* DrView::itemOnTopAtPosition(QPoint check_point)
{
    QGraphicsItem         *item_on_top;
    QList<QGraphicsItem*>  possible_items;

    // Make a list of all items at point excluding selection group
    for (auto item : items(check_point)) {

        possible_items.append(item);

        // !!!!! DEBUG: Shows red, green, blue and alpha of pixel under mouse
        if (Dr::CheckDebugFlag(Debug_Flags::Label_Top_Item_RGBA)) {
            QColor pixel_color = dynamic_cast<DrItem*>(item)->getColorAtPoint(check_point, this);
            Dr::SetLabelText(Label_Names::Label_1, "R: " + QString::number(pixel_color.red()) +
                                                   "G: " + QString::number(pixel_color.green()) +
                                                   "B: " + QString::number(pixel_color.blue()) );
            Dr::SetLabelText(Label_Names::Label_2, "Aplha: " + QString::number(pixel_color.alpha()) );
        }
        // !!!!! END
    }

    // If no items at position, exit
    if (possible_items.count() == 0) return nullptr;

    // Find higest item that is not selection group
    item_on_top = possible_items.first();
    for (auto item : possible_items) {
        if (item->zValue() > item_on_top->zValue())
            item_on_top = item;
    }

    return item_on_top;
}


//####################################################################################
//##        Mouse Moved
//####################################################################################
void DrView::mouseMoveEvent(QMouseEvent *event)
{
    // Test for scene, convert to our custom class and lock the scene
    if (scene() == nullptr) return;
    DrScene *my_scene = dynamic_cast<DrScene*>(scene());
    if (my_scene->scene_mutex.tryLock(10) == false) return;

    // Store event mouse position
    m_last_mouse_pos = event->pos();

    // Allow movement if it has been more than x milliseconds or mouse has moved more than 2 pixels
    if (m_allow_movement == false) {
        if (m_origin_timer.elapsed() > 200) {
            m_allow_movement = true;
        } else if (QPoint(m_origin - m_last_mouse_pos).manhattanLength() > 2) {
            m_allow_movement = true;
        }
    }

    // Updates our tool tip position
    if (m_tool_tip->isHidden() == false)
        m_tool_tip->updateToolTipPosition(m_last_mouse_pos);

    // ******************** Grab item under mouse
    QGraphicsItem *check_item = itemOnTopAtPosition(m_last_mouse_pos);

    // ******************** Check selection handles to see if mouse is over one
    if (m_over_handle == Position_Flags::Move_Item) m_over_handle = Position_Flags::No_Position;

    if (my_scene->getSelectionCount() > 0 && m_view_mode == View_Mode::None && m_flag_key_down_spacebar == false) {
        m_over_handle = Position_Flags::No_Position;
        if (m_handles[Position_Flags::Top].containsPoint(m_last_mouse_pos, Qt::FillRule::OddEvenFill))    m_over_handle = Position_Flags::Top;
        if (m_handles[Position_Flags::Bottom].containsPoint(m_last_mouse_pos, Qt::FillRule::OddEvenFill)) m_over_handle = Position_Flags::Bottom;
        if (m_handles[Position_Flags::Left].containsPoint(m_last_mouse_pos, Qt::FillRule::OddEvenFill))   m_over_handle = Position_Flags::Left;
        if (m_handles[Position_Flags::Right].containsPoint(m_last_mouse_pos, Qt::FillRule::OddEvenFill))  m_over_handle = Position_Flags::Right;

        // Check corners second as they're more important
        if (m_handles[Position_Flags::Top_Left].containsPoint(m_last_mouse_pos, Qt::FillRule::OddEvenFill))     m_over_handle = Position_Flags::Top_Left;
        if (m_handles[Position_Flags::Top_Right].containsPoint(m_last_mouse_pos, Qt::FillRule::OddEvenFill))    m_over_handle = Position_Flags::Top_Right;
        if (m_handles[Position_Flags::Bottom_Left].containsPoint(m_last_mouse_pos, Qt::FillRule::OddEvenFill))  m_over_handle = Position_Flags::Bottom_Left;
        if (m_handles[Position_Flags::Bottom_Right].containsPoint(m_last_mouse_pos, Qt::FillRule::OddEvenFill)) m_over_handle = Position_Flags::Bottom_Right;

        if (m_handles[Position_Flags::Rotate].containsPoint(m_last_mouse_pos, Qt::FillRule::OddEvenFill)) m_over_handle = Position_Flags::Rotate;

        if (m_over_handle == Position_Flags::No_Position && my_scene->getSelectionItems().contains(check_item))
            if (m_flag_key_down_alt == false)
                m_over_handle = Position_Flags::Move_Item;

        if (m_flag_key_down_alt == true)
            m_over_handle = Position_Flags::Rotate;
    }

    // If we are over a handle, and not doing anything, set cursor based on precalculated angle
    double a = 0;
    if (m_over_handle != Position_Flags::No_Position && m_view_mode == View_Mode::None && m_flag_key_down_spacebar == false) {

        if (m_over_handle == Position_Flags::Move_Item) {
            viewport()->setCursor(Qt::CursorShape::SizeAllCursor);
        } else if (m_over_handle == Position_Flags::Rotate) {
            viewport()->setCursor(c_rotate_all);
        } else {
            a = m_handles_angles[m_over_handle];

            ///// Custom rotated cursor
            ///QPixmap arrow = QPixmap(":/cursors/size_vertical.png", nullptr, Qt::ImageConversionFlag::AutoDither);
            ///QPixmap rotated = arrow.transformed(QTransform().rotate(a));
            ///int xoffset = (rotated.width() - arrow.width()) / 2;
            ///int yoffset = (rotated.height() - arrow.height()) / 2;
            ///rotated = rotated.copy(xoffset, yoffset, arrow.width(), arrow.height());
            ///viewport()->setCursor(rotated);

            if      (a <  11.25) viewport()->setCursor(c_size_vertical);                              // 0        Top
            else if (a <  33.75) viewport()->setCursor(c_size_022);                                   // 22.5
            else if (a <  56.25) viewport()->setCursor(c_size_045);                                   // 45       Top Right
            else if (a <  78.75) viewport()->setCursor(c_size_067);                                   // 67.5
            else if (a < 101.25) viewport()->setCursor(c_size_horizontal);                            // 90       Right
            else if (a < 123.75) viewport()->setCursor(c_size_112);                                   // 112.5
            else if (a < 146.25) viewport()->setCursor(c_size_135);                                   // 135      Bottom Right
            else if (a < 168.75) viewport()->setCursor(c_size_157);                                   // 157
            else if (a < 191.25) viewport()->setCursor(c_size_vertical);                              // 180      Bottom
            else if (a < 213.75) viewport()->setCursor(c_size_022);                                   // 202
            else if (a < 236.25) viewport()->setCursor(c_size_045);                                   // 225      Bottom Left
            else if (a < 258.75) viewport()->setCursor(c_size_067);                                   // 247
            else if (a < 281.25) viewport()->setCursor(c_size_horizontal);                            // 270      Left
            else if (a < 303.75) viewport()->setCursor(c_size_112);                                   // 292
            else if (a < 326.25) viewport()->setCursor(c_size_135);                                   // 315      Top Left
            else if (a < 348.75) viewport()->setCursor(c_size_157);                                   // 337
            else                 viewport()->setCursor(c_size_vertical);                              // 360      Top
        }
    }


    // If no longer over handle and mouse is up, reset mouse cursor
    if (m_over_handle == Position_Flags::No_Position && m_view_mode == View_Mode::None && m_flag_key_down_spacebar == false)
        viewport()->unsetCursor();



    // !!!!! #DEBUG:    Draw mouse coords on screen
    if (Dr::CheckDebugFlag(Debug_Flags::Label_Mouse_Coordinates)) {
        m_relay->setLabelText(Label_Names::Label_Mouse_1, "Mouse Scene X: " + QString::number(mapToScene(m_last_mouse_pos).x()) +
                                                                    ", Y: " + QString::number(mapToScene(m_last_mouse_pos).y()) );
        m_relay->setLabelText(Label_Names::Label_Mouse_2, "Mouse View  X: " + QString::number(m_last_mouse_pos.x()) +
                                                                    ", Y: " + QString::number(m_last_mouse_pos.y()) );
        m_relay->setLabelText(Label_Names::Label_Pos_Flag, "Position Flag: " + QString::number(static_cast<int>(m_over_handle)) + QString("\t") +
                                                               "Pos Angle: " + QString::number(a));
    }
    // !!!!! END

    // !!!!! #DEBUG:    Showing object data
    if (Dr::CheckDebugFlag(Debug_Flags::Label_Selected_Item_Data) && check_item != nullptr) {
        QGraphicsItem *item = check_item;
        QPointF my_scale =  item->data(User_Roles::Scale).toPointF();
        double  my_angle =  item->data(User_Roles::Rotation).toDouble();
        QPointF my_center = item->sceneTransform().map( item->boundingRect().center() );
        m_relay->setLabelText(Label_Names::Label_Position, "Pos X: " +  QString::number(item->pos().x()) +
                                                         ", Pos Y: " +  QString::number(item->pos().y()) );
        m_relay->setLabelText(Label_Names::Label_Center, "Center X: " + QString::number(my_center.x()) +
                                                              ", Y: " + QString::number(my_center.y()) );
        m_relay->setLabelText(Label_Names::Label_Scale, "Scale X: " +   QString::number(my_scale.x()) +
                                                      ", Scale Y: " +   QString::number(my_scale.y()) );
        m_relay->setLabelText(Label_Names::Label_Rotate, "Rotation: " + QString::number(my_angle));
        m_relay->setLabelText(Label_Names::Label_Z_Order, "Z Order: " + QString::number(item->zValue()) + QString("\t") +
                                                             "Name: " + item->data(User_Roles::Name).toString() );

    } else if (Dr::CheckDebugFlag(Debug_Flags::Label_Selected_Item_Data)) {
        if (m_view_mode == View_Mode::None && check_item == nullptr) {
            m_relay->setLabelText(Label_Names::Label_Position, "Null");
            m_relay->setLabelText(Label_Names::Label_Center, "Null");
            m_relay->setLabelText(Label_Names::Label_Scale, "Null");
            m_relay->setLabelText(Label_Names::Label_Rotate, "Null");
            m_relay->setLabelText(Label_Names::Label_Z_Order, "Null");
        }
    }
    // !!!!! END


    // ***** If we're not doing anything, update the advisor based on item under the mouse
    if (m_view_mode == View_Mode::None) {
        if (check_item != nullptr) {
            m_relay->setAdvisorInfo(check_item->data(User_Roles::Name).toString(), check_item->data(User_Roles::Type).toString());
        } else {
            m_relay->setAdvisorInfo(Advisor_Info::Stage_Area);
        }
    }


    // ******************* If we're in selection mode, process mouse movement and resize box as needed
   if (m_view_mode == View_Mode::Selecting)
        processSelection(event->pos());

    // ******************* If mouse moved while over Size Grip, resize
    if (m_view_mode == View_Mode::Resizing)
        resizeSelection(mapToScene(event->pos()));

    // ******************* If mouse moved while alt pressed, rotate
    if (m_view_mode == View_Mode::Rotating) {
        viewport()->setCursor(c_rotate_all);
        rotateSelection(event->pos());
    }

    // ******************* If mouse moved while in translating mode, update tooltip
    if (m_view_mode == View_Mode::Translating) {
        if (m_allow_movement) {
            // Pass on event to allow movement
            QGraphicsView::mouseMoveEvent(event);

            my_scene->updateSelectionBox();

            if (m_tool_tip->getTipType() != View_Mode::Translating)
                m_tool_tip->startToolTip(View_Mode::Translating, m_origin, mapToScene( m_handles_centers[Position_Flags::Center].toPoint()) );
            else {
                m_tool_tip->updateToolTipData( mapToScene( m_handles_centers[Position_Flags::Center].toPoint()) );
                my_scene->updateChildrenPositionData();
            }
        }
    } else {
        // Pass on event to allow movement
        QGraphicsView::mouseMoveEvent(event);
    }

    // Update
    if (m_view_mode != View_Mode::None) update();

    // Unlock scene mutex
    my_scene->scene_mutex.unlock();
}



//####################################################################################
//##        Mouse Released
//####################################################################################
void DrView::mouseReleaseEvent(QMouseEvent *event)
{
    // Test for scene, convert to our custom class
    if (scene() == nullptr) return;

    // Process left mouse button released
    if (event->button() & Qt::LeftButton)
    {
        m_rubber_band->hide();
        m_tool_tip->stopToolTip();
        m_view_mode = View_Mode::None;
    }

    // Pass on event, update
    QGraphicsView::mouseReleaseEvent(event);
    update();
}



//####################################################################################
//##        Wheel Event / Zoom Functions
//####################################################################################

// Handles zooming in / out of view with mouse wheel
#if QT_CONFIG(wheelevent)
void DrView::wheelEvent(QWheelEvent *event)
{
    // Allow for scene scrolling if ctrl (cmd) is down
    if (event->modifiers() & Qt::KeyboardModifier::ControlModifier) {
        QGraphicsView::wheelEvent(event);
        return;
    }

    if (event->delta() > 0) {
        zoomInOut(10);  }
    else {
        zoomInOut(-10); }
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
void DrView::stoppedZooming()
{
    // If over 1.2 seconds have passed since last time mouse wheel was activated, stop tool tip
    if (m_tool_tip->getTipType() == View_Mode::Zooming) {
        if (m_zoom_timer.elapsed() > 1200)
            m_tool_tip->stopToolTip();
        else
            QTimer::singleShot(300, this, SLOT(stoppedZooming()));
    }
}

void DrView::zoomInOut(int level)
{
    m_zoom += level;
    if (m_zoom > 500) m_zoom = 500;
    if (m_zoom < 100) m_zoom = 100;
    applyUpdatedMatrix();
}

void DrView::applyUpdatedMatrix()
{
    m_zoom_scale = qPow(qreal(2), (m_zoom - 250) / qreal(50));
    QMatrix matrix;
    matrix.scale(m_zoom_scale, m_zoom_scale);
    matrix.rotate(m_rotate);
    this->setMatrix(matrix);
    updateSelectionBoundingBox();
}














