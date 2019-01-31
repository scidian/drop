//
//      Created by Stephens Nunnally on 1/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Tree Asset Definitions
//
//

#include "project.h"
#include "project_asset.h"
#include "project_world.h"
#include "project_world_scene.h"
#include "project_world_scene_object.h"

#include "settings.h"
#include "settings_component.h"
#include "settings_component_property.h"

#include "editor_tree_assets.h"
#include "editor_tree_inspector.h"
#include "interface_relay.h"



//####################################################################################
//##        Constructor
//####################################################################################
TreeAssetList::TreeAssetList(QWidget *parent, DrProject *project, InterfaceRelay *relay) :
                             QTreeWidget (parent), m_project(project), m_relay(relay)
{
    m_widget_hover = new WidgetHoverHandler(this);
    connect(m_widget_hover, SIGNAL(signalMouseHover(QString, QString)), this, SLOT(setAdvisorInfo(QString, QString)));
}



//####################################################################################
//##        Advisor Info Functions
//####################################################################################
// Handles changing the Advisor on Mouse Enter
void TreeAssetList::enterEvent(QEvent *event)
{
    m_relay->setAdvisorInfo(Advisor_Info::Asset_List);
    QTreeWidget::enterEvent(event);
}

void TreeAssetList::setAdvisorInfo(QString header, QString body)
{
    m_relay->setAdvisorInfo(header, body);
}

void TreeAssetList::applyHeaderBodyProperties(QWidget *widget, DrProperty *property)
{
    widget->setProperty(User_Property::Header, property->getDisplayName());
    widget->setProperty(User_Property::Body, property->getDescription());
    m_widget_hover->attach(widget);
}

void TreeAssetList::applyHeaderBodyProperties(QWidget *widget, QString header, QString body)
{
    widget->setProperty(User_Property::Header, header);
    widget->setProperty(User_Property::Body, body);
    m_widget_hover->attach(widget);
}





void TreeAssetList::buildAssetList()
{
    // Initialize some QWidget helper items
    QSizePolicy sp_left(QSizePolicy::Preferred, QSizePolicy::Preferred);
    QSizePolicy sp_right(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sp_left.setHorizontalStretch(ASSET_SIZE_LEFT);
    sp_right.setHorizontalStretch(ASSET_SIZE_RIGHT);

    QFont fp;
    fp.setPointSize(Dr::FontSize());


    // ***** Retrieve list of assets for project
    AssetMap list_assets = m_project->getAssets();
    int rowCount = 0;
    this->clear();


    // Create new item in list to hold component and add the TreeWidgetItem to the tree
    QTreeWidgetItem *category_item = new QTreeWidgetItem();
    this->addTopLevelItem(category_item);

    // Create and style a button to be used as a header item for the category
    AssetCategoryButton *category_button = new AssetCategoryButton("  Objects", this, category_item);
    category_button->setAdvisorHeaderText("Object Assets");
    category_button->setAdvisorBodyText("Objects for use in scene");
    QString buttonColor = QString(" QPushButton { height: 22px; font: 13px; text-align: left; icon-size: 20px 16px; color: #CCCCCC; "
                                                " border: 2px solid; "
                                                " border-color: #555555; "
                                                " border-radius: 1px; "
                                                " background: qlineargradient(spread:pad, x1:0 y1:0, x2:0 y2:1, stop:0 " +
                                                    Dr::GetColor(Window_Colors::Button_Light).name() + ", stop:1 " +
                                                    Dr::GetColor(Window_Colors::Background_Dark).name() + "); } "
                                  " QPushButton:hover:!pressed { color: " + Dr::GetColor(Window_Colors::Highlight).name() + "; } "
                                  " QPushButton:pressed { color: " + Dr::GetColor(Window_Colors::Shadow).name() + "; } ");
    //category_button->setIcon(QIcon(component_map.second->getIcon()));
    category_button->setStyleSheet(buttonColor);
    category_button->setEnabled(false);

    this->setItemWidget(category_item, 0, category_button);                             // Apply the button to the tree item



    // Loop through each property and add it to the component frame
    for (auto asset_pair: list_assets) {
        if (asset_pair.second->getAssetType() != DrAsset_Type::Object) continue;

        QFrame *single_row = new QFrame();
        QBoxLayout *horizontal_split = new QHBoxLayout(single_row);
        horizontal_split->setSpacing(0);
        horizontal_split->setMargin(0);
        horizontal_split->setContentsMargins(0,0,0,0);

        QLabel *asset_name = new QLabel(asset_pair.second->getAssetName());
        asset_name->setFont(fp);
        asset_name->setSizePolicy(sp_left);
        asset_name->setAlignment(Qt::AlignmentFlag::AlignCenter);
        applyHeaderBodyProperties(asset_name, asset_pair.second->getAssetName(), "None.");
        horizontal_split->addWidget(asset_name);

        QPixmap pix = asset_pair.second->getComponentProperty(Asset_Components::animation, Asset_Properties::animation_default)->getValue().value<QPixmap>();
        QLabel *pix_label = new QLabel();
        pix_label->setObjectName("assetFrame");
        pix_label->setFont(fp);
        pix_label->setSizePolicy(sp_right);
        pix_label->setMinimumHeight(60);
        pix_label->setMaximumHeight(60);
        pix_label->setAlignment(Qt::AlignmentFlag::AlignCenter);
        horizontal_split->addWidget( pix_label );

        // Draw pixmap onto label
        pix_label->setPixmap(pix.scaled(90, 45, Qt::KeepAspectRatio));

        // Create a child TreeWidgetItem attached to the TopLevel category item
        QTreeWidgetItem *property_item = new QTreeWidgetItem();
        property_item->setDisabled(true);
        category_item->addChild(property_item);
        this->setItemWidget(property_item, 0, single_row);



        category_button->setEnabled(true);
        rowCount++;
    }







    this->expandAll();

}




//####################################################################################
//##
//##    InspectorCategoryButton Class Functions
//##
//####################################################################################
// Constructor for category button, gives button a way to pass click to custom function
AssetCategoryButton::AssetCategoryButton(const QString &text, TreeAssetList *parent_tree, QTreeWidgetItem *parent_item)
    : QPushButton(text, parent_tree), m_parent_tree(parent_tree), m_parent_item(parent_item)
{
    // Forwards user button click to function that expands / contracts
    connect(this, SIGNAL(clicked()), this, SLOT(buttonPressed()));
}

// Handles changing the Advisor on Mouse Enter
void AssetCategoryButton::enterEvent(QEvent *event)
{
    m_parent_tree->getRelay()->setAdvisorInfo(m_header, m_body);
    QPushButton::enterEvent(event);
}

// Called by click signal, expands or contracts category after user click
void AssetCategoryButton::buttonPressed()
{
    m_is_shrunk = !m_is_shrunk;
    m_parent_item->setExpanded(!m_is_shrunk);
}






