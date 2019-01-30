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
#include "project_world_scene.h"
#include "project_world_scene_object.h"

#include "settings.h"
#include "settings_component.h"
#include "settings_component_property.h"

#include "editor_tree_inspector.h"
#include "interface_relay.h"


//####################################################################################
//##        Constructor
//####################################################################################
TreeInspector::TreeInspector(QWidget *parent, DrProject *project, InterfaceRelay *relay) :
                              QTreeWidget (parent), m_project(project), m_relay(relay)
{
    connect(this, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(itemWasClicked(QTreeWidgetItem *, int)));
    m_widget_hover = new WidgetHoverHandler(this);
    connect(m_widget_hover, SIGNAL(signalMouseHover(QString, QString)), this, SLOT(setAdvisorInfo(QString, QString)));
}


//####################################################################################
//##        Advisor Info Functions
//####################################################################################
// Handles changing the Advisor on Mouse Enter
void TreeInspector::enterEvent(QEvent *event)
{
    m_relay->setAdvisorInfo(Advisor_Info::Object_Inspector);
    QTreeWidget::enterEvent(event);
}

void TreeInspector::setAdvisorInfo(QString header, QString body)
{
    m_relay->setAdvisorInfo(header, body);
}

void TreeInspector::applyHeaderBodyProperties(QWidget *widget, DrProperty *property)
{
    widget->setProperty(User_Property::Header, property->getDisplayName());
    widget->setProperty(User_Property::Body, property->getDescription());
    m_widget_hover->attach(widget);
}

void TreeInspector::applyHeaderBodyProperties(QWidget *widget, QString header, QString body)
{
    widget->setProperty(User_Property::Header, header);
    widget->setProperty(User_Property::Body, body);
    m_widget_hover->attach(widget);
}


//####################################################################################
//##    Dynamically build object inspector
//####################################################################################
void TreeInspector::buildInspectorFromKeys(QList<long> key_list)
{
    // First, retrieve unique key of item clicked in list
    long        selected_key = key_list[0];
    DrType      selected_type = m_project->findChildTypeFromKey( selected_key );
    QString     type_string = StringFromType(selected_type);

    // !!!!! #DEBUG:    Show selected item key and info
    if (Dr::CheckDebugFlag(Debug_Flags::Label_Object_Inspector_Build)) {
        m_relay->setLabelText(Label_Names::Label_Object_1, "KEY: " + QString::number( selected_key ) + ", TYPE: " + type_string);

        if (IsDrObjectClass(selected_type)) {
            DrObject* object = dynamic_cast<DrObject*>(m_project->findChildSettingsFromKey(selected_key));
            long asset_key = object->getAssetKey();
            QString asset_name = m_project->findChildSettingsFromKey(asset_key)->getAssetName();

            m_relay->setLabelText(Label_Names::Label_Object_2, "ASSET KEY:  " + QString::number(asset_key) +
                                                               ", NAME: " + asset_name);
        } else {
            m_relay->setLabelText(Label_Names::Label_Object_2, "");
            m_relay->setLabelText(Label_Names::Label_Object_3, "");
        }
    }
    // !!!!! END

    // Change Advisor text after new item selection
    switch (selected_type) {
    case DrType::World:        m_relay->setAdvisorInfo(Advisor_Info::World_Object);         break;
    case DrType::Scene:        m_relay->setAdvisorInfo(Advisor_Info::Scene_Object);         break;
    case DrType::Camera:       m_relay->setAdvisorInfo(Advisor_Info::Camera_Object);        break;
    case DrType::Character:    m_relay->setAdvisorInfo(Advisor_Info::Character_Object);     break;
    case DrType::Object:       m_relay->setAdvisorInfo(Advisor_Info::Object_Object);        break;
    default:                   m_relay->setAdvisorInfo(Advisor_Info::Not_Set);
    }

    // Find out if we should hide name for this object
    bool show_name;
    switch (selected_type)
    {
    case DrType::Object:
    case DrType::Character:
    case DrType::Action:
                show_name = false;  break;
    default:    show_name = true;
    }


    // Retrieve list of components for selected item
    ComponentMap list_components = m_project->findChildSettingsFromKey( selected_key )->getComponentList();

    // Loop through each component and add it to the Object Inspector list
    int rowCount = 0;
    this->clear();
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
            if (property_pair.second->getPropertyKey() == static_cast<long>(Object_Properties::name) && !show_name) continue;

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

            switch (property_pair.second->getPropertyType())
            {
            case Property_Type::Bool:       horizontal_split->addWidget(createCheckBox(property_pair.second, fp));                              break;
            case Property_Type::String:     horizontal_split->addWidget(createLineEdit(property_pair.second, fp));                              break;
            case Property_Type::Int:        horizontal_split->addWidget(createIntSpinBox(property_pair.second, fp, Spin_Type::Integer));        break;
            case Property_Type::Double:     horizontal_split->addWidget(createDoubleSpinBox(property_pair.second, fp, Spin_Type::Double));      break;
            case Property_Type::Percent:    horizontal_split->addWidget(createDoubleSpinBox(property_pair.second, fp, Spin_Type::Percent));     break;
            case Property_Type::Angle:      horizontal_split->addWidget(createDoubleSpinBox(property_pair.second, fp, Spin_Type::Angle));       break;
            case Property_Type::PointF:     horizontal_split->addWidget(createDoubleSpinBoxPair(property_pair.second, fp, Spin_Type::Point));   break;
            case Property_Type::SizeF:      horizontal_split->addWidget(createDoubleSpinBoxPair(property_pair.second, fp, Spin_Type::Size));    break;
            case Property_Type::Variable:   horizontal_split->addWidget(createVariableSpinBoxPair(property_pair.second, fp));                   break;
            case Property_Type::List:       horizontal_split->addWidget(createComboBox(property_pair.second, fp));
            }



            //spin_int->setUserData( setData(0, User_Roles::Key, QVariant::fromValue(object_pair.second->getKey()));






            vertical_layout->addWidget(single_row);
            rowCount++;
        }


        // Create a child TreeWidgetItem attached to the TopLevel category item
        QTreeWidgetItem *property_item = new QTreeWidgetItem();
        property_item->setDisabled(true);
        category_item->addChild(property_item);

        //->Create and style a button to be used as a header item for the category
        InspectorCategoryButton *category_button = new InspectorCategoryButton(QString(" ") + component_pair.second->getDisplayNameQString(),
                                                                               this, category_item, property_item, properties_frame);
        category_button->setAdvisorHeaderText(component_pair.second->getDisplayName());
        category_button->setAdvisorBodyText(component_pair.second->getDescription());
        QString buttonColor = QString(" QPushButton { height: 22px; font: 13px; text-align: left; icon-size: 20px 16px; color: #000000; "
                                                    " border: 2px solid; "
                                                    " border-color: " + component_pair.second->getColor().darker(250).name() + "; "
                                                    " border-radius: 1px; "
                                                    " background: qlineargradient(spread:pad, x1:0 y1:0, x2:0 y2:1, stop:0 " +
                                                        component_pair.second->getColor().name() + ", stop:1 " +
                                                        component_pair.second->getColor().darker(250).name() + "); } "
                                      " QPushButton:hover:!pressed { color: " + component_pair.second->getColor().lighter(200).name() + "; } "
                                      " QPushButton:pressed { color: " + component_pair.second->getColor().darker(400).name() + "; } ");
        category_button->setIcon(QIcon(component_pair.second->getIcon()));
        category_button->setStyleSheet(buttonColor);


        // Apply the button and property box widgets to the tree items
        this->setItemWidget(category_item, 0, category_button);
        this->setItemWidget(property_item, 0, properties_frame);
    }

    this->expandAll();
}



//####################################################################################
//##        On object inspector click show info about object and property
//####################################################################################
void TreeInspector::itemWasClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(item);
    Q_UNUSED(column);

    //Dr::ShowMessageBox("Item Clicked in Tree Object Inspector");

    // If no item is selected in tree view, exit function
    //if (treeScene->getSelectedKey() == 0) { return; }

    // First, retrieve property key of item clicked in tableWidget list
    //long        property_key = item->data(User_Roles::Key).toLongLong();

    // Grab a pointer to the component list of the first selected item from treeScene (stored in selected_list)
    //DrSettings  *selected_item_settings = m_project->findSettingsFromKey( treeScene->getSelectedKey() );
    //DrComponent *clicked_component = selected_item_settings->findComponentFromPropertyKey(property_key);
    //DrProperty  *clicked_property = clicked_component->getProperty(property_key);

    //QString     property_name = clicked_property->getDisplayName();
    //QString     component_name = clicked_component->getDisplayName();
    //long        component_key = clicked_component->getComponentKey();

    // Grab type of main selected item in selected tree list
    //QString     type_string2 = StringFromType(m_project->findTypeFromKey( treeScene->getSelectedKey() ));
    //QString     type_string = StringFromType(selected_item_settings->getType());

    // !!!!! #DEBUG:    Show selected item key and info
    //if (Dr::CheckDebugFlag(Debug_Flags::Object_Inspector_Build)) {
    //    setLabelText(Label_Names::LabelObject1, "KEY: " + QString::number( treeScene->getSelectedKey() ) + ", TYPE: " + QString::fromStdString(type_string));
    //    setLabelText(Label_Names::LabelObject2, "COMPONENT: " + QString::number(component_key) +   ", NAME: " + QString::fromStdString(component_name));
    //    setLabelText(Label_Names::LabelObject3, "PROPERTY: " + QString::number(property_key) +   ", NAME: " + QString::fromStdString(property_name));
    //}
    // !!!!! END

}



//####################################################################################
//##
//##    InspectorCategoryButton Class Functions
//##
//####################################################################################
// Constructor for category button, gives button a way to pass click to custom function
InspectorCategoryButton::InspectorCategoryButton(const QString &text, TreeInspector *parent_tree,
                                                 QTreeWidgetItem *parent_item, QTreeWidgetItem *child_item, QFrame *new_child)
    : QPushButton(text, parent_tree),
      m_parent_tree(parent_tree),           m_parent_item(parent_item),
      m_child_item(child_item),             m_child_frame(new_child)
{
    // Forwards user button click to function that expands / contracts
    connect(this, SIGNAL(clicked()), this, SLOT(buttonPressed()));
}

// Handles changing the Advisor on Mouse Enter
void InspectorCategoryButton::enterEvent(QEvent *event)
{
    m_parent_tree->getRelay()->setAdvisorInfo(m_header, m_body);
    QPushButton::enterEvent(event);
}

// Called by click signal, expands or contracts category after user click
void InspectorCategoryButton::animationDone() { if (m_is_shrunk) m_parent_item->setExpanded(false); }
void InspectorCategoryButton::animationUpdate(const QVariant &value) { Q_UNUSED(value); }
void InspectorCategoryButton::buttonPressed()
{
    ///// Item shrinking / expanding animation, doesnt work that great overall
    ///QPropertyAnimation *propAnimationFade = new QPropertyAnimation(m_child_frame, "geometry");
    ///propAnimationFade->setDuration(100);
    ///start_rect = m_child_frame->geometry();
    ///end_rect = start_rect;
    ///    if (m_is_shrunk) {
    ///        m_parent_item->setExpanded(true);
    ///        start_rect.setHeight(0);
    ///        end_rect.setHeight(m_height);
    ///    } else {
    ///        m_height = start_rect.height();
    ///        end_rect.setHeight(0);
    ///    }
    ///    m_is_shrunk = !m_is_shrunk;
    ///connect(propAnimationFade, SIGNAL(finished()), this, SLOT(animationDone()));
    ///connect(propAnimationFade, SIGNAL(valueChanged(QVariant)), this, SLOT(animationUpdate(QVariant)));
    ///propAnimationFade->setStartValue(start_rect);
    ///propAnimationFade->setEndValue(end_rect);
    ///propAnimationFade->start();

    m_is_shrunk = !m_is_shrunk;
    m_parent_item->setExpanded(!m_is_shrunk);

}



//####################################################################################
//##
//##    WidgetHoverHandler Class Functions
//##
//####################################################################################
void WidgetHoverHandler::attach(QWidget *widget)
{
    widget->setAttribute(Qt::WidgetAttribute::WA_Hover, true);
    widget->installEventFilter(this);
}

bool WidgetHoverHandler::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::HoverEnter)
    {
        QWidget *hover_widget = dynamic_cast<QWidget*>(obj);

        QString header = hover_widget->property(User_Property::Header).toString();
        QString body = hover_widget->property(User_Property::Body).toString();

        emit signalMouseHover(header, body);
    }

    return false;
}












