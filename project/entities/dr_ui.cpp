//
//      Created by Stephens Nunnally on 5/4/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "core/dr_debug.h"
#include "project/constants_component_info.h"
#include "project/dr_project.h"
#include "project/entities/dr_asset.h"
#include "project/entities/dr_ui.h"
#include "project/entities/dr_widget.h"
#include "project/settings/settings.h"
#include "project/settings/settings_component.h"
#include "project/settings/settings_component_property.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
DrUI::DrUI(DrProject *parent_project, long new_ui_key, std::string new_ui_name, bool is_start_ui) : DrSettings(parent_project) {
    this->setViewCenterPoint(DrPointF(0, 0));
    this->setKey(new_ui_key);                           // Assign key passed in from key generator, this key matches key in parent World map container
    this->setIsStartUI(is_start_ui);                    // Is this a "Start UI" or not, can only be one "Start UI" per Project
    initializeUISettings(new_ui_name);                  // Call to load in all the components / properties for this Stage entity

    // If this is a "Start UI", make name hidden to stop user from changing it
    if (isStartUI()) {
        DrProperty *my_name = getComponentProperty(Comps::Entity_Settings, Props::Entity_Name);
        my_name->setEditable(false);
    }
}

DrUI::~DrUI() {
    for (auto &widget_pair: m_widgets) { delete widget_pair.second; }
}


//####################################################################################
//##
//##    Adds a new Thing to the UI
//##
//##    ***** #NOTE: Y Axis is in Game Engine coordinates (i.e. positive is up, negative is down)
//##
//####################################################################################
DrWidget* DrUI::addWidget(DrWidgetType new_type, long from_asset_key, double x, double y, double z, long key) {
    DrSettings *asset = getParentProject()->findSettingsFromKey(from_asset_key);
    if (asset == nullptr) {
        Dr::PrintDebug("Error in DrUI::addWidget, Could not find underlying Entity to load from! \n "
                       "New Type: " + Dr::StringFromWidgetType(new_type) + " \n "
                       "Asset Key: " + std::to_string(from_asset_key) + " - End Error.....");
    }

    // Figure out name for Thing
    std::string new_name = asset->getComponentProperty(Comps::Entity_Settings, Props::Entity_Name)->getValue().toString();

    long new_widget_key = (key == c_no_key) ? getParentProject()->getNextKey() : key;
    m_widgets[new_widget_key] = new DrWidget(getParentProject(), this, new_widget_key, new_name, new_type, asset, x, y, z);
    return m_widgets[new_widget_key];
}

// Removes a Widget from the Project
void DrUI::deleteWidget(DrWidget *&widget) {
    widget->deleteAnimations();
    m_widgets.erase(widget->getKey());
    delete widget;
}


//####################################################################################
//##    Property loading - initializeUISettings
//####################################################################################
void DrUI::initializeUISettings(std::string new_name) {
    DrProperty *property_name = getComponentProperty(Comps::Entity_Settings, Props::Entity_Name);
    property_name->setDisplayName("UI Name");
    property_name->setDescription("Name of this UI.");
    property_name->setValue(new_name);

    addComponent(Comps::UI_Settings, "UI Settings", "Settings for this UI.", Component_Colors::RGB_18_Gray, true);
    getComponent(Comps::UI_Settings)->setIcon(Component_Icons::Settings);

}


//####################################################################################
//##    Returns a list of Widget keys contained in UI, sorted from high z value to low
//####################################################################################
std::vector<long> DrUI::widgetKeysSortedByZOrder(Sort_Order sort_order) {
    std::vector<long> z_ordered_keys;
    for (auto widget : widgetsSortedByZOrder(sort_order))
        z_ordered_keys.push_back(widget->getKey());
    return z_ordered_keys;
}

//####################################################################################
//##    Returns a list of Widgets contained in UI, sorted from high z value to low
//####################################################################################
std::vector<DrWidget*> DrUI::widgetsSortedByZOrder(Sort_Order sort_order, bool all_things, std::list<DrWidget*> just_these_widgets) {
    // Make a Vector of pairs for sorting
    std::vector<std::pair<double, DrWidget*>> zorder_key_pair;
    if (all_things) {
        for (auto &widget_pair : m_widgets)
            zorder_key_pair.push_back(std::make_pair(widget_pair.second->getZOrderWithSub(), widget_pair.second));
    } else {
        for (auto &widget : just_these_widgets)
            zorder_key_pair.push_back(std::make_pair(widget->getZOrderWithSub(), widget));
    }

    // Sort!
    std::sort(zorder_key_pair.begin(), zorder_key_pair.end());

    // Copy sorted Map into List
    std::deque<DrWidget*> z_ordered_widgets;
    for (auto one_pair : zorder_key_pair) {
        z_ordered_widgets.push_front(one_pair.second);
    }

    // Reverse if Ascending
    if (sort_order == Sort_Order::AscendingOrder) {
        std::reverse(z_ordered_widgets.begin(), z_ordered_widgets.end());
    }

    return { z_ordered_widgets.begin(), z_ordered_widgets.end() };
}


//####################################################################################
//##    Remove any references within the current UI Widgets to any GraphicsScene Items
//####################################################################################
void DrUI::removeGraphicsItemReferences() {
    for (auto &widget_pair : getWidgetMap()) {
        widget_pair.second->setEditorItem(nullptr);
    }
}














