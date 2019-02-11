//
//      Created by Stephens Nunnally on 1/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Tree Inspector Definitions
//
//

#include <cmath>

#include "library.h"

#include "project.h"
#include "project_asset.h"
#include "project_world.h"
#include "project_world_stage.h"
#include "project_world_stage_object.h"

#include "settings.h"
#include "settings_component.h"
#include "settings_component_property.h"

#include "editor_scene.h"
#include "editor_tree_inspector.h"
#include "editor_tree_widgets.h"
#include "interface_relay.h"


//####################################################################################
//##        Constructor
//####################################################################################
TreeInspector::TreeInspector(QWidget *parent, DrProject *project, InterfaceRelay *relay) :
                              QTreeWidget (parent), m_project(project), m_relay(relay)
{
    m_widget_hover = new WidgetHoverHandler(this);
    connect(m_widget_hover, SIGNAL(signalMouseHover(QString, QString)), this, SLOT(setAdvisorInfo(QString, QString)));

    m_widget_hover->applyHeaderBodyProperties(this, Advisor_Info::Object_Inspector);
}


// SLOT: Catches signals from m_widget_hover
void TreeInspector::setAdvisorInfo(QString header, QString body) {
    m_relay->setAdvisorInfo(header, body);
}
void TreeInspector::applyHeaderBodyProperties(QWidget *widget, DrProperty *property) {
    m_widget_hover->applyHeaderBodyProperties(widget, property);
}
void TreeInspector::applyHeaderBodyProperties(QWidget *widget, QString header, QString body) {
    m_widget_hover->applyHeaderBodyProperties(widget, header, body);
}




//####################################################################################
//##    Dynamically build object inspector
//####################################################################################
void TreeInspector::buildInspectorFromKeys(QList<long> key_list)
{
    // If no keys were passed in, clear inspector and exit
    if (key_list.count() == 0) {
        m_selected_key = 0;
        m_selected_type = DrType::NotFound;
        this->clear();
        m_widgets.clear();
        return;
    }

    // Retrieve unique key of item clicked in list
    long new_key = key_list[0];

    // If object inspector already contaitns this item exit now
    if (new_key == m_selected_key) return;
    DrType new_type = m_project->findChildTypeFromKey( new_key );

    // !!!!! #DEBUG:    Show selected item key and info
    if (Dr::CheckDebugFlag(Debug_Flags::Label_Object_Inspector_Build)) {
        QString type_string = StringFromType(new_type);
        m_relay->setLabelText(Label_Names::Label_Object_1, "KEY: " + QString::number( new_key ) + ", TYPE: " + type_string);

        if (IsDrObjectClass(new_type)) {
            DrObject* object = dynamic_cast<DrObject*>(m_project->findSettingsFromKey(new_key));
            long asset_key = object->getAssetKey();
            QString asset_name = m_project->findSettingsFromKey(asset_key)->getAssetName();

            m_relay->setLabelText(Label_Names::Label_Object_2, "ASSET KEY:  " + QString::number(asset_key) +
                                                               ", NAME: " + asset_name);
        } else {
            m_relay->setLabelText(Label_Names::Label_Object_2, "");
        }
    }
    // !!!!! END

    // If old selection and new selection are both objects, we don't need to completely rebuild inspector, just change values
    if (m_selected_type == DrType::Object && new_type == DrType::Object) {
        m_selected_key =  new_key;
        updatePropertyBoxes(m_project->findSettingsFromKey(m_selected_key), Object_Properties::all_properties);
        return;
    } else {
        m_selected_key =  new_key;
        m_selected_type = new_type;
    }

    // Change Advisor text after new item selection
    switch (m_selected_type) {
    case DrType::World:        m_relay->setAdvisorInfo(Advisor_Info::World_Object);         break;
    case DrType::Stage:        m_relay->setAdvisorInfo(Advisor_Info::Stage_Object);         break;
    case DrType::Camera:       m_relay->setAdvisorInfo(Advisor_Info::Camera_Object);        break;
    case DrType::Character:    m_relay->setAdvisorInfo(Advisor_Info::Character_Object);     break;
    case DrType::Object:       m_relay->setAdvisorInfo(Advisor_Info::Object_Object);        break;
    default:                   m_relay->setAdvisorInfo(Advisor_Info::Not_Set);
    }


    // Retrieve list of components for selected item
    ComponentMap list_components = m_project->findSettingsFromKey( m_selected_key )->getComponentList();

    // Loop through each component and add it to the Object Inspector list
    this->clear();
    m_widgets.clear();

    for (auto component_pair: list_components) {

        // Create new item in list to hold component and add the TreeWidgetItem to the tree
        QTreeWidgetItem *category_item = new QTreeWidgetItem();
        category_item->setData(0, User_Roles::Key, QVariant::fromValue(component_pair.second->getComponentKey()));           // Stores component key in list user data
        this->addTopLevelItem(category_item);

        // Creates a frame to hold all properties of component, with vertical layout
        QFrame *properties_frame = new QFrame(this);
        QBoxLayout *vertical_layout = new QVBoxLayout(properties_frame);
        vertical_layout->setSpacing(6);
        vertical_layout->setMargin(0);
        vertical_layout->setContentsMargins(8,4,8,4);

        // Loop through each property and add it to the component frame
        for (auto property_pair: component_pair.second->getPropertyList()) {
            if (property_pair.second->isHidden()) continue;

            QFrame *single_row = new QFrame(properties_frame);
            QBoxLayout *horizontal_split = new QHBoxLayout(single_row);
            horizontal_split->setSpacing(0);
            horizontal_split->setMargin(0);
            horizontal_split->setContentsMargins(0,0,0,0);

            QLabel *property_name = new QLabel(property_pair.second->getDisplayName());
            QFont fp;
            fp.setPointSize(Dr::FontSize());
            property_name->setFont(fp);
                QSizePolicy sp_left(QSizePolicy::Preferred, QSizePolicy::Preferred);
                sp_left.setHorizontalStretch(INSPECTOR_SIZE_LEFT);
            property_name->setSizePolicy(sp_left);
            applyHeaderBodyProperties(property_name, property_pair.second);
            horizontal_split->addWidget(property_name);

            QWidget *new_widget = nullptr;
            switch (property_pair.second->getPropertyType())
            {
            case Property_Type::Bool:       new_widget = createCheckBox(property_pair.second, fp);                              break;
            case Property_Type::String:     new_widget = createLineEdit(property_pair.second, fp);                              break;
            case Property_Type::Int:        new_widget = createIntSpinBox(property_pair.second, fp, Spin_Type::Integer);        break;
            case Property_Type::Positive:   new_widget = createIntSpinBox(property_pair.second, fp, Spin_Type::Positive);       break;
            case Property_Type::Float:      new_widget = createDoubleSpinBox(property_pair.second, fp, Spin_Type::Float);       break;
            case Property_Type::Percent:    new_widget = createDoubleSpinBox(property_pair.second, fp, Spin_Type::Percent);     break;
            case Property_Type::Angle:      new_widget = createDoubleSpinBox(property_pair.second, fp, Spin_Type::Angle);       break;
            case Property_Type::PointF:     new_widget = createDoubleSpinBoxPair(property_pair.second, fp, Spin_Type::Point);   break;
            case Property_Type::SizeF:      new_widget = createDoubleSpinBoxPair(property_pair.second, fp, Spin_Type::Size);    break;
            case Property_Type::Variable:   new_widget = createVariableSpinBoxPair(property_pair.second, fp);                   break;
            case Property_Type::List:       new_widget = createComboBox(property_pair.second, fp);                              break;
            case Property_Type::List2:      new_widget = createComboBox2(property_pair.second, fp);
            }
            horizontal_split->addWidget(new_widget);


            vertical_layout->addWidget(single_row);
        }


        // ***** Create and style a button to be used as a header item for the category
        QFrame *button_frame = new QFrame();
        QGridLayout *grid = new QGridLayout(button_frame);
        grid->setContentsMargins(0, 0, 0, 0);

        CategoryButton *category_button = new CategoryButton(QString(" ") + component_pair.second->getDisplayNameQString(), Qt::black, nullptr, category_item);
        QString buttonColor = QString(" QPushButton { height: 22px; font: 13px; text-align: left; icon-size: 20px 16px; color: black; "
                                                    " border: " + Dr::BorderWidth() + " solid; "
                                                    " border-color: " + component_pair.second->getColor().darker(250).name() + "; "
                                                    " border-radius: 1px; "
                                                    " background: qlineargradient(spread:pad, x1:0 y1:0, x2:0 y2:1, stop:0 " +
                                                        component_pair.second->getColor().name() + ", stop:1 " +
                                                        component_pair.second->getColor().darker(250).name() + "); } "
                                      " QPushButton:hover:!pressed { color: " + component_pair.second->getColor().lighter(200).name() + "; } "
                                      " QPushButton:pressed { color: " + component_pair.second->getColor().darker(400).name() + "; } ");
        category_button->setIcon(QIcon(component_pair.second->getIcon()));
        category_button->setStyleSheet(buttonColor);
        applyHeaderBodyProperties(category_button, component_pair.second->getDisplayName(), component_pair.second->getDescription());

        // Apply the button widget to the tree item
        grid->addWidget(category_button);
        ///QLabel *label = new QLabel("Mom");
        ///label->setAttribute( Qt::WA_TransparentForMouseEvents );
        ///grid->addWidget(label, 0, 0, Qt::AlignmentFlag::AlignRight);
        this->setItemWidget(category_item, 0, button_frame);


        // ***** Create a child TreeWidgetItem attached to the TopLevel category item
        QTreeWidgetItem *property_item = new QTreeWidgetItem();
        property_item->setDisabled(true);
        category_item->addChild(property_item);

        // Apply the property frame widget to the tree item
        this->setItemWidget(property_item, 0, properties_frame);
    }

    this->expandAll();
}




//####################################################################################
//##        Updates the property boxes already in the object inspector for the current item
//####################################################################################
void TreeInspector::updatePropertyBoxes(DrSettings* object, Object_Properties property)
{
    if (object->getKey() != m_selected_key) return;
    if (IsDrObjectClass(m_selected_type) == false) return;

    Dr::SetLabelText(Label_Names::Label_1, "Insp Widget Count: " + QString::number(m_widgets.count()) );

    long property_to_change = static_cast<long>(property);

    for (auto widget : m_widgets) {
        long prop_key = widget->property(User_Property::Key).toInt();
        if (prop_key != property_to_change &&
            property != Object_Properties::all_properties) continue;

        DrProperty *prop = object->findPropertyFromPropertyKey(prop_key);
        if (prop == nullptr) continue;

        // Must turn off signals while updating or we will cause recursive function calling as changes to the widgets are connected to updateObjectFromNewValue()
        widget->blockSignals(true);

        switch (prop->getPropertyType())
        {
        case Property_Type::Bool:       dynamic_cast<QCheckBox*>(widget)->setChecked(prop->getValue().toBool());        break;

        case Property_Type::Int:
        case Property_Type::Positive:   dynamic_cast<QSpinBox*>(widget)->setValue(prop->getValue().toInt());            break;

        case Property_Type::Float:
        case Property_Type::Percent:
        case Property_Type::Angle:      dynamic_cast<QDoubleSpinBox*>(widget)->setValue(prop->getValue().toDouble());   break;

        case Property_Type::PointF:
        case Property_Type::SizeF:
            if (dynamic_cast<QDoubleSpinBox*>(widget)->property(User_Property::Order).toInt() == 0)
                dynamic_cast<QDoubleSpinBox*>(widget)->setValue(prop->getValue().toPointF().x());
            else
                dynamic_cast<QDoubleSpinBox*>(widget)->setValue(prop->getValue().toPointF().y());
            break;
        }

        // Turn signals back on
        widget->blockSignals(false);
    }

    this->update();
}



//####################################################################################
//##        Updates the object we're displaying the properties of with the new value
//####################################################################################
void TreeInspector::updateObjectFromNewValue(long property_key, QVariant new_value, long sub_order)
{
    // Exit if the selected project object is not something in the scene that needs to be updated
    if (IsDrObjectClass(m_selected_type) == false) return;

    DrObject *object =   dynamic_cast<DrObject*>(m_project->findSettingsFromKey(m_selected_key));
    DrItem   *item =     object->getDrItem();
    DrScene  *my_scene = dynamic_cast<DrScene*>( item->scene() );

    Object_Properties property = static_cast<Object_Properties>(property_key);

    // Turn off itemChange() signals to stop recursive calling
    item->setFlag(QGraphicsItem::GraphicsItemFlag::ItemSendsScenePositionChanges, false);
    item->setFlag(QGraphicsItem::GraphicsItemFlag::ItemSendsGeometryChanges, false);

    double x, y;
    switch (property)
    {

    // Processes changes to spin boxes that represent item position
    case Object_Properties::position:
        x = object->getComponentPropertyValue(Object_Components::transform, Object_Properties::position).toPointF().x();
        y = object->getComponentPropertyValue(Object_Components::transform, Object_Properties::position).toPointF().y();
        if (sub_order == 0)
            x = new_value.toDouble();
        else
            y = new_value.toDouble();
        object->setComponentPropertyValue(Object_Components::transform, Object_Properties::position, QPointF(x, y));
        my_scene->setPositionByOrigin(item, Position_Flags::Center, x, y);
        break;


    default: ;
    }

    my_scene->updateSelectionBox();

    // Turn back on itemChange() signals
    item->setFlag(QGraphicsItem::GraphicsItemFlag::ItemSendsGeometryChanges, true);
    item->setFlag(QGraphicsItem::GraphicsItemFlag::ItemSendsScenePositionChanges, true);
}



















