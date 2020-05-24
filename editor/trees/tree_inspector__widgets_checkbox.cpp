//
//      Created by Stephens Nunnally on 4/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Object Inspector CheckBox
//
#include <QApplication>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QMenu>
#include <QPainter>

#include "core/colors/colors.h"
#include "editor/event_filters/event_filters.h"
#include "editor/forms/form_popup.h"
#include "editor/helper_library.h"
#include "editor/preferences.h"
#include "editor/style/style.h"
#include "editor/trees/tree_inspector.h"
#include "editor/widgets/widgets_inspector.h"
#include "project/dr_project.h"
#include "project/settings/settings.h"
#include "project/settings/settings_component.h"
#include "project/settings/settings_component_property.h"


//####################################################################################
//##    Checkbox
//####################################################################################
QCheckBox* TreeInspector::createCheckBox(DrProperty *property, QFont &font, QSizePolicy size_policy, Property_Type check_type) {

    InspectorCheckBox *check = new InspectorCheckBox();
    check->setObjectName("checkInspector");
    check->setFont(font);
    check->setDrawTop(3);
    check->setDrawLeft(4);
    check->setSizePolicy(size_policy);
    check->setTristate(false);
    check->setProperty(User_Property::Mouse_Over, false);               // Initialize some mouse user data, DrFilterHoverHandler updates this info,
    check->setProperty(User_Property::Mouse_Pos, QPoint(0, 0));         // Used to track when the mouse is within the indicator area for custom paint event
    check->setProperty(User_Property::CompKey,  QVariant::fromValue(       property->getParentComponent()->getComponentKey()) );
    check->setProperty(User_Property::CompName, QString::fromStdString(    property->getParentComponent()->getComponentName()) );
    check->setProperty(User_Property::PropKey,  QVariant::fromValue(       property->getPropertyKey()) );
    check->setProperty(User_Property::PropName, QString::fromStdString(    property->getPropertyName()) );


    if (check_type == Property_Type::BoolEnabled) {
        std::vector<DrVariant> prop_list = property->getValue().toVector();
        if (prop_list.size() > 0) check->setChecked(prop_list[0].toBool());
    } else {
        check->setChecked(property->getValue().toBool());
    }

    getHoverHandler()->attachToHoverHandler(check, property);
    addToWidgetList(check);

    connect (check, &QCheckBox::toggled, [this, property](bool checked) { updateSettingsFromNewValue( property->getCompPropPair(), checked );  });

    return check;
}


//####################################################################################
//##    Pair of Check Box / Double Spin Box, uses Property_Type::BoolDouble
//##        std::vector<DrVariant> of 6 values: bool, double value, min, max, double step size, string spinText
//####################################################################################
QFrame* TreeInspector::createCheckBoxSpinBoxPair(DrProperty *property, QFont &font, QSizePolicy size_policy) {

    QFrame *spin_pair = new QFrame();
    spin_pair->setFixedHeight(25);
    spin_pair->setSizePolicy(size_policy);

    QHBoxLayout *horizontal_split = new QHBoxLayout(spin_pair);
    horizontal_split->setAlignment(Qt::AlignVCenter);
    horizontal_split->setSpacing(2);
    horizontal_split->setContentsMargins(0,0,0,2);

    InspectorCheckBox *check_left = new InspectorCheckBox();
    check_left->setFixedWidth(36);
    check_left->setObjectName("checkInspector");
    check_left->setFont(font);
    check_left->setDrawLeft(0);
    check_left->setDrawTop(3);
    check_left->setTristate(false);
    check_left->setProperty(User_Property::Mouse_Over, false);              // Initialize some mouse user data, DrFilterHoverHandler updates this info,
    check_left->setProperty(User_Property::Mouse_Pos, QPoint(0, 0));        // Used to track when the mouse is within the indicator area for custom paint event
    check_left->setProperty(User_Property::CompKey,  QVariant::fromValue(       property->getParentComponent()->getComponentKey()) );
    check_left->setProperty(User_Property::CompName, QString::fromStdString(    property->getParentComponent()->getComponentName()) );
    check_left->setProperty(User_Property::PropKey,  QVariant::fromValue(       property->getPropertyKey()) );
    check_left->setProperty(User_Property::PropName, QString::fromStdString(    property->getPropertyName()) );
    check_left->setChecked(property->getValue().toVector()[0].toBool());
    getHoverHandler()->attachToHoverHandler(check_left, property);

    InspectorTripleSpinBox *spin_right = initializeEmptySpinBox(property, font, property->getValue().toVector()[1].toDouble());
    spin_right->setObjectName("spinBool");
    spin_right->setFixedHeight(22);
    spin_right->setPrefix(QString::fromStdString(property->getValue().toVector()[5].toString()));
    spin_right->setRange(property->getValue().toVector()[2].toDouble(), property->getValue().toVector()[3].toDouble());
    spin_right->setSingleStep(property->getValue().toVector()[4].toDouble());
    spin_right->setProperty(User_Property::CompKey,  QVariant::fromValue(       property->getParentComponent()->getComponentKey()) );
    spin_right->setProperty(User_Property::CompName, QString::fromStdString(    property->getParentComponent()->getComponentName()) );
    spin_right->setProperty(User_Property::PropKey,  QVariant::fromValue(       property->getPropertyKey()) );
    spin_right->setProperty(User_Property::PropName, QString::fromStdString(    property->getPropertyName()) );
    spin_right->setEnabled(property->getValue().toVector()[0].toBool());

    horizontal_split->addWidget(check_left);
    horizontal_split->addWidget(spin_right);

    check_left->setProperty(User_Property::Order, 0);
    spin_right->setProperty(User_Property::Order, 1);
    addToWidgetList(check_left);
    addToWidgetList(spin_right);

    // This stops mouse wheel from stealing focus unless user has selected the widget
    spin_right->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
    spin_right->installEventFilter(new DrFilterMouseWheelAdjustmentGuard(spin_right));

    connect (check_left, &QCheckBox::toggled, [this, property, spin_right](bool checked) {
        updateSettingsFromNewValue( property->getCompPropPair(), checked );
        spin_right->setEnabled( checked );
    });
    connect (spin_right, QOverload<double>::of(&InspectorTripleSpinBox::valueChanged),
         this, [this, property] (double d) { updateSettingsFromNewValue(property->getCompPropPair(), d, 1); });

    return spin_pair;
}


//####################################################################################
//##    Pair of Check Box / In Spin Box, uses Property_Type::BoolInt
//##        std::vector<DrVariant> of 6 values: bool, int value, min, max, int step size, string spinText
//####################################################################################
QFrame* TreeInspector::createCheckBoxIntBoxPair(DrProperty *property, QFont &font, QSizePolicy size_policy) {

    QFrame *spin_pair = new QFrame();
    spin_pair->setFixedHeight(25);
    spin_pair->setSizePolicy(size_policy);

    QHBoxLayout *horizontal_split = new QHBoxLayout(spin_pair);
    horizontal_split->setAlignment(Qt::AlignVCenter);
    horizontal_split->setSpacing(2);
    horizontal_split->setContentsMargins(0,0,0,0);

    InspectorCheckBox *check_left = new InspectorCheckBox();
    check_left->setFixedWidth(36);
    check_left->setFixedHeight(22);
    check_left->setObjectName("checkInspector");
    check_left->setFont(font);
    check_left->setDrawLeft(0);
    check_left->setDrawTop(2);
    check_left->setTristate(false);
    check_left->setProperty(User_Property::Mouse_Over, false);              // Initialize some mouse user data, DrFilterHoverHandler updates this info,
    check_left->setProperty(User_Property::Mouse_Pos, QPoint(0, 0));        // Used to track when the mouse is within the indicator area for custom paint event
    check_left->setProperty(User_Property::CompKey,  QVariant::fromValue(       property->getParentComponent()->getComponentKey()) );
    check_left->setProperty(User_Property::CompName, QString::fromStdString(    property->getParentComponent()->getComponentName()) );
    check_left->setProperty(User_Property::PropKey,  QVariant::fromValue(       property->getPropertyKey()) );
    check_left->setProperty(User_Property::PropName, QString::fromStdString(    property->getPropertyName()) );
    check_left->setChecked(property->getValue().toVector()[0].toBool());
    getHoverHandler()->attachToHoverHandler(check_left, property);

    QSpinBox *spin_right = new QSpinBox();
    spin_right->setObjectName("spinBool");
    spin_right->setFont(font);
    spin_right->setAttribute(Qt::WA_MacShowFocusRect, 0);
    spin_right->setMinimumWidth(50);
    spin_right->setFixedHeight(22);
    spin_right->setButtonSymbols(QAbstractSpinBox::ButtonSymbols::NoButtons);
    spin_right->setPrefix(QString::fromStdString(property->getValue().toVector()[5].toString()));
    spin_right->setRange(property->getValue().toVector()[2].toInt(), property->getValue().toVector()[3].toInt());
    spin_right->setSingleStep(property->getValue().toVector()[4].toInt());
    spin_right->setValue(property->getValue().toVector()[1].toInt());
    spin_right->setProperty(User_Property::CompKey,  QVariant::fromValue(       property->getParentComponent()->getComponentKey()) );
    spin_right->setProperty(User_Property::CompName, QString::fromStdString(    property->getParentComponent()->getComponentName()) );
    spin_right->setProperty(User_Property::PropKey,  QVariant::fromValue(       property->getPropertyKey()) );
    spin_right->setProperty(User_Property::PropName, QString::fromStdString(    property->getPropertyName()) );
    spin_right->setEnabled(property->getValue().toVector()[0].toBool());
    getHoverHandler()->attachToHoverHandler(spin_right, property);

    horizontal_split->addWidget(check_left);
    horizontal_split->addWidget(spin_right);

    check_left->setProperty(User_Property::Order, 0);
    spin_right->setProperty(User_Property::Order, 1);
    addToWidgetList(check_left);
    addToWidgetList(spin_right);

    // This stops mouse wheel from stealing focus unless user has selected the widget
    spin_right->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
    spin_right->installEventFilter(new DrFilterMouseWheelAdjustmentGuard(spin_right));

    connect (check_left, &QCheckBox::toggled, [this, property, spin_right](bool checked) {
        updateSettingsFromNewValue( property->getCompPropPair(), checked );
        spin_right->setEnabled( checked );
    });
    connect (spin_right, QOverload<int>::of(&QSpinBox::valueChanged),
         this, [this, property] (int i) { updateSettingsFromNewValue(property->getCompPropPair(), i, 1); });

    return spin_pair;
}












