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
#include "project/entities/dr_asset.h"
#include "project/entities/dr_stage.h"
#include "project/entities/dr_thing.h"
#include "project/entities/dr_world.h"
#include "project/settings/settings.h"
#include "project/settings/settings_component.h"
#include "project/settings/settings_component_property.h"


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

    temp_selected_label = new QLabel();
    temp_selected_label->setWordWrap(true);
    parent->layout()->addWidget(temp_selected_label);
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
void TreeInspector::buildInspectorFromKeys(QList<long> key_list, bool force_rebuild) {  

QString selected_label = "Selected (" + QString::number(key_list.size()) + ") items: ";
for (auto &long_key : key_list) {
    selected_label += QString::number(long_key) + ", ";
}
temp_selected_label->setText(selected_label);

qDebug() << selected_label;

    // ***** Store current scroll bar position
    if (m_selected_type != DrType::NotFound) {
        m_last_scroll_position = this->verticalScrollBar()->value();
    }

    // ***** If no keys were passed in, clear Inspector and exit
    if (key_list.size() == 0) {
        m_selected_keys.clear();
        m_selected_keys.push_back(c_no_key);
        m_selected_type = DrType::NotFound;
        this->clear();
        m_widgets.clear();
        return;
    }

    // ***** If Inspector already contains these items exit now
    QList<long> compare_1 = key_list;
    QList<long> compare_2 = m_selected_keys;
    std::sort(compare_1.begin(), compare_1.end());
    std::sort(compare_2.begin(), compare_2.end());
    if (compare_1 == compare_2 && !force_rebuild) return;

    // ***** Retrieve DrSettings* of items clicked in list, exit if not found or list contains more than one DrType
    if (key_list.contains(c_no_key)) { m_selected_type = DrType::NotFound; return; }
    QList<DrSettings*> settings_list { };
    DrType new_type = DrType::NotFound;
    bool first = true;
    for (auto &key : key_list) {
        DrSettings *dr_settings = getParentProject()->findSettingsFromKey( key );
                if (dr_settings == nullptr) { m_selected_type = DrType::NotFound; return; }
        if (first) {
            new_type = dr_settings->getType();
            first = false;
        } else if (dr_settings->getType() != new_type) {
            m_selected_type = DrType::NotFound; return;
        }
        settings_list.push_back(dr_settings);
    }


    // ********** Change Advisor text after new item selection
    switch (new_type) {
        case DrType::Asset:        break;///m_editor_relay->setAdvisorInfo(Advisor_Info::Asset_Description);     break;
        case DrType::World:        m_editor_relay->setAdvisorInfo(Advisor_Info::World_Description);     break;
        case DrType::Stage:        m_editor_relay->setAdvisorInfo(Advisor_Info::Stage_Description);     break;
        case DrType::Thing: {
                if (settings_list.size() == 1) {
                    DrThing *thing = dynamic_cast<DrThing*>(settings_list[0]);
                    QString asset_header = QString::fromStdString(settings_list[0]->getName());
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
        Dr::SetLabelText(Label_Names::Label_Object_1, QString::fromStdString("KEY: " + std::to_string( key_list[0] ) + ", TYPE: " + type_string));
        if (new_type == DrType::Thing) {
            DrThing* thing = getParentProject()->findThingFromKey(key_list[0]);
            QString asset_name = QString::fromStdString(getParentProject()->findSettingsFromKey(thing->getAssetKey())->getName());
            Dr::SetLabelText(Label_Names::Label_Object_2, "ASSET KEY:  " + QString::number(thing->getAssetKey()) +
                                                              ", NAME: " + asset_name);
        } else {
            Dr::SetLabelText(Label_Names::Label_Object_2, "");
        }
    }
    // !!!!! END


    // ********** If old selection and new selection are same, we don't need to completely rebuild Inspector, just update values
    if (m_selected_type == DrType::Thing && new_type == DrType::Thing && !force_rebuild) {
        DrSettings *settings1 = getParentProject()->findSettingsFromKey(m_selected_keys[0]);
        if (settings1 != nullptr) {
            DrThing *thing1 = dynamic_cast<DrThing*>(settings1);
            DrThing *thing2 = dynamic_cast<DrThing*>(settings_list[0]);
            if ((thing1 != nullptr) && (thing2 != nullptr)) {
                if (thing1->getThingType() == thing2->getThingType()) {
                    m_selected_keys = key_list;
                    updateInspectorPropertyBoxes( { thing2 }, { } );
                    updateLockedSettings();
                    return;
                }
            }
        }
    } else if (m_selected_type == new_type && !force_rebuild) {
        m_selected_keys = key_list;
        updateInspectorPropertyBoxes( { settings_list[0] }, { } );
        updateLockedSettings();
        return;
    }
    m_selected_keys = key_list;
    m_selected_type = new_type;


    // ***** Get component map, sort by listOrder
    std::vector<DrComponent*> components { };
    for (auto component_pair: settings_list[0]->getComponentMap())
        components.push_back(component_pair.second);
    std::sort(components.begin(), components.end(), [](DrComponent *a, DrComponent *b) {
        return a->getListOrder() < b->getListOrder();
    });


    // ********** Loop through each component and add it to the Inspector list
    this->clear();
    m_widgets.clear();

qDebug() << "Rebuilding inspector from scratch!";

    for (auto component: components) {
        if (component->isTurnedOn() == false) {
            continue;
        } else if (component->getComponentKey() == Comps::Hidden_Settings ||
                   component->getComponentKey() == Comps::Size_Settings) {
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
                    QGraphicsDropShadowEffect *text_effect_3d_b = new QGraphicsDropShadowEffect();
                    text_effect_3d_b->setBlurRadius(3.0);
                    text_effect_3d_b->setOffset(-3, 3);
                    text_effect_3d_b->setColor( Dr::ToQColor(Dr::GetColor(Window_Colors::Shadow)) );
                    property_name->setGraphicsEffect(text_effect_3d_b);
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
















