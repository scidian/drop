//
//      Created by Stephens Nunnally on 5/4/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "core/dr_debug.h"
#include "core/dr_math.h"
#include "project/dr_project.h"
#include "project/entities/dr_asset.h"
#include "project/entities/dr_ui.h"
#include "project/entities/dr_widget.h"
#include "project/settings/settings.h"
#include "project/settings/settings_component.h"
#include "project/settings/settings_component_property.h"


//####################################################################################
//##    Sets Z-Order and appropriate Sub Order
//####################################################################################
void ShiftOrder(std::list<Order_Info_Widget> &order_info, int needs_to_be_empty) {
    for (auto &order : order_info) {
        if (order.sub_order == needs_to_be_empty) {
            ShiftOrder(order_info, needs_to_be_empty + 1);
            order.sub_order++;
            order.widget->setComponentPropertyValue(Comps::Thing_Layering, Props::Thing_Sub_Z_Order, order.sub_order);
        }
    }
}

double DrWidget::getZOrderWithSub() {
    double z_order = getComponentPropertyValue(Comps::Thing_Layering, Props::Thing_Z_Order).toDouble();
    ///Dr::PrintDebug("Z Order: " + std::to_string(z_order));
    int  sub_order = getComponentPropertyValue(Comps::Thing_Layering, Props::Thing_Sub_Z_Order).toInt();
    ///Dr::PrintDebug("Sub Order: " + std::to_string(sub_order));
    return (z_order + (static_cast<double>(sub_order) / 100000.0));
}

void DrWidget::setZOrderWithSub(double z_order, Z_Insert insert, int position) {
    setComponentPropertyValue(Comps::Thing_Layering, Props::Thing_Z_Order, z_order);

    // ***** Get list of sub orders along same Z-Axis
    std::list<Order_Info_Widget> order_info;
    for (auto &widget_pair : m_parent_ui->getWidgetMap()) {
        if (widget_pair.first == getKey()) continue;
        double widget_z =   widget_pair.second->getComponentPropertyValue(Comps::Thing_Layering, Props::Thing_Z_Order).toDouble();
        int    widget_sub = widget_pair.second->getComponentPropertyValue(Comps::Thing_Layering, Props::Thing_Sub_Z_Order).toInt();
        if (Dr::IsCloseTo(widget_z, z_order, 0.0001)) { order_info.push_back(Order_Info_Widget { widget_pair.first, widget_pair.second, widget_sub }); }
    }

    // ***** Start at back
    int sub_z_order = 1;
    if (insert == Z_Insert::At_Position) sub_z_order = position;

    // ***** Insert in Top of Z Layer
    if (insert == Z_Insert::Front) {
        for (auto &order : order_info) {
            if (sub_z_order <= order.sub_order) {
                sub_z_order = order.sub_order + 1;
            }
        }

    // ***** Shift Positions Higher than new Sub Position
    } else {
        ShiftOrder(order_info, sub_z_order);
    }

    // ***** Set this Widgets' sub order
    setComponentPropertyValue(Comps::Thing_Layering, Props::Thing_Sub_Z_Order, sub_z_order);

    // ***** Fill in any empty spaces
    std::list<DrWidget*> widgets_sorted;
    for (auto widget : m_parent_ui->widgetsSortedByZOrder(Sort_Order::DescendingOrder)) {
        double widget_z = widget->getComponentPropertyValue(Comps::Thing_Layering, Props::Thing_Z_Order).toDouble();
        if (Dr::IsCloseTo(widget_z, z_order, 0.0001)) { widgets_sorted.push_back(widget); }
    }
    int i = static_cast<int>(widgets_sorted.size());
    for (auto widget : widgets_sorted) {
        widget->setComponentPropertyValue(Comps::Thing_Layering, Props::Thing_Sub_Z_Order, i);
        i--;
    }
}


//####################################################################################
//##    Move Z-Order / Sub Order
//####################################################################################
void DrWidget::moveBackward() {
    std::vector<long> list = m_parent_ui->widgetKeysSortedByZOrder(Sort_Order::DescendingOrder);

    // Find Widgets position within the UI
    int my_position = 0;
    for (size_t i = 0; i < list.size(); i++) {
        if (list[i] == getKey()) my_position = static_cast<int>(i);
    }
    if (my_position == static_cast<int>(list.size() - 1)) return;      // Already in the back

    // Find out Z Info on next lowest Widget
    long back_of_key =  list[static_cast<size_t>(my_position + 1)];
    DrWidget *widget =  getParentProject()->findWidgetFromKey(back_of_key);
    if (widget == nullptr) return;

    double z_order =    widget->getComponentPropertyValue(Comps::Thing_Layering, Props::Thing_Z_Order).toDouble();
    int  sub_order =    widget->getComponentPropertyValue(Comps::Thing_Layering, Props::Thing_Sub_Z_Order).toInt();

    setZOrderWithSub(z_order, Z_Insert::At_Position, sub_order);
}

void DrWidget::moveForward() {
    std::vector<long> list = m_parent_ui->widgetKeysSortedByZOrder(Sort_Order::DescendingOrder);

    // Find Widgets' position within the UI
    int my_position = 0;
    for (size_t i = 0; i < list.size(); i++) {
        if (list[i] == getKey()) my_position = static_cast<int>(i);
    }
    if (my_position == 0) return;                       // Already in the front

    // Find out Z Info on next highest Widget
    long front_of_key = list[static_cast<size_t>(my_position - 1)];
    DrWidget *widget =  getParentProject()->findWidgetFromKey(front_of_key);
    if (widget == nullptr) return;

    double z_order =    widget->getComponentPropertyValue(Comps::Thing_Layering, Props::Thing_Z_Order).toDouble();
    int  sub_order =    widget->getComponentPropertyValue(Comps::Thing_Layering, Props::Thing_Sub_Z_Order).toInt() + 1;

    setZOrderWithSub(z_order, Z_Insert::At_Position, sub_order);
}








