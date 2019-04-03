//
//      Created by Stephens Nunnally on 1/26/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Property Row Building Functions
//
//
#include <QApplication>
#include <QAction>
#include <QActionGroup>
#include <QColorDialog>
#include <QLabel>
#include <QLineEdit>
#include <QListView>
#include <QMenu>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPaintEvent>
#include <QStyleFactory>

#include "colors.h"
#include "constants.h"
#include "debug.h"
#include "editor_tree_inspector.h"
#include "form_color_magnifier.h"
#include "form_popup.h"
#include "interface_editor_relay.h"
#include "globals.h"
#include "library.h"
#include "project.h"
#include "project_world.h"
#include "project_world_stage.h"
#include "project_world_stage_object.h"
#include "settings.h"
#include "settings_component.h"
#include "settings_component_property.h"
#include "widgets_event_filters.h"


//####################################################################################
//##    Checkbox / PaintEvent that draws box and check mark
//####################################################################################
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

    m_widget_hover->attachToHoverHandler(check, property);
    addToWidgetList(check);

    connect (check, &QCheckBox::toggled, [this, property_key](bool checked) { updateSettingsFromNewValue( property_key, checked );  });

    return check;
}

void DrCheckBox::paintEvent(QPaintEvent *)
{
    QRect  checkbox_indicator(4, 0, 28, 22);
    QPoint mouse_position = property(User_Property::Mouse_Pos).toPoint();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QColor middle;
    //Hover
    if (property(User_Property::Mouse_Over).toBool() && checkbox_indicator.contains(mouse_position))
        middle = Dr::GetColor(Window_Colors::Background_Dark).darker(150);
    else
        middle = Dr::GetColor(Window_Colors::Background_Dark);

    // Draw bottom highlight
    painter.setPen( QPen( Dr::GetColor(Window_Colors::Background_Dark).lighter(200), Dr::BorderWidthAsInt() ) );
    painter.setBrush( Qt::NoBrush );
    painter.drawRoundedRect(5, 1, 20, 20, 4, 4);

    QLinearGradient gradient( 5, 1, 5, 20);
    gradient.setColorAt(0.00, Dr::GetColor(Window_Colors::Background_Dark).darker(150));
    gradient.setColorAt(0.14, Dr::GetColor(Window_Colors::Background_Dark).darker(150));
    gradient.setColorAt(0.18, middle);
    gradient.setColorAt(1.00, middle);
    painter.setBrush(gradient);
    painter.setPen( QPen( Dr::GetColor(Window_Colors::Background_Dark).darker(150), Dr::BorderWidthAsInt() ) );
    painter.drawRoundedRect(5, 1, 20, 19, 4, 4);
    painter.setPen( QPen( Dr::GetColor(Window_Colors::Text), 2, Qt::PenStyle::SolidLine, Qt::PenCapStyle::RoundCap ) );

    if (checkState()) {
        QVector<QLineF> check;
        check.append( QLineF( 10, 13, 13, 16) );
        check.append( QLineF( 13, 16, 21,  8) );
        painter.drawLines(check);
    }
}


//####################################################################################
//##    Line Edit
//####################################################################################
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

    m_widget_hover->attachToHoverHandler(edit, property);
    addToWidgetList(edit);

    connect (edit,  &QLineEdit::editingFinished,
             this, [this, property_key, edit] () { updateSettingsFromNewValue( property_key, edit->text() ); });

    return edit;
}


//####################################################################################
//##    Spin Boxes
//####################################################################################
QSpinBox* TreeInspector::createIntSpinBox(DrProperty *property, QFont &font, Property_Type spin_type)
{
    QSizePolicy size_policy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    size_policy.setHorizontalStretch(c_inspector_size_right);

    QSpinBox *spin = new QSpinBox();
    spin->setFont(font);
    spin->setSizePolicy(size_policy);
    switch (spin_type) {
    case Property_Type::Int:            spin->setRange(-100000000, 100000000);      spin->setSingleStep(  5 );      break;
    case Property_Type::Positive:       spin->setRange(0, 100000000);               spin->setSingleStep(  5 );      break;
    case Property_Type::Filter:         spin->setRange(-255, 255);                  spin->setSingleStep( 10 );      break;
    case Property_Type::FilterAngle:    spin->setRange(0, 360);                     spin->setSingleStep(  5 );      break;
    default:                            spin->setRange(-100000000, 100000000);
    }
    spin->setButtonSymbols(QAbstractSpinBox::ButtonSymbols::NoButtons);

    long property_key = property->getPropertyKey();

    spin->setProperty(User_Property::Key, QVariant::fromValue( property_key ));
    spin->setValue(property->getValue().toInt());

    m_widget_hover->attachToHoverHandler(spin, property);
    addToWidgetList(spin);

    // This stops mouse wheel from stealing focus unless user has selected the widget
    spin->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
    spin->installEventFilter(new MouseWheelAdjustmentGuard(spin));

    connect (spin,  QOverload<int>::of(&QSpinBox::valueChanged),
             this, [this, property_key] (int i) { updateSettingsFromNewValue(property_key, i); });

    return spin;
}

// SIGNAL: void QDoubleSpinBox::valueChanged(double d)
QDoubleSpinBox* TreeInspector::createDoubleSpinBox(DrProperty *property, QFont &font, Property_Type spin_type)
{
    ///// Could also try to use a QLineEdit with a QValidator
    ///myLineEdit->setValidator( new QDoubleValidator(0, 100, 2, this) );

    QSizePolicy size_policy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    size_policy.setHorizontalStretch(c_inspector_size_right);

    DrTripleSpinBox *spin = new DrTripleSpinBox();
    spin->setFont(font);
    spin->setSizePolicy(size_policy);
    spin->setDecimals(3);
    switch (spin_type) {
    case Property_Type::Double:     spin->setRange(-100000000, 100000000);              spin->setSingleStep(5);     break;
    case Property_Type::Percent:    spin->setRange(0, 100);     spin->setSuffix("%");   spin->setSingleStep(5);     break;
    case Property_Type::Angle:      spin->setRange(-360, 360);  spin->setSuffix("°");   spin->setSingleStep(5);     break;
    default:                        spin->setRange(-100000000, 100000000);
    }
    spin->setButtonSymbols(QAbstractSpinBox::ButtonSymbols::NoButtons);     // Hides little up / down buttons

    // Store property key within item, set initial starting value of spin box
    long property_key = property->getPropertyKey();
    spin->setProperty(User_Property::Key, QVariant::fromValue( property_key ));
    spin->setValue(property->getValue().toDouble());

    // Connect HoverHandler with proper text, add this widget to list of widgets in object inspector
    m_widget_hover->attachToHoverHandler(spin, property);
    addToWidgetList(spin);

    // This stops mouse wheel from stealing focus unless user has selected the widget
    spin->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
    spin->installEventFilter(new MouseWheelAdjustmentGuard(spin));

    // Connect value changed to our handler function
    connect (spin,  QOverload<double>::of(&DrTripleSpinBox::valueChanged),
             this, [this, property_key] (double d) { updateSettingsFromNewValue(property_key, d); });

    return spin;
}

QFrame* TreeInspector::createDoubleSpinBoxPair(DrProperty *property, QFont &font, Property_Type spin_type)
{
    QSizePolicy size_policy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    size_policy.setHorizontalStretch(c_inspector_size_right);

    QFrame *spin_pair = new QFrame();
    spin_pair->setSizePolicy(size_policy);

    QBoxLayout *horizontal_split = new QHBoxLayout(spin_pair);
    horizontal_split->setSpacing(6);
    horizontal_split->setContentsMargins(0,0,0,0);

    DrTripleSpinBox *spin_left  =  initializeEmptySpinBox(property, font, property->getValue().toPointF().x());
    DrTripleSpinBox *spin_right;

    if (spin_type == Property_Type::PositionF)
        spin_right  = initializeEmptySpinBox(property, font, -1 * property->getValue().toPointF().y());
    else
        spin_right  = initializeEmptySpinBox(property, font, property->getValue().toPointF().y());

    switch (spin_type) {
    case Property_Type::PositionF:
    case Property_Type::PointF:
    case Property_Type::ScaleF:
        spin_left->setPrefix("X: ");        spin_right->setPrefix("Y: ");   break;
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
    default: ;
    }
    if (spin_type == Property_Type::ScaleF || spin_type == Property_Type::GridScaleF) {
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
    m_widget_hover->attachToHoverHandler(variable_sign, Advisor_Info::Variable_Widget);
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
    m_widget_hover->attachToHoverHandler(new_spin, property);
    return new_spin;
}

// Catches event that Qt fires when value has changed to trim any excess zeros from the right side
QString DrTripleSpinBox::textFromValue(double value) const {
    return Dr::RemoveTrailingDecimals(value, decimals());
}



//####################################################################################
//##    Pushbutton with a popup menu instead of a QComboBox
//####################################################################################
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
    button->setProperty(User_Property::Key, QVariant::fromValue( property_key ));
    menu->installEventFilter(new PopUpMenuRelocater(menu, 2, 0));
    m_widget_hover->attachToHoverHandler(button, property);
    addToWidgetList(button);

    return button;
}

// Shows the QPushButton popupMenu, disables animation while we move it to the position we desire
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
//##    Colorful button used to represent a Color property
//####################################################################################
QWidget* TreeInspector::createColorBox(DrProperty *property, QFont &font)
{
    QSizePolicy size_policy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    size_policy.setHorizontalStretch(c_inspector_size_right);

    long   property_key =  property->getPropertyKey();
    QColor color =     QColor::fromRgba(property->getValue().toUInt());

    QWidget *color_box = new QWidget();
    color_box->setSizePolicy(size_policy);
    m_widget_hover->attachToHoverHandler(color_box, property);
        QHBoxLayout *color_layout = new QHBoxLayout(color_box);
        color_layout->setContentsMargins(0, 0, 0, 0);
        color_layout->setSpacing(0);

        // This is the button that shows the color and color name, clicking it opens a color popup menu
        QPushButton *color_button = new QPushButton();
        color_button->setObjectName(QStringLiteral("buttonColorBox"));
        color_button->setFont(font);
        color_button->setSizePolicy(size_policy);
        color_button->setProperty(User_Property::Key,   QVariant::fromValue( property_key ));
        this->updateColorButton(color_button, color);
        connect(color_button, &QPushButton::clicked, [this, color_box, color_button, color] () {
            ColorPopup *color_popup = new ColorPopup(m_project, color_box, color_button, -18, 5);
            color_popup->buildPopupColors(color_button, QColor::fromRgba(color_button->property(User_Property::Color).toUInt()) );
            connect(color_popup, SIGNAL(colorGrabbed(QWidget*, QColor)), this, SLOT(setButtonColor(QWidget*, QColor)) );
            color_popup->show();
        });
        m_widget_hover->attachToHoverHandler(color_button, Advisor_Info::ColorButton);
        addToWidgetList(color_button);
        color_layout->addWidget(color_button);

        // This is the color that shows the color picker dropper, clicking it starts the color magnifier
        QPushButton *picker_button = new QPushButton();
        picker_button->setObjectName(QStringLiteral("buttonColorPicker"));
        picker_button->setFixedSize(25, 20 + Dr::BorderWidthAsInt() * 2);           // Height has to include border thickness
        connect(picker_button, &QPushButton::pressed, this, [this, picker_button, color_button]() {
            FormColorMagnifier *picker = new FormColorMagnifier(color_button, QCursor::pos(), 115, 115, 8);
            connect(picker, SIGNAL(colorGrabbed(QWidget*, QColor)), this, SLOT(setButtonColor(QWidget*, QColor)) );
            picker->show();
            picker_button->setDown(false);
        });
        m_widget_hover->attachToHoverHandler(picker_button, Advisor_Info::ColorPicker);
        color_layout->addWidget(picker_button);

        // This is the button that shows the color wheel, clicking it opens the system color dialog
        QPushButton *dialog_button = new QPushButton();
        dialog_button->setObjectName(QStringLiteral("buttonColorDialog"));
        dialog_button->setFixedSize(25, 20 + Dr::BorderWidthAsInt() * 2);           // Height has to include border thickness
        connect(dialog_button, &QPushButton::clicked, [this, color_button] () {
            this->setButtonColorFromSystemDialog(color_button);
        });
        m_widget_hover->attachToHoverHandler(dialog_button, Advisor_Info::ColorDialog);
        color_layout->addWidget(dialog_button);

    return color_box;
}

void TreeInspector::setButtonColorFromSystemDialog(QPushButton *button)
{
    QColor old_color= QColor::fromRgba(button->property(User_Property::Color).toUInt());
    QColor color =    QColorDialog::getColor(old_color, this, "Select Color", QColorDialog::ColorDialogOption::ShowAlphaChannel);
    ///QColor color = QColorDialog::getColor(old_color, this, "Select Color", QColorDialog::DontUseNativeDialog);   // Qt Implementation
    setButtonColor(button, color);
}

// SLOT: Receives a new color and updates color button and appropriate Project Settings
void TreeInspector::setButtonColor(QWidget *button, QColor color)
{
    QPushButton *push = dynamic_cast<QPushButton*>(button);
    if (push && color.isValid()) {
        this->updateColorButton(push, color);
        this->updateSettingsFromNewValue(push->property(User_Property::Key).toInt(), color.rgba());
    }
}

void TreeInspector::updateColorButton(QPushButton *button, QColor color)
{
    QColor text_color = QColor(24, 24, 24);
    QColor highlight =  QColor(0, 0, 0);
    if (color.red() < 128 && color.green() < 128 && color.blue() < 128) {
        text_color = QColor(205, 205, 205);
        highlight =  QColor(255, 255, 255);
    }
    QString color_button = Dr::StyleSheetColorButton(color, text_color, highlight, 4, 0, 4, 0, true, true, "");
    button->setStyleSheet(color_button);
    button->setText( color.name().toUpper() );
    int alpha = static_cast<int>(color.alphaF() * 100.0);
    if (alpha != 100) button->setText( button->text() + " - " + QString::number(alpha) + "%" );
    button->setProperty(User_Property::Color, color.rgba());
}





