//
//      Created by Stephens Nunnally on 1/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Tree Inspector Definitions
//
//
#include <QDebug>
#include <QGraphicsDropShadowEffect>
#include <QLabel>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QScrollBar>
#include <QVBoxLayout>

#include <cmath>

#include "core/colors/colors.h"
#include "editor/event_filters.h"
#include "editor/helper_library.h"
#include "editor/interface_editor_relay.h"
#include "editor/pixmap/pixmap.h"
#include "editor/preferences.h"
#include "editor/style/style.h"
#include "editor/trees/tree_assets.h"
#include "editor/trees/tree_inspector.h"
#include "editor/view/editor_item.h"
#include "editor/view/editor_scene.h"
#include "engine/debug_flags.h"
#include "project/dr_project.h"
#include "project/entities/dr_stage.h"
#include "project/entities/dr_thing.h"
#include "project/entities/dr_world.h"
#include "project/entities_physics_2d/dr_asset.h"
#include "project/settings/settings.h"
#include "project/settings/settings_component.h"
#include "project/settings/settings_component_property.h"

// Local Constants
const bool c_update_reason = false;             // When set to true, prints reason function left early without completing


//####################################################################################
//##    Constructor
//####################################################################################
TreeInspector::TreeInspector(QWidget *parent, DrProject *project, IEditorRelay *editor_relay) :
                              QTreeWidget (parent), m_project(project), m_editor_relay(editor_relay) {
    // Initialize hover handler
    setHoverHandler( new DrFilterHoverHandler(this) );
    connect(m_filter_hover, SIGNAL(signalMouseHover(QString, QString)), this, SLOT(setAdvisorInfo(QString, QString)));

    // Connect this widget to the hover handler
    getHoverHandler()->attachToHoverHandler(this, Advisor_Info::Inspector_Window);

    // Connect to Expand / Collapse slots
    connect(this, SIGNAL(itemCollapsed(QTreeWidgetItem *)), this, SLOT(handleCollapsed(QTreeWidgetItem *)));
    connect(this, SIGNAL(itemExpanded(QTreeWidgetItem *)),  this, SLOT(handleExpanded(QTreeWidgetItem *)));

    if (Dr::CheckDebugFlag(Debug_Flags::Label_Inspector_Items)) {
        temp_selected_label = new QLabel();
        temp_selected_label->setWordWrap(true);
        parent->layout()->addWidget(temp_selected_label);
    }
}

DrFilterHoverHandler* TreeInspector::getHoverHandler() {
    if (m_filter_hover == nullptr)
        Dr::ShowErrorMessage("TreeInspector::getHoverHandler", "Could not get DrFilterHoverHandler pointer in TreeInspector, not initialized or corrupt!");
    return m_filter_hover;
}

// SLOT: Catches signals from m_filter_hover and passes to InterfaceEditorRelay
void TreeInspector::setAdvisorInfo(QString header, QString body) { m_editor_relay->setAdvisorInfo(header, body); }

// Focus In Event
void TreeInspector::focusInEvent(QFocusEvent *) {
    m_editor_relay->setActiveWidget(Editor_Widgets::Inspector_Tree);
}



//####################################################################################
//##    Dynamically build Inspector
//####################################################################################
void TreeInspector::buildInspectorFromKeys(QList<long> new_key_list, bool force_rebuild) {

    // !!!!! #DEBUG:    Show item keys selected at top of object inspector
    if (Dr::CheckDebugFlag(Debug_Flags::Label_Inspector_Items)) {
        QString selected_label = "Selected (" + QString::number(new_key_list.size()) + ") items: ";
        for (auto &long_key : new_key_list) {
            selected_label += QString::number(long_key) + ", ";
        }
        if (temp_selected_label != nullptr) temp_selected_label->setText(selected_label);
    }
    // !!!!!


    // ***** Store current scroll bar position
    if (m_selected_type != DrType::NotFound) {
        m_last_scroll_position = this->verticalScrollBar()->value();
    }

    // ***** If no keys were passed in, clear Inspector and exit
    if (new_key_list.size() == 0) {
        m_key_shown = c_no_key;
        m_selected_keys.clear();
        m_selected_keys.push_back(c_no_key);
        m_selected_type = DrType::NotFound;
        m_editor_relay->getAssetTree()->setSelectedKey(c_no_key);
        this->clear();
        m_widgets.clear();
        if (c_update_reason) qDebug() << "Inspector::buildInspectorFromKeys() exiting early... Inspector cleared...";
        return;
    }

    // ***** If Inspector already contains these items, exit now
    QList<long> new_key_list_sorted = new_key_list;
    QList<long> old_key_list_sorted = m_selected_keys;
    std::sort(new_key_list_sorted.begin(), new_key_list_sorted.end());
    std::sort(old_key_list_sorted.begin(), old_key_list_sorted.end());
    if (new_key_list_sorted == old_key_list_sorted && !force_rebuild) {
        if (c_update_reason) qDebug() << "Inspector::buildInspectorFromKeys() exiting early... New list same as m_selected_keys list...";
        return;
    }

    // ***** Retrieve DrSettings* of items clicked in list, exit if not found or list contains more than one DrType
    if (new_key_list.contains(c_no_key)) {
        m_selected_type = DrType::NotFound;
        if (c_update_reason) qDebug() << "Inspector::buildInspectorFromKeys() exiting early... New list contained c_no_key...";
        return;
    }
    std::list<DrSettings*> new_settings_list { };
    DrType new_type = DrType::NotFound;
    bool first = true;
    if (new_key_list.contains(m_key_shown)) {
        new_type = m_selected_type;
        first = false;
    }
    for (auto &key : new_key_list) {
        DrSettings *dr_settings = getParentProject()->findSettingsFromKey( key );
                if (dr_settings == nullptr) {
                    m_selected_type = DrType::NotFound;
                    if (c_update_reason) qDebug() << "Inspector::buildInspectorFromKeys() exiting early... New list contained invalid key: " << key << "...";
                    return;
                }
        if (first) {
            new_type = dr_settings->getType();
            first = false;
        } else if (dr_settings->getType() != new_type) {
            m_selected_type = DrType::NotFound;
            if (c_update_reason) {
                qDebug() << "Inspector::buildInspectorFromKeys() exiting early... " <<
                            "New list contained item of different type: " << QString::fromStdString(Dr::StringFromType(dr_settings->getType())) << "...";
            }
            return;
        }
        new_settings_list.push_back(dr_settings);
    }


    // ***** Selection list contains Entity already shown in Inspector, exit now
    long        new_key_to_show = 0;
    DrSettings *new_settings_to_show = nullptr;
    if (new_key_list.contains(m_key_shown) && !force_rebuild) {
        m_selected_keys = new_key_list;
        if (c_update_reason) qDebug() << "Inspector::buildInspectorFromKeys() exiting early... New list contained m_key_shown...";
        return;
    // ***** Otherwise, pick new Entity... #NOTE: keys are passed into this function in no particular order
    } else {
        new_settings_to_show =  new_settings_list.front();
        new_key_to_show =       new_settings_to_show->getKey();
    }


    // ********** Change Advisor text after new item selection
    switch (new_type) {
        case DrType::Asset:        break;///m_editor_relay->setAdvisorInfo(Advisor_Info::Asset_Description);     break;
        case DrType::World:        m_editor_relay->setAdvisorInfo(Advisor_Info::World_Description);     break;
        case DrType::Stage:        m_editor_relay->setAdvisorInfo(Advisor_Info::Stage_Description);     break;
        case DrType::Thing: {
                if (new_settings_list.size() == 1) {
                    DrThing *thing = dynamic_cast<DrThing*>(new_settings_to_show);
                    QString asset_header = QString::fromStdString(new_settings_to_show->getName());
                    QString asset_body = "Error converting to DrThing in buildInspectorFromKeys()...";
                    if (thing != nullptr) {
                        asset_body = "<b>Asset ID Key: " + QString::number(thing->getAssetKey()) + "</b><br>" +
                                     QString::fromStdString(Dr::StringFromThingType(thing->getThingType()));
                    }
                    m_editor_relay->setAdvisorInfo(asset_header, asset_body);
                } else {
                    m_editor_relay->setAdvisorInfo(Advisor_Info::Things_Description);
                }
            break;
        }
        case DrType::Device:    m_editor_relay->setAdvisorInfo(Advisor_Info::Asset_Device);             break;
        case DrType::Effect:    m_editor_relay->setAdvisorInfo(Advisor_Info::Asset_Effect);             break;
        case DrType::Image:     m_editor_relay->setAdvisorInfo(Advisor_Info::Asset_Image);              break;
        default:
            m_editor_relay->setAdvisorInfo(Advisor_Info::Not_Set);
    }


    // !!!!! #DEBUG:    Show selected item key and info
    if (Dr::CheckDebugFlag(Debug_Flags::Label_Inspector_Build)) {
        std::string type_string = Dr::StringFromType(new_type);
        Dr::SetLabelText(Label_Names::Label_Object_1, QString::fromStdString("KEY: " + std::to_string(new_key_to_show) + ", TYPE: " + type_string));
        if (new_type == DrType::Thing) {
            DrThing* thing = getParentProject()->findThingFromKey(new_key_to_show);
            QString asset_name = QString::fromStdString(getParentProject()->findSettingsFromKey(thing->getAssetKey())->getName());
            Dr::SetLabelText(Label_Names::Label_Object_2, "ASSET KEY:  " + QString::number(thing->getAssetKey()) +
                                                              ", NAME: " + asset_name);
        } else {
            Dr::SetLabelText(Label_Names::Label_Object_2, "");
        }
    }
    // !!!!! END


    // ********** Check if old selection and new selection are types are same
    bool same_type = false;
    DrSettings *settings_shown = nullptr;
    if (m_key_shown != c_no_key) settings_shown = getParentProject()->findSettingsFromKey(m_key_shown);
    if (settings_shown != nullptr && !force_rebuild) {
        if (m_selected_type == DrType::Thing && new_type == DrType::Thing) {
            DrThing *thing1 = dynamic_cast<DrThing*>(settings_shown);
            DrThing *thing2 = dynamic_cast<DrThing*>(new_settings_to_show);
            if ((thing1 != nullptr) && (thing2 != nullptr)) {
                if (thing1->getThingType() == thing2->getThingType()) same_type = true;
            }
        } else if (m_selected_type == DrType::Asset && new_type == DrType::Asset) {
            DrAsset *asset1 = dynamic_cast<DrAsset*>(settings_shown);
            DrAsset *asset2 = dynamic_cast<DrAsset*>(new_settings_to_show);
            if ((asset1 != nullptr) && (asset2 != nullptr)) {
                if (asset1->getAssetType() == asset2->getAssetType()) same_type = true;
            }
        } else if (m_selected_type == new_type) {
            same_type = true;
        }
    }

    // ***** Highlight DrAsset of selected Thing in AssetTree when a Thing is selected in the StageView or ProjectTree
    if (new_settings_to_show != nullptr) {
        if (new_settings_to_show->getType() == DrType::Thing) {
            DrThing *thing = dynamic_cast<DrThing*>(new_settings_to_show);
            m_editor_relay->getAssetTree()->setSelectedKey(thing->getAssetKey());
        } else if (new_settings_to_show->getType() != DrType::Asset) {
            m_editor_relay->getAssetTree()->setSelectedKey(new_settings_to_show->getKey());
        }
    }

    // ***** Update selected / shown member variables
    m_key_shown =     new_key_to_show;
    m_selected_keys = new_key_list;
    m_selected_type = new_type;

    // ***** If types were the same, we don't need to completely rebuild Inspector, just update values
    if (same_type) {
        m_selected_keys = new_key_list;
        updateInspectorPropertyBoxes( new_settings_list, { } );
        updateLockedSettings();
        if (c_update_reason) qDebug() << "Inspector::buildInspectorFromKeys() exiting early... new_key_to_show and m_key_shown are same type...";
        return;
    }


    // ***** Get component map, sort by listOrder
    std::vector<DrComponent*> components { };
    for (auto component_pair: new_settings_to_show->getComponentMap())
        components.push_back(component_pair.second);
    std::sort(components.begin(), components.end(), [](DrComponent *a, DrComponent *b) {
        return a->getListOrder() < b->getListOrder();
    });


    // ********** Loop through each component and add it to the Inspector list
    if (c_update_reason) qDebug() << "Rebuilding object Inspector... Key: " << QString::number(new_settings_to_show->getKey())
                                  << ", Name: " << QString::fromStdString(new_settings_to_show->getName());
    this->clear();
    m_widgets.clear();

    for (auto component: components) {
        if (component->isTurnedOn() == false) {
            continue;
        } else if (component->getComponentKey() == Comps::Hidden_Settings   ||
                   component->getComponentKey() == Comps::Size_Settings     ||
                   component->getComponentKey() == Comps::Local_Variables   ||
                   component->getComponentKey() == Comps::User_Variables) {
            if (Dr::CheckDebugFlag(Debug_Flags::Show_Hidden_Component) == false) continue;
        }

        // *****Create new item in list to hold component and add the TreeWidgetItem to the tree
        QTreeWidgetItem *category_item = new QTreeWidgetItem();
        category_item->setData(0, User_Roles::CompKey, QString::fromStdString(component->getComponentKey()));       // Stores component key
        this->addTopLevelItem(category_item);

        // Create and style a button to be used as a header item for the category
        QFrame *button_frame = new QFrame();
        QGridLayout *grid = new QGridLayout(button_frame);
        grid->setContentsMargins(0, 0, 0, 0);

        if (component->getComponentKey() == Comps::Entity_Settings) {
            // Make it really small but not zero to hide category button for Name, zero causes scroll bar to stop working for some reason
            category_item->setSizeHint(0, QSize(1, 1));
        } else {
            // Build category button
            AssetCategoryButton *category_button = new AssetCategoryButton(QString(" ") + QString::fromStdString(component->getDisplayName()),
                                                                           Qt::black, Qt::black, nullptr, category_item);
            std::string button_style =
                        " QPushButton { height: 22px; font: 13px; text-align: left; icon-size: 20px 16px; color: black; "
                                        " padding-left: 2px;"
                                        " border: none; "
                                        " border-style: solid; "
                                        " border-top-width: " + Dr::BorderWidth() + "; "
                                        " border-bottom-width: " + Dr::BorderWidth() + "; "
                                        " border-color: " + Dr::GetColor(Window_Colors::Header_Fade).name() + "; "
                                        " background: qlineargradient(spread:pad, x1:0 y1:0, x2:0 y2:1, "
                                        "   stop:0.00 " + component->getColor().lighter(120).name() +
                                        "   stop:0.05 " + component->getColor().lighter(120).name() +
                                        "   stop:0.10 " + component->getColor().name() +
                                        " , stop:0.90 " + component->getColor().darker(200).name() +
                                        " , stop:0.95 " + component->getColor().darker(300).name() +
                                        " , stop:1.00 " + component->getColor().darker(300).name() + "); } "
                          " QPushButton:hover:!pressed { color: " + component->getColor().lighter(200).name() + "; } "
                          " QPushButton:pressed { color: " + component->getColor().darker(250).name() + "; } ";
            // Assign icon to category button
            QPixmap cat_icon(QString::fromStdString(component->getIcon()));
            QGraphicsDropShadowEffect *drop_shadow = new QGraphicsDropShadowEffect();
            drop_shadow->setColor( Dr::ToQColor(component->getColor()).darker(200) );
            drop_shadow->setBlurRadius( 2.0 );
            drop_shadow->setOffset( -3, 3 );
            cat_icon = QPixmap::fromImage( Dr::ApplyEffectToImage(cat_icon.toImage(), drop_shadow, 0) );
            category_button->setIcon(QIcon(cat_icon));
            category_button->setStyleSheet(QString::fromStdString(button_style));

            // Add the button widget to the tree item
            getHoverHandler()->attachToHoverHandler(category_button,
                                                    QString::fromStdString(component->getDisplayName()),
                                                    QString::fromStdString(component->getDescription()));
            grid->addWidget(category_button);
        }
        this->setItemWidget(category_item, 0, button_frame);


        // ***** Creates a frame to hold all properties of component, with vertical layout
        QFrame *properties_frame = new QFrame();
        properties_frame->setObjectName("propertiesFrame");
        QVBoxLayout *vertical_layout = new QVBoxLayout(properties_frame);
        vertical_layout->setSpacing(0);
        vertical_layout->setMargin(0);
        vertical_layout->setContentsMargins(0,2,0,2);

        // ***** Get property map, sort by listOrder
        std::vector<DrProperty*> properties { };
        for (auto property_pair: component->getPropertyMap()) properties.push_back(property_pair.second);
        std::sort(properties.begin(), properties.end(), [](DrProperty *a, DrProperty *b) {
            return a->getListOrder() < b->getListOrder();
        });

        // ***** Loop through each property and add it to the component frame
        for (auto property: properties) {
            if (property->isHidden() && Dr::CheckDebugFlag(Debug_Flags::Show_Hidden_Component) == false) {
                continue;
            }

            QFrame *single_row = new QFrame(properties_frame);
            single_row->setObjectName("propertyRow");
            single_row->setProperty(User_Property::CompKey, QString::fromStdString(property->getParentComponent()->getComponentKey()) );
            single_row->setProperty(User_Property::PropKey, QString::fromStdString(property->getPropertyKey()) );

            QHBoxLayout *horizontal_split = new QHBoxLayout(single_row);
            horizontal_split->setSpacing(6);
            horizontal_split->setMargin(0);
            horizontal_split->setContentsMargins(4,2,4,2);

            QFont fp = Dr::CustomFont();

            QLabel *property_name = new QLabel(QString::fromStdString(property->getDisplayName()));
            property_name->setFont(fp);
                QSizePolicy sp_left(QSizePolicy::Preferred, QSizePolicy::Preferred);    sp_left.setHorizontalStretch(c_inspector_size_left);
                QSizePolicy sp_right(QSizePolicy::Preferred, QSizePolicy::Preferred);   sp_right.setHorizontalStretch(c_inspector_size_right);
                property_name->setSizePolicy(sp_left);
                getHoverHandler()->attachToHoverHandler(property_name, property);

                if (property->getPropertyKey() == Props::Thing_Filter_Convert_3D ||
                    property->getPropertyKey() == Props::World_Filter_Convert_3D) {
                    // 3Dish Shadow Effect
                    ///QGraphicsDropShadowEffect *text_effect_3d_b = new QGraphicsDropShadowEffect();
                    ///text_effect_3d_b->setBlurRadius(2.0);
                    ///text_effect_3d_b->setOffset(3, 3);
                    ///text_effect_3d_b->setColor( Dr::ToQColor(Dr::GetColor(Window_Colors::Shadow)) );
                    ///property_name->setGraphicsEffect(text_effect_3d_b);
                }

            horizontal_split->addWidget(property_name);

            QWidget    *new_widget = nullptr;
            DrProperty *prop = property;

            switch (property->getPropertyType()) {
                case Property_Type::BoolEnabled:    new_widget = createCheckBox(            prop, fp, sp_right, Property_Type::BoolEnabled);    break;
                case Property_Type::Bool:           new_widget = createCheckBox(            prop, fp, sp_right, Property_Type::Bool);           break;
                case Property_Type::BoolDouble:     new_widget = createCheckBoxSpinBoxPair( prop, fp, sp_right);                                break;
                case Property_Type::BoolInt:        new_widget = createCheckBoxIntBoxPair(  prop, fp, sp_right);                                break;
                case Property_Type::String:         new_widget = createLineEdit(            prop, fp, sp_right);                                break;
                case Property_Type::Textbox:        new_widget = createTextEdit(            prop, fp, sp_right);                                break;
                case Property_Type::Int:            new_widget = createIntSpinBox(          prop, fp, sp_right, Property_Type::Int);            break;
                case Property_Type::RangedInt:      new_widget = createIntSpinBox(          prop, fp, sp_right, Property_Type::RangedInt);      break;
                case Property_Type::Positive:       new_widget = createIntSpinBox(          prop, fp, sp_right, Property_Type::Positive);       break;

                case Property_Type::Percent:        new_widget = createSlider(              prop, fp, sp_right, Property_Type::Percent);        break;
                case Property_Type::Slider:         new_widget = createSlider(              prop, fp, sp_right, Property_Type::Slider);         break;

                case Property_Type::Double:         new_widget = createDoubleSpinBox(       prop, fp, sp_right, Property_Type::Double);         break;
                case Property_Type::PositiveDouble: new_widget = createDoubleSpinBox(       prop, fp, sp_right, Property_Type::PositiveDouble); break;
                case Property_Type::RangedDouble:   new_widget = createDoubleSpinBox(       prop, fp, sp_right, Property_Type::RangedDouble);   break;
                case Property_Type::Angle:          new_widget = createDoubleSpinBox(       prop, fp, sp_right, Property_Type::Angle);          break;
                case Property_Type::PointF:         new_widget = createDoubleSpinBoxPair(   prop, fp, sp_right, Property_Type::PointF);         break;
                case Property_Type::PositionF:      new_widget = createDoubleSpinBoxPair(   prop, fp, sp_right, Property_Type::PositionF);      break;
                case Property_Type::SizeF:          new_widget = createDoubleSpinBoxPair(   prop, fp, sp_right, Property_Type::SizeF);          break;
                case Property_Type::PositiveSizeF:  new_widget = createDoubleSpinBoxPair(   prop, fp, sp_right, Property_Type::PositiveSizeF);  break;
                case Property_Type::OneSizeF:       new_widget = createDoubleSpinBoxPair(   prop, fp, sp_right, Property_Type::OneSizeF);       break;
                case Property_Type::Point3D:        new_widget = createDoubleSpinBoxTrio(   prop, fp, sp_right, Property_Type::Point3D);        break;
                case Property_Type::ScaleF:         new_widget = createDoubleSpinBoxPair(   prop, fp, sp_right, Property_Type::ScaleF);         break;
                case Property_Type::PositiveScaleF: new_widget = createDoubleSpinBoxPair(   prop, fp, sp_right, Property_Type::PositiveScaleF); break;
                case Property_Type::GridF:          new_widget = createDoubleSpinBoxPair(   prop, fp, sp_right, Property_Type::GridF);          break;
                case Property_Type::GridScaleF:     new_widget = createDoubleSpinBoxPair(   prop, fp, sp_right, Property_Type::GridScaleF);     break;
                case Property_Type::Variable:       new_widget = createVariableSpinBoxPair( prop, fp, sp_right);                                break;
                case Property_Type::List:           new_widget = createListBox(             prop, fp, sp_right);                                break;
                case Property_Type::Color:          new_widget = createColorBox(            prop, fp, sp_right);                                break;

                case Property_Type::Image:
                    new_widget = createImageFrame(prop, fp, sp_right);
                    new_widget->installEventFilter(new DrFilterInspectorImage(single_row, m_editor_relay));
                    break;


                //################ !!!!!!!!!!!!!!!!!!!!!!!
                //
                //      CASES NOT ACCOUNTED FOR
                //
                //################ !!!!!!!!!!!!!!!!!!!!!!!
                case Property_Type::Collision:                                      // For Collision Shapes
                case Property_Type::Icon:
                case Property_Type::Array:

                    break;
            }

            if (new_widget != nullptr) {
                horizontal_split->addWidget(new_widget);
            }

            vertical_layout->addWidget(single_row);
        }


        // ***** Create a child TreeWidgetItem attached to the TopLevel category item
        QTreeWidgetItem *property_item = new QTreeWidgetItem();
        property_item->setDisabled(true);
        category_item->addChild(property_item);

        // Apply the property frame widget to the tree item
        this->setItemWidget(property_item, 0, properties_frame);
    }


    // ***** Adds a spacer item at the bottom to allow for comfortable scrolling to the bottom of the tree
    QTreeWidgetItem *spacer_item  = new QTreeWidgetItem();
    spacer_item->setData(0, User_Roles::Key, QVariant::fromValue(c_spacer_item_key));
    QLabel *spacer_label = new QLabel();
    spacer_label->setFixedHeight(500);
    this->addTopLevelItem(spacer_item);
    this->setItemWidget(spacer_item, 0, spacer_label);
    this->expandItem(spacer_item);
    spacer_item->setFlags(Qt::NoItemFlags);                                         // Stops from being selectable and changing Inspector scroll on click


    // ***** Disable / enable widgets based on property status
    updateLockedSettings();

    // ***** Hide / show rows depending on sub property types (Property_Type::BoolEnabled) being true / false
    bool calling_from_build = true;
    updateSubProperties(calling_from_build);

    // ***** Expand / collapse top level items based on last known setting
    expandCollapseComponents();

    // ***** Scroll back to previous position
    this->verticalScrollBar()->setValue( m_last_scroll_position );
    this->update();
}
















