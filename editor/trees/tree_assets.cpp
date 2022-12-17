//
//      Created by Stephens Nunnally on 1/3/2019, (c) 2019 Scidian Studios, All Rights Reserved
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
#include "editor/event_filters/event_filters.h"
#include "editor/form_main/form_main.h"
#include "editor/helper_library.h"
#include "editor/interface_editor_relay.h"
#include "editor/pixmap/pixmap.h"
#include "editor/preferences.h"
#include "editor/project/project.h"
#include "editor/style/style.h"
#include "editor/trees/tree_assets.h"
#include "editor/widgets/widgets_trees.h"
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
#include "project/entities_sound/dr_mix.h"
#include "project/entities_sound/dr_sound.h"
#include "project/entities_sound/dr_track.h"
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
    m_search_widget->setObjectName(QStringLiteral("assetSearchWidget"));
    getHoverHandler()->attachToHoverHandler(m_search_widget, Advisor_Info::Asset_Search);
    m_search_widget->setToolTip( Advisor_Info::Asset_Search[0] );
        m_search_layout = new QVBoxLayout(m_search_widget);
        m_search_layout->setContentsMargins(4, 4, 4, 4);
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

    // ***** Store current scroll bar position
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
    MixMap      &list_mixes =   getParentProject()->getMixMap();
    PrefabMap   &list_prefabs = getParentProject()->getPrefabMap();
    SoundMap    &list_sounds =  getParentProject()->getSoundMap();
    int rowCount = 0;
    this->clear();
    m_asset_frames.clear();

    // ***** Create new items in list to hold asset categories
    std::map<std::string, QTreeWidgetItem*>         widget_items;
    std::map<std::string, TreeCategoryButton*>      category_buttons;
    std::map<std::string, QFrame*>                  assets_frames;
    ///std::map<Asset_Categories, AssetFlowLayout*> grid_layouts;
                                                    m_grid_layouts.clear();

    // ********** Create and add Categories, !!!!! #NOTE: This code sets the Order of the Categories as well
    std::vector<std::pair<std::string, QTreeWidgetItem*>> asset_categories;
    QList<DrType>   show_types = getShowTypes();
    std::set<long>  image_keys_used;
    if (show_types.contains(DrType::Asset))  {
        widget_items[Asset_Category::Characters] =      new QTreeWidgetItem();
        widget_items[Asset_Category::Objects] =         new QTreeWidgetItem();
        asset_categories.push_back( std::make_pair(Asset_Category::Characters,      widget_items[Asset_Category::Characters]) );
        asset_categories.push_back( std::make_pair(Asset_Category::Objects,         widget_items[Asset_Category::Objects]) );
    }
    if (show_types.contains(DrType::Device)) {
        widget_items[Asset_Category::Devices] =         new QTreeWidgetItem();
        asset_categories.push_back( std::make_pair(Asset_Category::Devices,         widget_items[Asset_Category::Devices]) );
    }
    if (show_types.contains(DrType::Effect)) {
        widget_items[Asset_Category::Effects] =         new QTreeWidgetItem();
        asset_categories.push_back( std::make_pair(Asset_Category::Effects,         widget_items[Asset_Category::Effects]) );
    }
    if (show_types.contains(DrType::Font)) {
        widget_items[Asset_Category::Text] =            new QTreeWidgetItem();
        asset_categories.push_back( std::make_pair(Asset_Category::Text,            widget_items[Asset_Category::Text]) );
    }
    if (show_types.contains(DrType::Item)) {
        widget_items[Asset_Category::Items] =           new QTreeWidgetItem();
        asset_categories.push_back( std::make_pair(Asset_Category::Items,           widget_items[Asset_Category::Items]) );
    }
    if (show_types.contains(DrType::Mix)) {
        widget_items[Asset_Category::Mixes] =           new QTreeWidgetItem();
        asset_categories.push_back( std::make_pair(Asset_Category::Mixes,           widget_items[Asset_Category::Mixes]) );
    }
    if (show_types.contains(DrType::Prefab)) {
        widget_items[Asset_Category::Prefabs] =         new QTreeWidgetItem();
        asset_categories.push_back( std::make_pair(Asset_Category::Prefabs,         widget_items[Asset_Category::Prefabs]) );
    }
    if (show_types.contains(DrType::Sound)) {
        widget_items[Asset_Category::Audio_Files] =     new QTreeWidgetItem();
        asset_categories.push_back( std::make_pair(Asset_Category::Audio_Files,     widget_items[Asset_Category::Audio_Files]) );
        widget_items[Asset_Category::Instruments] =     new QTreeWidgetItem();
        asset_categories.push_back( std::make_pair(Asset_Category::Instruments,     widget_items[Asset_Category::Instruments]) );
        widget_items[Asset_Category::White_Noise] =     new QTreeWidgetItem();
        asset_categories.push_back( std::make_pair(Asset_Category::White_Noise,     widget_items[Asset_Category::White_Noise]) );
        widget_items[Asset_Category::Sound_Effects] =   new QTreeWidgetItem();
        asset_categories.push_back( std::make_pair(Asset_Category::Sound_Effects,   widget_items[Asset_Category::Sound_Effects]) );
        widget_items[Asset_Category::Speech] =          new QTreeWidgetItem();
        asset_categories.push_back( std::make_pair(Asset_Category::Speech,          widget_items[Asset_Category::Speech]) );
    }

    DrProject *external_images = nullptr;
    std::set<std::string> image_categories;
    if (show_types.contains(DrType::Image)) {
        widget_items[Asset_Category::Images] =          new QTreeWidgetItem();
        asset_categories.push_back( std::make_pair(Asset_Category::Images,          widget_items[Asset_Category::Images]) );
        image_keys_used = getParentProject()->getImageKeysUsedByProject();

        // Find out which image categories to create for external images
        if (Dr::GetActiveFormMain() != nullptr) external_images = Dr::GetActiveFormMain()->getExternalImageProject();
        if (external_images != nullptr) {
            for (auto &image_pair : external_images->getImageMap()) {
                if (image_pair.second->getFolderName() != "") {
                    image_categories.insert(image_pair.second->getFolderName());
                }
            }
        }
        for (auto image_category : image_categories) {
            widget_items[image_category] =              new QTreeWidgetItem();
            asset_categories.push_back( std::make_pair(image_category,              widget_items[image_category]) );
        }
    }


    // ********** Create Asset Categories
    for (auto item_pair : asset_categories) {
        std::string      category_name = item_pair.first;
        QTreeWidgetItem *item =          item_pair.second;
        item->setData(0, User_Roles::Category_Name, QString::fromStdString(category_name));

        // ***** Add Item to Tree
        this->addTopLevelItem( item );

        // Create Category Button
        category_buttons[category_name] = createCategoryButton(item, category_name);

        // Creates a frame to hold all assets of each type
        assets_frames[category_name] = new QFrame();
        assets_frames[category_name]->setObjectName("assetsContainer");

        m_grid_layouts[category_name] = new AssetFlowLayout(getParentProject(), assets_frames[category_name], 8, 0, 4, 0, 0, 0);
    }



    // ********** Get Entities to show, sort by listOrder
    std::vector<DrSettings*> entities { };
    if (show_types.contains(DrType::Asset))  { for (auto asset_pair :  list_assets)  { if (asset_pair.first > c_no_key)  entities.push_back(asset_pair.second); } }
    if (show_types.contains(DrType::Device)) { for (auto device_pair : list_devices) { if (device_pair.first > c_no_key) entities.push_back(device_pair.second); } }
    if (show_types.contains(DrType::Effect)) { for (auto effect_pair : list_effects) { if (effect_pair.first > c_no_key) entities.push_back(effect_pair.second); } }
    if (show_types.contains(DrType::Font))   { for (auto font_pair :   list_fonts)   { if (font_pair.first > c_no_key)   entities.push_back(font_pair.second); } }
    if (show_types.contains(DrType::Item))   { for (auto item_pair :   list_items)   { if (item_pair.first > c_no_key)   entities.push_back(item_pair.second); } }
    if (show_types.contains(DrType::Mix))    { for (auto mix_pair :    list_mixes)   { if (mix_pair.first > c_no_key)    entities.push_back(mix_pair.second); } }
    if (show_types.contains(DrType::Prefab)) { for (auto prefab_pair : list_prefabs) { if (prefab_pair.first > c_no_key) entities.push_back(prefab_pair.second); } }
    if (show_types.contains(DrType::Sound))  { for (auto sound_pair :  list_sounds)  { if (sound_pair.first > c_no_key)  entities.push_back(sound_pair.second); } }
    if (show_types.contains(DrType::Image))  {
        // Add Images to be shown, only add built in images to list if they're being used in current DrProject
        for (auto &image_pair : list_images) {
            if (image_pair.first > c_no_key) {
                if (image_keys_used.find(image_pair.first) != image_keys_used.end()) entities.push_back(image_pair.second);
            }
        }

        // External Images
        if (external_images) {
            for (auto &image_pair : external_images->getImageMap()) {
                if (image_pair.second->getFolderName() != "") entities.push_back(image_pair.second);
            }
        }
    }

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

        QSize frame_size = QSize(Dr::Scale(100), Dr::Scale(66));
        QRect name_rect =  QRect(10, 0, frame_size.width() - 20, Dr::Scale(25));
        QSize pix_size =   QSize(frame_size.width() - Dr::Scale(25), frame_size.height() - Dr::Scale(30));

        // ***** Store current asset key in widget and install a mouse handler event filter on the item, DrFilterAssetMouseHandler
        QFrame *single_asset = new QFrame();
        single_asset->setObjectName(QStringLiteral("assetFrame"));
        single_asset->setProperty(User_Property::Key,           QVariant::fromValue( entity->getKey() ));
        single_asset->setProperty(User_Property::Mouse_Down,    false);
        single_asset->setProperty(User_Property::External,      entity->getParentProject() == external_images);
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
        vertical_split->setContentsMargins(0, 14, 0, 0);                    // Put some space at the top
            QPixmap pix;
            QString description;
            std::string hidden_txt = entity->getComponentProperty(Comps::Hidden_Settings, Props::Hidden_Advisor_Description)->getValue().toString();

            if (entity->getType() == DrType::Asset) {
                DrAsset *asset = static_cast<DrAsset*>(entity);
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
                DrDevice *device = static_cast<DrDevice*>(entity);
                pix = Dr::GetAssetPixmapDevice( device->getDeviceType() );
                description = "<b>ID Key: " + QString::number(entity->getKey()) + "</b><br>" + Advisor_Info::Asset_Device[1];

            } else if (entity->getType() == DrType::Effect) {
                DrEffect *effect = static_cast<DrEffect*>(entity);
                pix = Dr::GetAssetPixmapEffect( effect->getEffectType() );
                description = "<b>ID Key: " + QString::number(entity->getKey()) + "</b><br>" + Advisor_Info::Asset_Effect[1];

            } else if (entity->getType() == DrType::Font) {
                DrFont *font = static_cast<DrFont*>(entity);
                pix = Dr::CreateText(font, "Aa" );
                description = "<b>ID Key: " + QString::number(entity->getKey()) + "</b><br>" + Advisor_Info::Asset_Text[1];

            } else if (entity->getType() == DrType::Image) {
                DrImage *image = static_cast<DrImage*>(entity);
                pix = Dr::ToQPixmap(image->getBitmap());
                description = "<b>ID Key: " + QString::number(entity->getKey()) + "</b><br>" + Advisor_Info::Asset_Image[1];

            } else if (entity->getType() == DrType::Item) {
                DrItem *item = static_cast<DrItem*>(entity);
                pix = Dr::GetAssetPixmapItem( item->getItemType() );
                description = "<b>ID Key: " + QString::number(entity->getKey()) + "</b><br>" + Advisor_Info::Asset_Item[1];

            } else if (entity->getType() == DrType::Mix) {
                DrMix *mix = static_cast<DrMix*>(entity);
                pix = Dr::GetAssetPixmapSound(DrSoundType::Mix, mix);
                description = "<b>ID Key: " + QString::number(entity->getKey()) + "</b><br>" + Advisor_Info::Asset_Mix[1];

            } else if (entity->getType() == DrType::Prefab) {
                DrPrefab *prefab = static_cast<DrPrefab*>(entity);
                pix = Dr::GetAssetPixmapPrefab( prefab->getPrefabType() );
                description = "<b>ID Key: " + QString::number(entity->getKey()) + "</b><br>" + Advisor_Info::Asset_Prefab[1];

            } else if (entity->getType() == DrType::Sound) {
                DrSound *sound = static_cast<DrSound*>(entity);
                pix = Dr::GetAssetPixmapSound(sound->getSoundType(), sound);
                description = "<b>ID Key: " + QString::number(entity->getKey()) + "</b><br>" + Advisor_Info::Asset_Sound[1];

            }
            description += "<br>" + QString::fromStdString(hidden_txt);
            getHoverHandler()->attachToHoverHandler(single_asset, entity_name, description);

            QLabel *asset_pix = new QLabel(single_asset);
            asset_pix->setObjectName(QStringLiteral("assetPixmap"));
            asset_pix->setFont(font);
            asset_pix->setSizePolicy(sp_right);
            asset_pix->setAlignment(Qt::AlignmentFlag::AlignCenter);
            asset_pix->setProperty(User_Property::Size, pix_size);
            vertical_split->addWidget( asset_pix );

            // Draw pixmap onto label
            if (pix.isNull() == false) asset_pix->setPixmap(pix.scaled(pix_size, Qt::KeepAspectRatio, Qt::SmoothTransformation));


        // ***** Add widget to proper category
        std::string category_name;
        if (entity->getType() == DrType::Asset) {
            DrAsset *asset = static_cast<DrAsset*>(entity);
            if (asset->getAssetType() == DrAssetType::Object)           category_name = Asset_Category::Objects;
            else if (asset->getAssetType() == DrAssetType::Character)   category_name = Asset_Category::Characters;
        } else if (entity->getType() == DrType::Device) {               category_name = Asset_Category::Devices;
        } else if (entity->getType() == DrType::Effect) {               category_name = Asset_Category::Effects;
        } else if (entity->getType() == DrType::Font) {                 category_name = Asset_Category::Text;
        } else if (entity->getType() == DrType::Image) {
            if (entity->getParentProject() == external_images) {
                DrImage *dr_image = static_cast<DrImage*>(entity);
                category_name = dr_image->getFolderName();
            } else {
                category_name = Asset_Category::Images;
            }
        } else if (entity->getType() == DrType::Item) {                 category_name = Asset_Category::Items;
        } else if (entity->getType() == DrType::Mix) {                  category_name = Asset_Category::Mixes;
        } else if (entity->getType() == DrType::Prefab) {               category_name = Asset_Category::Prefabs;
        } else if (entity->getType() == DrType::Sound) {
            DrSound *sound = static_cast<DrSound*>(entity);
            switch (sound->getSoundType()) {
                case DrSoundType::Audio_File:                           category_name = Asset_Category::Audio_Files;        break;
                case DrSoundType::Instrument:                           category_name = Asset_Category::Instruments;        break;
                case DrSoundType::Mix:                                  break;
                case DrSoundType::Sound_Effect:                         category_name = Asset_Category::Sound_Effects;      break;
                case DrSoundType::Speech:                               category_name = Asset_Category::Speech;             break;
                case DrSoundType::White_Noise:                          category_name = Asset_Category::White_Noise;        break;
            }
        } else { continue; }

        m_grid_layouts[category_name]->addWidget(single_asset);         // Category now has at least one Asset, setEnabled so it will be displayed
        category_buttons[category_name]->setEnabled(true);

        rowCount++;
    }


    // ***** Create a child TreeWidgetItem attached to the TopLevel category item containing all the Assets for that category
    for (auto button_pair : category_buttons) {
        std::string          category_name =    button_pair.first;
        TreeCategoryButton  *button =           button_pair.second;

        // If enabled, show Asset Category
        if (button->isEnabled()) {
            QTreeWidgetItem *asset_collection = new QTreeWidgetItem();
            asset_collection->setDisabled(true);
            widget_items[category_name]->addChild(asset_collection);
            this->setItemWidget(asset_collection, 0, assets_frames[category_name]);
        // Otherwise, hide it
        } else {
            this->takeTopLevelItem( this->indexOfTopLevelItem(widget_items[category_name]) );
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
TreeCategoryButton* TreeAssets::createCategoryButton(QTreeWidgetItem *item, std::string category_name) {
    QString name, icon;
    std::vector<QString> info;

    bool external_category = false;
    name = QString::fromStdString(category_name);
    if      (category_name == Asset_Category::Characters)       { icon = "comp_character.png";          info = Advisor_Info::Asset_Character; }
    else if (category_name == Asset_Category::Objects)          { icon = "comp_object.png";             info = Advisor_Info::Asset_Object;    }
    else if (category_name == Asset_Category::Devices)          { icon = "comp_camera.png";             info = Advisor_Info::Asset_Device;    }
    else if (category_name == Asset_Category::Effects)          { icon = "comp_effects.png";            info = Advisor_Info::Asset_Effect;    }
    else if (category_name == Asset_Category::Items)            { icon = "comp_item.png";               info = Advisor_Info::Asset_Item;      }
    else if (category_name == Asset_Category::Mixes)            { icon = "comp_sound_mix.png";          info = Advisor_Info::Asset_Mix;       }
    else if (category_name == Asset_Category::Prefabs)          { icon = "comp_foliage.png";            info = Advisor_Info::Asset_Prefab;    }
    else if (category_name == Asset_Category::Text)             { icon = "comp_font.png";               info = Advisor_Info::Asset_Text;      }
    // Sounds
    else if (category_name == Asset_Category::Audio_Files)      { icon = "comp_sound_wave.png";         info = Advisor_Info::Asset_Sound;     }
    else if (category_name == Asset_Category::Instruments)      { icon = "comp_sound_instrument.png";   info = Advisor_Info::Asset_Sound;     }
    else if (category_name == Asset_Category::Sound_Effects)    { icon = "comp_sound_effect.png";       info = Advisor_Info::Asset_Sound;     }
    else if (category_name == Asset_Category::Speech)           { icon = "comp_sound_speech.png";       info = Advisor_Info::Asset_Sound;     }
    else if (category_name == Asset_Category::White_Noise)      { icon = "comp_sound_noise.png";        info = Advisor_Info::Asset_Sound;     }
    // Images
    else if (category_name == Asset_Category::Images)           { icon = "comp_images.png";             info = Advisor_Info::Asset_Image;     }
    else {
        external_category = true;
        name.replace(0, 1, name.at(0).toUpper());
        icon = "";
        info = Advisor_Info::Asset_Image;
    }

    // Add spacing and translate
    name = " " + tr(name.toLatin1());

    QString icon_size =     QString::number(Dr::Scale(20)) + "px " + QString::number(Dr::Scale(16)) + "px";
    QString padding_left =  "9px";

    // ***** Create Category Button
    QString buttonColor = QString(" icon-size: " + icon_size + "; padding-left: " + padding_left + "; ");
    TreeCategoryButton *button = new TreeCategoryButton(name, Dr::ToQColor(Dr::GetColor(Window_Colors::Text)),
                                                              Dr::ToQColor(Dr::GetColor(Window_Colors::Text_Dark)), nullptr, item);
    button->setObjectName("buttonAssetCategory");
    button->setStyleSheet(buttonColor);
    button->setEnabled(false);                                              // Created as false, becomes true if we add an Asset to the category
    this->setItemWidget(item, 0, button);                                   // Apply the button to the tree item

    // ***** Grab Icon to use, colorize it to the current palette
    QPixmap text_icon;
    if (external_category) {
        text_icon = Dr::GetAssetCategoryIcon(category_name);
    } else {
        text_icon = ( ":/assets/inspector_icons/" + icon );
    }
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






















