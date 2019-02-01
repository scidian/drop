//
//      Created by Stephens Nunnally on 1/26/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//

#include "library.h"

#include "project.h"
#include "project_world.h"
#include "project_world_scene.h"
#include "project_world_scene_object.h"

#include "settings.h"
#include "settings_component.h"
#include "settings_component_property.h"

#include "editor_tree_inspector.h"
#include "interface_relay.h"


//####################################################################################
//##        Property Row Building Functions
//####################################################################################

QCheckBox* TreeInspector::createCheckBox(DrProperty *property, QFont &font)
{
    QSizePolicy sp_right(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sp_right.setHorizontalStretch(INSPECTOR_SIZE_RIGHT);

    QCheckBox *check = new QCheckBox();
    check->setObjectName("checkInspector");
    check->setFont(font);
    check->setSizePolicy(sp_right);
    check->setTristate(false);

    check->setProperty(User_Property::Key, QVariant::fromValue( property->getPropertyKey() ));
    check->setChecked(property->getValue().toBool());

    applyHeaderBodyProperties(check, property);
    addToWidgetList(check);
    return check;
}

QLineEdit* TreeInspector::createLineEdit(DrProperty *property, QFont &font)
{
    QSizePolicy sp_right(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sp_right.setHorizontalStretch(INSPECTOR_SIZE_RIGHT);

    QLineEdit *edit = new QLineEdit();
    edit->setFont(font);
    edit->setSizePolicy(sp_right);

    edit->setProperty(User_Property::Key, QVariant::fromValue( property->getPropertyKey() ));
    edit->setText(property->getValue().toString());

    applyHeaderBodyProperties(edit, property);
    addToWidgetList(edit);
    return edit;
}

QSpinBox* TreeInspector::createIntSpinBox(DrProperty *property, QFont &font, Spin_Type spin_type)
{
    QSizePolicy size_policy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    size_policy.setHorizontalStretch(INSPECTOR_SIZE_RIGHT);

    QSpinBox *spin = new QSpinBox();
    spin->setFont(font);
    spin->setSizePolicy(size_policy);
    switch (spin_type)
    {
    case Spin_Type::Integer:    spin->setRange(-100000000, 100000000);         break;
    case Spin_Type::Positive:   spin->setRange(0, 100000000);                  break;
    default:                    spin->setRange(-100000000, 100000000);
    }
    spin->setButtonSymbols(QAbstractSpinBox::ButtonSymbols::NoButtons);

    spin->setProperty(User_Property::Key, QVariant::fromValue( property->getPropertyKey() ));
    spin->setValue(property->getValue().toInt());

    applyHeaderBodyProperties(spin, property);
    addToWidgetList(spin);
    return spin;
}

QDoubleSpinBox* TreeInspector::createDoubleSpinBox(DrProperty *property, QFont &font, Spin_Type spin_type)
{
    ///// Could also try to use a QLineEdit with a QValidator
    ///myLineEdit->setValidator( new QDoubleValidator(0, 100, 2, this) );

    QSizePolicy size_policy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    size_policy.setHorizontalStretch(INSPECTOR_SIZE_RIGHT);

    TripleSpinBox *spin = new TripleSpinBox();
    spin->setFont(font);
    spin->setSizePolicy(size_policy);
    spin->setDecimals(3);
    switch (spin_type)
    {
    case Spin_Type::Float:      spin->setRange(-100000000, 100000000);                  break;
    case Spin_Type::Percent:    spin->setRange(0, 100);     spin->setSuffix("%");       break;
    case Spin_Type::Angle:      spin->setRange(-360, 360);  spin->setSuffix("°");       break;
    default:                    spin->setRange(-100000000, 100000000);
    }
    spin->setButtonSymbols(QAbstractSpinBox::ButtonSymbols::NoButtons);

    spin->setProperty(User_Property::Key, QVariant::fromValue( property->getPropertyKey() ));
    spin->setValue(property->getValue().toDouble());

    applyHeaderBodyProperties(spin, property);
    addToWidgetList(spin);
    return spin;
}

QFrame* TreeInspector::createDoubleSpinBoxPair(DrProperty *property, QFont &font, Spin_Type spin_type)
{
    QSizePolicy size_policy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    size_policy.setHorizontalStretch(INSPECTOR_SIZE_RIGHT);

    QFrame *spin_pair = new QFrame();
    spin_pair->setSizePolicy(size_policy);

    QBoxLayout *horizontal_split = new QHBoxLayout(spin_pair);
    horizontal_split->setSpacing(6);
    horizontal_split->setContentsMargins(0,0,0,0);

    TripleSpinBox *spin_left  =  initializeEmptySpinBox(property, font, property->getValue().toPointF().x());
    TripleSpinBox *spin_right  = initializeEmptySpinBox(property, font, property->getValue().toPointF().y());

    switch (spin_type)
    {
    case Spin_Type::Point:  spin_left->setPrefix("X: ");    spin_right->setPrefix("Y: ");   break;
    case Spin_Type::Size:   spin_left->setPrefix("W: ");    spin_right->setPrefix("H: ");   break;
    default: ;
    }

    horizontal_split->addWidget(spin_left);
    horizontal_split->addWidget(spin_right);

    spin_left->setProperty(User_Property::Key, QVariant::fromValue( property->getPropertyKey() ));
    spin_left->setProperty(User_Property::First, true);
    addToWidgetList(spin_left);

    spin_right->setProperty(User_Property::Key, QVariant::fromValue( property->getPropertyKey() ));
    spin_right->setProperty(User_Property::First, false);
    addToWidgetList(spin_right);
    return spin_pair;
}

QFrame* TreeInspector::createVariableSpinBoxPair(DrProperty *property, QFont &font)
{
    QSizePolicy size_policy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    size_policy.setHorizontalStretch(INSPECTOR_SIZE_RIGHT);

    QFrame *spin_pair = new QFrame();
    spin_pair->setSizePolicy(size_policy);

    QBoxLayout *horizontal_split = new QHBoxLayout(spin_pair);
    horizontal_split->setSpacing(6);
    horizontal_split->setContentsMargins(0,0,0,0);

    TripleSpinBox *spin_left  = initializeEmptySpinBox(property, font, property->getValue().toPointF().x());
    size_policy.setHorizontalStretch(4);
    spin_left->setSizePolicy(size_policy);

    QLabel *variable_sign = new QLabel("+ / -");
    variable_sign->setFont(font);
    size_policy.setHorizontalStretch(1);
    variable_sign->setSizePolicy(size_policy);
    applyHeaderBodyProperties(variable_sign, "Variable Amount", "Plus or minus modifier to initial value, the following value allows for some "
                                                                "variable amount to the initial value. For example, an initial value of 100 with "
                                                                "a variable amount of 5, allows for values ranging from 95 to 105.");
    TripleSpinBox *spin_right  = initializeEmptySpinBox(property, font, property->getValue().toPointF().y());
    spin_right->setMinimum(0);
    size_policy.setHorizontalStretch(3);
    spin_right->setSizePolicy(size_policy);

    horizontal_split->addWidget(spin_left);
    horizontal_split->addWidget(variable_sign);
    horizontal_split->addWidget(spin_right);

    spin_left->setProperty(User_Property::Key, QVariant::fromValue( property->getPropertyKey() ));
    spin_left->setProperty(User_Property::First, true);
    addToWidgetList(spin_left);

    spin_right->setProperty(User_Property::Key, QVariant::fromValue( property->getPropertyKey() ));
    spin_right->setProperty(User_Property::First, false);
    addToWidgetList(spin_right);
    return spin_pair;
}

TripleSpinBox* TreeInspector::initializeEmptySpinBox(DrProperty *property, QFont &font, double start_value)
{
    TripleSpinBox *new_spin  = new TripleSpinBox();
    new_spin->setFont(font);
    new_spin->setMinimumWidth(50);
    new_spin->setDecimals(3);
    new_spin->setRange(-100000000, 100000000);
    new_spin->setButtonSymbols(QAbstractSpinBox::ButtonSymbols::NoButtons);
    new_spin->setValue(start_value);
    applyHeaderBodyProperties(new_spin, property);
    return new_spin;
}


QComboBox* TreeInspector::createComboBox(DrProperty *property, QFont &font)
{
    QSizePolicy size_policy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    size_policy.setHorizontalStretch(INSPECTOR_SIZE_RIGHT);

    DropDownComboBox *combo = new DropDownComboBox();
    combo->setObjectName(QStringLiteral("comboBox"));
    combo->view()->parentWidget()->setStyleSheet(" background: none; border-radius: 0px; ");

    QString style =
            " QComboBox QAbstractItemView { "
            "       font-size: " + QString::number(font.pointSize()) + "px; "
            "       border: 2px solid; margin: 0px; "
            "       border-color: " + Dr::GetColor(Window_Colors::Icon_Dark).name() + "; } "

            " QComboBox QAbstractItemView::item { padding: 4px; "
            "       padding-left: 4px;"
            "       color: " + Dr::GetColor(Window_Colors::Text).name() + "; "
            "       background: " + Dr::GetColor(Window_Colors::Button_Light).name() + "; } "

            " QComboBox QAbstractItemView::item:selected { "
            "       padding-left: 6px; "
            "       image: url(:/gui_misc/check.png); "
            "       image-position: right center; "
            "       color: " + Dr::GetColor(Window_Colors::Highlight).name() + "; "
            "       background: " + Dr::GetColor(Window_Colors::Shadow).name() + "; } ";

    QListView *combo_list = new QListView();
    combo->setView(combo_list);
    combo->setStyleSheet(style);

    //// Alternate way to remove white border around QComboBox ListView
    ///combo->setEditable(true);
    ///combo->lineEdit()->setReadOnly(true);
    ///combo->lineEdit()->setDisabled(true);

    combo->setFont(font);
    combo->setSizePolicy(size_policy);
    QStringList options;
    if (property->getPropertyKey() == static_cast<int>(Object_Properties::damage)) {
        options << tr("No Damage") << tr("Damage Player") << tr("Damage Enemy") << tr("Damage All");
    } else {
        options << tr("Unknown List");
    }
    combo->addItems(options);
    ///connect(combo, SIGNAL(currentIndexChanged(QString)), this, SLOT(sceneScaleChanged(QString)));

    combo->setProperty(User_Property::Key, QVariant::fromValue( property->getPropertyKey() ));
    combo->setCurrentIndex(property->getValue().toInt());

    applyHeaderBodyProperties(combo, property);
    addToWidgetList(combo);
    return combo;
}


//####################################################################################
//##
//##    TripleSpinBox Class Functions
//##
//####################################################################################
QString TripleSpinBox::textFromValue(double value) const
{
    ///double intpart;
    ///if (std::modf(value, &intpart) == 0.0)              return QWidget::locale().toString(value, QLatin1Char('f').unicode(), 0);
    ///else if (std::modf(value * 10, &intpart) == 0.0)    return QWidget::locale().toString(value, QLatin1Char('f').unicode(), 1);
    ///else if (std::modf(value * 100, &intpart) == 0.0)   return QWidget::locale().toString(value, QLatin1Char('f').unicode(), 2);
    ///else                                                return QWidget::locale().toString(value, QLatin1Char('f').unicode(), decimals());
    return Dr::RemoveTrailingDecimals(value, decimals());
}



//####################################################################################
//##
//##    DropDownComboBox Class Functions
//##
//####################################################################################
void DropDownComboBox::showPopup()
{
    bool oldAnimationEffects = qApp->isEffectEnabled(Qt::UI_AnimateCombo);
    qApp->setEffectEnabled(Qt::UI_AnimateCombo, false);

    QComboBox::showPopup();
    QWidget *frame = findChild<QFrame*>();
    frame->move( frame->x() + 7, mapToGlobal(this->geometry().bottomLeft()).y() - 1);

    qApp->setEffectEnabled(Qt::UI_AnimateCombo, oldAnimationEffects);
}








