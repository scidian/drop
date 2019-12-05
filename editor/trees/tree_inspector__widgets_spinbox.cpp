//
//      Created by Stephens Nunnally on 1/26/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Object Inspector Spin Boxes
//
//
#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QSpinBox>
#include <QVector3D>

#include "editor/event_filters.h"
#include "editor/globals_editor.h"
#include "editor/helper_editor.h"
#include "editor/trees/tree_inspector.h"
#include "helper.h"
#include "model/settings/settings_component_property.h"


//####################################################################################
//##    Integer Spin Box
//####################################################################################
QSpinBox* TreeInspector::createIntSpinBox(DrProperty *property, QFont &font, QSizePolicy size_policy, Property_Type spin_type) {

    int property_value;
    if (spin_type == Property_Type::RangedInt)
        property_value = property->getValue().toList().first().toInt();
    else
        property_value = property->getValue().toInt();

    QSpinBox *spin = new QSpinBox();
    spin->setFixedHeight(22);
    spin->setFont(font);
    spin->setSizePolicy(size_policy);
    spin->setAttribute(Qt::WA_MacShowFocusRect, 0);
    switch (spin_type) {
        case Property_Type::Int:            spin->setRange(-100000000, 100000000);      spin->setSingleStep(  5 );      break;
        case Property_Type::Positive:       spin->setRange(0, 100000000);               spin->setSingleStep(  5 );      break;
        case Property_Type::RangedInt:
            spin->setRange(property->getValue().toList().at(1).toInt(), property->getValue().toList().at(2).toInt());
            spin->setSingleStep(property->getValue().toList().at(3).toInt());
            break;
        default:                            spin->setRange(-100000000, 100000000);
    }
    spin->setButtonSymbols(QAbstractSpinBox::ButtonSymbols::NoButtons);

    long property_key = property->getPropertyKey();
    spin->setProperty(User_Property::Key, QVariant::fromValue( property_key ));
    spin->setValue( property_value );

    getHoverHandler()->attachToHoverHandler(spin, property);
    addToWidgetList(spin);

    // This stops mouse wheel from stealing focus unless user has selected the widget
    spin->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
    spin->installEventFilter(new DrFilterMouseWheelAdjustmentGuard(spin));

    connect (spin,  QOverload<int>::of(&QSpinBox::valueChanged),
             this, [this, property_key] (int i) { updateSettingsFromNewValue(property_key, i); });

    return spin;
}


//####################################################################################
//##    Double Spin Box
//####################################################################################
// SIGNAL: void QDoubleSpinBox::valueChanged(double d)
QDoubleSpinBox* TreeInspector::createDoubleSpinBox(DrProperty *property, QFont &font, QSizePolicy size_policy, Property_Type spin_type) {
    ///// Could also try to use a QLineEdit with a QValidator
    ///myLineEdit->setValidator( new QDoubleValidator(0, 100, 2, this) );

    double property_value;
    if (spin_type == Property_Type::RangedDouble)
        property_value = property->getValue().toList().first().toDouble();
    else
        property_value = property->getValue().toDouble();

    DrQTripleSpinBox *spin = new DrQTripleSpinBox();
    spin->setFixedHeight(22);
    spin->setFont(font);
    spin->setSizePolicy(size_policy);
    spin->setDecimals(3);
    spin->setAttribute(Qt::WA_MacShowFocusRect, 0);
    switch (spin_type) {
        case Property_Type::Double:         spin->setRange(-100000000, 100000000);              spin->setSingleStep(1.0);   break;
        case Property_Type::PositiveDouble: spin->setRange(0, 100000000);                       spin->setSingleStep(0.1);   break;
        case Property_Type::Percent:        spin->setRange(0, 100);     spin->setSuffix("%");   spin->setSingleStep(5);     break;
        case Property_Type::Angle:          spin->setRange(-360, 360);  spin->setSuffix("°");   spin->setSingleStep(5);     break;
        case Property_Type::RangedDouble:
            spin->setRange(property->getValue().toList().at(1).toDouble(), property->getValue().toList().at(2).toDouble());
            spin->setSingleStep(property->getValue().toList().at(3).toDouble());
            break;
        default:                            spin->setRange(-100000000, 100000000);
    }
    spin->setButtonSymbols(QAbstractSpinBox::ButtonSymbols::NoButtons);     // Hides little up / down buttons

    // Store property key within item, set initial starting value of spin box
    long property_key = property->getPropertyKey();
    spin->setProperty(User_Property::Key, QVariant::fromValue( property_key ));
    spin->setValue( property_value );

    // Connect HoverHandler with proper text, add this widget to list of widgets in Inspector
    getHoverHandler()->attachToHoverHandler(spin, property);
    addToWidgetList(spin);

    // This stops mouse wheel from stealing focus unless user has selected the widget
    spin->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
    spin->installEventFilter(new DrFilterMouseWheelAdjustmentGuard(spin));

    // Connect value changed to our handler function
    connect (spin,  QOverload<double>::of(&DrQTripleSpinBox::valueChanged),
             this, [this, property_key] (double d) { updateSettingsFromNewValue(property_key, d); });

    return spin;
}


//####################################################################################
//##    Pair of Double Spin Boxes
//####################################################################################
QFrame* TreeInspector::createDoubleSpinBoxPair(DrProperty *property, QFont &font, QSizePolicy size_policy, Property_Type spin_type) {
    QFrame *spin_pair = new QFrame();
    spin_pair->setFixedHeight(24);
    spin_pair->setSizePolicy(size_policy);

    QHBoxLayout *horizontal_split = new QHBoxLayout(spin_pair);
    horizontal_split->setSpacing(6);
    horizontal_split->setContentsMargins(0,0,0,0);

    DrQTripleSpinBox *spin_left  =  initializeEmptySpinBox(property, font, property->getValue().toPointF().x());
    DrQTripleSpinBox *spin_right;

    if (spin_type == Property_Type::PositionF)
        spin_right  = initializeEmptySpinBox(property, font, -1 * property->getValue().toPointF().y());
    else
        spin_right  = initializeEmptySpinBox(property, font, property->getValue().toPointF().y());

    spin_left->setFixedHeight(22);
    spin_right->setFixedHeight(22);

    switch (spin_type) {
        case Property_Type::PositionF:
        case Property_Type::PointF:
        case Property_Type::ScaleF:
            spin_left->setPrefix("X: ");        spin_right->setPrefix("Y: ");   break;
        case Property_Type::PositiveScaleF:
            spin_left->setPrefix("X: ");        spin_right->setPrefix("Y: ");
            spin_left->setRange( 0.0, 100000000);
            spin_right->setRange(0.0, 100000000);
            break;
        case Property_Type::GridF:
            spin_left->setPrefix("W: ");        spin_right->setPrefix("H: ");
            spin_left->setRange( c_minimum_grid_size, 100000000);
            spin_right->setRange(c_minimum_grid_size, 100000000);
            break;
        case Property_Type::GridScaleF:
            spin_left->setPrefix("X: ");        spin_right->setPrefix("Y: ");
            spin_left->setRange( c_minimum_grid_scale, 100000000);
            spin_right->setRange(c_minimum_grid_scale, 100000000);
            break;
        case Property_Type::SizeF:
            spin_left->setPrefix("W: ");        spin_right->setPrefix("H: ");   break;
        case Property_Type::PositiveSizeF:
            spin_left->setPrefix("W: ");        spin_right->setPrefix("H: ");
            spin_left->setRange( 1.0, 100000000);
            spin_right->setRange(1.0, 100000000);
            break;
        default: ;
    }
    if (spin_type == Property_Type::ScaleF || spin_type == Property_Type::GridScaleF || spin_type == Property_Type::PositiveScaleF) {
        spin_left->setSingleStep(.1);
        spin_right->setSingleStep(.1);
    } else if (spin_type == Property_Type::PositiveSizeF) {
        spin_left->setSingleStep(1.0);
        spin_right->setSingleStep(1.0);
    } else {
        spin_left->setSingleStep(5.0);
        spin_right->setSingleStep(5.0);
    }

    horizontal_split->addWidget(spin_left);
    horizontal_split->addWidget(spin_right);

    long property_key = property->getPropertyKey();

    spin_left->setProperty(User_Property::Key, QVariant::fromValue( property_key ));
    spin_right->setProperty(User_Property::Key, QVariant::fromValue( property_key ));
    spin_left->setProperty(User_Property::Order, 0);
    spin_right->setProperty(User_Property::Order, 1);
    addToWidgetList(spin_left);
    addToWidgetList(spin_right);

    // This stops mouse wheel from stealing focus unless user has selected the widget
    spin_left->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
    spin_right->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
    spin_left->installEventFilter(new DrFilterMouseWheelAdjustmentGuard(spin_left));
    spin_right->installEventFilter(new DrFilterMouseWheelAdjustmentGuard(spin_right));

    connect (spin_left,  QOverload<double>::of(&DrQTripleSpinBox::valueChanged),
         this, [this, property_key] (double d) { updateSettingsFromNewValue(property_key, d, 0); });
    connect (spin_right, QOverload<double>::of(&DrQTripleSpinBox::valueChanged),
         this, [this, property_key] (double d) { updateSettingsFromNewValue(property_key, d, 1); });

    return spin_pair;
}



//####################################################################################
//##    Trio of Double Spin Boxes, great for QVector3D
//####################################################################################
QFrame* TreeInspector::createDoubleSpinBoxTrio(DrProperty *property, QFont &font, QSizePolicy size_policy, Property_Type spin_type) {
    QFrame *spin_trio = new QFrame();
    spin_trio->setFixedHeight(24);
    spin_trio->setSizePolicy(size_policy);

    QHBoxLayout *horizontal_split = new QHBoxLayout(spin_trio);
    horizontal_split->setSpacing(6);
    horizontal_split->setContentsMargins(0,0,0,0);

    QVector3D property_value = property->getValue().value<QVector3D>();
    double x = static_cast<double>(property_value.x());
    double y = static_cast<double>(property_value.y());
    double z = static_cast<double>(property_value.z());

    DrQTripleSpinBox *spin_x = initializeEmptySpinBox(property, font, x);
    DrQTripleSpinBox *spin_y = initializeEmptySpinBox(property, font, y);
    DrQTripleSpinBox *spin_z = initializeEmptySpinBox(property, font, z);

    spin_x->setFixedHeight(22);
    spin_y->setFixedHeight(22);
    spin_z->setFixedHeight(22);

    switch (spin_type) {
        case Property_Type::Point3D:
            spin_x->setPrefix("X: ");
            spin_y->setPrefix("Y: ");
            spin_z->setPrefix("Z: ");
            spin_x->setRange(-1000000, 1000000);
            spin_y->setRange(-1000000, 1000000);
            spin_z->setRange(-1000000, 1000000);
            spin_x->setSingleStep(5.0);
            spin_y->setSingleStep(5.0);
            spin_z->setSingleStep(5.0);
            break;
        default: ;
    }

    horizontal_split->addWidget(spin_x);
    horizontal_split->addWidget(spin_y);
    horizontal_split->addWidget(spin_z);

    long property_key = property->getPropertyKey();

    spin_x->setProperty(User_Property::Key, QVariant::fromValue( property_key ));
    spin_y->setProperty(User_Property::Key, QVariant::fromValue( property_key ));
    spin_z->setProperty(User_Property::Key, QVariant::fromValue( property_key ));
    spin_x->setProperty(User_Property::Order, 0);
    spin_y->setProperty(User_Property::Order, 1);
    spin_z->setProperty(User_Property::Order, 2);
    addToWidgetList(spin_x);
    addToWidgetList(spin_y);
    addToWidgetList(spin_z);

    // This stops mouse wheel from stealing focus unless user has selected the widget
    spin_x->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
    spin_y->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
    spin_z->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
    spin_x->installEventFilter(new DrFilterMouseWheelAdjustmentGuard(spin_x));
    spin_y->installEventFilter(new DrFilterMouseWheelAdjustmentGuard(spin_y));
    spin_z->installEventFilter(new DrFilterMouseWheelAdjustmentGuard(spin_z));

    connect (spin_x,  QOverload<double>::of(&DrQTripleSpinBox::valueChanged),
         this, [this, property_key] (double d) { updateSettingsFromNewValue(property_key, d, 0); });
    connect (spin_y,  QOverload<double>::of(&DrQTripleSpinBox::valueChanged),
         this, [this, property_key] (double d) { updateSettingsFromNewValue(property_key, d, 1); });
    connect (spin_z, QOverload<double>::of(&DrQTripleSpinBox::valueChanged),
         this, [this, property_key] (double d) { updateSettingsFromNewValue(property_key, d, 2); });

    return spin_trio;
}


//####################################################################################
//##    Pair of Double Spin Boxes, with +/- in between
//####################################################################################
QFrame* TreeInspector::createVariableSpinBoxPair(DrProperty *property, QFont &font, QSizePolicy size_policy) {
    QFrame *spin_pair = new QFrame();
    spin_pair->setFixedHeight(24);
    spin_pair->setSizePolicy(size_policy);

    QHBoxLayout *horizontal_split = new QHBoxLayout(spin_pair);
    horizontal_split->setSpacing(6);
    horizontal_split->setContentsMargins(0,0,0,0);

    DrQTripleSpinBox *spin_left  = initializeEmptySpinBox(property, font, property->getValue().toPointF().x());
    size_policy.setHorizontalStretch(4);
    spin_left->setSizePolicy(size_policy);

    QLabel *variable_sign = new QLabel("+ / -");
    variable_sign->setFont(font);
    size_policy.setHorizontalStretch(1);
    variable_sign->setSizePolicy(size_policy);
    getHoverHandler()->attachToHoverHandler(variable_sign, Advisor_Info::Variable_Widget);
    DrQTripleSpinBox *spin_right  = initializeEmptySpinBox(property, font, property->getValue().toPointF().y());
    spin_right->setMinimum(0);
    size_policy.setHorizontalStretch(3);
    spin_right->setSizePolicy(size_policy);

    spin_left->setFixedHeight(22);
    spin_right->setFixedHeight(22);

    horizontal_split->addWidget(spin_left);
    horizontal_split->addWidget(variable_sign);
    horizontal_split->addWidget(spin_right);

    long property_key = property->getPropertyKey();

    spin_left->setProperty(User_Property::Key, QVariant::fromValue( property_key ));
    spin_right->setProperty(User_Property::Key, QVariant::fromValue( property_key));
    spin_left->setProperty(User_Property::Order, 0);
    spin_right->setProperty(User_Property::Order, 1);
    addToWidgetList(spin_left);
    addToWidgetList(spin_right);

    // This stops mouse wheel from stealing focus unless user has selected the widget
    spin_left->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
    spin_right->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
    spin_left->installEventFilter(new DrFilterMouseWheelAdjustmentGuard(spin_left));
    spin_right->installEventFilter(new DrFilterMouseWheelAdjustmentGuard(spin_right));

    connect(spin_left,  QOverload<double>::of(&DrQTripleSpinBox::valueChanged),
            this, [this, property_key] (double d) { updateSettingsFromNewValue(property_key, d, 0); });
    connect(spin_right, QOverload<double>::of(&DrQTripleSpinBox::valueChanged),
            this, [this, property_key] (double d) { updateSettingsFromNewValue(property_key, d, 1); });

    return spin_pair;
}


//####################################################################################
//##    Spin Box Helper Functions
//####################################################################################
DrQTripleSpinBox* TreeInspector::initializeEmptySpinBox(DrProperty *property, QFont &font, double start_value) {
    DrQTripleSpinBox *new_spin  = new DrQTripleSpinBox();
    new_spin->setFont(font);
    new_spin->setAttribute(Qt::WA_MacShowFocusRect, 0);
    new_spin->setMinimumWidth(50);
    new_spin->setDecimals(3);
    new_spin->setRange(-100000000, 100000000);
    new_spin->setButtonSymbols(QAbstractSpinBox::ButtonSymbols::NoButtons);
    new_spin->setValue(start_value);
    getHoverHandler()->attachToHoverHandler(new_spin, property);
    return new_spin;
}

// Catches event that Qt fires when value has changed to trim any excess zeros from the right side
QString DrQTripleSpinBox::textFromValue(double value) const {
    return Dr::RemoveTrailingDecimals(value, decimals());
}


//####################################################################################
//##    Spin Box w/Slider Combo
//####################################################################################
QWidget* TreeInspector::createSlider(DrProperty *property, QFont &font, QSizePolicy size_policy, Property_Type spin_type) {
    long property_key = property->getPropertyKey();

    double property_value;
    if (spin_type == Property_Type::Slider)
        property_value = property->getValue().toList().first().toDouble();
    else
        property_value = property->getValue().toDouble();

    QWidget *slider_pair = new QWidget();
    slider_pair->setMaximumHeight(25);
    slider_pair->setSizePolicy(size_policy);
    getHoverHandler()->attachToHoverHandler(slider_pair, property);                            // Connect to hover handler for advisor

    QHBoxLayout *horizontal_split = new QHBoxLayout(slider_pair);
    horizontal_split->setSpacing(6);
    horizontal_split->setContentsMargins(0,0,0,0);

        DrQTripleSpinBox *spin = new DrQTripleSpinBox();
        spin->setFont(font);
        spin->setAttribute(Qt::WA_MacShowFocusRect, 0);
        spin->setFixedHeight(22);
        spin->setMinimumWidth(40);
        size_policy.setHorizontalStretch(1);
        spin->setSizePolicy(size_policy);
        spin->setDecimals(3);
        switch (spin_type) {
            case Property_Type::Double:         spin->setRange(-100000000, 100000000);              spin->setSingleStep(5);     break;
            case Property_Type::Percent:        spin->setRange(   0, 100); spin->setSuffix("%");    spin->setSingleStep(5);     break;
            case Property_Type::Angle:          spin->setRange(-360, 360); spin->setSuffix("°");    spin->setSingleStep(5);     break;
            case Property_Type::Slider:
                spin->setRange(property->getValue().toList().at(1).toDouble(),
                               property->getValue().toList().at(2).toDouble());
                spin->setSingleStep(property->getValue().toList().at(3).toDouble());
                spin->setSuffix(property->getValue().toList().at(4).toString());
                break;

            default:                            spin->setRange(-100000000, 100000000);
        }
        spin->setButtonSymbols(QAbstractSpinBox::ButtonSymbols::NoButtons);                 // Hides little up / down buttons
        spin->setProperty(User_Property::Key, QVariant::fromValue( property_key ));         // Store property key within item
        spin->setProperty(User_Property::Order, 0);
        spin->setValue(property_value);                                                     // Set initial starting value of spin box
        this->addToWidgetList(spin);

        // This stops mouse wheel from stealing focus unless user has selected the widget
        spin->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
        spin->installEventFilter(new DrFilterMouseWheelAdjustmentGuard(spin));

        QSlider *slider = new QSlider(Qt::Orientation::Horizontal);
        slider->setMaximumHeight(26);
        size_policy.setHorizontalStretch(3);
        slider->setSizePolicy(size_policy);
        slider->setTickPosition(QSlider::TickPosition::NoTicks);
        slider->setRange( static_cast<int>(spin->minimum()), static_cast<int>(spin->maximum()) );
        slider->setValue( static_cast<int>(property_value));
        slider->setProperty(User_Property::Key, QVariant::fromValue( property_key ));       // Store property key within item
        slider->setProperty(User_Property::Order, 2);
        this->addToWidgetList(slider);

    // Connect value changed to our handler function
    connect (spin,  QOverload<double>::of(&DrQTripleSpinBox::valueChanged), this, [this, property_key, slider] (double d) {
        slider->blockSignals(true);
        slider->setValue( static_cast<int>(d) );
        updateSettingsFromNewValue(property_key, d);
        slider->blockSignals(false);
    });

    connect(slider, &QSlider::valueChanged, this, [spin, slider] () {
        spin->setValue( slider->value() );
    });

    horizontal_split->addWidget(spin);
    horizontal_split->addWidget(slider);

    return slider_pair;
}












