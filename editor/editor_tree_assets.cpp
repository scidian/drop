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
#include <QLineEdit>
#include <QVBoxLayout>

#include "colors/colors.h"
#include "debug.h"
#include "editor_tree_assets.h"
#include "globals.h"
#include "image_filter_color.h"
#include "interface_editor_relay.h"
#include "library.h"
#include "project/project.h"
#include "project/project_asset.h"
#include "project/project_font.h"
#include "project/project_world.h"
#include "project/project_world_stage.h"
#include "project/project_world_stage_object.h"
#include "settings/settings.h"
#include "settings/settings_component.h"
#include "settings/settings_component_property.h"
#include "widgets/widgets.h"
#include "widgets/widgets_event_filters.h"
#include "widgets/widgets_layout.h"


//####################################################################################
//##        Constructor
//####################################################################################
TreeAssets::TreeAssets(QWidget *parent, DrProject *project, IEditorRelay *editor_relay) :
                       QTreeWidget (parent), m_project(project), m_editor_relay(editor_relay)
{
    // Initialize hover handler
    m_widget_hover = new WidgetHoverHandler(this);
    connect(m_widget_hover, SIGNAL(signalMouseHover(QString, QString)), this, SLOT(setAdvisorInfo(QString, QString)));

    // Connect this widget to the hover handler
    m_widget_hover->attachToHoverHandler(this, Advisor_Info::Asset_List);

    // Build search bar
    m_search_widget = new QWidget(parent);
    m_search_widget->setFixedHeight(30);
    m_search_widget->setObjectName(QStringLiteral("assetSearchWidget"));
    m_widget_hover->attachToHoverHandler(m_search_widget, Advisor_Info::Asset_Search);
    m_search_widget->setToolTip( Advisor_Info::Asset_Search[0] );
        m_search_layout = new QVBoxLayout(m_search_widget);
        m_search_layout->setMargin(4);
            m_search_bar = new QLineEdit();
            m_search_bar->setObjectName("assetSearchBar");
            m_search_bar->setPlaceholderText("Search");
                QFont font;
                font.setPointSize(Dr::FontSize());
            m_search_bar->setFont(font);
            connect (m_search_bar, SIGNAL(textChanged(QString)), this, SLOT(searchTextChanged(QString)) );
        m_search_layout->addWidget(m_search_bar);

    parent->layout()->addWidget(m_search_widget);
}

// SLOT: Catches signals from m_widget_hover and passes to InterfaceEditorRelay
void TreeAssets::setAdvisorInfo(QString header, QString body) { m_editor_relay->setAdvisorInfo(header, body); }

// SLOT: Catches signal textChanged() from m_search_bar QLineEdit search bar
void TreeAssets::searchTextChanged(QString new_text)
{
    buildAssetTree(new_text);
}



//####################################################################################
//##        Tree Building Functions
//####################################################################################
void TreeAssets::buildAssetTree(QString search_text)
{
    // ***** Initialize some QWidget helper items
    QSizePolicy sp_left(QSizePolicy::Preferred, QSizePolicy::Preferred);
    QSizePolicy sp_right(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sp_left.setHorizontalStretch(c_asset_size_left);
    sp_right.setHorizontalStretch(c_asset_size_right);

    QFont font;
    font.setPointSize(Dr::FontSize());


    // ***** Retrieve list of assets for project
    AssetMap &list_assets = m_project->getAssetMap();
    int rowCount = 0;
    this->clear();
    m_asset_frames.clear();


    // ***** Create new items in list to hold asset categories
    QTreeWidgetItem *character_item =   new QTreeWidgetItem();
    QTreeWidgetItem *objects_item =     new QTreeWidgetItem();
    QTreeWidgetItem *text_item =        new QTreeWidgetItem();
    this->addTopLevelItem(character_item);
    this->addTopLevelItem(objects_item);
    this->addTopLevelItem(text_item);
    CategoryButton *character_button = initializeCatergoryButton(character_item, "  Characters");
    CategoryButton *objects_button =   initializeCatergoryButton(objects_item,   "  Objects");
    CategoryButton *text_button =      initializeCatergoryButton(text_item,      "  Text");

    QPixmap char_icon( ":/assets/tree_icons/tree_character.png" );
    char_icon = DrFilter::changeToGrayscale(char_icon);
    char_icon = DrFilter::changeBrightness(char_icon, 200);
    character_button->setIcon( char_icon );
    QPixmap object_icon( ":/assets/tree_icons/tree_object.png" );
    object_icon = DrFilter::changeToGrayscale(object_icon);
    object_icon = DrFilter::changeBrightness(object_icon, 200);
    objects_button->setIcon( object_icon );
    QPixmap text_icon( ":/assets/tree_icons/tree_text.png" );
    text_icon = DrFilter::changeToGrayscale(text_icon);
    text_icon = DrFilter::changeBrightness(text_icon, 200);
    text_button->setIcon( text_icon );

    m_widget_hover->attachToHoverHandler(character_button, Advisor_Info::Asset_Character);
    m_widget_hover->attachToHoverHandler(objects_button,   Advisor_Info::Asset_Object);
    m_widget_hover->attachToHoverHandler(text_button,      Advisor_Info::Asset_Text);


    // ***** Creates a frame sto hold all assets of each type
    QFrame      *assets_frame_characters = new QFrame();
    QFrame      *assets_frame_objects =    new QFrame();
    QFrame      *assets_frame_text =       new QFrame();
    FlowLayout  *grid_layout_characters =  new FlowLayout(assets_frame_characters, 8, 0, 4, 0, 0, 0);
    FlowLayout  *grid_layout_objects =     new FlowLayout(assets_frame_objects,    8, 0, 4, 0, 0, 0);
    FlowLayout  *grid_layout_text =        new FlowLayout(assets_frame_text,       8, 0, 4, 0, 0, 0);
    assets_frame_characters->setObjectName("assetsContainer");
    assets_frame_objects->setObjectName(   "assetsContainer");
    assets_frame_text->setObjectName(      "assetsContainer");


    // ********** Loop through each object asset and add it to the component frame
    bool skipped_any = false;

    for (auto asset_pair: list_assets) {
        QString asset_name = asset_pair.second->getName();
        if (!asset_name.toLower().contains(search_text.toLower())) {
            skipped_any = true;
            continue;
        }

        QSize frame_size = QSize(100, 66);
        QRect name_rect =  QRect(10, 0, frame_size.width() - 20, 25);
        QSize pix_size =   QSize(frame_size.width() - 25, frame_size.height() - 30);

        // ***** Store current asset key in widget and install a mouse handler event filter on the item, AssetMouseHandler
        QFrame *single_asset = new QFrame();
        single_asset->setObjectName("assetFrame");
        single_asset->setProperty(User_Property::Key,        QVariant::fromValue( asset_pair.second->getKey() ));
        single_asset->setProperty(User_Property::Mouse_Down, false);
        single_asset->installEventFilter(new AssetMouseHandler(single_asset, m_editor_relay));
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
            switch (asset_pair.second->getAssetType()) {
            case DrAssetType::Object:
                pix = asset_pair.second->getComponentProperty(Components::Asset_Animation, Properties::Asset_Animation_Default)->getValue().value<QPixmap>();
                m_widget_hover->attachToHoverHandler(single_asset, asset_name, Advisor_Info::Asset_Object[1] );
                break;
            case DrAssetType::Text:
                ///pix = m_project->getDrFont( asset_pair.second->getSourceKey() )->getFontPixmap();
                pix = m_project->getDrFont( asset_pair.second->getSourceKey() )->createText( "Aa" );
                m_widget_hover->attachToHoverHandler(single_asset, asset_name, Advisor_Info::Asset_Text[1] );
                break;
            case DrAssetType::Character:
                pix = asset_pair.second->getComponentProperty(Components::Asset_Animation, Properties::Asset_Animation_Default)->getValue().value<QPixmap>();
                m_widget_hover->attachToHoverHandler(single_asset, asset_name, Advisor_Info::Asset_Character[1] );
                break;
            }

            QLabel *asset_pix = new QLabel(single_asset);
            asset_pix->setObjectName(QStringLiteral("assetPixmap"));
            asset_pix->setFont(font);
            asset_pix->setSizePolicy(sp_right);
            asset_pix->setAlignment(Qt::AlignmentFlag::AlignCenter);
            vertical_split->addWidget( asset_pix );

            // Draw pixmap onto label
            asset_pix->setPixmap(pix.scaled(pix_size, Qt::KeepAspectRatio, Qt::SmoothTransformation));


        // ***** Add widget to proper category
        switch (asset_pair.second->getAssetType()) {
        case DrAssetType::Object:
            grid_layout_objects->addWidget(single_asset);
            objects_button->setEnabled(true);
            break;
        case DrAssetType::Text:
            grid_layout_text->addWidget(single_asset);
            text_button->setEnabled(true);
            break;
        case DrAssetType::Character:
            grid_layout_characters->addWidget(single_asset);
            character_button->setEnabled(true);
            break;
        }

        rowCount++;
    }


    // ***** Create a child TreeWidgetItem attached to the TopLevel category item
    if (character_button->isEnabled()) addAssetsToCategory(character_item, assets_frame_characters);
    if (objects_button->isEnabled())   addAssetsToCategory(objects_item,   assets_frame_objects);
    if (text_button->isEnabled())      addAssetsToCategory(text_item,      assets_frame_text);

    if (skipped_any) {
        if (!character_button->isEnabled()) this->takeTopLevelItem( this->indexOfTopLevelItem(character_item) );
        if (!objects_button->isEnabled())   this->takeTopLevelItem( this->indexOfTopLevelItem(objects_item) );
        if (!text_button->isEnabled())      this->takeTopLevelItem( this->indexOfTopLevelItem(text_item) );
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
void TreeAssets::addAssetsToCategory(QTreeWidgetItem *tree_item, QFrame *asset_frame)
{
    QTreeWidgetItem *asset_collection = new QTreeWidgetItem();
    asset_collection->setDisabled(true);
    tree_item->addChild(asset_collection);
    this->setItemWidget(asset_collection, 0, asset_frame);
}

// Create and style a buttons to be used as a header items for the categories
CategoryButton* TreeAssets::initializeCatergoryButton(QTreeWidgetItem *tree_item, QString name)
{
    QString buttonColor = QString(" QPushButton { height: 22px; font: 13px; text-align: left; icon-size: 14px 14px; "
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
    CategoryButton *button = new CategoryButton(name,
                                                Dr::GetColor(Window_Colors::Text),
                                                Dr::GetColor(Window_Colors::Text_Dark),
                                                nullptr, tree_item);
    button->setStyleSheet(buttonColor);
    button->setEnabled(false);
    this->setItemWidget(tree_item, 0, button);                             // Apply the button to the tree item
    return button;
}













