//
//      Created by Stephens Nunnally on 10/19/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "editor/helper_library.h"
#include "library/dr_math.h"
#include "model/project/project.h"
#include "model/project/project_asset.h"
#include "model/project/project_world.h"
#include "model/project/project_stage.h"
#include "model/project/project_thing.h"
#include "model/settings/settings.h"
#include "model/settings/settings_component.h"
#include "model/settings/settings_component_property.h"


//####################################################################################
//##    Sets Z-Order and appropriate Sub Order
//####################################################################################
void ShiftOrder(std::list<OrderInfo> &order_info, int needs_to_be_empty) {
    for (auto &order : order_info) {
        if (order.sub_order == needs_to_be_empty) {
            ShiftOrder(order_info, needs_to_be_empty + 1);
            order.sub_order++;
            order.thing->setComponentPropertyValue(Components::Thing_Layering, Properties::Thing_Sub_Z_Order, order.sub_order);
        }
    }
}

double DrThing::getZOrderWithSub() {
    double z_order = getComponentPropertyValue(Components::Thing_Layering, Properties::Thing_Z_Order).toDouble();
    int  sub_order = getComponentPropertyValue(Components::Thing_Layering, Properties::Thing_Sub_Z_Order).toInt();
    return (z_order + (static_cast<double>(sub_order) / 100000.0));
}

void DrThing::setZOrderWithSub(double z_order, Z_Insert insert, int position) {
    setComponentPropertyValue(Components::Thing_Layering, Properties::Thing_Z_Order, z_order);

    // ***** Get list of sub orders along same Z-Axis
    std::list<OrderInfo> order_info;
    for (auto &thing_pair : m_parent_stage->getThingMap()) {
        if (thing_pair.first == getKey()) continue;
        double thing_z =   thing_pair.second->getComponentPropertyValue(Components::Thing_Layering, Properties::Thing_Z_Order).toDouble();
        int    thing_sub = thing_pair.second->getComponentPropertyValue(Components::Thing_Layering, Properties::Thing_Sub_Z_Order).toInt();
        if (Dr::IsCloseTo(thing_z, z_order, 0.0001)) { order_info.push_back(OrderInfo { thing_pair.first, thing_pair.second, thing_sub }); }
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

    // ***** Set this Things sub order
    setComponentPropertyValue(Components::Thing_Layering, Properties::Thing_Sub_Z_Order, sub_z_order);

    // ***** Fill in any empty spaces
    std::list<DrThing*> things_sorted;
    for (auto thing : m_parent_stage->thingsSortedByZOrder(Qt::DescendingOrder)) {
        double thing_z = thing->getComponentPropertyValue(Components::Thing_Layering, Properties::Thing_Z_Order).toDouble();
        if (Dr::IsCloseTo(thing_z, z_order, 0.0001)) { things_sorted.push_back(thing); }
    }
    int i = static_cast<int>(things_sorted.size());
    for (auto thing : things_sorted) {
        thing->setComponentPropertyValue(Components::Thing_Layering, Properties::Thing_Sub_Z_Order, i);
        i--;
    }
}


//####################################################################################
//##    Move Z-Order / Sub Order
//####################################################################################
void DrThing::moveBackward() {
    std::vector<long> list = m_parent_stage->thingKeysSortedByZOrder(Qt::DescendingOrder);

    // Find Things position within the Stage
    int my_position = 0;
    for (size_t i = 0; i < list.size(); i++) {
        if (list[i] == getKey()) my_position = static_cast<int>(i);
    }
    if (my_position == static_cast<int>(list.size() - 1)) return;      // Already in the back

    // Find out Z Info on next lowest Thing
    long back_of_key =  list[static_cast<size_t>(my_position + 1)];
    DrThing *thing =    getParentProject()->findThingFromKey(back_of_key);
    if (thing == nullptr) return;

    double z_order =    thing->getComponentPropertyValue(Components::Thing_Layering, Properties::Thing_Z_Order).toDouble();
    int  sub_order =    thing->getComponentPropertyValue(Components::Thing_Layering, Properties::Thing_Sub_Z_Order).toInt();

    setZOrderWithSub(z_order, Z_Insert::At_Position, sub_order);
}

void DrThing::moveForward() {
    std::vector<long> list = m_parent_stage->thingKeysSortedByZOrder(Qt::DescendingOrder);

    // Find Things position within the Stage
    int my_position = 0;
    for (size_t i = 0; i < list.size(); i++) {
        if (list[i] == getKey()) my_position = static_cast<int>(i);
    }
    if (my_position == 0) return;                       // Already in the front

    // Find out Z Info on next highest Thing
    long front_of_key = list[static_cast<size_t>(my_position - 1)];
    DrThing *thing =    getParentProject()->findThingFromKey(front_of_key);
    if (thing == nullptr) return;

    double z_order =    thing->getComponentPropertyValue(Components::Thing_Layering, Properties::Thing_Z_Order).toDouble();
    int  sub_order =    thing->getComponentPropertyValue(Components::Thing_Layering, Properties::Thing_Sub_Z_Order).toInt() + 1;

    setZOrderWithSub(z_order, Z_Insert::At_Position, sub_order);
}















