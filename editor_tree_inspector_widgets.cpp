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
#include <QPaintEvent>

#include "colors.h"
#include "debug.h"

#include "editor_tree_inspector.h"
#include "editor_tree_widgets.h"

#include "interface_editor_relay.h"
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

    DrCheckBox *check = new DrCheckBox();
    check->setObjectName("checkInspector");
    check->setFont(font);
    check->setSizePolicy(sp_right);
    check->setTristate(false);

    long property_key = property->getPropertyKey();

    check->setProperty(User_Property::Mouse_Over, false);               // Initialize some mouse user data, WidgetHoverHandler updates this info,
    check->setProperty(User_Property::Mouse_Pos, QPoint(0, 0));         // Used to track when the mouse is within the indicator area for custom paint event
    check->setProperty(User_Property::Key, QVariant::fromValue( property_key ));

    check->setChecked(property->getValue().toBool());

    attachToHoverHandler(check, property);
    addToWidgetList(check);

    connect (check, &QCheckBox::toggled, [this, property_key](bool checked) { updateSettingsFromNewValue( property_key, checked );  });

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

    attachToHoverHandler(edit, property);
    addToWidgetList(edit);

    connect (edit,  &QLineEdit::editingFinished,
             this, [this, property_key, edit] () { updateSettingsFromNewValue( property_key, edit->text() ); });

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

    attachToHoverHandler(spin, property);
    addToWidgetList(spin);

    // This stops mouse wheel from stealing focus unless user has selected the widget
    spin->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
    spin->installEventFilter(new MouseWheelAdjustmentGuard(spin));

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

    DrTripleSpinBox *spin = new DrTripleSpinBox();
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
    attachToHoverHandler(spin, property);
    addToWidgetList(spin);

    // This stops mouse wheel from stealing focus unless user has selected the widget
    spin->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
    spin->installEventFilter(new MouseWheelAdjustmentGuard(spin));

    // Connect value changed to our handler function
    connect (spin,  QOverload<double>::of(&DrTripleSpinBox::valueChanged),
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

    DrTripleSpinBox *spin_left  =  initializeEmptySpinBox(property, font, property->getValue().toPointF().x());
    DrTripleSpinBox *spin_right  = initializeEmptySpinBox(property, font, property->getValue().toPointF().y());

    switch (spin_type)
    {
    case Spin_Type::Point:
        spin_left->setPrefix("X: ");        spin_right->setPrefix("Y: ");   break;
    case Spin_Type::Grid:
        spin_left->setPrefix("W: ");        spin_right->setPrefix("H: ");
        spin_left->setRange( c_minimum_grid_size, 100000000);
        spin_right->setRange(c_minimum_grid_size, 100000000);
        break;
    case Spin_Type::Size:
        spin_left->setPrefix("W: ");        spin_right->setPrefix("H: ");   break;
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
    spin_right->setProperty(User_Property::Key, QVariant::fromValue( property_key ));
    spin_left->setProperty(User_Property::Order, 0);
    spin_right->setProperty(User_Property::Order, 1);
    addToWidgetList(spin_left);
    addToWidgetList(spin_right);

    // This stops mouse wheel from stealing focus unless user has selected the widget
    spin_left->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
    spin_right->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
    spin_left->installEventFilter(new MouseWheelAdjustmentGuard(spin_left));
    spin_right->installEventFilter(new MouseWheelAdjustmentGuard(spin_right));

    connect (spin_left,  QOverload<double>::of(&DrTripleSpinBox::valueChanged),
             this, [this, property_key] (double d) { updateSettingsFromNewValue(property_key, d, 0); });
    connect (spin_right, QOverload<double>::of(&DrTripleSpinBox::valueChanged),
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

    DrTripleSpinBox *spin_left  = initializeEmptySpinBox(property, font, property->getValue().toPointF().x());
    size_policy.setHorizontalStretch(4);
    spin_left->setSizePolicy(size_policy);

    QLabel *variable_sign = new QLabel("+ / -");
    variable_sign->setFont(font);
    size_policy.setHorizontalStretch(1);
    variable_sign->setSizePolicy(size_policy);
    attachToHoverHandler(variable_sign, "Variable Amount", "Plus or minus modifier to initial value, the following value allows for some "
                                                                "variable amount to the initial value. For example, an initial value of 100 with "
                                                                "a variable amount of 5, allows for values ranging from 95 to 105.");
    DrTripleSpinBox *spin_right  = initializeEmptySpinBox(property, font, property->getValue().toPointF().y());
    spin_right->setMinimum(0);
    size_policy.setHorizontalStretch(3);
    spin_right->setSizePolicy(size_policy);

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
    spin_left->installEventFilter(new MouseWheelAdjustmentGuard(spin_left));
    spin_right->installEventFilter(new MouseWheelAdjustmentGuard(spin_right));

    connect (spin_left,  QOverload<double>::of(&DrTripleSpinBox::valueChanged),
             this, [this, property_key] (double d) { updateSettingsFromNewValue(property_key, d, 0); });
    connect (spin_right, QOverload<double>::of(&DrTripleSpinBox::valueChanged),
             this, [this, property_key] (double d) { updateSettingsFromNewValue(property_key, d, 1); });

    return spin_pair;
}

DrTripleSpinBox* TreeInspector::initializeEmptySpinBox(DrProperty *property, QFont &font, double start_value)
{
    DrTripleSpinBox *new_spin  = new DrTripleSpinBox();
    new_spin->setFont(font);
    new_spin->setMinimumWidth(50);
    new_spin->setDecimals(3);
    new_spin->setRange(-100000000, 100000000);
    new_spin->setButtonSymbols(QAbstractSpinBox::ButtonSymbols::NoButtons);
    new_spin->setValue(start_value);
    attachToHoverHandler(new_spin, property);
    return new_spin;
}


// Uses a pushbutton with a popup menu instead of a QComboBox
QPushButton* TreeInspector::createListBox(DrProperty *property, QFont &font)
{
    QSizePolicy size_policy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    size_policy.setHorizontalStretch(c_inspector_size_right);

    QPushButton *button = new QPushButton();
    button->setObjectName(QStringLiteral("buttonDropDown"));
    button->setFont(font);
    button->setSizePolicy(size_policy);

    long   property_key =  property->getPropertyKey();

    QStringList options;
    if          (property_key == static_cast<int>(Properties::Object_Damage)) {
        options << tr("No Damage")
                << tr("Damage Player")
                << tr("Damage Enemy")
                << tr("Damage All");
    } else if   (property_key == static_cast<int>(Properties::Stage_Grid_Style)) {
        options << tr("Lines")
                << tr("Dots");
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
        QAction *action = new QAction(string);
        group->addAction(action);
        action->setCheckable(true);
        menu->addAction(action);

        if (property->getValue().toInt() == string_count) {
            action->setChecked(true);
            button->setText(string);
        }

        action->setProperty(User_Property::Order, QVariant::fromValue(string_count));

        // Create a callback function to update DrSettings when a new value is selected
        connect(action,   &QAction::triggered, [this, button, action, property_key]() {
            button->setText(action->text());
            this->updateSettingsFromNewValue(property_key, action->property(User_Property::Order).toInt());
        });

        string_count++;
    }

    button->setMenu(menu);
    button->setProperty(User_Property::Key, QVariant::fromValue( property->getPropertyKey() ));
    menu->installEventFilter(new PopUpMenuRelocater(menu));
    attachToHoverHandler(button, property);
    addToWidgetList(button);

    return button;
}


//####################################################################################
//##
//##    TripleSpinBox Class Functions
//##
//####################################################################################
QString DrTripleSpinBox::textFromValue(double value) const
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
void DrDropDownComboBox::showPopup()
{
    bool oldAnimationEffects = qApp->isEffectEnabled(Qt::UI_AnimateCombo);
    qApp->setEffectEnabled(Qt::UI_AnimateCombo, false);

    QComboBox::showPopup();
    QWidget *frame = findChild<QFrame*>();
    frame->move( frame->x() + 7, mapToGlobal(this->geometry().bottomLeft()).y() - 1);

    qApp->setEffectEnabled(Qt::UI_AnimateCombo, oldAnimationEffects);
}


//####################################################################################
//##
//##    DropDownComboBox Class Functions, paints the checkbox & check mark
//##
//####################################################################################
void DrCheckBox::paintEvent(QPaintEvent *)
{
    QRect checkbox_indicator(4, 0, 28, 22);
    QPoint mouse_position = property(User_Property::Mouse_Pos).toPoint();

    QPainter painter(this);
    if (property(User_Property::Mouse_Over).toBool() && checkbox_indicator.contains(mouse_position)) {
        painter.setPen( QPen( Dr::GetColor(Window_Colors::Background_Light), Dr::BorderWidthAsInt() ) );
        painter.setBrush( QBrush(Dr::GetColor(Window_Colors::Button_Light)) );
        painter.drawRoundRect(5, 1, 20, 20, 20, 20);
        painter.setPen( QPen( Dr::GetColor(Window_Colors::Text_Light), 2, Qt::PenStyle::SolidLine, Qt::PenCapStyle::RoundCap ) );

    } else {
        painter.setPen( QPen( Dr::GetColor(Window_Colors::Button_Light), Dr::BorderWidthAsInt() ) );
        painter.setBrush( QBrush(Dr::GetColor(Window_Colors::Background_Light)) );
        painter.drawRoundRect(5, 1, 20, 20, 20, 20);
        painter.setPen( QPen( Dr::GetColor(Window_Colors::Text), 2, Qt::PenStyle::SolidLine, Qt::PenCapStyle::RoundCap ) );
    }

    if (checkState()) {
        QVector<QLineF> check;
        check.append( QLineF( 10, 13, 13, 16) );
        check.append( QLineF( 13, 16, 21,  8) );
        painter.drawLines(check);
    }

}



















