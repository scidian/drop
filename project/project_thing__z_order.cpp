//
//      Created by Stephens Nunnally on 10/19/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "imaging/imaging.h"
#include "opengl/opengl.h"
#include "project/project.h"
#include "project/project_asset.h"
#include "project/project_world.h"
#include "project/project_stage.h"
#include "project/project_thing.h"
#include "settings/settings.h"
#include "settings/settings_component.h"
#include "settings/settings_component_property.h"

#include "debug.h"
#include "globals.h"
#include "helper.h"
#include "helper_qt.h"


//####################################################################################
//##    Sets Z-Order and appropriate Sub Order
//####################################################################################
void ShiftOrder(QList<OrderInfo> &order_info, int needs_to_be_empty) {
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
    setComponentPropertyValue(Components::Thing_Layering, Properties::Thing_Z_Order, QVariant::fromValue(z_order));

    // ***** Get list of sub orders along same Z-Axis
    QList<OrderInfo> order_info;
    for (auto &thing_pair : m_parent_stage->getThingMap()) {
        if (thing_pair.first == getKey()) continue;
        double thing_z =   thing_pair.second->getComponentPropertyValue(Components::Thing_Layering, Properties::Thing_Z_Order).toDouble();
        int    thing_sub = thing_pair.second->getComponentPropertyValue(Components::Thing_Layering, Properties::Thing_Sub_Z_Order).toInt();
        if (Dr::IsCloseTo(thing_z, z_order, 0.0001)) { order_info.append(OrderInfo { thing_pair.first, thing_pair.second, thing_sub }); }
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
    setComponentPropertyValue(Components::Thing_Layering, Properties::Thing_Sub_Z_Order, QVariant(sub_z_order));

    // ***** Fill in any empty spaces
    QList<DrThing*> things_sorted;
    for (auto thing : m_parent_stage->thingsSortedByZOrder(Qt::DescendingOrder)) {
        double thing_z = thing->getComponentPropertyValue(Components::Thing_Layering, Properties::Thing_Z_Order).toDouble();
        if (Dr::IsCloseTo(thing_z, z_order, 0.0001)) { things_sorted.append(thing); }
    }
    int i = things_sorted.count();
    for (auto thing : things_sorted) {
        thing->setComponentPropertyValue(Components::Thing_Layering, Properties::Thing_Sub_Z_Order, QVariant(i));
        i--;
    }
}


//####################################################################################
//##    Move Z-Order / Sub Order
//####################################################################################
void DrThing::moveBackward() {
    QList<long> list = m_parent_stage->thingKeysSortedByZOrder(Qt::DescendingOrder);

    // Find Things position within the Stage
    int my_position = 0;
    for (int i = 0; i < list.count(); i++) {
        if (list[i] == getKey()) my_position = i;
    }
    if (my_position == (list.count() - 1)) return;      // Already in the back

    // Find out Z Info on next lowest Thing
    long back_of_key =  list[my_position + 1];
    DrThing *thing =    getParentProject()->findThingFromKey(back_of_key);
    if (thing == nullptr) return;

    double z_order =    thing->getComponentPropertyValue(Components::Thing_Layering, Properties::Thing_Z_Order).toDouble();
    int  sub_order =    thing->getComponentPropertyValue(Components::Thing_Layering, Properties::Thing_Sub_Z_Order).toInt();

    setZOrderWithSub(z_order, Z_Insert::At_Position, sub_order);
}

void DrThing::moveForward() {
    QList<long> list = m_parent_stage->thingKeysSortedByZOrder(Qt::DescendingOrder);

    // Find Things position within the Stage
    int my_position = 0;
    for (int i = 0; i < list.count(); i++) {
        if (list[i] == getKey()) my_position = i;
    }
    if (my_position == 0) return;                       // Already in the front

    // Find out Z Info on next highest Thing
    long front_of_key = list[my_position - 1];
    DrThing *thing =    getParentProject()->findThingFromKey(front_of_key);
    if (thing == nullptr) return;

    double z_order =    thing->getComponentPropertyValue(Components::Thing_Layering, Properties::Thing_Z_Order).toDouble();
    int  sub_order =    thing->getComponentPropertyValue(Components::Thing_Layering, Properties::Thing_Sub_Z_Order).toInt() + 1;

    setZOrderWithSub(z_order, Z_Insert::At_Position, sub_order);
}















