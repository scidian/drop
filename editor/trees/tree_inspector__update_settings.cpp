//
//      Created by Stephens Nunnally on 11/20/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QDebug>
#include <QVBoxLayout>

#include "editor/colors/colors.h"
#include "editor/globals_editor.h"
#include "editor/helper_editor.h"
#include "editor/trees/tree_inspector.h"
#include "helper.h"
#include "model/project/project.h"
#include "model/settings/settings.h"
#include "model/settings/settings_component.h"
#include "model/settings/settings_component_property.h"


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
    // Go through each widget in Object Inspector property widget list
    for (auto widget : m_widgets) {
        long prop_key = widget->property(User_Property::Key).toInt();
        DrProperty *prop = getParentProject()->findSettingsFromKey( m_selected_key )->findPropertyFromPropertyKey(prop_key);
        if (prop == nullptr) continue;

        // Make sure Hidden Component Properties stay enabled, otherwise disable if Property is not editable or Thing is locked
        if (prop->getParentComponent()->getComponentKey() == Dr::EnumToInt(Components::Hidden_Settings) ||
            prop->getParentComponent()->getComponentKey() == Dr::EnumToInt(Components::Size_Settings)) {
            widget->setEnabled( true );
        } else {
            bool enabled = prop->isEditable() && !(prop->getParentSettings()->isLocked());

            if ( prop->getPropertyType() == Property_Type::BoolDouble ||
                 prop->getPropertyType() == Property_Type::BoolInt) {
                if (widget->property(User_Property::Order).toInt() == 1) {
                    if (prop->getValue().toList().first().toBool() == false) enabled = false;
                }
            }

            if ( prop->getPropertyKey() == static_cast<int>(Properties::Entity_Key) ||
                 prop->getPropertyKey() == static_cast<int>(Properties::Entity_Asset_Key)) {
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

        QList<QVariant> affected { };
        int  last_found = -1;
        bool enabled = true;

        // Go through each property in component property list
        for (int i = 0; i < property_layout->count(); ++i) {
            QWidget *row = property_layout->itemAt(i)->widget();                                if (row == nullptr) continue;
            long row_key = row->property(User_Property::Key).toLongLong();

            DrProperty *prop = getParentProject()->findSettingsFromKey( m_selected_key )->findPropertyFromPropertyKey(row_key);
            if (prop == nullptr) continue;

            // We found a property that has sub properties, enabled / disable sub properties based on if on or off
            if (prop->getPropertyType() == Property_Type::Enabled) {
                QList<QVariant> prop_value = prop->getValue().toList();
                if (prop_value.count() == 2) {
                    enabled =  prop_value[0].toBool();
                    affected = prop_value[1].toList();
                    last_found = i;
                }
            } else {
                if (last_found != -1) {
                    if (affected.contains(QVariant::fromValue(prop->getPropertyKey()))) {
                        row->setStyleSheet("QFrame#propertyRow { background: " + Dr::GetColor(Window_Colors::Button_Dark).name() + "; }");
                        row->setVisible(enabled);
                        changed = true;
                    }
                }
            }
        } // end for i
    } // end for component

    if (changed && !called_from_build) forceUpdateOfItemSizes();
}
















