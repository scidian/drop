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
#include <QGraphicsColorizeEffect>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>

#include "colors/colors.h"
#include "debug.h"
#include "editor/tree_assets.h"
#include "globals.h"
#include "helper.h"
#include "helper_qt.h"
#include "imaging/imaging.h"
#include "interface_editor_relay.h"
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
#include "style/style.h"
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
            m_search_bar->setAttribute(Qt::WA_MacShowFocusRect, 0);
            connect (m_search_bar, SIGNAL(textChanged(QString)), this, SLOT(searchTextChanged(QString)) );
        m_search_layout->addWidget(m_search_bar);

    parent->layout()->addWidget(m_search_widget);
}

// Removes selected key on focus loss
void TreeAssets::focusOutEvent(QFocusEvent *) {
    m_editor_relay->updateItemSelection(Editor_Widgets::Asset_Tree, { } );
}

// SLOT: Catches signals from m_filter_hover and passes to InterfaceEditorRelay
void TreeAssets::setAdvisorInfo(QString header, QString body) {
    m_editor_relay->setAdvisorInfo(header, body);
}

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
    if (list_assets.size() < 1) return;

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
        DrAssetType asset_type = item_pair.first;
        QTreeWidgetItem  *item = item_pair.second;

        this->addTopLevelItem( item );

        DrQPushButtonCategory* c;
        switch (asset_type) {
            case DrAssetType::Character: c = createCategoryButton(item, "  Characters", "tree_character.png", Advisor_Info::Asset_Character);   break;
            case DrAssetType::Object:    c = createCategoryButton(item, "  Objects",    "tree_object.png",    Advisor_Info::Asset_Object);      break;
            case DrAssetType::Effect:    c = createCategoryButton(item,  " Effects",    "tree_effects.png",   Advisor_Info::Asset_Effect);      break;
            case DrAssetType::Text:      c = createCategoryButton(item, "  Text",       "tree_text.png",      Advisor_Info::Asset_Text);        break;
        }
        category_buttons[asset_type] = c;

        // Creates a frame to hold all assets of each type
        assets_frames[asset_type] = new QFrame();
        assets_frames[asset_type]->setObjectName("assetsContainer");

        grid_layouts[asset_type] = new DrQLayoutFlow(assets_frames[asset_type], 8, 0, 4, 0, 0, 0);
    }




    // ********** Loop through each object asset and add it to the component frame
    for (auto asset_pair: list_assets) {
        DrAsset *asset = asset_pair.second;
        if (!asset) continue;

        if (asset->getComponentPropertyValue(Components::Hidden_Settings, Properties::Hidden_Hide_From_Trees).toBool()) {
            if (Dr::CheckDebugFlag(Debug_Flags::Show_Hidden_Component) == false) continue;
        }

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
                    pix = m_project->getEffect( asset->getSourceKey() )->getPixmap();
                    if (description == "") description = Advisor_Info::Asset_Effect[1];
                    break;
                case DrAssetType::Text:
                    ///pix = m_project->getDrFont( asset->getSourceKey() )->getFontPixmap();
                    pix = m_project->getFont( asset->getSourceKey() )->createText( "Aa" );
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
        category_buttons[asset->getAssetType()]->setEnabled(true);          // Category now has at least one Asset, setEnabled so it will be displayed

        rowCount++;
    }


    // ***** Create a child TreeWidgetItem attached to the TopLevel category item containing all the Assets for that category
    for (auto button_pair : category_buttons) {
        DrAssetType             asset_type = button_pair.first;
        DrQPushButtonCategory*  button     = button_pair.second;

        // If enabled, show Asset Category
        if (button->isEnabled()) {
            QTreeWidgetItem *asset_collection = new QTreeWidgetItem();
            asset_collection->setDisabled(true);
            widget_items[asset_type]->addChild(asset_collection);
            this->setItemWidget(asset_collection, 0, assets_frames[asset_type]);
        // Otherwise, hide it
        } else {
            this->takeTopLevelItem( this->indexOfTopLevelItem(widget_items[asset_type]) );
        }
    }


    // ***** Adds a spacer item at the bottom to allow for comfortable scrolling to the bottom of the tree
    QTreeWidgetItem *spacer_item  = new QTreeWidgetItem();
    QLabel *spacer_label = new QLabel();
    spacer_label->setFixedHeight(120);
    this->addTopLevelItem(spacer_item);
    this->setItemWidget(spacer_item, 0, spacer_label);

    this->expandAll();
}


//####################################################################################
//##    Create and style a buttons to be used as a header items for the categories
//####################################################################################
DrQPushButtonCategory* TreeAssets::createCategoryButton(QTreeWidgetItem *item, QString name, QString icon_resource, QList<QString> advisor_info) {
    QString icon_size =     "14px 14px";
    QString padding_left =  "10px";
    if (name.toLower().contains("effects")) {
        icon_size =     "20px 18px";
        padding_left =  "7px";
    }

    // ***** Create Category Button
    QString buttonColor = QString(" icon-size: " + icon_size + "; padding-left: " + padding_left + "; ");
    DrQPushButtonCategory *button = new DrQPushButtonCategory(name, Dr::GetColor(Window_Colors::Text),
                                                              Dr::GetColor(Window_Colors::Text_Dark), nullptr, item);
    button->setObjectName("buttonAssetCategory");
    button->setStyleSheet(buttonColor);
    button->setEnabled(false);                                              // Created as false, becomes true if we add an Asset to the category
    this->setItemWidget(item, 0, button);                                   // Apply the button to the tree item

    // ***** Grab Icon to use, colorize it to the current palette
    QPixmap text_icon( ":/assets/tree_icons/" + icon_resource );
    text_icon = QPixmap::fromImage( DrImaging::colorizeImage(text_icon.toImage(), Dr::GetColor(Window_Colors::Text)) );

    // Alternate method of colorizing
    ///QGraphicsColorizeEffect *colorize = new QGraphicsColorizeEffect();
    ///colorize->setColor( Dr::GetColor(Window_Colors::Text) );
    ///text_icon = QPixmap::fromImage( Dr::ApplyEffectToImage(text_icon.toImage(), colorize, 0) );

    QGraphicsDropShadowEffect *drop_shadow = new QGraphicsDropShadowEffect();
    drop_shadow->setColor( Dr::GetColor(Window_Colors::Shadow) );
    drop_shadow->setColor( Dr::GetColor(Window_Colors::Button_Light).darker(200) );
    drop_shadow->setOffset( -4, 4 );
    text_icon = QPixmap::fromImage( Dr::ApplyEffectToImage(text_icon.toImage(), drop_shadow, 0) );

    button->setIcon( text_icon );
    m_filter_hover->attachToHoverHandler(button, advisor_info);
    return button;
}





















