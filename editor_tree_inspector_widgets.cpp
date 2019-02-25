//
//      Created by Stephens Nunnally on 1/26/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QApplication>
#include <QAction>
#include <QActionGroup>
#include <QLabel>
#include <QLineEdit>
#include <QListView>
#include <QMenu>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "colors.h"
#include "debug.h"

#include "editor_tree_inspector.h"
#include "editor_tree_widgets.h"

#include "interface_relay.h"
#include "library.h"

#include "project.h"
#include "project_world.h"
#include "project_world_stage.h"
#include "project_world_stage_object.h"
#include "settings.h"
#include "settings_component.h"
#include "settings_component_property.h"

//####################################################################################
//##        Property Row Building Functions
//####################################################################################

// SIGNAL: void QCheckBox::stateChanged(int state)
QCheckBox* TreeInspector::createCheckBox(DrProperty *property, QFont &font)
{
    QSizePolicy sp_right(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sp_right.setHorizontalStretch(c_inspector_size_right);

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

// SIGNAL: void QLineEdit::editingFinished()
QLineEdit* TreeInspector::createLineEdit(DrProperty *property, QFont &font)
{
    QSizePolicy sp_right(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sp_right.setHorizontalStretch(c_inspector_size_right);

    QLineEdit *edit = new QLineEdit();
    edit->setFont(font);
    edit->setSizePolicy(sp_right);

    long property_key = property->getPropertyKey();

    edit->setProperty(User_Property::Key, QVariant::fromValue( property_key ));
    edit->setText(property->getValue().toString());

    applyHeaderBodyProperties(edit, property);
    addToWidgetList(edit);

    connect (edit,  &QLineEdit::editingFinished,
             this, [this, property_key, edit] () { updateSettingsFromNewValue(property_key, edit->text() ); });

    return edit;
}

// SIGNAL: void QSpinBox::valueChanged(int i)
QSpinBox* TreeInspector::createIntSpinBox(DrProperty *property, QFont &font, Spin_Type spin_type)
{
    QSizePolicy size_policy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    size_policy.setHorizontalStretch(c_inspector_size_right);

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

    long property_key = property->getPropertyKey();

    spin->setProperty(User_Property::Key, QVariant::fromValue( property_key ));
    spin->setValue(property->getValue().toInt());

    applyHeaderBodyProperties(spin, property);
    addToWidgetList(spin);

    // This stops mouse wheel from stealing focus unless user has selected the widget
    spin->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
    spin->installEventFilter(new MouseWheelWidgetAdjustmentGuard(spin));

    connect (spin,  QOverload<int>::of(&QSpinBox::valueChanged),
             this, [this, property_key] (int i) { updateSettingsFromNewValue(property_key, i); });

    return spin;
}

// SIGNAL: void QDoubleSpinBox::valueChanged(double d)
QDoubleSpinBox* TreeInspector::createDoubleSpinBox(DrProperty *property, QFont &font, Spin_Type spin_type)
{
    ///// Could also try to use a QLineEdit with a QValidator
    ///myLineEdit->setValidator( new QDoubleValidator(0, 100, 2, this) );

    QSizePolicy size_policy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    size_policy.setHorizontalStretch(c_inspector_size_right);

    TripleSpinBox *spin = new TripleSpinBox();
    spin->setFont(font);
    spin->setSizePolicy(size_policy);
    spin->setDecimals(3);
    switch (spin_type)
    {
    case Spin_Type::Float:      spin->setRange(-100000000, 100000000);              spin->setSingleStep(5);     break;
    case Spin_Type::Percent:    spin->setRange(0, 100);     spin->setSuffix("%");   spin->setSingleStep(5);     break;
    case Spin_Type::Angle:      spin->setRange(-360, 360);  spin->setSuffix("°");   spin->setSingleStep(5);     break;
    default:                    spin->setRange(-100000000, 100000000);
    }
    spin->setButtonSymbols(QAbstractSpinBox::ButtonSymbols::NoButtons);     // Hides little up / down buttons

    // Store property key within item, set initial starting value of spin box
    long property_key = property->getPropertyKey();
    spin->setProperty(User_Property::Key, QVariant::fromValue( property_key ));
    spin->setValue(property->getValue().toDouble());

    // Connect HoverHandler with proper text, add this widget to list of widgets in object inspector
    applyHeaderBodyProperties(spin, property);
    addToWidgetList(spin);

    // This stops mouse wheel from stealing focus unless user has selected the widget
    spin->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
    spin->installEventFilter(new MouseWheelWidgetAdjustmentGuard(spin));

    // Connect value changed to our handler function
    connect (spin,  QOverload<double>::of(&TripleSpinBox::valueChanged),
             this, [this, property_key] (double d) { updateSettingsFromNewValue(property_key, d); });

    return spin;
}

QFrame* TreeInspector::createDoubleSpinBoxPair(DrProperty *property, QFont &font, Spin_Type spin_type)
{
    QSizePolicy size_policy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    size_policy.setHorizontalStretch(c_inspector_size_right);

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
    if (spin_type == Spin_Type::Scale) {
        spin_left->setSingleStep(.1);
        spin_right->setSingleStep(.1);
    } else {
        spin_left->setSingleStep(5);
        spin_right->setSingleStep(5);
    }

    horizontal_split->addWidget(spin_left);
    horizontal_split->addWidget(spin_right);

    long property_key = property->getPropertyKey();

    spin_left->setProperty(User_Property::Key, QVariant::fromValue( property_key ));
    spin_left->setProperty(User_Property::Order, 0);
    addToWidgetList(spin_left);

    spin_right->setProperty(User_Property::Key, QVariant::fromValue( property_key ));
    spin_right->setProperty(User_Property::Order, 1);
    addToWidgetList(spin_right);

    // This stops mouse wheel from stealing focus unless user has selected the widget
    spin_left->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
    spin_left->installEventFilter(new MouseWheelWidgetAdjustmentGuard(spin_left));
    spin_right->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
    spin_right->installEventFilter(new MouseWheelWidgetAdjustmentGuard(spin_right));

    connect (spin_left,  QOverload<double>::of(&TripleSpinBox::valueChanged),
             this, [this, property_key] (double d) { updateSettingsFromNewValue(property_key, d, 0); });
    connect (spin_right, QOverload<double>::of(&TripleSpinBox::valueChanged),
             this, [this, property_key] (double d) { updateSettingsFromNewValue(property_key, d, 1); });


    return spin_pair;
}

QFrame* TreeInspector::createVariableSpinBoxPair(DrProperty *property, QFont &font)
{
    QSizePolicy size_policy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    size_policy.setHorizontalStretch(c_inspector_size_right);

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

    long property_key = property->getPropertyKey();

    spin_left->setProperty(User_Property::Key, QVariant::fromValue( property_key ));
    spin_left->setProperty(User_Property::Order, 0);
    addToWidgetList(spin_left);

    spin_right->setProperty(User_Property::Key, QVariant::fromValue( property_key));
    spin_right->setProperty(User_Property::Order, 1);
    addToWidgetList(spin_right);

    // This stops mouse wheel from stealing focus unless user has selected the widget
    spin_left->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
    spin_left->installEventFilter(new MouseWheelWidgetAdjustmentGuard(spin_left));
    spin_right->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
    spin_right->installEventFilter(new MouseWheelWidgetAdjustmentGuard(spin_right));

    connect (spin_left,  QOverload<double>::of(&TripleSpinBox::valueChanged),
             this, [this, property_key] (double d) { updateSettingsFromNewValue(property_key, d, 0); });
    connect (spin_right, QOverload<double>::of(&TripleSpinBox::valueChanged),
             this, [this, property_key] (double d) { updateSettingsFromNewValue(property_key, d, 1); });

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

// SIGNAL: void QComboBox::currentIndexChanged(int index)
QComboBox* TreeInspector::createComboBox(DrProperty *property, QFont &font)
{
    QSizePolicy size_policy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    size_policy.setHorizontalStretch(c_inspector_size_right);

    DropDownComboBox *combo = new DropDownComboBox();
    combo->setObjectName(QStringLiteral("comboBox"));
    combo->view()->parentWidget()->setStyleSheet(" background: none; border-radius: 0px; ");

    QString style =
            " QComboBox QAbstractItemView { "
            "       font-size: " + QString::number(font.pointSize()) + "px; "
            "       border: " + Dr::BorderWidth() + " solid; margin: 0px; "
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
    combo->setFont(font);
    combo->setSizePolicy(size_policy);

    //// Alternate way to remove white border around QComboBox ListView
    ///combo->setEditable(true);
    ///combo->lineEdit()->setReadOnly(true);
    ///combo->lineEdit()->setDisabled(true);

    long   property_key =  property->getPropertyKey();
    DrType object_type  =  property->getParentComponent()->getParentSettings()->getType();

    QStringList options;    
    if (object_type == DrType::Object && property_key == static_cast<int>(Properties::Object_Damage))
        options << tr("No Damage") << tr("Damage Player") << tr("Damage Enemy") << tr("Damage All");
    else if ((object_type == DrType::Stage || object_type == DrType::StartStage) && (property_key == static_cast<int>(Properties::Stage_Grid_Style)))
        options << tr("Lines") << tr("Dots");
    else
        options << tr("Unknown List");
    combo->addItems(options);

    connect (combo, QOverload<int>::of(&QComboBox::currentIndexChanged),
             this, [this, property_key] (int index) { updateSettingsFromNewValue(property_key, index); });

    combo->setProperty(User_Property::Key, QVariant::fromValue( property_key ));
    combo->setCurrentIndex(property->getValue().toInt());

    applyHeaderBodyProperties(combo, property);
    addToWidgetList(combo);
    return combo;
}


// Uses a pushbutton with a popup menu instead of a QComboBox
QPushButton* TreeInspector::createComboBox2(DrProperty *property, QFont &font)
{
    QSizePolicy size_policy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    size_policy.setHorizontalStretch(c_inspector_size_right);

    QPushButton *button = new QPushButton();
    button->setObjectName(QStringLiteral("buttonDropDown"));
    button->setFont(font);
    button->setSizePolicy(size_policy);

    QStringList options;
    if (property->getPropertyKey() == static_cast<int>(Properties::Object_Damage)) {
        options << tr("No Damage") << tr("Damage Player") << tr("Damage Enemy") << tr("Damage All");
    } else {
        options << tr("Unknown List");
    }

    QString menu_style = " QMenu { "
                         "      min-width: 100px; padding-left: 6px; padding-top: 4px; padding-bottom: 4px; "
                         "      color: " + Dr::GetColor(Window_Colors::Text).name() + "; "
                         "      font-size: " + QString::number(font.pointSize()) + "px; "
                         "      border: " + Dr::BorderWidth() + " solid; margin: 0px; "
                         "      border-color: " + Dr::GetColor(Window_Colors::Icon_Dark).name() + "; "
                         "      background: " + Dr::GetColor(Window_Colors::Shadow).name() + "; } "
                         " QMenu::item { padding-top: 2px; padding-bottom: 2px; } "
                         " QMenu::item:selected { "
                         "       padding-left: 2px; "
                         "       color: " + Dr::GetColor(Window_Colors::Highlight).name() + "; "
                         "       background: " + Dr::GetColor(Window_Colors::Shadow).name() + "; } ";
                         " QMenu::item:checked { "
                         "       padding-left: 0px; "
                         "       font-weight: bold; "
                         "       color: " + Dr::GetColor(Window_Colors::Highlight).name() + "; "
                         "       background: " + Dr::GetColor(Window_Colors::Shadow).name() + "; } ";

    QMenu *menu = new QMenu();
    menu->setStyleSheet(menu_style);


    QActionGroup *group;
    group = new QActionGroup(menu);
    group->setExclusive(true);

    QAction *action1 = new QAction("First");    QAction *action2 = new QAction("Second");       QAction *action3 = new QAction("Third");
    group->addAction(action1);                  group->addAction(action2);                      group->addAction(action3);
    action1->setCheckable(true);                action2->setCheckable(true);                    action3->setCheckable(true);
    menu->addAction(action1);                   menu->addAction(action2);                       menu->addAction(action3);

    connect(action1,   &QAction::triggered, [button, action1]() { button->setText(action1->text()); });
    connect(action2,   &QAction::triggered, [button, action2]() { button->setText(action2->text()); });
    connect(action3,   &QAction::triggered, [button, action3]() { button->setText(action3->text()); });

    action2->setChecked(true);

    button->setMenu(menu);
    button->setProperty(User_Property::Key, QVariant::fromValue( property->getPropertyKey() ));
    button->setText(" First");

    //button->setCurrentIndex(property->getValue().toInt());
    applyHeaderBodyProperties(button, property);
    addToWidgetList(button);
    return button;
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








