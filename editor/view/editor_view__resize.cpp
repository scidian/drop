//
//      Created by Stephens Nunnally on 1/8/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Graphics View Handling - View_Mode::Resizing
//
//
#include <math.h>

#include "editor/colors/colors.h"
#include "editor/debug.h"
#include "editor/globals_editor.h"
#include "editor/helper_editor.h"
#include "editor/interface_editor_relay.h"
#include "editor/view/editor_item.h"
#include "editor/view/editor_scene.h"
#include "editor/view/editor_view.h"
#include "engine/opengl/opengl.h"
#include "library/dr_math.h"
#include "model/project/project.h"
#include "model/project/project_world.h"
#include "model/project/project_stage.h"
#include "model/project/project_thing.h"
#include "model/settings/settings.h"
#include "model/settings/settings_component.h"
#include "model/settings/settings_component_property.h"


//####################################################################################
//##    Starts resizing mode
//####################################################################################
void DrView::startResize(QPoint mouse_in_view, bool use_tool_tip) {
    // Figure out starting orientation
    m_pre_resize_scale = QPointF(1, 1);
    if (my_scene->getSelectionScale().x() < 0) m_pre_resize_scale.setX(-1);
    if (my_scene->getSelectionScale().y() < 0) m_pre_resize_scale.setY(-1);

    // Create a graphics item group with a copy of selected items to use for resizing
    double angle = my_scene->getSelectionAngle();
    m_group = my_scene->createEmptyItemGroup(angle, m_pre_resize_scale);
    for (auto child : my_scene->getSelectionItems()) {
        child->setData(User_Roles::Pre_Resize_Scale, (child->data(User_Roles::Scale).toPointF()) );
        DrItem *child_as_item = dynamic_cast<DrItem*>(child);
        DrItem *dritem = new DrItem(m_project, child_as_item->getEditorRelay(), child_as_item->getThing(), true);
        dritem->setVisible(false);
        dritem->setEnabled(false);
        dritem->setPos(child_as_item->pos());
        dritem->setTransform(child_as_item->transform());
        m_group->addToGroup(dritem);
    }
    m_group->setEnabled(false);
    m_group->setVisible(false);

    // Store which grip handle we started over and the initial slection bounding box
    m_start_resize_grip = m_over_handle;
    m_start_resize_rect = my_scene->totalSelectionSceneRect();

    // Save pre resize selection box points
    for (auto point : m_selection_points) {
        m_pre_resize_corners[point.first] = m_selection_points[point.first];
    }

    // Set up our custom tool tip
    double group_width =  QLineF( m_selection_points[Position_Flags::Left], m_selection_points[Position_Flags::Right] ).length();
    double group_height = QLineF( m_selection_points[Position_Flags::Top],  m_selection_points[Position_Flags::Bottom]).length();
    if (use_tool_tip)
        m_tool_tip->startToolTip(m_view_mode, mouse_in_view, QPointF( group_width, group_height ) );
}


//####################################################################################
//##    Call appropriate resize function
//####################################################################################
void DrView::resizeSelection(QPointF mouse_in_scene, bool use_exact_scale, QPointF scale_to_use) {
    // Figure out what sides to use for x axis and y axis
    switch (m_start_resize_grip) {
        case Position_Flags::Top_Left:      m_do_x = X_Axis::Left;    m_do_y = Y_Axis::Top;     break;
        case Position_Flags::Top:           m_do_x = X_Axis::None;    m_do_y = Y_Axis::Top;     break;
        case Position_Flags::Top_Right:     m_do_x = X_Axis::Right;   m_do_y = Y_Axis::Top;     break;
        case Position_Flags::Right:         m_do_x = X_Axis::Right;   m_do_y = Y_Axis::None;    break;
        case Position_Flags::Bottom_Right:  m_do_x = X_Axis::Right;   m_do_y = Y_Axis::Bottom;  break;
        case Position_Flags::Bottom:        m_do_x = X_Axis::None;    m_do_y = Y_Axis::Bottom;  break;
        case Position_Flags::Bottom_Left:   m_do_x = X_Axis::Left;    m_do_y = Y_Axis::Bottom;  break;
        case Position_Flags::Left:          m_do_x = X_Axis::Left;    m_do_y = Y_Axis::None;    break;
        default:                            m_do_x = X_Axis::None;    m_do_y = Y_Axis::None;
    }
    resizeSelectionWithRotate(mouse_in_scene, use_exact_scale, scale_to_use);
}


// Finds the ooposite side name of side passed in, no error catch if corner passed in
Position_Flags DrView::findOppositeSide(Position_Flags start_side) {
    switch (start_side) {
        case Position_Flags::Top:          return Position_Flags::Bottom;
        case Position_Flags::Bottom:       return Position_Flags::Top;
        case Position_Flags::Left:         return Position_Flags::Right;
        case Position_Flags::Right:        return Position_Flags::Left;
        case Position_Flags::Top_Left:     return Position_Flags::Bottom_Right;
        case Position_Flags::Top_Right:    return Position_Flags::Bottom_Left;
        case Position_Flags::Bottom_Left:  return Position_Flags::Top_Right;
        case Position_Flags::Bottom_Right: return Position_Flags::Top_Left;
        default:    return Position_Flags::Top;
    }
}



//####################################################################################
//##    Main resize function
//####################################################################################
void DrView::resizeSelectionWithRotate(QPointF mouse_in_scene, bool use_exact_scale, QPointF scale_to_use) {
    // Test for scene, convert to our custom class
    if (scene() == nullptr) return;

    // Initialize point variable used to keep track of last snapping resize, this is so we can limit a snap to one time. At some angles
    // when we try to keep resizing to a snapped point the item will vibrate as it tries to continously remove shearing
    if (m_resize_started == false) {
        m_last_mouse_snap = QPointF( -10000100001, -10000100001 );
        m_resize_started  = true;
    }

    // Load item starting width and height
    double item_width =  m_group->boundingRect().width();
    double item_height = m_group->boundingRect().height();

    // ********** Find corners / sides we're working with, calculate new width / height
    QPointF corner_opposite = m_selection_points[findOppositeSide(m_start_resize_grip)];
    QPointF center_point =    m_selection_points[Position_Flags::Center];
    double  angle = my_scene->getSelectionAngle();

    // Create transform to rotate center line back to zero
    QTransform remove_rotation;
    remove_rotation.translate(center_point.x(), center_point.y());
    remove_rotation.rotate(-angle);
    remove_rotation.translate(-center_point.x(), -center_point.y());

    // Rotate back to zero degree shape angle, starts in view coordinates - ends up in scene coords
    QPointF zero_rotated_opposite_in_scene =remove_rotation.map( corner_opposite );

    // Snap if snapping is turned on
    if ((m_grid_resize_snap && m_handles_shape == Handle_Shapes::Squares) && !use_exact_scale) {
        mouse_in_scene = roundToGrid( mouse_in_scene );
        // If we already resized to this point, cancel the resize to the current position
        if (mouse_in_scene == m_last_mouse_snap)    return;
        // Otherwise continue, but store this resize position as the last snapped to
        else                                        m_last_mouse_snap = mouse_in_scene;
    }

    // Map resize point to scene coordinates
    QPointF point_in_shape = remove_rotation.map( mouse_in_scene);
    double  scale_x;
    double  scale_y;

    if (!use_exact_scale) {
        // ***** Calculate X scale
        if (m_do_x == X_Axis::Right)
            scale_x = QLineF(zero_rotated_opposite_in_scene, point_in_shape).dx() / item_width;
        else if (m_do_x == X_Axis::Left)
            scale_x = QLineF(point_in_shape, zero_rotated_opposite_in_scene).dx() / item_width;
        else
            scale_x = m_pre_resize_scale.x();

        // ***** Calculate Y scale
        if (m_do_y == Y_Axis::Bottom)
            scale_y = QLineF(zero_rotated_opposite_in_scene, point_in_shape).dy() / item_height;
        else if (m_do_y == Y_Axis::Top)
            scale_y = QLineF(point_in_shape, zero_rotated_opposite_in_scene).dy() / item_height;
        else
            scale_y = m_pre_resize_scale.y();

    } else {
        scale_x = scale_to_use.x() * m_pre_resize_scale.x();
        scale_y = scale_to_use.y() * m_pre_resize_scale.y();
    }

    // Make sure it doesnt disappear off the screen, Qt doesnt like when items scale go to zero
    if (scale_x <  .0001 && scale_x >= 0) scale_x =  .0001;
    if (scale_x > -.0001 && scale_x <= 0) scale_x = -.0001;
    if (scale_y <  .0001 && scale_y >= 0) scale_y =  .0001;
    if (scale_y > -.0001 && scale_y <= 0) scale_y = -.0001;

    // Check if a light is selected (or another item that needs to be square)
    bool contains_square_item = false;
    // Incorporated this into removeShearing, but this works too for a group of items that need to remain square
    ///for (auto child : my_scene->getSelectionItems()) {
    ///    contains_square_item |= containsSquareItem(child);
    ///}

    // If shift or control keys are down, maintain starting aspect ratio
    if (m_flag_key_down_shift || contains_square_item) {
        double pre_resize_ratio;
        if (m_do_y == Y_Axis::None) {
            pre_resize_ratio = m_pre_resize_scale.y() / m_pre_resize_scale.x();
            scale_y = scale_x * pre_resize_ratio;
        } else {
            pre_resize_ratio = m_pre_resize_scale.x() / m_pre_resize_scale.y();
            scale_x = scale_y * pre_resize_ratio;
        }
    }


    // ***** Apply new scale
    double transform_scale_x = Dr::CheckScaleNotZero(scale_x);
    double transform_scale_y = Dr::CheckScaleNotZero(scale_y);
    QTransform t = QTransform().rotate(angle).scale(transform_scale_x, transform_scale_y);
    m_group->setTransform(t);

    // ***** Translate if needed
    Position_Flags resize_flag = Position_Flags::Top_Left;

    if (m_flag_key_down_control || use_exact_scale)
        resize_flag = Position_Flags::Center;

    else if (m_do_x == X_Axis::Left && m_do_y == Y_Axis::None)
        resize_flag = Position_Flags::Right;
    else if (m_do_x == X_Axis::Left && m_do_y == Y_Axis::Top)
        resize_flag = Position_Flags::Bottom_Right;
    else if (m_do_x == X_Axis::Left && m_do_y == Y_Axis::Bottom)
        resize_flag = Position_Flags::Top_Right;

    else if (m_do_x == X_Axis::Right && m_do_y == Y_Axis::None)
        resize_flag = Position_Flags::Left;
    else if (m_do_x == X_Axis::Right && m_do_y == Y_Axis::Top)
        resize_flag = Position_Flags::Bottom_Left;
    else if (m_do_x == X_Axis::Right && m_do_y == Y_Axis::Bottom)
        resize_flag = Position_Flags::Top_Left;

    else if (m_do_x == X_Axis::None && m_do_y == Y_Axis::Top)
        resize_flag = Position_Flags::Bottom;
    else if (m_do_x == X_Axis::None && m_do_y == Y_Axis::Bottom)
        resize_flag = Position_Flags::Top;

    QPointF new_pos = m_pre_resize_corners[resize_flag] ;
    my_scene->setPositionByOrigin(m_group, resize_flag, new_pos.x(), new_pos.y());


    // ***** Remove any shearing
    QPointF new_scale(scale_x, scale_y);
    for (auto child : my_scene->getSelectionItems()) {
        removeShearing(child, new_scale);
    }


    // ***** Aligns new selected items origin location with proper resize starting point
    Position_Flags origin_flag = resize_flag;
    if (scale_y < 0 && scale_x > 0) {
         switch (origin_flag) {
             case Position_Flags::Bottom:        origin_flag = Position_Flags::Top;              break;
             case Position_Flags::Bottom_Right:  origin_flag = Position_Flags::Top_Right;        break;
             case Position_Flags::Bottom_Left:   origin_flag = Position_Flags::Top_Left;         break;
             case Position_Flags::Top:           origin_flag = Position_Flags::Bottom;           break;
             case Position_Flags::Top_Right:     origin_flag = Position_Flags::Bottom_Right;     break;
             case Position_Flags::Top_Left:      origin_flag = Position_Flags::Bottom_Left;      break;
             default: ;
        }
    } else if (scale_y > 0 && scale_x < 0) {
        switch (origin_flag) {
            case Position_Flags::Right:         origin_flag = Position_Flags::Left;             break;
            case Position_Flags::Top_Right:     origin_flag = Position_Flags::Top_Left;         break;
            case Position_Flags::Bottom_Right:  origin_flag = Position_Flags::Bottom_Left;      break;
            case Position_Flags::Left:          origin_flag = Position_Flags::Right;            break;
            case Position_Flags::Top_Left:      origin_flag = Position_Flags::Top_Right;        break;
            case Position_Flags::Bottom_Left:   origin_flag = Position_Flags::Bottom_Right;     break;
            default: ;
        }
    } else if (scale_y < 0 && scale_x < 0) {
        switch (origin_flag) {
            case Position_Flags::Right:         origin_flag = Position_Flags::Left;             break;
            case Position_Flags::Left:          origin_flag = Position_Flags::Right;            break;
            case Position_Flags::Top:           origin_flag = Position_Flags::Bottom;           break;
            case Position_Flags::Bottom:        origin_flag = Position_Flags::Top;              break;
            case Position_Flags::Top_Right:     origin_flag = Position_Flags::Bottom_Left;      break;
            case Position_Flags::Bottom_Right:  origin_flag = Position_Flags::Top_Left;         break;
            case Position_Flags::Top_Left:      origin_flag = Position_Flags::Bottom_Right;     break;
            case Position_Flags::Bottom_Left:   origin_flag = Position_Flags::Top_Right;        break;
            default: ;
        }
    }
    QGraphicsItemGroup *temp = my_scene->createEmptyItemGroup(angle);
    for (auto child : my_scene->getSelectionItems()) {
        temp->addToGroup(child);
    }
    my_scene->setPositionByOrigin(temp, origin_flag, new_pos.x(), new_pos.y());
    my_scene->destroyItemGroup(temp);

    // ***** Update stored scale and selection box
    my_scene->setSelectionScale( new_scale );
    my_scene->updateSelectionBox();
}


//####################################################################################
//##    Remove shearing from item
//####################################################################################
void DrView::removeShearing(QGraphicsItem *item, QPointF scale) {

    // Figure out which item in invisible cloned resize group to base transform off of
    DrItem *original = dynamic_cast<DrItem*>(item);
    DrItem *clone = nullptr;
    for (auto child : m_group->childItems()) {
        DrItem *child_as_item = dynamic_cast<DrItem*>(child);
        if (child_as_item->getThingKey() == original->getThingKey()) {
            clone = child_as_item;
            break;
        }
    }
    if (clone == nullptr) return;

    // Calculate a transform that includes rotation and new scale
    double  angle =         original->data(User_Roles::Rotation).toDouble();
    QPointF start_scale =   original->data(User_Roles::Pre_Resize_Scale).toPointF();

    // If item is roated closer to a vertical orientation swap the x and y scaling factor we calculated for the group resize
    double new_scale_x;
    double new_scale_y;
    double select_angle = my_scene->getSelectionAngle();
    while (select_angle >  360) { select_angle -= 360; }
    while (select_angle <    0) { select_angle += 360; }
    double diff_angle = angle - select_angle;
    while (diff_angle >  360)   { diff_angle -= 360; }
    while (diff_angle <    0)   { diff_angle += 360; }

    // Check if item is more vertical or more horizontal, figure out our new scale factor for this item accordingly
    if ((diff_angle > 45 && diff_angle < 135) || (diff_angle > 225 && diff_angle < 315)) {
        new_scale_x = abs(start_scale.x()) * abs(scale.y());
        new_scale_y = abs(start_scale.y()) * abs(scale.x());

        // Check to make sure we respect flipping
        if ((m_pre_resize_scale.y() < 0 && start_scale.x() < 0) || (m_pre_resize_scale.y() > 0 && start_scale.x() > 0)) {
            if (scale.y() < 0) new_scale_x *= -1;
        } else {
            if (scale.y() > 0) new_scale_x *= -1;
        }
        if ((m_pre_resize_scale.x() < 0 && start_scale.y() < 0) || (m_pre_resize_scale.x() > 0 && start_scale.y() > 0)) {
            if (scale.x() < 0) new_scale_y *= -1;
        } else {
            if (scale.x() > 0) new_scale_y *= -1;
        }

    } else {
        new_scale_x = abs(start_scale.x()) * abs(scale.x());
        new_scale_y = abs(start_scale.y()) * abs(scale.y());

        // Check to make sure we respect flipping
        if ((m_pre_resize_scale.x() < 0 && start_scale.x() < 0) || (m_pre_resize_scale.x() > 0 && start_scale.x() > 0)) {
            if (scale.x() < 0) new_scale_x *= -1;
        } else {
            if (scale.x() > 0) new_scale_x *= -1;
        }
        if ((m_pre_resize_scale.y() < 0 && start_scale.y() < 0) || (m_pre_resize_scale.y() > 0 && start_scale.y() > 0)) {
            if (scale.y() < 0) new_scale_y *= -1;
        } else {
            if (scale.y() > 0) new_scale_y *= -1;
        }
    }

    // ***** Checks if Items should have to remain Square shaped (light, etc) and forces it to do so
    // ***** Also limits max size
    //       #KEYWORD: "keep square", "lock size", "same size", "limit size"
    if (original->getThing() != nullptr) {
        DrThing* thing = original->getThing();
        bool    has_max_x = !(Dr::FuzzyCompare(thing->maxSize().x, 0.0)),     has_max_y = !(Dr::FuzzyCompare(thing->maxSize().y, 0.0));
        bool    has_min_x = !(Dr::FuzzyCompare(thing->minSize().x, 0.0)),     has_min_y = !(Dr::FuzzyCompare(thing->minSize().y, 0.0));
        bool    keep_square = thing->keepSquare();

        if (keep_square || has_max_x || has_max_y || has_min_x || has_min_y) {
            if (has_max_y && new_scale_y * original->getAssetHeight() > thing->maxSize().y) new_scale_y = thing->maxSize().y / original->getAssetHeight();
            if (has_min_y && new_scale_y * original->getAssetHeight() < thing->minSize().y) new_scale_y = thing->minSize().y / original->getAssetHeight();
            if (has_max_x && new_scale_x * original->getAssetWidth()  > thing->maxSize().x) new_scale_x = thing->maxSize().x / original->getAssetWidth();
            if (has_min_x && new_scale_x * original->getAssetWidth()  < thing->minSize().x) new_scale_x = thing->minSize().x / original->getAssetWidth();

            if (keep_square) {
                if (Dr::FuzzyCompare(start_scale.x(), new_scale_x))
                     new_scale_x = new_scale_y;
                else
                     new_scale_y = new_scale_x;
            }
        }
    }

    // Update item property
    original->setData(User_Roles::Scale, QPointF(new_scale_x, new_scale_y) );
    double transform_scale_x = Dr::CheckScaleNotZero(new_scale_x);
    double transform_scale_y = Dr::CheckScaleNotZero(new_scale_y);

    QTransform no_shear = QTransform()
            .rotate(angle)
            .scale(transform_scale_x, transform_scale_y);
    original->setTransform(no_shear);

    // Set item to center point of clone
    QPointF new_center = clone->mapToScene( clone->boundingRect().center() );
    my_scene->setPositionByOrigin(original, Position_Flags::Center, new_center.x(), new_center.y());

}


//####################################################################################
//##    Checks if any of these Items should have to remain Square shaped
//####################################################################################
bool DrView::containsSquareItem(QGraphicsItem *item) {
    DrItem *original = dynamic_cast<DrItem*>(item);
    if (original == nullptr) return false;

    DrThing *thing = original->getThing();
    if (thing == nullptr) return false;

    return (thing->getThingType() == DrThingType::Light);
}
















