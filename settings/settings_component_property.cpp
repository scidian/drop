//
//      Created by Stephens Nunnally on 12/13/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrProperty Class Definitions
//
//      !!!!! #TODO: Replace 'QVariant' value with our own 'void *' value to use outside of Qt Framework
//
#include <QDebug>

#include "model/project/project.h"
#include "settings/settings.h"
#include "settings/settings_component.h"
#include "settings/settings_component_property.h"

#include "helper.h"


//####################################################################################
//##    Constructor
//####################################################################################
DrProperty::DrProperty(DrSettings      *parent_settings,
                       DrComponent     *parent_component,
                       QString          display_name,
                       QString          description,
                       Property_Type    type,
                       QVariant         value,
                       long             new_key,
                       bool             is_hidden,
                       bool             is_editable) {
    m_parent_settings =     parent_settings;
    m_parent_component =    parent_component;

    m_display_name =    display_name;
    m_description =     description;
    m_property_type =   type;
    m_value =           value;

    m_property_key =    new_key;

    m_is_hidden =       is_hidden;
    m_is_editable =     is_editable;
}


//####################################################################################
//##    Called when template function setValue() is called
//####################################################################################
void DrProperty::setValueInternal() {
    m_parent_settings->getParentProject()->setHasSaved(false);

    // Debug Logging
    ///qDebug() << "Entity: " << getParentSettings()->getName() << ", Comp: " << m_parent_component->getDisplayName() <<
    ///            ", Prop: " << getDisplayName() << ", Value: " << getValue();
}







