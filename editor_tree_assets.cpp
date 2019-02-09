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
#include "project_world_stage.h"
#include "project_world_stage_object.h"

#include "settings.h"
#include "settings_component.h"
#include "settings_component_property.h"

#include "editor_tree_assets.h"
#include "editor_tree_widgets.h"
#include "interface_relay.h"



//####################################################################################
//##        Constructor
//####################################################################################
TreeAssetList::TreeAssetList(QWidget *parent, DrProject *project, InterfaceRelay *relay) :
                             QTreeWidget (parent), m_project(project), m_relay(relay)
{
    m_widget_hover = new WidgetHoverHandler(this);
    connect(m_widget_hover, SIGNAL(signalMouseHover(QString, QString)), this, SLOT(setAdvisorInfo(QString, QString)));

    m_widget_hover->applyHeaderBodyProperties(this, Advisor_Info::Asset_List);
}

// SLOT: Catches signals from m_widget_hover
void TreeAssetList::setAdvisorInfo(QString header, QString body) {
    m_relay->setAdvisorInfo(header, body);
}
void TreeAssetList::applyHeaderBodyProperties(QWidget *widget, DrProperty *property) {
    m_widget_hover->applyHeaderBodyProperties(widget, property);
}
void TreeAssetList::applyHeaderBodyProperties(QWidget *widget, QString header, QString body) {
    m_widget_hover->applyHeaderBodyProperties(widget, header, body);
}



//####################################################################################
//##        Tree Building Functions
//####################################################################################
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
    CategoryButton *category_button = new CategoryButton("  Objects", category_item);
    QString buttonColor = QString(" QPushButton { height: 22px; font: 13px; text-align: left; icon-size: 20px 16px; color: #CCCCCC; "
                                                " border: 2px solid; border-radius: 1px; "
                                                " border-color: #555555; "
                                                " background: qlineargradient(spread:pad, x1:0 y1:0, x2:0 y2:1, stop:0 " +
                                                    Dr::GetColor(Window_Colors::Button_Light).name() + ", stop:1 " +
                                                    Dr::GetColor(Window_Colors::Background_Dark).name() + "); } "
                                  " QPushButton:hover:!pressed { color: " + Dr::GetColor(Window_Colors::Highlight).name() + "; } "
                                  " QPushButton:pressed { color: " + Dr::GetColor(Window_Colors::Shadow).name() + "; } ");
    //category_button->setIcon(QIcon(component_map.second->getIcon()));
    category_button->setStyleSheet(buttonColor);
    category_button->setEnabled(false);
    applyHeaderBodyProperties(category_button, "Object Assets", "Objects for use in Stage");

    this->setItemWidget(category_item, 0, category_button);                             // Apply the button to the tree item

    // Loop through each property and add it to the component frame
    for (auto asset_pair: list_assets) {
        if (asset_pair.second->getAssetType() != DrAsset_Type::Object) continue;

        QFrame *single_row = new QFrame();
        single_row->setObjectName("assetFrame");

        QBoxLayout *vertical_split = new QVBoxLayout(single_row);
        vertical_split->setSpacing(0);
        vertical_split->setMargin(0);
        vertical_split->setContentsMargins(0,0,0,0);

        QLabel *asset_name = new QLabel(asset_pair.second->getAssetName());
        asset_name->setFont(fp);
        asset_name->setSizePolicy(sp_left);
        asset_name->setAlignment(Qt::AlignmentFlag::AlignCenter);
        m_widget_hover->applyHeaderBodyProperties(asset_name, asset_pair.second->getAssetName(), "None.");
        vertical_split->addWidget(asset_name);

        QPixmap pix = asset_pair.second->getComponentProperty(Asset_Components::animation, Asset_Properties::animation_default)->getValue().value<QPixmap>();
        QLabel *pix_label = new QLabel();
        pix_label->setFont(fp);
        pix_label->setSizePolicy(sp_right);
        pix_label->setFixedHeight(50);
        pix_label->setAlignment(Qt::AlignmentFlag::AlignCenter);
        m_widget_hover->applyHeaderBodyProperties(pix_label, asset_pair.second->getAssetName(), "None.");
        vertical_split->addWidget( pix_label );

        // Draw pixmap onto label
        pix_label->setPixmap(pix.scaled(160, 38, Qt::KeepAspectRatio));




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

























