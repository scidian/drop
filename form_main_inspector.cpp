//
//      Created by Stephens Nunnally on 12/17/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Handles dealing with the Object Inspector
//
//

#include "01_project.h"
#include "02_world.h"
#include "03_scene.h"
#include "04_object.h"

#include "30_settings.h"
#include "31_component.h"
#include "32_property.h"

#include "form_main.h"


//####################################################################################
//
//  Need to finish dynamically building object inspector
//
void FormMain::buildObjectInspector()
{
    // First, retrieve unique key of item clicked in list
    long        selected_key = treeScene->getSelectedKey();
    DrTypes     selected_type = project->findTypeFromKey( selected_key );
    std::string type_string = StringFromType(selected_type);
    setLabelText(Label_Names::LabelObject1, "KEY: " + QString::number( selected_key ) + ", TYPE: " + QString::fromStdString(type_string));
    setLabelText(Label_Names::LabelObject2, "");
    setLabelText(Label_Names::LabelObject3, "");


    // Change Advisor text after new item selection
    switch (selected_type) {
    case DrTypes::World:        setAdvisorInfo(Advisor_Info::World_Object);         break;
    case DrTypes::Scene:        setAdvisorInfo(Advisor_Info::Scene_Object);         break;
    case DrTypes::Camera:       setAdvisorInfo(Advisor_Info::Camera_Object);        break;
    case DrTypes::Character:    setAdvisorInfo(Advisor_Info::Character_Object);     break;
    case DrTypes::Object:       setAdvisorInfo(Advisor_Info::Object_Object);        break;
    default:                    setAdvisorInfo(Advisor_Info::Not_Set);
    }


    // Retrieve list of components for selected item
    ComponentMap components = project->findSettingsFromKey( selected_key )->getComponentList();

    // Loop through each component and add it to the Object Inspector list
    int rowCount = 0;
    treeObject->clear();
    for (auto i: components) {
        // Creates a frame to hold all properties of component, with vertical layout
        QFrame *properties_frame = new QFrame(treeObject);
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

        // Create new item in list to hold component and add the TreeWidgetItem to the tree
        QTreeWidgetItem *category_item = new QTreeWidgetItem();
        category_item->setData(0, User_Roles::Key, QVariant::fromValue(i.second->getComponentKey()));           // Stores component key in list user data
        treeObject->addTopLevelItem(category_item);

        // Create a child TreeWidgetItem attached to the TopLevel category item
        QTreeWidgetItem *property_item = new QTreeWidgetItem();
        property_item->setDisabled(true);
        category_item->addChild(property_item);

        //->Create and style a button to be used as a header item for the category
        InspectorCategoryButton *category_button = new InspectorCategoryButton(QString(" ") + i.second->getDisplayNameQString(),
                                                                               treeObject, category_item, property_item, properties_frame);
        QString buttonColor = QString(" QPushButton { height: 24px; font: 13px; text-align: left; icon-size: 20px 16px; color: #000000; "
                                                    " border: none; border-radius: 0px; background: qlineargradient(spread:pad, x1:0 y1:0, x2:0 y2:1, stop:0 " +
                                                    i.second->getColor().name() + ", stop:1 " + i.second->getColor().darker(250).name() + "); } "
                                      " QPushButton:hover:!pressed { color: #FFFFFF; } "
                                      " QPushButton:pressed { color: #333333; }");
        category_button->setIcon(QIcon(i.second->getIcon()));
        category_button->setStyleSheet(buttonColor);

        // Apply the button and property box widgets to the tree items
        treeObject->setItemWidget(category_item, 0, category_button);
        treeObject->setItemWidget(property_item, 0, properties_frame);
    }

    treeObject->expandAll();
}


// Handles changing the Advisor on Mouse Enter
void TreeObjectInspector::enterEvent(QEvent *event)
{
    getMainWindow()->setAdvisorInfo(Advisor_Info::Object_Inspector);
    QTreeWidget::enterEvent(event);
}




// Constructor for category button, gives button a way to pass click to custom function
InspectorCategoryButton::InspectorCategoryButton(const QString &text, TreeObjectInspector *parent_tree,
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












