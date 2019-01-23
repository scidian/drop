//
//      Created by Stephens Nunnally on 1/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Tree Inspector Definitions
//
//

#include "library.h"

#include "project.h"
#include "project_world.h"
#include "project_world_scene.h"
#include "project_world_scene_object.h"

#include "settings.h"
#include "settings_component.h"
#include "settings_component_property.h"

#include "editor_tree_inspector.h"
#include "interface_relay.h"



//####################################################################################
//
//  Need to finish dynamically building object inspector
//
void TreeInspector::buildInspectorFromKeys(QList<long> key_list)
{
    // First, retrieve unique key of item clicked in list
    long        selected_key = key_list[0];
    DrTypes     selected_type = m_project->findTypeFromKey( selected_key );
    std::string type_string = StringFromType(selected_type);

    // !!!!! #DEBUG:    Show selected item key and info
    if (Dr::CheckDebugFlag(Debug_Flags::Label_Object_Inspector_Build)) {
        m_relay->setLabelText(Label_Names::Label_Object_1, "KEY: " + QString::number( selected_key ) + ", TYPE: " + QString::fromStdString(type_string));
        m_relay->setLabelText(Label_Names::Label_Object_2, "");
        m_relay->setLabelText(Label_Names::Label_Object_3, "");
    }
    // !!!!! END

    // Change Advisor text after new item selection
    switch (selected_type) {
    case DrTypes::World:        m_relay->setAdvisorInfo(Advisor_Info::World_Object);         break;
    case DrTypes::Scene:        m_relay->setAdvisorInfo(Advisor_Info::Scene_Object);         break;
    case DrTypes::Camera:       m_relay->setAdvisorInfo(Advisor_Info::Camera_Object);        break;
    case DrTypes::Character:    m_relay->setAdvisorInfo(Advisor_Info::Character_Object);     break;
    case DrTypes::Object:       m_relay->setAdvisorInfo(Advisor_Info::Object_Object);        break;
    default:                    m_relay->setAdvisorInfo(Advisor_Info::Not_Set);
    }


    // Retrieve list of components for selected item
    ComponentMap components = m_project->findSettingsFromKey( selected_key )->getComponentList();

    // Loop through each component and add it to the Object Inspector list
    int rowCount = 0;
    this->clear();
    for (auto i: components) {

        // Create new item in list to hold component and add the TreeWidgetItem to the tree
        QTreeWidgetItem *category_item = new QTreeWidgetItem();
        category_item->setData(0, User_Roles::Key, QVariant::fromValue(i.second->getComponentKey()));           // Stores component key in list user data
        this->addTopLevelItem(category_item);

        // Creates a frame to hold all properties of component, with vertical layout
        QFrame *properties_frame = new QFrame(this);
        QBoxLayout *vertical_layout = new QVBoxLayout(properties_frame);
        vertical_layout->setSpacing(0);
        vertical_layout->setMargin(0);
        vertical_layout->setContentsMargins(0,0,0,0);

        // Loop through each property and add it to the component frame
        for (auto j: i.second->getPropertyList()) {
            QFrame *single_row = new QFrame(properties_frame);
            QBoxLayout *horizontal_split = new QHBoxLayout(single_row);
            horizontal_split->setSpacing(0);
            horizontal_split->setMargin(0);
            horizontal_split->setContentsMargins(2,2,2,2);

            QLabel *property_name = new QLabel(j.second->getDisplayNameQString());
            QFont font_property;
            font_property.setPointSize(11);
            property_name->setFont(font_property);
                QSizePolicy sp_left(QSizePolicy::Preferred, QSizePolicy::Preferred);
                sp_left.setHorizontalStretch(1);
            property_name->setSizePolicy(sp_left);
            horizontal_split->addWidget(property_name);



            QSpinBox *spin_int = new QSpinBox();
            spin_int->setFont(font_property);
                QSizePolicy sp_right(QSizePolicy::Preferred, QSizePolicy::Preferred);
                sp_right.setHorizontalStretch(2);
            spin_int->setSizePolicy(sp_right);


            //spin_int->setUserData( setData(0, User_Roles::Key, QVariant::fromValue(object_pair.second->getKey()));


            horizontal_split->addWidget(spin_int);



            vertical_layout->addWidget(single_row);
            rowCount++;
        }


        // Create a child TreeWidgetItem attached to the TopLevel category item
        QTreeWidgetItem *property_item = new QTreeWidgetItem();
        property_item->setDisabled(true);
        category_item->addChild(property_item);

        //->Create and style a button to be used as a header item for the category
        InspectorCategoryButton *category_button = new InspectorCategoryButton(QString(" ") + i.second->getDisplayNameQString(),
                                                                               this, category_item, property_item, properties_frame);
        QString buttonColor = QString(" QPushButton { height: 24px; font: 13px; text-align: left; icon-size: 20px 16px; color: #000000; "
                                                    " border: none; border-radius: 0px; background: qlineargradient(spread:pad, x1:0 y1:0, x2:0 y2:1, stop:0 " +
                                                    i.second->getColor().name() + ", stop:1 " + i.second->getColor().darker(250).name() + "); } "
                                      " QPushButton:hover:!pressed { color: #FFFFFF; } "
                                      " QPushButton:pressed { color: #333333; }");
        category_button->setIcon(QIcon(i.second->getIcon()));
        category_button->setStyleSheet(buttonColor);


        // Apply the button and property box widgets to the tree items
        this->setItemWidget(category_item, 0, category_button);
        this->setItemWidget(property_item, 0, properties_frame);
    }

    this->expandAll();
}


// Handles changing the Advisor on Mouse Enter
void TreeInspector::enterEvent(QEvent *event)
{
    m_relay->setAdvisorInfo(Advisor_Info::Object_Inspector);
    QTreeWidget::enterEvent(event);
}



//####################################################################################
//##        On object inspector click show info about object and property
//####################################################################################
void TreeInspector::itemWasClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(item);
    Q_UNUSED(column);

    Dr::ShowMessageBox("Hi There Joe");

    // If no item is selected in tree view, exit function
    //if (treeScene->getSelectedKey() == 0) { return; }

    // First, retrieve property key of item clicked in tableWidget list
    //long        property_key = item->data(User_Roles::Key).toLongLong();

    // Grab a pointer to the component list of the first selected item from treeScene (stored in selected_list)
    //DrSettings  *selected_item_settings = m_project->findSettingsFromKey( treeScene->getSelectedKey() );
    //DrComponent *clicked_component = selected_item_settings->findComponentFromPropertyKey(property_key);
    //DrProperty  *clicked_property = clicked_component->getProperty(property_key);

    //std::string property_name = clicked_property->getDisplayName();
    //std::string component_name = clicked_component->getDisplayName();
    //long        component_key = clicked_component->getComponentKey();

    // Grab type of main selected item in selected tree list
    //std::string type_string2 = StringFromType(m_project->findTypeFromKey( treeScene->getSelectedKey() ));
    //std::string type_string = StringFromType(selected_item_settings->getType());

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

// Called by click signal, expands or contracts category after user click
void InspectorCategoryButton::animationDone() { if (m_is_shrunk) m_parent_item->setExpanded(false); }
void InspectorCategoryButton::animationUpdate(const QVariant &value) { Q_UNUSED(value); }
void InspectorCategoryButton::buttonPressed()
{
    QPropertyAnimation *propAnimationFade = new QPropertyAnimation(m_child_frame, "geometry");

    propAnimationFade->setDuration(100);
    start_rect = m_child_frame->geometry();
    end_rect = start_rect;

    if (m_is_shrunk) {
        m_parent_item->setExpanded(true);
        start_rect.setHeight(0);
        end_rect.setHeight(m_height);
    } else {
        m_height = start_rect.height();
        end_rect.setHeight(0);
    }
    m_is_shrunk = !m_is_shrunk;

    connect(propAnimationFade, SIGNAL(finished()), this, SLOT(animationDone()));
    connect(propAnimationFade, SIGNAL(valueChanged(QVariant)), this, SLOT(animationUpdate(QVariant)));

    propAnimationFade->setStartValue(start_rect);
    propAnimationFade->setEndValue(end_rect);
    propAnimationFade->start();
}












