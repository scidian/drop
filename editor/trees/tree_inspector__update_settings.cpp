//
//      Created by Stephens Nunnally on 11/20/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QDebug>
#include <QVBoxLayout>

#include "core/colors/colors.h"
#include "core/dr_containers.h"
#include "editor/helper_library.h"
#include "editor/preferences.h"
#include "editor/trees/tree_inspector.h"
#include "project/dr_project.h"
#include "project/settings/settings.h"
#include "project/settings/settings_component.h"
#include "project/settings/settings_component_property.h"


//####################################################################################
//##    Returns List of Top Level (Component) Items
//####################################################################################
QList<QTreeWidgetItem*> TreeInspector::getListOfTopLevelItems() {
    QTreeWidgetItem         *item = this->invisibleRootItem();
    QList <QTreeWidgetItem*> items;
    for (int i = 0; i < item->childCount(); ++i) {
        items.append( item->child(i) );
    }
    return items;
}


//####################################################################################
//##    Disable / Enable property widgets based on property status
//####################################################################################
void TreeInspector::updateLockedSettings() {
    DrSettings *entity = getParentProject()->findSettingsFromKey(m_key_shown);
    if (entity == nullptr) return;

    // Go through each widget in Object Inspector property widget list
    for (auto widget : m_widgets) {
        std::string component_name = widget->property(User_Property::CompName).toString().toStdString();
        std::string property_name =  widget->property(User_Property::PropName).toString().toStdString();
        DrProperty *prop = entity->getComponentProperty(component_name, property_name);
        if (prop == nullptr) continue;

        // Make sure Hidden Component Properties stay enabled, otherwise disable if Property is not editable or Thing is locked
        if (prop->getParentComponent()->getComponentName() == Comps::Hidden_Settings ||
            prop->getParentComponent()->getComponentName() == Comps::Size_Settings) {
            widget->setEnabled( true );
        } else {
            bool enabled = prop->isEditable() && !(prop->getParentSettings()->isLocked());

            if ( prop->getPropertyType() == Property_Type::BoolDouble ||
                 prop->getPropertyType() == Property_Type::BoolInt) {
                if (widget->property(User_Property::Order).toInt() == 1) {
                    if (prop->getValue().toVector()[0].toBool() == false) enabled = false;
                }
            }

            if ( prop->getPropertyName() == Props::Entity_Key ||
                 prop->getPropertyName() == Props::Entity_Asset_Key) {
                QSpinBox *spin = dynamic_cast<QSpinBox*>(widget);
                if (spin != nullptr) {
                    spin->setReadOnly(true);
                    enabled = true;
                }
            }

            widget->setEnabled( enabled );
        }
    }
}


//####################################################################################
//##    Hide / Show Rows Based on "Enabled" sub properties Turned On / Off
//####################################################################################
void TreeInspector::updateSubProperties(bool called_from_build) {
    bool changed = false;

    // Go through each componenet to check for sub properties
    for (auto component_item : getListOfTopLevelItems()) {
        QTreeWidgetItem *property_item = component_item->child(0);                              if (property_item == nullptr) continue;
        QWidget *property_widget = this->itemWidget(property_item, 0);                          if (property_widget == nullptr) continue;
        QFrame *properties_frame = dynamic_cast<QFrame*>(property_widget);                      if (properties_frame == nullptr) continue;
        QVBoxLayout *property_layout = dynamic_cast<QVBoxLayout*>(properties_frame->layout());  if (property_layout == nullptr) continue;

        std::vector<DrVariant> affected { };
        int  last_found = -1;
        bool enabled = true;

        // Go through each property in component property list
        for (int i = 0; i < property_layout->count(); ++i) {
            QWidget *row = property_layout->itemAt(i)->widget();                                if (row == nullptr) continue;
            std::string component_name = row->property(User_Property::CompName).toString().toStdString();
            std::string property_name =  row->property(User_Property::PropName).toString().toStdString();

            DrProperty *prop = getParentProject()->findSettingsFromKey(m_key_shown)->getComponentProperty(component_name, property_name);
            if (prop == nullptr) continue;

            // We found a property that has sub properties, enabled / disable sub properties based on if on or off
            if (prop->getPropertyType() == Property_Type::BoolEnabled) {
                std::vector<DrVariant> prop_value = prop->getValue().toVector();
                if (prop_value.size() == 2) {
                    enabled =  prop_value[0].toBool();
                    affected = prop_value[1].toVector();
                    last_found = i;
                }
            } else {
                if (last_found != -1) {

                    std::vector<std::string> affected_as_strings;
                    for (auto variant : affected)
                        affected_as_strings.push_back(variant.toString());

                    if (Dr::VectorContains(affected_as_strings, prop->getPropertyName())) {
                        row->setStyleSheet(QString::fromStdString("QFrame#propertyRow { background: " + Dr::GetColor(Window_Colors::Button_Dark).name() + "; }"));
                        row->setVisible(enabled);
                        changed = true;
                    }
                }
            }
        }   // End For i
    }   // End For component

    if (changed && !called_from_build) forceUpdateOfItemSizes();
}

















