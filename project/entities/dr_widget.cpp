//
//      Created by Stephens Nunnally on 5/4/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "core/dr_debug.h"
#include "engine/enums_engine.h"
#include "engine/opengl/opengl.h"
#include "project/constants_component_info.h"
#include "project/dr_project.h"
#include "project/entities/dr_asset.h"
#include "project/entities/dr_font.h"
#include "project/entities/dr_ui.h"
#include "project/entities/dr_widget.h"
#include "project/settings/settings.h"
#include "project/settings/settings_component.h"
#include "project/settings/settings_component_property.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
DrWidget::DrWidget(DrProject *parent_project, DrUI *parent_ui,
                   long new_widget_key, std::string new_widget_name, DrWidgetType new_widget_type,
                   DrSettings *from_entity,
                   double x, double y, double z)
    : DrSettings(parent_project), m_parent_ui(parent_ui) {

    this->setKey(new_widget_key);                   // assign key passed in from key generator, this key matches key in parent Stage map container

    m_widget_type = new_widget_type;                // assign thing type
    m_asset_key =  from_entity->getKey();           // associated asset key

    getComponentProperty(Comps::Entity_Settings, Props::Entity_Name)->setEditable(false);
    getComponentProperty(Comps::Entity_Settings, Props::Entity_Key)->setHidden(true);
    addPropertyToComponent(Comps::Entity_Settings, Props::Entity_Asset_Key, Property_Type::Int, m_asset_key,
                           "Asset ID Key", "ID Key of Asset this Thing represents.", false, false);

    // ********** Call to load in all the Components / Properties for this Thing
    switch (new_widget_type) {
        case DrWidgetType::Button: {
            getComponentProperty(Comps::Entity_Settings, Props::Entity_Key)->setHidden(false);
            getComponentProperty(Comps::Entity_Settings, Props::Entity_Key)->setDisplayName("Button Key");
            break;
        }
        case DrWidgetType::Joystick: {
            getComponentProperty(Comps::Entity_Settings, Props::Entity_Key)->setHidden(false);
            getComponentProperty(Comps::Entity_Settings, Props::Entity_Key)->setDisplayName("Button Key");
            break;
        }
        case DrWidgetType::Navigation: {
            getComponentProperty(Comps::Entity_Settings, Props::Entity_Key)->setHidden(false);
            getComponentProperty(Comps::Entity_Settings, Props::Entity_Key)->setDisplayName("Button Key");
            break;
        }
        case DrWidgetType::Text: {
            getComponentProperty(Comps::Entity_Settings, Props::Entity_Key)->setHidden(false);
            getComponentProperty(Comps::Entity_Settings, Props::Entity_Key)->setDisplayName("Button Key");
            break;
        }

        default:
            Dr::PrintDebug("Error in DrWidget Constructor, DrWidgetType not handled! Type: " + Dr::StringFromWidgetType(new_widget_type));
    }

}

DrWidget::~DrWidget() { }










