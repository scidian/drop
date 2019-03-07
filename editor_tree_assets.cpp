//
//      Created by Stephens Nunnally on 1/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Tree Asset Definitions
//
//
#include <QEvent>
#include <QFrame>
#include <QLabel>
#include <QVBoxLayout>

#include "colors.h"
#include "debug.h"

#include "editor_tree_assets.h"
#include "editor_tree_widgets.h"

#include "interface_editor_relay.h"

#include "project.h"
#include "project_asset.h"
#include "project_world.h"
#include "project_world_stage.h"
#include "project_world_stage_object.h"
#include "settings.h"
#include "settings_component.h"
#include "settings_component_property.h"


//####################################################################################
//##        Constructor
//####################################################################################
TreeAssets::TreeAssets(QWidget *parent, DrProject *project, IEditorRelay *editor_relay) :
                       QTreeWidget (parent), m_project(project), m_editor_relay(editor_relay)
{
    m_widget_hover = new WidgetHoverHandler(this);
    connect(m_widget_hover, SIGNAL(signalMouseHover(QString, QString)), this, SLOT(setAdvisorInfo(QString, QString)));

    m_widget_hover->attachToHoverHandler(this, Advisor_Info::Asset_List);
}

// SLOT: Catches signals from m_widget_hover
void TreeAssets::setAdvisorInfo(QString header, QString body) {
    m_editor_relay->setAdvisorInfo(header, body);
}
void TreeAssets::attachToHoverHandler(QWidget *widget, DrProperty *property) {
    m_widget_hover->attachToHoverHandler(widget, property);
}
void TreeAssets::attachToHoverHandler(QWidget *widget, QString header, QString body) {
    m_widget_hover->attachToHoverHandler(widget, header, body);
}



//####################################################################################
//##        Tree Building Functions
//####################################################################################
void TreeAssets::buildAssetTree()
{
    // ***** Initialize some QWidget helper items
    QSizePolicy sp_left(QSizePolicy::Preferred, QSizePolicy::Preferred);
    QSizePolicy sp_right(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sp_left.setHorizontalStretch(c_asset_size_left);
    sp_right.setHorizontalStretch(c_asset_size_right);

    QFont fp;
    fp.setPointSize(Dr::FontSize());


    // ***** Retrieve list of assets for project
    AssetMap list_assets = m_project->getAssets();
    int rowCount = 0;
    this->clear();
    m_asset_frames.clear();

    // Create new item in list to hold component and add the TreeWidgetItem to the tree
    QTreeWidgetItem *category_item = new QTreeWidgetItem();
    this->addTopLevelItem(category_item);

    // Create and style a button to be used as a header item for the category
    CategoryButton *category_button = new CategoryButton("  Objects", QColor(204, 204, 204), nullptr, category_item);
    QString buttonColor = QString(" QPushButton { height: 22px; font: 13px; text-align: left; icon-size: 20px 16px; color: #CCCCCC; "
                                                " border: " + Dr::BorderWidth() + " solid; border-radius: 1px; "
                                                " border-color: #555555; "
                                                " background: qlineargradient(spread:pad, x1:0 y1:0, x2:0 y2:1, stop:0 " +
                                                    Dr::GetColor(Window_Colors::Button_Light).name() + ", stop:1 " +
                                                    Dr::GetColor(Window_Colors::Background_Dark).name() + "); } "
                                  " QPushButton:hover:!pressed { color: " + Dr::GetColor(Window_Colors::Highlight).name() + "; } "
                                  " QPushButton:pressed { color: " + Dr::GetColor(Window_Colors::Shadow).name() + "; } ");

    /// !!!!! TODO: Set Icon call, need to implement
    ///category_button->setIcon(QIcon(component_map.second->getIcon()));

    category_button->setStyleSheet(buttonColor);
    category_button->setEnabled(false);
    attachToHoverHandler(category_button, "Object Assets", "Objects for use in Stage");

    this->setItemWidget(category_item, 0, category_button);                             // Apply the button to the tree item

    // ********** Loop through each object asset and add it to the component frame
    for (auto asset_pair: list_assets) {
        if (asset_pair.second->getAssetType() != DrAssetType::Object) continue;

        // ***** Store current asset key in widget and install a mouse handler event filter on the item, AssetMouseHandler
        QFrame *single_row = new QFrame();
        single_row->setObjectName("assetFrame");
        single_row->setProperty(User_Property::Key, QVariant::fromValue( asset_pair.second->getKey() ));
        single_row->installEventFilter(new AssetMouseHandler(single_row, m_editor_relay));

        // Store pointer to frame in a list for future reference
        m_asset_frames.append(single_row);

        QBoxLayout *vertical_split = new QVBoxLayout(single_row);
        vertical_split->setSpacing(0);
        vertical_split->setMargin(0);
        vertical_split->setContentsMargins(0,0,0,0);

        QLabel *asset_name = new QLabel(asset_pair.second->getAssetName());
        asset_name->setObjectName(QStringLiteral("assetName"));
        asset_name->setFont(fp);
        asset_name->setSizePolicy(sp_left);
        asset_name->setAlignment(Qt::AlignmentFlag::AlignCenter);
        m_widget_hover->attachToHoverHandler(asset_name, asset_pair.second->getAssetName(), Advisor_Info::Asset_Object[1] );
        vertical_split->addWidget(asset_name);

        // ***** Create the label that will display the asset
        QPixmap pix = asset_pair.second->getComponentProperty(Components::Asset_Animation, Properties::Asset_Animation_Default)->getValue().value<QPixmap>();
        QLabel *asset_pix = new QLabel();
        asset_pix->setObjectName(QStringLiteral("assetPixmap"));
        asset_pix->setFont(fp);
        asset_pix->setSizePolicy(sp_right);
        asset_pix->setFixedHeight(45);
        asset_pix->setAlignment(Qt::AlignmentFlag::AlignCenter);
        m_widget_hover->attachToHoverHandler(asset_pix, asset_pair.second->getAssetName(), Advisor_Info::Asset_Object[1] );
        vertical_split->addWidget( asset_pix );

        // Draw pixmap onto label
        asset_pix->setPixmap(pix.scaled(120, 35, Qt::KeepAspectRatio));


        // ***** Create a child TreeWidgetItem attached to the TopLevel category item
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
//##        Updates Asset List (like asset names) if items have been changed
//####################################################################################
void TreeAssets::updateAssetList(QList<DrSettings*> changed_items, QList<long> property_keys)
{
    QLabel *label;
    for (auto item : changed_items) {
        long item_key = item->getKey();

        for (auto frame : m_asset_frames) {
            long label_key = frame->property(User_Property::Key).toLongLong();

            if (item_key == label_key) {
                for (auto property : property_keys) {
                    Properties check_property = static_cast<Properties>(property);

                    switch (check_property)
                    {
                    case Properties::Asset_Name:
                        label = frame->findChild<QLabel*>("assetName");
                        if (label) label->setText(item->getComponentPropertyValue(Components::Asset_Settings, Properties::Asset_Name).toString() );
                        break;
                    default: ;
                    }
                }
            }
        }
    }
    update();
}



//####################################################################################
//##    AssetMouseHandler Class Functions
//##        eventFilter - handles mouse click on asset, loads object inspector for clicked asset
//####################################################################################
AssetMouseHandler::AssetMouseHandler(QObject *parent, IEditorRelay *editor_relay) : QObject(parent), m_editor_relay(editor_relay) {}

bool AssetMouseHandler::eventFilter(QObject *obj, QEvent *event)
{
    if (!obj) return false;
    QWidget *asset = dynamic_cast<QWidget*>(obj);

    if (event->type() == QEvent::MouseButtonPress)
    {
        long asset_key = asset->property(User_Property::Key).toLongLong();
        m_editor_relay->buildObjectInspector( { asset_key } );
    }

    return QObject::eventFilter(obj, event);
}


















