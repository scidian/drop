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
#include <QScrollBar>
#include <QVBoxLayout>

#include "core/colors/colors.h"
#include "editor/event_filters.h"
#include "editor/helper_library.h"
#include "editor/interface_editor_relay.h"
#include "editor/pixmap/pixmap.h"
#include "editor/preferences.h"
#include "editor/project/project.h"
#include "editor/style/style.h"
#include "editor/trees/tree_assets.h"
#include "engine/debug_flags.h"
#include "project/dr_project.h"
#include "project/entities/dr_animation.h"
#include "project/entities/dr_font.h"
#include "project/entities/dr_image.h"
#include "project/entities/dr_stage.h"
#include "project/entities/dr_thing.h"
#include "project/entities/dr_world.h"
#include "project/entities_physics_2d/dr_asset.h"
#include "project/entities_physics_2d/dr_device.h"
#include "project/entities_physics_2d/dr_effect.h"
#include "project/entities_physics_2d/dr_item.h"
#include "project/entities_physics_2d/dr_prefab.h"
#include "project/settings/settings.h"
#include "project/settings/settings_component.h"
#include "project/settings/settings_component_property.h"


//####################################################################################
//##    Constructor
//####################################################################################
TreeAssets::TreeAssets(QWidget *parent, DrProject *project, IEditorRelay *editor_relay)
    : QTreeWidget (parent), m_project(project), m_editor_relay(editor_relay) {

    // Initialize hover handler
    setHoverHandler( new DrFilterHoverHandler(this) );
    connect(m_filter_hover, SIGNAL(signalMouseHover(QString, QString)), this, SLOT(setAdvisorInfo(QString, QString)));

    // Connect this widget to the hover handler
    getHoverHandler()->attachToHoverHandler(this, Advisor_Info::Asset_List);

    // Connect to Expand / Collapse slots
    connect(this, SIGNAL(itemCollapsed(QTreeWidgetItem *)), this, SLOT(handleCollapsed(QTreeWidgetItem *)));
    connect(this, SIGNAL(itemExpanded(QTreeWidgetItem *)),  this, SLOT(handleExpanded(QTreeWidgetItem *)));

    // Build search bar
    m_search_widget = new QWidget(parent);
    m_search_widget->setFixedHeight(30);
    m_search_widget->setObjectName(QStringLiteral("assetSearchWidget"));
    getHoverHandler()->attachToHoverHandler(m_search_widget, Advisor_Info::Asset_Search);
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

DrFilterHoverHandler* TreeAssets::getHoverHandler() {
    if (m_filter_hover == nullptr)
        Dr::ShowErrorMessage("TreeAssets::getHoverHandler", "Could not get DrFilterHoverHandler pointer in TreeAssets, not initialized or corrupt!");
    return m_filter_hover;
}

// Focus In Event
void TreeAssets::focusInEvent(QFocusEvent *) {
    m_editor_relay->setActiveWidget(Editor_Widgets::Asset_Tree);
}

// Removes selected key on focus loss
void TreeAssets::focusOutEvent(QFocusEvent *) {
    m_editor_relay->updateItemSelection(Editor_Widgets::Asset_Tree, { });
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

    // Store current scroll bar position
    int scroll_position = this->verticalScrollBar()->value();

    // ***** Initialize some QWidget helper items
    QSizePolicy sp_left(QSizePolicy::Preferred, QSizePolicy::Preferred);
    QSizePolicy sp_right(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sp_left.setHorizontalStretch( c_asset_size_left);
    sp_right.setHorizontalStretch(c_asset_size_right);

    QFont font = Dr::CustomFont();

    // ***** Retrieve list of assets for project
    AssetMap    &list_assets =  getParentProject()->getAssetMap();
    DeviceMap   &list_devices = getParentProject()->getDeviceMap();
    EffectMap   &list_effects = getParentProject()->getEffectMap();
    FontMap     &list_fonts =   getParentProject()->getFontMap();
    ImageMap    &list_images =  getParentProject()->getImageMap();
    ItemMap     &list_items =   getParentProject()->getItemMap();
    PrefabMap   &list_prefabs = getParentProject()->getPrefabMap();
    int rowCount = 0;
    this->clear();
    m_asset_frames.clear();

    // ***** Create new items in list to hold asset categories
    std::map <Asset_Category, QTreeWidgetItem*>       widget_items;
    std::map <Asset_Category, AssetCategoryButton*>   category_buttons;
    std::map <Asset_Category, QFrame*>                assets_frames;
    ///std::map <Asset_Categories, AssetFlowLayout*>  grid_layouts;
                                                    m_grid_layouts.clear();

    // Create and add Categories, !!!!! #NOTE: This code sets the Order of the Categories as well
    std::vector<std::pair <Asset_Category, QTreeWidgetItem*>> asset_categories;
    QList<DrType> show_types = getShowTypes();
    if (show_types.contains(DrType::Asset))  {
        widget_items[Asset_Category::Character] =   new QTreeWidgetItem();
        widget_items[Asset_Category::Object] =      new QTreeWidgetItem();
        asset_categories.push_back( std::make_pair(Asset_Category::Character,       widget_items[Asset_Category::Character]) );
        asset_categories.push_back( std::make_pair(Asset_Category::Object,          widget_items[Asset_Category::Object]) );
    }
    if (show_types.contains(DrType::Device)) {
        widget_items[Asset_Category::Device] =      new QTreeWidgetItem();
        asset_categories.push_back( std::make_pair(Asset_Category::Device,          widget_items[Asset_Category::Device]) );
    }
    if (show_types.contains(DrType::Effect)) {
        widget_items[Asset_Category::Effect] =      new QTreeWidgetItem();
        asset_categories.push_back( std::make_pair(Asset_Category::Effect,          widget_items[Asset_Category::Effect]) );
    }
    if (show_types.contains(DrType::Item)) {
        widget_items[Asset_Category::Item] =        new QTreeWidgetItem();
        asset_categories.push_back( std::make_pair(Asset_Category::Item,            widget_items[Asset_Category::Item]) );
    }
    if (show_types.contains(DrType::Prefab)) {
        widget_items[Asset_Category::Prefab] =      new QTreeWidgetItem();
        asset_categories.push_back( std::make_pair(Asset_Category::Prefab,          widget_items[Asset_Category::Prefab]) );
    }
    if (show_types.contains(DrType::Font)) {
        widget_items[Asset_Category::Text] =        new QTreeWidgetItem();
        asset_categories.push_back( std::make_pair(Asset_Category::Text,            widget_items[Asset_Category::Text]) );
    }
    if (show_types.contains(DrType::Image)) {
        widget_items[Asset_Category::Image] =       new QTreeWidgetItem();
        widget_items[Asset_Category::Basic] =       new QTreeWidgetItem();
        widget_items[Asset_Category::Outline] =     new QTreeWidgetItem();
        widget_items[Asset_Category::Gradient] =    new QTreeWidgetItem();
        widget_items[Asset_Category::Decoration] =  new QTreeWidgetItem();
        widget_items[Asset_Category::Ground] =      new QTreeWidgetItem();
        widget_items[Asset_Category::Polygon] =     new QTreeWidgetItem();
        widget_items[Asset_Category::Shape] =       new QTreeWidgetItem();
        widget_items[Asset_Category::Isometric] =   new QTreeWidgetItem();
        asset_categories.push_back( std::make_pair(Asset_Category::Image,           widget_items[Asset_Category::Image]) );
        asset_categories.push_back( std::make_pair(Asset_Category::Basic,           widget_items[Asset_Category::Basic]) );
        asset_categories.push_back( std::make_pair(Asset_Category::Outline,         widget_items[Asset_Category::Outline]) );
        asset_categories.push_back( std::make_pair(Asset_Category::Gradient,        widget_items[Asset_Category::Gradient]) );
        asset_categories.push_back( std::make_pair(Asset_Category::Decoration,      widget_items[Asset_Category::Decoration]) );
        asset_categories.push_back( std::make_pair(Asset_Category::Ground,          widget_items[Asset_Category::Ground]) );
        asset_categories.push_back( std::make_pair(Asset_Category::Polygon,         widget_items[Asset_Category::Polygon]) );
        asset_categories.push_back( std::make_pair(Asset_Category::Shape,           widget_items[Asset_Category::Shape]) );
        asset_categories.push_back( std::make_pair(Asset_Category::Isometric,       widget_items[Asset_Category::Isometric]) );
    }


    // Create Asset Categories
    for (auto item_pair : asset_categories) {
        Asset_Category   asset_type = item_pair.first;
        QTreeWidgetItem *item =       item_pair.second;
        item->setData(0, User_Roles::Type, QVariant::fromValue(static_cast<long>(asset_type)));

        // ***** Add Item to Tree
        this->addTopLevelItem( item );

        // Create Category Button
        category_buttons[asset_type] = createCategoryButton(item, asset_type);

        // Creates a frame to hold all assets of each type
        assets_frames[asset_type] = new QFrame();
        assets_frames[asset_type]->setObjectName("assetsContainer");

        m_grid_layouts[asset_type] = new AssetFlowLayout(getParentProject(), assets_frames[asset_type], 8, 0, 4, 0, 0, 0);
    }



    // ***** Get Entities to show, sort by listOrder
    std::vector<DrSettings*> entities { };
    if (show_types.contains(DrType::Asset))  { for (auto asset_pair :  list_assets)  { if (asset_pair.first > c_no_key)  entities.push_back(asset_pair.second); } }
    if (show_types.contains(DrType::Device)) { for (auto device_pair : list_devices) { if (device_pair.first > c_no_key) entities.push_back(device_pair.second); } }
    if (show_types.contains(DrType::Effect)) { for (auto effect_pair : list_effects) { if (effect_pair.first > c_no_key) entities.push_back(effect_pair.second); } }
    if (show_types.contains(DrType::Font))   { for (auto font_pair :   list_fonts)   { if (font_pair.first > c_no_key)   entities.push_back(font_pair.second); } }
    if (show_types.contains(DrType::Image))  { for (auto image_pair :  list_images)  { if (image_pair.first > c_no_key)  entities.push_back(image_pair.second); } }
    if (show_types.contains(DrType::Item))   { for (auto item_pair :   list_items)   { if (item_pair.first > c_no_key)   entities.push_back(item_pair.second); } }
    if (show_types.contains(DrType::Prefab)) { for (auto prefab_pair : list_prefabs) { if (prefab_pair.first > c_no_key) entities.push_back(prefab_pair.second); } }

    std::sort(entities.begin(), entities.end(), [](DrSettings *a, DrSettings *b) {
        return QString::fromStdString(a->getName()).toLower() < QString::fromStdString(b->getName()).toLower();
    });


    // ********** Loop through each object asset and add it to the component frame
    for (auto entity: entities) {
        if (entity == nullptr) continue;

        if (entity->getComponentPropertyValue(Comps::Hidden_Settings, Props::Hidden_Hide_From_Trees).toBool()) {
            if (Dr::CheckDebugFlag(Debug_Flags::Show_Hidden_Component) == false) continue;
        }

        QString entity_name = QString::fromStdString(entity->getName());
        if (!entity_name.toLower().contains(search_text.toLower())) continue;

        QSize frame_size = QSize(100, 66);
        QRect name_rect =  QRect(10, 0, frame_size.width() - 20, 25);
        QSize pix_size =   QSize(frame_size.width() - 25, frame_size.height() - 30);

        // ***** Store current asset key in widget and install a mouse handler event filter on the item, DrFilterAssetMouseHandler
        QFrame *single_asset = new QFrame();
        single_asset->setObjectName("assetFrame");
        single_asset->setProperty(User_Property::Key,        QVariant::fromValue( entity->getKey() ));
        single_asset->setProperty(User_Property::Mouse_Down, false);
        single_asset->installEventFilter(new DrFilterAssetMouseHandler(single_asset, m_editor_relay));
        single_asset->setFixedSize(frame_size);

        // Store pointer to frame in a list for future reference
        m_asset_frames.append(single_asset);


        // ***** Create the label that will display the asset name
        QFrame *text_holder = new QFrame(single_asset);
        text_holder->setGeometry(name_rect);
        text_holder->setObjectName(QStringLiteral("textHolder"));
            QLabel *asset_text = new QLabel(entity_name, text_holder);
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
            QString description;
            std::string hidden_txt = entity->getComponentProperty(Comps::Hidden_Settings, Props::Hidden_Advisor_Description)->getValue().toString();

            if (entity->getType() == DrType::Asset) {
                DrAsset *asset = dynamic_cast<DrAsset*>(entity);
                switch (asset->getAssetType()) {
                    case DrAssetType::Character: {
                        long animation_key = asset->getComponentPropertyValue(Comps::Asset_Animation, Props::Asset_Animation_Idle).toLong();
                        DrAnimation *ani = getParentProject()->findAnimationFromKey(animation_key);
                        if (ani != nullptr) pix = Dr::ToQPixmap(ani->getFirstFrameImage()->getBitmap());
                        description = "<b>ID Key: " + QString::number(entity->getKey()) + "</b><br>" + Advisor_Info::Asset_Character[1];
                        break;
                    }
                    case DrAssetType::Object: {
                        long animation_key = asset->getComponentPropertyValue(Comps::Asset_Animation, Props::Asset_Animation_Idle).toLong();
                        DrAnimation *ani = getParentProject()->findAnimationFromKey(animation_key);
                        if (ani != nullptr) pix = Dr::ToQPixmap(ani->getFirstFrameImage()->getBitmap());
                        description = "<b>ID Key: " + QString::number(entity->getKey()) + "</b><br>" + Advisor_Info::Asset_Object[1];
                        break;
                    }
                }

            } else if (entity->getType() == DrType::Device) {
                DrDevice *device = dynamic_cast<DrDevice*>(entity);
                pix = Dr::GetAssetPixmapDevice( device->getDeviceType() );
                description = "<b>ID Key: " + QString::number(entity->getKey()) + "</b><br>" + Advisor_Info::Asset_Device[1];

            } else if (entity->getType() == DrType::Effect) {
                DrEffect *effect = dynamic_cast<DrEffect*>(entity);
                pix = Dr::GetAssetPixmapEffect( effect->getEffectType() );
                description = "<b>ID Key: " + QString::number(entity->getKey()) + "</b><br>" + Advisor_Info::Asset_Effect[1];

            } else if (entity->getType() == DrType::Font) {
                DrFont *font = dynamic_cast<DrFont*>(entity);
                pix = Dr::CreateText(font, "Aa" );
                description = "<b>ID Key: " + QString::number(entity->getKey()) + "</b><br>" + Advisor_Info::Asset_Text[1];

            } else if (entity->getType() == DrType::Image) {
                DrImage *image = dynamic_cast<DrImage*>(entity);
                pix = Dr::ToQPixmap(image->getBitmap());
                if (entity->getKey() < c_starting_key_entity) {
                    description = "<b>ID Key: " + QString::number(entity->getKey()) + "</b><br>" + Advisor_Info::Asset_Image_Built_In[1];
                } else {
                    description = "<b>ID Key: " + QString::number(entity->getKey()) + "</b><br>" + Advisor_Info::Asset_Image[1];
                }

            } else if (entity->getType() == DrType::Item) {
                DrItem *item = dynamic_cast<DrItem*>(entity);
                pix = Dr::GetAssetPixmapItem( item->getItemType() );
                description = "<b>ID Key: " + QString::number(entity->getKey()) + "</b><br>" + Advisor_Info::Asset_Item[1];

            } else if (entity->getType() == DrType::Prefab) {
                DrPrefab *prefab = dynamic_cast<DrPrefab*>(entity);
                pix = Dr::GetAssetPixmapPrefab( prefab->getPrefabType() );
                description = "<b>ID Key: " + QString::number(entity->getKey()) + "</b><br>" + Advisor_Info::Asset_Prefab[1];

            }
            description += "<br>" + QString::fromStdString(hidden_txt);
            getHoverHandler()->attachToHoverHandler(single_asset, entity_name, description);

            QLabel *asset_pix = new QLabel(single_asset);
            asset_pix->setObjectName(QStringLiteral("assetPixmap"));
            asset_pix->setFont(font);
            asset_pix->setSizePolicy(sp_right);
            asset_pix->setAlignment(Qt::AlignmentFlag::AlignCenter);
            vertical_split->addWidget( asset_pix );

            // Draw pixmap onto label
            if (pix.isNull())
                asset_pix->setPixmap(pix);
            else
                asset_pix->setPixmap(pix.scaled(pix_size, Qt::KeepAspectRatio, Qt::SmoothTransformation));


        // ***** Add widget to proper category
        Asset_Category asset_type;

        if (entity->getType() == DrType::Asset) {
            DrAsset *asset = dynamic_cast<DrAsset*>(entity);
            if (asset->getAssetType() == DrAssetType::Object)           asset_type = Asset_Category::Object;
            else if (asset->getAssetType() == DrAssetType::Character)   asset_type = Asset_Category::Character;
        } else if (entity->getType() == DrType::Device) {               asset_type = Asset_Category::Device;
        } else if (entity->getType() == DrType::Effect) {               asset_type = Asset_Category::Effect;
        } else if (entity->getType() == DrType::Font) {                 asset_type = Asset_Category::Text;
        } else if (entity->getType() == DrType::Image) {
            DrImage *image = dynamic_cast<DrImage*>(entity);            asset_type = image->getAssetCategory();
        } else if (entity->getType() == DrType::Item) {                 asset_type = Asset_Category::Item;
        } else if (entity->getType() == DrType::Prefab) {               asset_type = Asset_Category::Prefab;
        } else { continue; }

        m_grid_layouts[asset_type]->addWidget(single_asset);            // Category now has at least one Asset, setEnabled so it will be displayed
        category_buttons[asset_type]->setEnabled(true);

        rowCount++;
    }


    // ***** Create a child TreeWidgetItem attached to the TopLevel category item containing all the Assets for that category
    for (auto button_pair : category_buttons) {
        Asset_Category       asset_type = button_pair.first;
        AssetCategoryButton *button     = button_pair.second;

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

    // ***** Expand / collapse top level items based on last known setting
    expandCollapseComponents();

    // ***** Scroll back to previous position
    if (scroll_position > 0) {
        if (scroll_position > this->verticalScrollBar()->maximum()) scroll_position = this->verticalScrollBar()->maximum();
        this->verticalScrollBar()->setValue(scroll_position);
        QTimer::singleShot(0, this, SLOT(ensureSelectedKeyVisible()));
    }
}


//####################################################################################
//##    Returns List of Top Level (Component) Items
//####################################################################################
QList<QTreeWidgetItem*> TreeAssets::getListOfTopLevelItems() {
    QTreeWidgetItem        *item = this->invisibleRootItem();
    QList<QTreeWidgetItem*> items;
    for (int i = 0; i < item->childCount(); ++i) {
        items.append( item->child(i) );
    }
    return items;
}


//####################################################################################
//##    Create and style a buttons to be used as a header items for the categories
//####################################################################################
AssetCategoryButton* TreeAssets::createCategoryButton(QTreeWidgetItem *item, Asset_Category asset_type) {
    QString name, icon;
    std::vector<QString> info;

    switch (asset_type) {
        case Asset_Category::Character:     name = "Characters";    icon = "comp_character.png";    info = Advisor_Info::Asset_Character;       break;
        case Asset_Category::Object:        name = "Objects";       icon = "comp_object.png";       info = Advisor_Info::Asset_Object;          break;
        case Asset_Category::Device:        name = "Devices";       icon = "comp_camera.png";       info = Advisor_Info::Asset_Device;          break;
        case Asset_Category::Effect:        name = "Effects";       icon = "comp_effects.png";      info = Advisor_Info::Asset_Effect;          break;
        case Asset_Category::Item:          name = "Items";         icon = "comp_item.png";         info = Advisor_Info::Asset_Item;            break;
        case Asset_Category::Prefab:        name = "Prefabs";       icon = "comp_foliage.png";      info = Advisor_Info::Asset_Prefab;          break;
        case Asset_Category::Text:          name = "Text";          icon = "comp_font.png";         info = Advisor_Info::Asset_Text;            break;

        case Asset_Category::Image:         name = "Images";        icon = "comp_images.png";               info = Advisor_Info::Asset_Image;           break;
        case Asset_Category::Basic:         name = "Basic";         icon = "comp_images_basic.png";         info = Advisor_Info::Asset_Image_Built_In;  break;
        case Asset_Category::Outline:       name = "Outlines";      icon = "comp_images_outline.png";       info = Advisor_Info::Asset_Image_Built_In;  break;
        case Asset_Category::Gradient:      name = "Gradient";      icon = "comp_images_gradient.png";      info = Advisor_Info::Asset_Image_Built_In;  break;
        case Asset_Category::Decoration:    name = "Decoration";    icon = "comp_images_decoration.png";    info = Advisor_Info::Asset_Image_Built_In;  break;
        case Asset_Category::Ground:        name = "Ground";        icon = "comp_images_ground.png";        info = Advisor_Info::Asset_Image_Built_In;  break;
        case Asset_Category::Polygon:       name = "Polygons";      icon = "comp_images_polygon.png";       info = Advisor_Info::Asset_Image_Built_In;  break;
        case Asset_Category::Shape:         name = "Shapes";        icon = "comp_images_shapes.png";        info = Advisor_Info::Asset_Image_Built_In;  break;
        case Asset_Category::Isometric:     name = "Isometric";     icon = "comp_object.png";               info = Advisor_Info::Asset_Image_Built_In;  break;
    }
    name = "  " + tr(name.toLatin1());                                      // Add spacing and translate

    QString icon_size =     "20px 18px";
    QString padding_left =  "7px";

    // ***** Create Category Button
    QString buttonColor = QString(" icon-size: " + icon_size + "; padding-left: " + padding_left + "; ");
    AssetCategoryButton *button = new AssetCategoryButton(name, Dr::ToQColor(Dr::GetColor(Window_Colors::Text)),
                                                                Dr::ToQColor(Dr::GetColor(Window_Colors::Text_Dark)), nullptr, item);
    button->setObjectName("buttonAssetCategory");
    button->setStyleSheet(buttonColor);
    button->setEnabled(false);                                              // Created as false, becomes true if we add an Asset to the category
    this->setItemWidget(item, 0, button);                                   // Apply the button to the tree item

    // ***** Grab Icon to use, colorize it to the current palette
    QPixmap text_icon( ":/assets/inspector_icons/" + icon );
    text_icon = QPixmap::fromImage( Dr::ColorizeImage(text_icon.toImage(), Dr::ToQColor(Dr::GetColor(Window_Colors::Text))) );

    // Alternate method of colorizing
    ///QGraphicsColorizeEffect *colorize = new QGraphicsColorizeEffect();
    ///colorize->setColor( Dr::GetColor(Window_Colors::Text) );
    ///text_icon = QPixmap::fromImage( Dr::ApplyEffectToImage(text_icon.toImage(), colorize, 0) );

    QGraphicsDropShadowEffect *drop_shadow = new QGraphicsDropShadowEffect();
    drop_shadow->setColor( Dr::ToQColor(Dr::GetColor(Window_Colors::Shadow)) );
    drop_shadow->setColor( Dr::ToQColor(Dr::GetColor(Window_Colors::Button_Light)).darker(200) );
    drop_shadow->setOffset( -4, 4 );
    text_icon = QPixmap::fromImage( Dr::ApplyEffectToImage(text_icon.toImage(), drop_shadow, 0) );

    button->setIcon( text_icon );
    getHoverHandler()->attachToHoverHandler(button, info);
    return button;
}






















