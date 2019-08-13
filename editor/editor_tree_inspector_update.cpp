//
//      Created by Stephens Nunnally on 2/16/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Update Functions for Tree Inspector
//
//
#include <QAction>
#include <QDebug>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QMenu>

#include "colors/colors.h"
#include "editor_tree_inspector.h"
#include "enums.h"
#include "interface_editor_relay.h"
#include "helper.h"
#include "project/project.h"
#include "settings/settings.h"
#include "settings/settings_component.h"
#include "settings/settings_component_property.h"


//####################################################################################
//##        Updates the property boxes already in the Inspector when a new
//##            item is selected or when something in the project has changed
//##
//##        Inspector Widget SIGNALS are blocked to prevent recursive updating
//####################################################################################
void TreeInspector::updateInspectorPropertyBoxes(QList<DrSettings*> changed_items, QList<long> property_keys_to_update) {
    if (changed_items.isEmpty()) return;
    ///if (property_keys_to_update.isEmpty()) return;   // ********** Don't do this! This function is made so all properties update if none are received

    // !!!!! #TEMP: Need to be more than just one item represented in Inspector
    DrSettings* thing = changed_items.first();
    // !!!!!

    if (thing->getKey() != m_selected_key) return;
    if (m_selected_type != DrType::Thing) return;


    for (auto widget : m_widgets) {
        long prop_key = widget->property(User_Property::Key).toInt();

        if (property_keys_to_update.contains(prop_key) == false &&
            property_keys_to_update.count() != 0) continue;

        DrProperty *prop = thing->findPropertyFromPropertyKey(prop_key);
        if (prop == nullptr) continue;

        // ***** Must turn off signals while updating or we will cause recursive function calling as changes to the
        //       widgets are connected to updateThingFromNewValue()
        widget->blockSignals(true);

        // Enable / disable widgets as necessary
        widget->setEnabled( prop->isEditable() );

        switch (prop->getPropertyType()) {
            case Property_Type::Bool:       dynamic_cast<QCheckBox*>(widget)->setChecked(prop->getValue().toBool());        break;

            case Property_Type::Int:
            case Property_Type::Positive:
                                            dynamic_cast<QSpinBox*>(widget)->setValue(prop->getValue().toInt());            break;

            case Property_Type::RangedInt:
                dynamic_cast<QSpinBox*>(widget)->setValue(prop->getValue().toList().first().toInt() );                      break;

            case Property_Type::Double:
            case Property_Type::PositiveDouble:
            case Property_Type::Percent:
            case Property_Type::Angle:
                if (widget->property(User_Property::Order).toInt() == 2)
                    dynamic_cast<QSlider*>(widget)->setValue(prop->getValue().toInt());
                else
                    dynamic_cast<QDoubleSpinBox*>(widget)->setValue(prop->getValue().toDouble());
                break;

            case Property_Type::RangedDouble:
            case Property_Type::Slider:
                if (widget->property(User_Property::Order).toInt() == 2)
                    dynamic_cast<QSlider*>(widget)->setValue( prop->getValue().toList().first().toInt() );
                else
                    dynamic_cast<QDoubleSpinBox*>(widget)->setValue( prop->getValue().toList().first().toDouble() );
                break;

            case Property_Type::String:     dynamic_cast<QLineEdit*>(widget)->setText(prop->getValue().toString());         break;

            case Property_Type::PositionF:
            case Property_Type::PointF:
            case Property_Type::SizeF:
            case Property_Type::PositiveSizeF:
            case Property_Type::ScaleF:
            case Property_Type::GridF:
            case Property_Type::GridScaleF:
            case Property_Type::Variable: {
                QDoubleSpinBox *doublespin = dynamic_cast<QDoubleSpinBox*>(widget);
                if (doublespin->property(User_Property::Order).toInt() == 0)
                    doublespin->setValue(prop->getValue().toPointF().x());
                else
                    if (prop->getPropertyType() == Property_Type::PositionF)
                        doublespin->setValue(prop->getValue().toPointF().y() * -1);
                    else
                        doublespin->setValue(prop->getValue().toPointF().y());
                break;
            }

            case Property_Type::List: {
                QPushButton *pushbutton = dynamic_cast<QPushButton*>(widget);
                pushbutton->setText( pushbutton->menu()->actions().at(prop->getValue().toInt())->text() );
                pushbutton->menu()->actions().at(prop->getValue().toInt())->setChecked(true);
                break;
            }

            case Property_Type::Color: {
                QPushButton *pushbutton = dynamic_cast<QPushButton*>(widget);
                this->updateColorButton( pushbutton, QColor::fromRgba(prop->getValue().toUInt()) );
                break;
            }

            case Property_Type::Image:                                  // QPixmap
            case Property_Type::Icon:
            case Property_Type::Polygon:                                // For Collision Shapes
            case Property_Type::Vector3D:

                //################ !!!!!!!!!!!!!!!!!!!!!!!
                //
                //      CASES NOT ACCOUNTED FOR
                //
                //################ !!!!!!!!!!!!!!!!!!!!!!!

                break;
        }

        // Turn signals back on
        widget->blockSignals(false);
    }

    this->update();
}



//####################################################################################
//##        SLOT: connceted from SIGNAL that is emmited when the user changes the values
//##            of the input boxes in the Inspector.
//##
//##        Updates the appropriate DrSettings DrProperty Values of the item changed
//##            in the Inspector after a new value has been accepted
//####################################################################################
void TreeInspector::updateSettingsFromNewValue(long property_key, QVariant new_value, long sub_order) {
    if (m_selected_key == c_no_key) return;
    DrSettings *settings = m_project->findSettingsFromKey( m_selected_key );

    QPointF temp_pointf;

    if (settings != nullptr) {
        DrProperty *property = settings->findPropertyFromPropertyKey(property_key);

        switch (property->getPropertyType()) {
            case Property_Type::Bool:                                   // true or false
            case Property_Type::Int:                                    // any integer
            case Property_Type::Positive:                               // integer >= 0
            case Property_Type::Double:                                 // any floating point
            case Property_Type::PositiveDouble:
            case Property_Type::Percent:                                // floating point from 0.0 to 100.0
            case Property_Type::Angle:                                  // floating point for showing degrees
            case Property_Type::List:                                   // index value
            case Property_Type::String:
                property->setValue(new_value);
                break;

            case Property_Type::RangedInt:
            case Property_Type::RangedDouble:
            case Property_Type::Slider: {
                QList<QVariant> ranged_list = property->getValue().toList();
                ranged_list[0] = new_value;
                property->setValue(ranged_list);
                break;
            }

            case Property_Type::PositionF:                              // Floating pair x and y, y is flipped
            case Property_Type::PointF:                                 // Floating pair x and y
            case Property_Type::SizeF:                                  // Floating pair w and h
            case Property_Type::PositiveSizeF:                          // Floating pair x and y, both floats are >= 1.0
            case Property_Type::ScaleF:                                 // Floating pair, has smaller step in spin box
            case Property_Type::GridF:                                  // Floating pair x and y, minimum value c_minimum_grid_size
            case Property_Type::GridScaleF:                             // Floating pair x and y, minimum value c_minimum_grid_scale
            case Property_Type::Variable:                               // floating point pair, number followed by a +/- number
                temp_pointf = property->getValue().toPointF();
                if (sub_order == 0)
                    temp_pointf.setX( new_value.toDouble() );
                else {
                    if (property->getPropertyType() == Property_Type::PositionF)
                        temp_pointf.setY( new_value.toDouble() * -1);
                    else
                        temp_pointf.setY( new_value.toDouble() );
                }
                property->setValue(temp_pointf);
                break;

            case Property_Type::Color:                                  // QColor.rgba()
                property->setValue(new_value);
                break;

            case Property_Type::Image:                                  // QPixmap
            case Property_Type::Icon:
            case Property_Type::Polygon:                                // For Collision Shapes
            case Property_Type::Vector3D:

                //################ !!!!!!!!!!!!!!!!!!!!!!!
                //
                //      CASES NOT ACCOUNTED FOR
                //
                //################ !!!!!!!!!!!!!!!!!!!!!!!

                break;
        }

        m_editor_relay->updateEditorWidgetsAfterItemChange(Editor_Widgets::Inspector_Tree, { settings }, { static_cast<Properties>(property_key) } );
    }
}











