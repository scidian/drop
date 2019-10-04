//
//      Created by Stephens Nunnally on 1/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Tree Asset Definitions
//
//
#include <QDebug>
#include <QEvent>
#include <QFrame>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>

#include "colors/colors.h"
#include "debug.h"
#include "editor_tree_assets.h"
#include "globals.h"
#include "imaging/imaging.h"
#include "interface_editor_relay.h"
#include "helper.h"
#include "helper_qt.h"
#include "project/project.h"
#include "project/project_asset.h"
#include "project/project_effect.h"
#include "project/project_font.h"
#include "project/project_world.h"
#include "project/project_world_stage.h"
#include "project/project_world_stage_thing.h"
#include "settings/settings.h"
#include "settings/settings_component.h"
#include "settings/settings_component_property.h"
#include "widgets/widgets.h"
#include "widgets/widgets_event_filters.h"
#include "widgets/widgets_layout.h"


//####################################################################################
//##    Constructor
//####################################################################################
TreeAssets::TreeAssets(QWidget *parent, DrProject *project, IEditorRelay *editor_relay) :
                       QTreeWidget (parent), m_project(project), m_editor_relay(editor_relay) {
    // Initialize hover handler
    m_filter_hover = new DrFilterHoverHandler(this);
    connect(m_filter_hover, SIGNAL(signalMouseHover(QString, QString)), this, SLOT(setAdvisorInfo(QString, QString)));

    // Connect this widget to the hover handler
    m_filter_hover->attachToHoverHandler(this, Advisor_Info::Asset_List);

    // Build search bar
    m_search_widget = new QWidget(parent);
    m_search_widget->setFixedHeight(30);
    m_search_widget->setObjectName(QStringLiteral("assetSearchWidget"));
    m_filter_hover->attachToHoverHandler(m_search_widget, Advisor_Info::Asset_Search);
    m_search_widget->setToolTip( Advisor_Info::Asset_Search[0] );
        m_search_layout = new QVBoxLayout(m_search_widget);
        m_search_layout->setMargin(4);
            m_search_bar = new QLineEdit();
            m_search_bar->setObjectName("assetSearchBar");
            m_search_bar->setPlaceholderText("Search");
            m_search_bar->setFont(Dr::CustomFont());
            connect (m_search_bar, SIGNAL(textChanged(QString)), this, SLOT(searchTextChanged(QString)) );
        m_search_layout->addWidget(m_search_bar);

    parent->layout()->addWidget(m_search_widget);
}

// SLOT: Catches signals from m_filter_hover and passes to InterfaceEditorRelay
void TreeAssets::setAdvisorInfo(QString header, QString body) { m_editor_relay->setAdvisorInfo(header, body); }

// SLOT: Catches signal textChanged() from m_search_bar QLineEdit search bar
void TreeAssets::searchTextChanged(QString new_text) {
    buildAssetTree(new_text);
}



//####################################################################################
//##    Tree Building Functions
//####################################################################################
void TreeAssets::buildAssetTree(QString search_text) {

    // ***** Initialize some QWidget helper items
    QSizePolicy sp_left(QSizePolicy::Preferred, QSizePolicy::Preferred);
    QSizePolicy sp_right(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sp_left.setHorizontalStretch(c_asset_size_left);
    sp_right.setHorizontalStretch(c_asset_size_right);

    QFont font = Dr::CustomFont();

    // ***** Retrieve list of assets for project
    AssetMap &list_assets = m_project->getAssetMap();
    int rowCount = 0;
    this->clear();
    m_asset_frames.clear();


    // ***** Create new items in list to hold asset categories
    std::map <DrAssetType, QTreeWidgetItem*>       widget_items;
    std::map <DrAssetType, DrQPushButtonCategory*> category_buttons;
    std::map <DrAssetType, QFrame*>                assets_frames;
    std::map <DrAssetType, DrQLayoutFlow*>         grid_layouts;

    widget_items[DrAssetType::Character] =  new QTreeWidgetItem();
    widget_items[DrAssetType::Object] =     new QTreeWidgetItem();
    widget_items[DrAssetType::Effect] =     new QTreeWidgetItem();
    widget_items[DrAssetType::Text] =       new QTreeWidgetItem();

    for (auto item_pair : widget_items) {
        this->addTopLevelItem(item_pair.second);

        DrQPushButtonCategory* c;
        switch (item_pair.first) {
            case DrAssetType::Character: c = createCategoryButton(item_pair.second, "  Characters", "tree_character.png", Advisor_Info::Asset_Character);   break;
            case DrAssetType::Object:    c = createCategoryButton(item_pair.second, "  Objects",    "tree_object.png",    Advisor_Info::Asset_Object);      break;
            case DrAssetType::Effect:    c = createCategoryButton(item_pair.second, "  Effects",    "tree_effects.png",   Advisor_Info::Asset_Effect);      break;
            case DrAssetType::Text:      c = createCategoryButton(item_pair.second, "  Text",       "tree_text.png",      Advisor_Info::Asset_Text);        break;
        }
        category_buttons[item_pair.first] = c;

        // Creates a frame to hold all assets of each type
        assets_frames[item_pair.first] = new QFrame();
        grid_layouts[item_pair.first] =  new DrQLayoutFlow(assets_frames[item_pair.first], 8, 0, 4, 0, 0, 0);
        assets_frames[item_pair.first]->setObjectName("assetsContainer");
    }




    // ********** Loop through each object asset and add it to the component frame
    for (auto asset_pair: list_assets) {
        DrAsset *asset = asset_pair.second;
        if (!asset) continue;

        QString asset_name = asset->getName();
        if (!asset_name.toLower().contains(search_text.toLower())) continue;

        QSize frame_size = QSize(100, 66);
        QRect name_rect =  QRect(10, 0, frame_size.width() - 20, 25);
        QSize pix_size =   QSize(frame_size.width() - 25, frame_size.height() - 30);

        // ***** Store current asset key in widget and install a mouse handler event filter on the item, DrFilterAssetMouseHandler
        QFrame *single_asset = new QFrame();
        single_asset->setObjectName("assetFrame");
        single_asset->setProperty(User_Property::Key,        QVariant::fromValue( asset->getKey() ));
        single_asset->setProperty(User_Property::Mouse_Down, false);
        single_asset->installEventFilter(new DrFilterAssetMouseHandler(single_asset, m_editor_relay));
        single_asset->setFixedSize(frame_size);

        // Store pointer to frame in a list for future reference
        m_asset_frames.append(single_asset);


        // ***** Create the label that will display the asset name
        QFrame *text_holder = new QFrame(single_asset);
        text_holder->setGeometry(name_rect);
        text_holder->setObjectName(QStringLiteral("textHolder"));
            QLabel *asset_text = new QLabel(asset_name, text_holder);
            asset_text->setObjectName(QStringLiteral("assetName"));
            asset_text->setFont(font);
            asset_text->setSizePolicy(sp_left);
            asset_text->setGeometry(0, 0, text_holder->width(), text_holder->height());
            asset_text->setAlignment(Qt::AlignmentFlag::AlignCenter);
            asset_text->setText( Dr::FitStringToWidth( asset_text->font(), asset_text->text(), name_rect.width() ) );


        // ***** Create the label that will display the asset
        QBoxLayout *vertical_split = new QVBoxLayout(single_asset);
        vertical_split->setSpacing(0);
        vertical_split->setMargin(0);
        vertical_split->setContentsMargins(0, 14, 0, 0);                    // Put some space at the top
            QPixmap pix;
            QString description = asset->getComponentProperty(Components::Hidden_Settings, Properties::Hidden_Advisor_Description)->getValue().toString();
            switch (asset->getAssetType()) {
                case DrAssetType::Character:
                    pix = asset->getComponentProperty(Components::Asset_Animation, Properties::Asset_Animation_Default)->getValue().value<QPixmap>();
                    if (description == "") description = Advisor_Info::Asset_Character[1];
                    break;
                case DrAssetType::Object:
                    pix = asset->getComponentProperty(Components::Asset_Animation, Properties::Asset_Animation_Default)->getValue().value<QPixmap>();
                    if (description == "") description = Advisor_Info::Asset_Object[1];
                    break;
                case DrAssetType::Effect:
                    pix = m_project->getDrEffect( asset->getSourceKey() )->getPixmap();
                    if (description == "") description = Advisor_Info::Asset_Effect[1];
                    break;
                case DrAssetType::Text:
                    ///pix = m_project->getDrFont( asset->getSourceKey() )->getFontPixmap();
                    pix = m_project->getDrFont( asset->getSourceKey() )->createText( "Aa" );
                    if (description == "") description = Advisor_Info::Asset_Text[1];
                    break;
            }
            m_filter_hover->attachToHoverHandler(single_asset, asset_name, description);

            QLabel *asset_pix = new QLabel(single_asset);
            asset_pix->setObjectName(QStringLiteral("assetPixmap"));
            asset_pix->setFont(font);
            asset_pix->setSizePolicy(sp_right);
            asset_pix->setAlignment(Qt::AlignmentFlag::AlignCenter);
            vertical_split->addWidget( asset_pix );

            // Draw pixmap onto label
            asset_pix->setPixmap(pix.scaled(pix_size, Qt::KeepAspectRatio, Qt::SmoothTransformation));


        // ***** Add widget to proper category            
        grid_layouts[asset->getAssetType()]->addWidget(single_asset);
        category_buttons[asset->getAssetType()]->setEnabled(true);

        rowCount++;
    }


    // ***** Create a child TreeWidgetItem attached to the TopLevel category item
    for (auto button_pair : category_buttons) {
        if (button_pair.second->isEnabled())
            addAssetsToCategory(widget_items[button_pair.first], assets_frames[button_pair.first]);
        else
            this->takeTopLevelItem( this->indexOfTopLevelItem(widget_items[button_pair.first]) );
    }


    // ***** Adds a spacer item at the bottom to allow for comfortable scrolling to the bottom of the tree
    QTreeWidgetItem *spacer_item  = new QTreeWidgetItem();
    QLabel *spacer_label = new QLabel();
    spacer_label->setFixedHeight(120);
    this->addTopLevelItem(spacer_item);
    this->setItemWidget(spacer_item, 0, spacer_label);

    this->expandAll();
}


// Create a child TreeWidgetItem attached to the TopLevel category item
void TreeAssets::addAssetsToCategory(QTreeWidgetItem *tree_item, QFrame *asset_frame) {
    QTreeWidgetItem *asset_collection = new QTreeWidgetItem();
    asset_collection->setDisabled(true);
    tree_item->addChild(asset_collection);
    this->setItemWidget(asset_collection, 0, asset_frame);
}

// Create and style a buttons to be used as a header items for the categories
DrQPushButtonCategory* TreeAssets::createCategoryButton(QTreeWidgetItem *item, QString name, QString icon_resource, QList<QString> advisor_info) {
    QString icon_size = "14px 14px";
    if (name.toLower().contains("effects")) icon_size = "20px 18px";

    QString buttonColor = QString(" QPushButton { height: 22px; font: 13px; text-align: left; icon-size: " + icon_size + "; "
                                                " padding-left: 10px;"
                                                " color: " + Dr::GetColor(Window_Colors::Text).name() + "; "
                                                " border: none; "
                                                " border-style: solid; "
                                                " border-top-width: " + Dr::BorderWidth() + "; "
                                                " border-bottom-width: " + Dr::BorderWidth() + "; "
                                                " border-color: " + Dr::GetColor(Window_Colors::Shadow).name() + "; "
                                                " background: qlineargradient(spread:pad, x1:0 y1:0, x2:0 y2:1, "
                                                "   stop:0.00 " + Dr::GetColor(Window_Colors::Button_Light).lighter(150).name() +
                                                "   stop:0.05 " + Dr::GetColor(Window_Colors::Button_Light).lighter(150).name() +
                                                "   stop:0.10 " + Dr::GetColor(Window_Colors::Button_Light).name() +
                                                " , stop:0.90 " + Dr::GetColor(Window_Colors::Button_Light).darker(200).name() +
                                                " , stop:0.95 " + Dr::GetColor(Window_Colors::Button_Light).darker(400).name() +
                                                " , stop:1.00 " + Dr::GetColor(Window_Colors::Button_Light).darker(400).name() + "); } "
                                  " QPushButton:hover:!pressed { color: " + Dr::GetColor(Window_Colors::Highlight).name() + "; } "
                                  " QPushButton:pressed  { color: " + Dr::GetColor(Window_Colors::Text_Dark).name() + "; } "
                                  " QPushButton:disabled { color: " + Dr::GetColor(Window_Colors::Text_Dark).name() + "; } ");
    DrQPushButtonCategory *button = new DrQPushButtonCategory(name, Dr::GetColor(Window_Colors::Text),
                                                              Dr::GetColor(Window_Colors::Text_Dark), nullptr, item);
    button->setStyleSheet(buttonColor);
    button->setEnabled(false);
    this->setItemWidget(item, 0, button);                                   // Apply the button to the tree item

    QPixmap text_icon( ":/assets/tree_icons/" + icon_resource );
    text_icon = DrImaging::applySinglePixelFilter( Image_Filter_Type::Grayscale, text_icon, 0 );
    text_icon = DrImaging::applySinglePixelFilter( Image_Filter_Type::Brightness, text_icon, 200 );
    button->setIcon( text_icon );
    m_filter_hover->attachToHoverHandler(button, advisor_info);
    return button;
}












