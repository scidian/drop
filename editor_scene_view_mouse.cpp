//
//      Created by Stephens Nunnally on 1/15/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Graphics View Handling - Mouse Events
//
//

#include "project.h"
#include "project_world.h"
#include "project_world_scene.h"
#include "project_world_scene_object.h"
#include "editor_scene_item.h"

#include "settings.h"
#include "settings_component.h"
#include "settings_component_property.h"

#include "editor_scene_scene.h"
#include "editor_scene_view.h"
#include "interface_relay.h"



//####################################################################################
//##        Mouse Enter
//####################################################################################
void SceneGraphicsView::enterEvent(QEvent *event)
{
    setFocus(Qt::FocusReason::MouseFocusReason);                        // Set focus on mouse enter to allow for space bar pressing hand grab
    m_relay->setAdvisorInfo(Advisor_Info::Scene_Area);                  // Set Advisor text on mouse enter
    QGraphicsView::enterEvent(event);
}



//####################################################################################
//##        Mouse Pressed
//####################################################################################
void SceneGraphicsView::mousePressEvent(QMouseEvent *event)
{
    SceneGraphicsScene    *my_scene = dynamic_cast<SceneGraphicsScene*>(scene());
    QList<QGraphicsItem*>  my_items = my_scene->getSelectionGroupItems();

    // On initial mouse down, store mouse origin point
    m_origin =          event->pos();
    m_origin_in_scene = mapToScene(m_origin);
    m_origin_item =     itemAt(event->pos());

    // If space bar isnt down, process mouse down
    if (dragMode() == QGraphicsView::DragMode::NoDrag) {

        // If left mouse button down
        if (event->button() & Qt::LeftButton) {


            if (my_scene->getSelectionGroupCount() > 0) {
                // ******************* If clicked while holding Alt key
                if (event->modifiers() & Qt::KeyboardModifier::AltModifier ||
                    m_over_handle == Position_Flags::Rotate) {
                    startRotate();
                    return;
                }

                // ******************* If clicked while in a Size Grip Handle
                if (m_over_handle != Position_Flags::No_Position &&
                    m_over_handle != Position_Flags::Move_Item) {
                    startResize();
                    return;
                }
            }


            // ******************* If no keys are down, only select item under mouse
            QGraphicsItem *item_under;

            if (event->modifiers() == Qt::KeyboardModifier::NoModifier) {
                item_under = itemOnTopAtPosition(event->pos());

                // If no item under mouse, deselect all
                if (item_under == nullptr) {
                    my_scene->emptySelectionGroup();

                } else {
                    // If we clicked clicked a new item, set selection group to that
                    if (my_items.contains(item_under) == false) {
                        my_scene->emptySelectionGroup();
                        my_scene->addItemToSelectionGroup(item_under);
                        my_scene->selectSelectionGroup();
                    }

                    // Process press event for item movement (Translation)
                    QGraphicsView::mousePressEvent(event);
                    viewport()->setCursor(Qt::CursorShape::SizeAllCursor);
                    m_view_mode = View_Mode::Translating;
                }

            } else if (event->modifiers() & Qt::KeyboardModifier::ControlModifier) {

                // If clicked while control is down, add to selection group, or take out
                if (m_origin_item != nullptr) {

                    // Break apart selection group, put back together without item clicked
                    int start_count = my_scene->getSelectionGroupCount();
                    item_under = itemOnTopAtPosition(event->pos(), true);

                    // If we originally clicked on group, but that point is empty, start selection box
                    if (item_under == nullptr) {
                        startSelect(event);
                        return;

                    // Otherwise, if we clicked a new item, add that to group
                    } else if (my_items.contains(item_under) == false) {
                        my_scene->addItemToSelectionGroup(item_under);
                    }

                    // If we went one more than one item, to one item. Reset selection box to that item
                    if (start_count > 1 && my_scene->getSelectionGroupCount() == 1) {
                        QGraphicsItem *one_item = my_scene->getSelectionGroupItems().first();
                        my_scene->emptySelectionGroup();
                        my_scene->addItemToSelectionGroup(one_item);
                    }

                    // If selection box is empty, clear it
                    if (my_scene->getSelectionGroup()->childItems().count() < 1) my_scene->emptySelectionGroup();
                }

            }


            // ******************* If theres no item under mouse, start selection box
            if (m_origin_item == nullptr) {
                startSelect(event);
                return;
            }

        }

    } else {
        // Process mouse press event for hand drag
        QGraphicsView::mousePressEvent(event);
        m_view_mode = View_Mode::Dragging;
    }

    update();
}


//####################################################################################
//##        Returns item on top of scene at point in View, ignoring selection group
//####################################################################################
QGraphicsItem* SceneGraphicsView::itemOnTopAtPosition(QPoint check_point, bool take_item_on_top_out)
{
    SceneGraphicsScene    *my_scene = dynamic_cast<SceneGraphicsScene*>(scene());
    QGraphicsItem*         selection = my_scene->getSelectionGroupAsGraphicsItem();
    QGraphicsItem         *item_on_top;
    QList<QGraphicsItem*>  possible_items;

    // Make a list of all items at point excluding selection group
    for (auto item : items(check_point))
        if (item != selection)
            possible_items.append(item);

    // If no items at position, return null
    if (possible_items.count() == 0) return nullptr;

    // Find higest item that is not selection group
    item_on_top = possible_items.first();
    for (auto item : possible_items) {
        if (item->zValue() > item_on_top->zValue())
            item_on_top = item;
    }

    // If we should take out item at point if its in selection group, remove it
    // (This is used for when the user is holding control and clicks an item in the selection group)
    if (take_item_on_top_out && my_scene->getSelectionGroupItems().contains(item_on_top))
        my_scene->getSelectionGroup()->removeFromGroup(item_on_top);

    return item_on_top;
}


//####################################################################################
//##        Mouse Moved
//####################################################################################
void SceneGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    // Try and lock function, so we ony run this once at a time
    if (mouse_move_mutex.tryLock(10) == false) return;

    m_last_mouse_pos = event->pos();                                    // Store event mouse position
    if (m_view_mode != View_Mode::None) update();                       // Call Paint Event (which update rects) before we start

    // Adjust for Qt Arrow not having point right at tip of
    QPointF adjust_mouse = m_last_mouse_pos;
    adjust_mouse.setX(m_last_mouse_pos.x() - 2);
    adjust_mouse.setY(m_last_mouse_pos.y() - 2);

    // Grab the scene as a SceneGraphicsScene
    SceneGraphicsScene *my_scene = dynamic_cast<SceneGraphicsScene *>(scene());
    QGraphicsItem *check_item;

    // ******************** Grab item under mouse
    check_item = itemOnTopAtPosition(adjust_mouse.toPoint());

    // ******************** Check selection handles to see if mouse is over one
    if (m_over_handle == Position_Flags::Move_Item) m_over_handle = Position_Flags::No_Position;

    if (my_scene->getSelectionGroupCount() > 0 && m_view_mode == View_Mode::None && m_flag_key_down_spacebar == false) {
        m_over_handle = Position_Flags::No_Position;
        if (m_handles[Position_Flags::Top].containsPoint(adjust_mouse, Qt::FillRule::OddEvenFill))    m_over_handle = Position_Flags::Top;
        if (m_handles[Position_Flags::Bottom].containsPoint(adjust_mouse, Qt::FillRule::OddEvenFill)) m_over_handle = Position_Flags::Bottom;
        if (m_handles[Position_Flags::Left].containsPoint(adjust_mouse, Qt::FillRule::OddEvenFill))   m_over_handle = Position_Flags::Left;
        if (m_handles[Position_Flags::Right].containsPoint(adjust_mouse, Qt::FillRule::OddEvenFill))  m_over_handle = Position_Flags::Right;

        // Check corners second as they're more important
        if (m_handles[Position_Flags::Top_Left].containsPoint(adjust_mouse, Qt::FillRule::OddEvenFill))     m_over_handle = Position_Flags::Top_Left;
        if (m_handles[Position_Flags::Top_Right].containsPoint(adjust_mouse, Qt::FillRule::OddEvenFill))    m_over_handle = Position_Flags::Top_Right;
        if (m_handles[Position_Flags::Bottom_Left].containsPoint(adjust_mouse, Qt::FillRule::OddEvenFill))  m_over_handle = Position_Flags::Bottom_Left;
        if (m_handles[Position_Flags::Bottom_Right].containsPoint(adjust_mouse, Qt::FillRule::OddEvenFill)) m_over_handle = Position_Flags::Bottom_Right;

        if (m_handles[Position_Flags::Rotate].containsPoint(adjust_mouse, Qt::FillRule::OddEvenFill)) m_over_handle = Position_Flags::Rotate;

        if (m_over_handle == Position_Flags::No_Position && my_scene->getSelectionGroupItems().contains(check_item))
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
    if (m_relay->debugFlag(Debug_Flags::Mouse_Coordinates)) {
        m_relay->setLabelText(Label_Names::Label_Mouse_1, "Mouse Scene X: " + QString::number(mapToScene(m_last_mouse_pos).x()) +
                                                                    ", Y: " + QString::number(mapToScene(m_last_mouse_pos).y()) );
        m_relay->setLabelText(Label_Names::Label_Mouse_2, "Mouse View  X: " + QString::number(m_last_mouse_pos.x()) +
                                                                    ", Y: " + QString::number(m_last_mouse_pos.y()) );
        m_relay->setLabelText(Label_Names::Label_Pos_Flag, "Position Flag: " + QString::number(static_cast<int>(m_over_handle)) +
                                                             ", Pos Angle: " + QString::number(a));
    }
    // !!!!! END

    // !!!!! #DEBUG:    Showing object data
    if (m_relay->debugFlag(Debug_Flags::Selected_Item_Data) && check_item != nullptr) {
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
        m_relay->setLabelText(Label_Names::Label_Z_Order, "Z Order: " + QString::number(item->zValue()) +
                                                           ", Name: " + item->data(User_Roles::Name).toString() );
        m_relay->setLabelText(Label_Names::Label_Object_5,
                        "Group Scale X: " + QString::number(my_scene->getSelectionGroupAsGraphicsItem()->data(User_Roles::Scale).toPointF().x()) +
                                  ", Y: " + QString::number(my_scene->getSelectionGroupAsGraphicsItem()->data(User_Roles::Scale).toPointF().y()) );
    } else if (m_relay->debugFlag(Debug_Flags::Selected_Item_Data)) {
        if (m_view_mode == View_Mode::None && check_item == nullptr) {
            m_relay->setLabelText(Label_Names::Label_Position, "Null");
            m_relay->setLabelText(Label_Names::Label_Center, "Null");
            m_relay->setLabelText(Label_Names::Label_Scale, "Null");
            m_relay->setLabelText(Label_Names::Label_Rotate, "Null");
            m_relay->setLabelText(Label_Names::Label_Z_Order, "Null");
            m_relay->setLabelText(Label_Names::Label_Object_5, "Null");
        }
    }
    // !!!!! END



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

    // Pass on event, update and unlock mutex
    QGraphicsView::mouseMoveEvent(event);
    if (m_view_mode != View_Mode::None) update();

    mouse_move_mutex.unlock();
}



//####################################################################################
//##        Mouse Released
//####################################################################################
void SceneGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    // Process left mouse button released
    if (event->button() & Qt::LeftButton)
    {
        m_rubber_band->hide();
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
void SceneGraphicsView::wheelEvent(QWheelEvent *event)
{
    if (event->modifiers() & Qt::KeyboardModifier::ControlModifier) {
        QGraphicsView::wheelEvent(event);
        return;
    }
    if (event->delta() > 0) {
        zoomInOut(10);  }
    else {
        zoomInOut(-10); }
    event->accept();
}
#endif

void SceneGraphicsView::zoomInOut(int level)
{
    m_zoom += level;
    if (m_zoom > 500) m_zoom = 500;
    if (m_zoom < 100) m_zoom = 100;
    applyUpdatedMatrix();
}

void SceneGraphicsView::applyUpdatedMatrix()
{
    m_zoom_scale = qPow(qreal(2), (m_zoom - 250) / qreal(50));
    QMatrix matrix;
    matrix.scale(m_zoom_scale, m_zoom_scale);
    matrix.rotate(m_rotate);
    this->setMatrix(matrix);
}














