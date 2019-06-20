//
//      Created by Stephens Nunnally on 1/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Tree Inspector Definitions
//
//
#include <cmath>

#include <QLabel>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTime>

#include "colors/colors.h"
#include "debug.h"
#include "editor_item.h"
#include "editor_scene.h"
#include "editor_tree_inspector.h"
#include "globals.h"
#include "interface_editor_relay.h"
#include "helper.h"
#include "project/project.h"
#include "project/project_asset.h"
#include "project/project_world.h"
#include "project/project_world_stage.h"
#include "project/project_world_stage_thing.h"
#include "settings/settings.h"
#include "settings/settings_component.h"
#include "settings/settings_component_property.h"
#include "widgets/widgets.h"
#include "widgets/widgets_event_filters.h"


//####################################################################################
//##        Constructor
//####################################################################################
TreeInspector::TreeInspector(QWidget *parent, DrProject *project, IEditorRelay *editor_relay) :
                              QTreeWidget (parent), m_project(project), m_editor_relay(editor_relay) {
    // Initialize hover handler
    m_filter_hover = new DrFilterHoverHandler(this);
    connect(m_filter_hover, SIGNAL(signalMouseHover(QString, QString)), this, SLOT(setAdvisorInfo(QString, QString)));

    // Connect this widget to the hover handler
    m_filter_hover->attachToHoverHandler(this, Advisor_Info::Object_Inspector);
}

// SLOT: Catches signals from m_filter_hover and passes to InterfaceEditorRelay
void TreeInspector::setAdvisorInfo(QString header, QString body) { m_editor_relay->setAdvisorInfo(header, body); }





//####################################################################################
//##    Dynamically build object inspector
//####################################################################################
void TreeInspector::buildInspectorFromKeys(QList<long> key_list) {
    // If no keys were passed in, clear inspector and exit
    if (key_list.count() == 0) {
        m_selected_key = c_no_key;
        m_selected_type = DrType::NotFound;
        this->clear();
        m_widgets.clear();
        return;
    }

    // Retrieve unique key of item clicked in list
    long new_key = key_list[0];

    // If object inspector already contaitns this item exit now
    if (new_key == c_no_key) return;
    if (new_key == m_selected_key) return;
    DrType new_type = m_project->findChildTypeFromKey( new_key );

    // !!!!! #DEBUG:    Show selected item key and info
    if (Dr::CheckDebugFlag(Debug_Flags::Label_Object_Inspector_Build)) {
        QString type_string = Dr::StringFromType(new_type);
        Dr::SetLabelText(Label_Names::Label_Object_1, "KEY: " + QString::number( new_key ) + ", TYPE: " + type_string);

        if (new_type == DrType::Thing) {
            DrThing* thing = m_project->findThingFromKey(new_key);
            long asset_key = thing->getAssetKey();
            QString asset_name = m_project->findAssetFromKey(asset_key)->getName();

            Dr::SetLabelText(Label_Names::Label_Object_2, "ASSET KEY:  " + QString::number(asset_key) +
                                                              ", NAME: " + asset_name);
        } else {
            Dr::SetLabelText(Label_Names::Label_Object_2, "");
        }
    }
    // !!!!! END

    // If old selection and new selection are both Object Things, we don't need to completely rebuild inspector, just change values
    if (m_selected_type == DrType::Thing && new_type == DrType::Thing) {
        m_selected_key =  new_key;
        updateInspectorPropertyBoxes( { m_project->findThingFromKey(m_selected_key) }, { });
        return;
    } else {
        m_selected_key =  new_key;
        m_selected_type = new_type;
    }

    // Change Advisor text after new item selection
    switch (m_selected_type) {
        case DrType::World:        m_editor_relay->setAdvisorInfo(Advisor_Info::World_Description);     break;
        case DrType::Stage:        m_editor_relay->setAdvisorInfo(Advisor_Info::Stage_Description);     break;
        case DrType::Thing:        m_editor_relay->setAdvisorInfo(Advisor_Info::Thing_Description);     break;
        case DrType::Asset:        m_editor_relay->setAdvisorInfo(Advisor_Info::Asset_Object);          break;
        default:                   m_editor_relay->setAdvisorInfo(Advisor_Info::Not_Set);
    }


    // Retrieve list of components for selected item
    ComponentMap list_components = m_project->findSettingsFromKey( m_selected_key )->getComponentList();

    // Loop through each component and add it to the Object Inspector list
    this->clear();
    m_widgets.clear();

    for (auto component_pair: list_components) {


        // *****Create new item in list to hold component and add the TreeWidgetItem to the tree
        QTreeWidgetItem *category_item = new QTreeWidgetItem();
        category_item->setData(0, User_Roles::Key, QVariant::fromValue(component_pair.second->getComponentKey()));      // Stores component key
        this->addTopLevelItem(category_item);

        // Create and style a button to be used as a header item for the category
        QFrame *button_frame = new QFrame();
        QGridLayout *grid = new QGridLayout(button_frame);
        grid->setContentsMargins(0, 0, 0, 0);

        DrQPushButtonCategory *category_button = new DrQPushButtonCategory(QString(" ") + component_pair.second->getDisplayNameQString(),
                                                             Qt::black, Qt::black, nullptr, category_item);
        QString buttonColor = QString(" QPushButton { height: 22px; font: 13px; text-align: left; icon-size: 20px 16px; color: black; "
                                                    " padding-left: 2px;"
                                                    " border: none; "
                                                    " border-style: solid; "
                                                    " border-top-width: " + Dr::BorderWidth() + "; "
                                                    " border-bottom-width: " + Dr::BorderWidth() + "; "
                                                    " border-color: " + Dr::GetColor(Window_Colors::Shadow).name() + "; "
                                                    " background: qlineargradient(spread:pad, x1:0 y1:0, x2:0 y2:1, "
                                                    "   stop:0.00 " + component_pair.second->getColor().lighter(120).name() +
                                                    "   stop:0.05 " + component_pair.second->getColor().lighter(120).name() +
                                                    "   stop:0.10 " + component_pair.second->getColor().name() +
                                                    " , stop:0.90 " + component_pair.second->getColor().darker(200).name() +
                                                    " , stop:0.95 " + component_pair.second->getColor().darker(300).name() +
                                                    " , stop:1.00 " + component_pair.second->getColor().darker(300).name() + "); } "
                                      " QPushButton:hover:!pressed { color: " + component_pair.second->getColor().lighter(200).name() + "; } "
                                      " QPushButton:pressed { color: " + component_pair.second->getColor().darker(250).name() + "; } ");
        category_button->setIcon(QIcon(component_pair.second->getIcon()));
        category_button->setStyleSheet(buttonColor);
        m_filter_hover->attachToHoverHandler(category_button,
                                             component_pair.second->getDisplayName(),
                                             component_pair.second->getDescription());

        // Apply the button widget to the tree item
        grid->addWidget(category_button);
        this->setItemWidget(category_item, 0, button_frame);



        // ***** Creates a frame to hold all properties of component, with vertical layout
        QFrame *properties_frame = new QFrame();
        properties_frame->setObjectName("propertiesFrame");
        QBoxLayout *vertical_layout = new QVBoxLayout(properties_frame);
        vertical_layout->setSpacing(4);
        vertical_layout->setMargin(0);
        vertical_layout->setContentsMargins(6,4,8,4);

        // Loop through each property and add it to the component frame
        for (auto property_pair: component_pair.second->getPropertyList()) {
            if (property_pair.second->isHidden()) continue;

            QFrame *single_row = new QFrame(properties_frame);
            QBoxLayout *horizontal_split = new QHBoxLayout(single_row);
            horizontal_split->setSpacing(0);
            horizontal_split->setMargin(0);
            horizontal_split->setContentsMargins(0,0,0,0);

            QLabel *property_name = new QLabel(property_pair.second->getDisplayName());
            QFont fp = Dr::CustomFont();
            property_name->setFont(fp);
                QSizePolicy sp_left(QSizePolicy::Preferred, QSizePolicy::Preferred);
                sp_left.setHorizontalStretch(c_inspector_size_left);

                QSizePolicy sp_right(QSizePolicy::Preferred, QSizePolicy::Preferred);
                sp_right.setHorizontalStretch(c_inspector_size_right);

            property_name->setSizePolicy(sp_left);
            m_filter_hover->attachToHoverHandler(property_name, property_pair.second);
            horizontal_split->addWidget(property_name);

            QWidget    *new_widget = nullptr;
            DrProperty *prop = property_pair.second;

            switch (property_pair.second->getPropertyType()) {
                case Property_Type::Bool:           new_widget = createCheckBox(        prop, fp, sp_right);                                    break;
                case Property_Type::String:         new_widget = createLineEdit(        prop, fp, sp_right);                                    break;
                case Property_Type::Int:            new_widget = createIntSpinBox(      prop, fp, sp_right, Property_Type::Int);                break;
                case Property_Type::Positive:       new_widget = createIntSpinBox(      prop, fp, sp_right, Property_Type::Positive);           break;
                case Property_Type::BitRate:        new_widget = createIntSpinBox(      prop, fp, sp_right, Property_Type::BitRate);            break;
                case Property_Type::BitDepth:       new_widget = createIntSpinBox(      prop, fp, sp_right, Property_Type::BitDepth);           break;
                case Property_Type::Filter:         new_widget = createSlider(prop, fp, sp_right, Property_Type::Filter);                       break;
                case Property_Type::FilterAngle:    new_widget = createSlider(prop, fp, sp_right, Property_Type::FilterAngle);                  break;
                case Property_Type::Double:         new_widget = createDoubleSpinBox(   prop, fp, sp_right, Property_Type::Double);             break;
                case Property_Type::PositiveDouble: new_widget = createDoubleSpinBox(   prop, fp, sp_right, Property_Type::PositiveDouble);     break;
                case Property_Type::Percent:        new_widget = createSlider(prop, fp, sp_right, Property_Type::Percent);                      break;
                case Property_Type::Angle:          new_widget = createDoubleSpinBox(   prop, fp, sp_right, Property_Type::Angle);              break;
                case Property_Type::PositionF:      new_widget = createDoubleSpinBoxPair(   prop, fp, sp_right, Property_Type::PositionF);      break;
                case Property_Type::PointF:         new_widget = createDoubleSpinBoxPair(   prop, fp, sp_right, Property_Type::PointF);         break;
                case Property_Type::SizeF:          new_widget = createDoubleSpinBoxPair(   prop, fp, sp_right, Property_Type::SizeF);          break;
                case Property_Type::PositiveSizeF:  new_widget = createDoubleSpinBoxPair(   prop, fp, sp_right, Property_Type::PositiveSizeF);  break;
                case Property_Type::ScaleF:         new_widget = createDoubleSpinBoxPair(   prop, fp, sp_right, Property_Type::ScaleF);         break;
                case Property_Type::GridF:          new_widget = createDoubleSpinBoxPair(   prop, fp, sp_right, Property_Type::GridF);          break;
                case Property_Type::GridScaleF:     new_widget = createDoubleSpinBoxPair(   prop, fp, sp_right, Property_Type::GridScaleF);     break;
                case Property_Type::Variable:       new_widget = createVariableSpinBoxPair( prop, fp, sp_right);                                break;
                case Property_Type::List:           new_widget = createListBox(             prop, fp, sp_right);                                break;
                case Property_Type::Color:          new_widget = createColorBox(            prop, fp, sp_right);                                break;

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

            if (new_widget != nullptr) {
                horizontal_split->addWidget(new_widget);
            }

            vertical_layout->addWidget(single_row);
        }


        // ***** Create a child TreeWidgetItem attached to the TopLevel category item
        QTreeWidgetItem *property_item = new QTreeWidgetItem();
        property_item->setDisabled(true);
        category_item->addChild(property_item);

        // Apply the property frame widget to the tree item
        this->setItemWidget(property_item, 0, properties_frame);
    }

    // ***** Disable / enable widgets based on property status
    for (auto widget : m_widgets) {
        long prop_key = widget->property(User_Property::Key).toInt();
        DrProperty *prop = m_project->findSettingsFromKey( m_selected_key )->findPropertyFromPropertyKey(prop_key);
        if (prop == nullptr) continue;
        widget->setEnabled( prop->isEditable() );
    }

    this->expandAll();
}



















