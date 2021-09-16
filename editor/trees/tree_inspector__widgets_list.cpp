//
//      Created by Stephens Nunnally on 10/14/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QActionGroup>
#include <QApplication>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QMenu>
#include <QPainter>

#include "core/colors/colors.h"
#include "editor/event_filters/event_filters.h"
#include "editor/forms/form_color_magnifier.h"
#include "editor/forms/form_popup.h"
#include "editor/preferences.h"
#include "editor/style/style.h"
#include "editor/trees/tree_inspector.h"
#include "project/dr_project.h"
#include "project/settings/settings.h"
#include "project/settings/settings_component.h"
#include "project/settings/settings_component_property.h"


//####################################################################################
//##    Pushbutton with a popup menu instead of a QComboBox
//####################################################################################
QPushButton* TreeInspector::createListBox(DrProperty *property, QFont &font, QSizePolicy size_policy) {
    QPushButton *button = new QPushButton();
    button->setObjectName(QStringLiteral("buttonDropDown"));
    button->setFont(font);
    button->setSizePolicy(size_policy);

    std::string property_name = property->getPropertyName();

    QStringList options;
    if          (property_name == Props::Thing_Object_Damage) {
        options << tr("No Damage")
                << tr("Damage Player")
                << tr("Damage Enemy")
                << tr("Damage All");
    } else if   (property_name == Props::Thing_Object_Physics_Type) {
        options << tr("Static")
                << tr("Kinematic")
                << tr("Dynamic");
    } else if   (property_name == Props::Stage_Grid_Style) {
        options << tr("Lines")
                << tr("Dots");
    } else if   (property_name == Props::Thing_Light_Type) {
        options << tr("Opaque")
                << tr("Glow");
    } else if   (property_name == Props::World_Light_Blend) {
        options << tr("Default")
                << tr("Multiply")
                << tr("Overlay")
                << tr("Hard Light");
    } else if   (property_name == Props::Thing_Water_Texture) {
        options << tr("None")
                << tr("Ripples")
                << tr("Cells")
                << tr("Caustic")
                << tr("Liquid");
    } else if   (property_name == Props::Thing_Fire_Shape) {
        options << tr("Torch")
                << tr("Candle")
                << tr("Square")
                << tr("Triangle");
    } else if   (property_name == Props::Thing_3D_Type) {
        options << tr("None")
                << tr("Extruded")
                << tr("Wrapped Cube")
                << tr("Pyramid");
    } else if   (property_name == Props::Asset_Collision_One_Way_Type) {
        options << tr("None")
                << tr("Pass Through")
                << tr("Weak Spot")
                << tr("Directional Spring");
    } else if   (property_name == Props::Asset_Collision_Shape) {
        options << tr("Image Shape")
                << tr("Circle")
                << tr("Square")
                << tr("Triangle");
    } else if   (property_name == Props::Asset_Health_Death_Animation) {
        options << tr("None")
                << tr("Fade")
                << tr("Shrink");
    } else if   (property_name == Props::World_Camera_Type) {
        options << tr("Perspective")
                << tr("Orthographic");
    } else if   (property_name == Props::Thing_Character_Camera_Up_Vector ||
                 property_name == Props::Thing_Camera_Up_Vector) {
        options << tr("Y Axis (Default)")
                << tr("Z Axis (First Person)");
    } else if   (property_name == Props::Thing_Object_Collision_Group) {
        options << tr("None")
                << tr("All")
                << tr("Active Players Only")
                << tr("Players")
                << tr("Enemies");
    } else if   (property_name == Props::Thing_Spawn_Type) {
        options << tr("Permanent")
                << tr("Shoot Button")
                << tr("Jump Button")
                << tr("Object Death");
    } else if   (property_name == Props::World_Camera_Zoom_In_Or_Out) {
        options << tr("Zoom Out")
                << tr("Zoom In");
    } else if   (property_name == Props::World_Filter_PixelType || property_name == Props::Thing_Filter_PixelType) {
        options << tr("None")
                << tr("Ascii Text")
                << tr("Brick Wall")
                << tr("Ceramic Tile")
                << tr("Cross Stitch")
                << tr("Grid Blocks")
                << tr("Grid Lines")
                << tr("Knitted Stitch")
                << tr("Woven Cloth")
                << tr("Wood Blocks");
    } else if   (property_name == Props::Thing_Lighting_Blend_Object) {
        options << tr("Standard")
                << tr("Additive")
                << tr("Subtractive");
    } else if   (property_name == Props::Asset_Physics_Body_Style) {
        options << tr("Rigid Body")
                << tr("Circular Blob")
                << tr("Square Blob")
                << tr("Mesh Cloth")
                << tr("Foliage");
    } else {
        options << tr("Unknown List");
    }

    QMenu *menu = new QMenu(this);
    menu->setObjectName(QStringLiteral("menuComboBox"));
    menu->setMinimumWidth(130);

    QActionGroup *group;
    group = new QActionGroup(menu);
    group->setExclusive(true);

    // Loop through possible strings, add them to sub menu along with a connect to a lambda function that can update object settings
    int string_count = 0;
    for (auto string : options) {
        QAction *action = new QAction(string + " ");
        group->addAction(action);
        action->setCheckable(true);
        menu->addAction(action);

        if (property->getValue().toInt() == string_count) {
            action->setChecked(true);
            button->setText(string);
        }
        action->setProperty(User_Property::Order, QVariant::fromValue(string_count));

        // Create a callback function to update DrSettings when a new value is selected
        connect(action, &QAction::triggered, [this, button, action, property]() {
            button->setText(action->text());
            this->updateSettingsFromNewValue(property->getCompPropPair(), action->property(User_Property::Order).toInt());
        });

        if (property_name == Props::Thing_Object_Damage) {
            connect(action, &QAction::hovered, [this, string_count]() {
                switch (string_count) {
                    case 0: this->setAdvisorInfo( Advisor_Info::Damage_None[0],     Advisor_Info::Damage_None[1]);          break;
                    case 1: this->setAdvisorInfo( Advisor_Info::Damage_Player[0],   Advisor_Info::Damage_Player[1]);        break;
                    case 2: this->setAdvisorInfo( Advisor_Info::Damage_Enemy[0],    Advisor_Info::Damage_Enemy[1]);         break;
                    case 3: this->setAdvisorInfo( Advisor_Info::Damage_All[0],      Advisor_Info::Damage_All[1]);           break;
                }
            });
        }

        if (property_name == Props::Thing_Object_Physics_Type) {
            connect(action, &QAction::hovered, [this, string_count]() {
                switch (string_count) {
                    case 0: this->setAdvisorInfo( Advisor_Info::Object_Static[0],    Advisor_Info::Object_Static[1]);       break;
                    case 1: this->setAdvisorInfo( Advisor_Info::Object_Kinematic[0], Advisor_Info::Object_Kinematic[1]);    break;
                    case 2: this->setAdvisorInfo( Advisor_Info::Object_Dynamic[0],   Advisor_Info::Object_Dynamic[1]);      break;
                }
            });
        }

        if (property_name == Props::Thing_Light_Type) {
            connect(action, &QAction::hovered, [this, string_count]() {
                switch (string_count) {
                    case 0: this->setAdvisorInfo( Advisor_Info::Light_Opaque[0],    Advisor_Info::Light_Opaque[1]);          break;
                    case 1: this->setAdvisorInfo( Advisor_Info::Light_Glow[0],      Advisor_Info::Light_Glow[1]);           break;
                }
            });
        }

        string_count++;
    }

    button->setMenu(menu);
    button->setProperty(User_Property::CompKey,  QVariant::fromValue(       property->getParentComponent()->getComponentKey()) );
    button->setProperty(User_Property::CompName, QString::fromStdString(    property->getParentComponent()->getComponentName()) );
    button->setProperty(User_Property::PropKey,  QVariant::fromValue(       property->getPropertyKey()) );
    button->setProperty(User_Property::PropName, QString::fromStdString(    property->getPropertyName()) );
    menu->installEventFilter(new DrFilterPopUpMenuRelocater(menu, 2, 0));
    getHoverHandler()->attachToHoverHandler(button, property);
    addToWidgetList(button);

    return button;
}








