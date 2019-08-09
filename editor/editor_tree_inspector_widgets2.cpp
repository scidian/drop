//
//      Created by Stephens Nunnally on 4/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Non-numerical property row building functions
//          - CheckBox
//          - Line Edit
//          - Pushbutton with a popup menu instead of a QComboBox
//          - Colorful button used to represent a Color property
//
#include <QApplication>
#include <QCheckBox>
#include <QColorDialog>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QMenu>
#include <QPainter>

#include "colors/colors.h"
#include "editor_tree_inspector.h"
#include "forms/form_color_magnifier.h"
#include "forms/form_popup.h"
#include "helper.h"
#include "settings/settings_component_property.h"
#include "style/style.h"
#include "widgets/widgets_event_filters.h"


//####################################################################################
//##    Checkbox / PaintEvent that draws box and check mark
//####################################################################################
QCheckBox* TreeInspector::createCheckBox(DrProperty *property, QFont &font, QSizePolicy size_policy) {
    DrQCheckBox *check = new DrQCheckBox();
    check->setObjectName("checkInspector");
    check->setFont(font);
    check->setSizePolicy(size_policy);
    check->setTristate(false);

    long property_key = property->getPropertyKey();

    check->setProperty(User_Property::Mouse_Over, false);               // Initialize some mouse user data, DrFilterHoverHandler updates this info,
    check->setProperty(User_Property::Mouse_Pos, QPoint(0, 0));         // Used to track when the mouse is within the indicator area for custom paint event
    check->setProperty(User_Property::Key, QVariant::fromValue( property_key ));

    check->setChecked(property->getValue().toBool());

    m_filter_hover->attachToHoverHandler(check, property);
    addToWidgetList(check);

    connect (check, &QCheckBox::toggled, [this, property_key](bool checked) { updateSettingsFromNewValue( property_key, checked );  });

    return check;
}

void DrQCheckBox::paintEvent(QPaintEvent *) {
    QRect  checkbox_indicator(5, 0, 26, 22);
    QPoint mouse_position = property(User_Property::Mouse_Pos).toPoint();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QColor top, middle;
    bool hover = (property(User_Property::Mouse_Over).toBool() && checkbox_indicator.contains(mouse_position));
    if (hover) {
        top =    Dr::GetColor(Window_Colors::Background_Dark).darker(110);
        middle = Dr::GetColor(Window_Colors::Background_Light);
    } else {
        top =    Dr::GetColor(Window_Colors::Background_Dark).darker(150);
        middle = Dr::GetColor(Window_Colors::Background_Dark);
    }

    if (this->isEnabled()) {
        // Draw bottom highlight
        painter.setPen( QPen( Dr::GetColor(Window_Colors::Background_Dark).lighter(200), Dr::BorderWidthAsInt() ) );
        painter.setBrush( Qt::NoBrush );
        painter.drawRoundedRect(6, 1, 20, 20, 4, 4);

        // Draw main square
        QLinearGradient gradient(6, 1, 5, 20);
        gradient.setColorAt(0.00, top);
        gradient.setColorAt(0.14, top);
        gradient.setColorAt(0.18, middle);
        gradient.setColorAt(1.00, middle);
        painter.setBrush(gradient);
        painter.setPen( QPen( Dr::GetColor(Window_Colors::Background_Dark).darker(200), Dr::BorderWidthAsInt() ) );
        painter.drawRoundedRect(6, 1, 20, 19, 4, 4);

        // Draw check mark
        if (checkState()) {
            painter.setPen( QPen( Dr::GetColor(Window_Colors::Text), 2, Qt::PenStyle::SolidLine, Qt::PenCapStyle::RoundCap ) );
            QVector<QLineF> check;
            check.append( QLineF( 11, 13, 13, 16) );
            check.append( QLineF( 14, 16, 21,  8) );
            painter.drawLines(check);
        }
    } else {
        // Draw check mark
        if (checkState()) {
            painter.setPen( QPen( Dr::GetColor(Window_Colors::Text_Dark), 2, Qt::PenStyle::SolidLine, Qt::PenCapStyle::RoundCap ) );
            QVector<QLineF> check;
            check.append( QLineF( 11, 13, 13, 16) );
            check.append( QLineF( 14, 16, 21,  8) );
            painter.drawLines(check);
        }
    }
}


//####################################################################################
//##    Line Edit
//####################################################################################
QLineEdit* TreeInspector::createLineEdit(DrProperty *property, QFont &font, QSizePolicy size_policy) {
    QLineEdit *edit = new QLineEdit();
    edit->setFont(font);
    edit->setSizePolicy(size_policy);

    long property_key = property->getPropertyKey();

    edit->setProperty(User_Property::Key, QVariant::fromValue( property_key ));
    edit->setText(property->getValue().toString());

    m_filter_hover->attachToHoverHandler(edit, property);
    addToWidgetList(edit);

    connect (edit,  &QLineEdit::editingFinished,
             this, [this, property_key, edit] () { updateSettingsFromNewValue( property_key, edit->text() ); });

    return edit;
}



//####################################################################################
//##    Pushbutton with a popup menu instead of a QComboBox
//####################################################################################
QPushButton* TreeInspector::createListBox(DrProperty *property, QFont &font, QSizePolicy size_policy) {
    QPushButton *button = new QPushButton();
    button->setObjectName(QStringLiteral("buttonDropDown"));
    button->setFont(font);
    button->setSizePolicy(size_policy);

    long   property_key =  property->getPropertyKey();

    QStringList options;
    if          (property_key == static_cast<int>(Properties::Thing_Object_Damage)) {
        options << tr("No Damage")
                << tr("Damage Player")
                << tr("Damage Enemy")
                << tr("Damage All");
    } else if   (property_key == static_cast<int>(Properties::Thing_Object_Physics_Type)) {
        options << tr("Static")
                << tr("Kinematic")
                << tr("Dynamic");
    } else if   (property_key == static_cast<int>(Properties::Stage_Grid_Style)) {
        options << tr("Lines")
                << tr("Dots");
    } else if   (property_key == static_cast<int>(Properties::Thing_Light_Type)) {
        options << tr("Opaque")
                << tr("Glow");
    } else if   (property_key == static_cast<int>(Properties::World_Light_Blend)) {
        options << tr("Default")
                << tr("Multiply")
                << tr("Overlay")
                << tr("Hard Light");
    } else if   (property_key == static_cast<int>(Properties::Thing_Water_Texture)) {
        options << tr("None")
                << tr("Ripples")
                << tr("Cells")
                << tr("Caustic")
                << tr("Liquid");
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

        if (property_key == static_cast<int>(Properties::Thing_Object_Damage)) {
            connect(action,   &QAction::hovered, [this, string_count]() {
                switch (string_count) {
                    case 0: this->setAdvisorInfo( Advisor_Info::Damage_None[0],     Advisor_Info::Damage_None[1]);          break;
                    case 1: this->setAdvisorInfo( Advisor_Info::Damage_Player[0],   Advisor_Info::Damage_Player[1]);        break;
                    case 2: this->setAdvisorInfo( Advisor_Info::Damage_Enemy[0],    Advisor_Info::Damage_Enemy[1]);         break;
                    case 3: this->setAdvisorInfo( Advisor_Info::Damage_All[0],      Advisor_Info::Damage_All[1]);           break;
                }
            });
        }

        if (property_key == static_cast<int>(Properties::Thing_Object_Physics_Type)) {
            connect(action,   &QAction::hovered, [this, string_count]() {
                switch (string_count) {
                    case 0: this->setAdvisorInfo( Advisor_Info::Object_Static[0],    Advisor_Info::Object_Static[1]);       break;
                    case 1: this->setAdvisorInfo( Advisor_Info::Object_Kinematic[0], Advisor_Info::Object_Kinematic[1]);    break;
                    case 2: this->setAdvisorInfo( Advisor_Info::Object_Dynamic[0],   Advisor_Info::Object_Dynamic[1]);      break;
                }
            });
        }

        if (property_key == static_cast<int>(Properties::Thing_Light_Type)) {
            connect(action,   &QAction::hovered, [this, string_count]() {
                switch (string_count) {
                    case 0: this->setAdvisorInfo( Advisor_Info::Light_Opaque[0],    Advisor_Info::Light_Opaque[1]);          break;
                    case 1: this->setAdvisorInfo( Advisor_Info::Light_Glow[0],      Advisor_Info::Light_Glow[1]);           break;
                }
            });
        }

        string_count++;
    }

    button->setMenu(menu);
    button->setProperty(User_Property::Key, QVariant::fromValue( property_key ));
    menu->installEventFilter(new DrFilterPopUpMenuRelocater(menu, 2, 0));
    m_filter_hover->attachToHoverHandler(button, property);
    addToWidgetList(button);

    return button;
}

// Shows the QPushButton popupMenu, disables animation while we move it to the position we desire
void DrQComboBoxDropDown::showPopup() {
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
QWidget* TreeInspector::createColorBox(DrProperty *property, QFont &font, QSizePolicy size_policy) {
    long   property_key =  property->getPropertyKey();
    QColor color =     QColor::fromRgba(property->getValue().toUInt());

    QWidget *color_box = new QWidget();
    color_box->setSizePolicy(size_policy);
    m_filter_hover->attachToHoverHandler(color_box, property);
        QHBoxLayout *color_layout = new QHBoxLayout(color_box);
        color_layout->setContentsMargins(0, 0, 0, 0);
        color_layout->setSpacing(0);

        // This is the button that shows the color and color name, clicking it opens a color popup menu
        QPushButton *color_button = new QPushButton();
        color_button->setObjectName(QStringLiteral("buttonColorBox"));
        color_button->setToolTip( Advisor_Info::ColorButton[0] );
        color_button->setFont(font);
        color_button->setSizePolicy(size_policy);
        color_button->setProperty(User_Property::Key,   QVariant::fromValue( property_key ));
        this->updateColorButton(color_button, color);
        connect(color_button, &QPushButton::clicked, [this, color_box, color_button, color] () {
            FormPopupColor *color_popup = new FormPopupColor(m_project, color_box, color_button, -18, 5);
            color_popup->buildPopupColors(color_button, QColor::fromRgba(color_button->property(User_Property::Color).toUInt()) );
            connect(color_popup, SIGNAL(colorGrabbed(QWidget*, QColor)), this, SLOT(setButtonColor(QWidget*, QColor)) );
            color_popup->show();
        });
        m_filter_hover->attachToHoverHandler(color_button, Advisor_Info::ColorButton);
        addToWidgetList(color_button);
        color_layout->addWidget(color_button);

        // This is the color that shows the color picker dropper, clicking it starts the color magnifier
        QPushButton *picker_button = new QPushButton();
        picker_button->setObjectName(QStringLiteral("buttonColorPicker"));
        picker_button->setToolTip( Advisor_Info::ColorPicker[0] );
        picker_button->setFixedSize(25, 20 + Dr::BorderWidthAsInt() * 2);           // Height has to include border thickness
        connect(picker_button, &QPushButton::pressed, this, [this, picker_button, color_button]() {
            FormColorMagnifier *picker = new FormColorMagnifier(color_button, QCursor::pos(), 115, 115, 8);
            connect(picker, SIGNAL(colorGrabbed(QWidget*, QColor)), this, SLOT(setButtonColor(QWidget*, QColor)) );
            picker->show();
            picker_button->setDown(false);
        });
        m_filter_hover->attachToHoverHandler(picker_button, Advisor_Info::ColorPicker);
        color_layout->addWidget(picker_button);

        // This is the button that shows the color wheel, clicking it opens the system color dialog
        QPushButton *dialog_button = new QPushButton();
        dialog_button->setObjectName(QStringLiteral("buttonColorDialog"));
        dialog_button->setToolTip( Advisor_Info::ColorDialog[0] );
        dialog_button->setFixedSize(25, 20 + Dr::BorderWidthAsInt() * 2);           // Height has to include border thickness
        connect(dialog_button, &QPushButton::clicked, [this, color_button] () {
            this->setButtonColorFromSystemDialog(color_button);
        });
        m_filter_hover->attachToHoverHandler(dialog_button, Advisor_Info::ColorDialog);
        color_layout->addWidget(dialog_button);

    return color_box;
}

void TreeInspector::setButtonColorFromSystemDialog(QPushButton *button) {
    QColor old_color= QColor::fromRgba(button->property(User_Property::Color).toUInt());
    QColor color =    QColorDialog::getColor(old_color, this, "Select Color");///, QColorDialog::ColorDialogOption::ShowAlphaChannel);
    ///QColor color = QColorDialog::getColor(old_color, this, "Select Color", QColorDialog::DontUseNativeDialog);   // Qt Implementation
    setButtonColor(button, color);
}

// SLOT: Receives a new color and updates color button and appropriate Project Settings
void TreeInspector::setButtonColor(QWidget *button, QColor color) {
    QPushButton *push = dynamic_cast<QPushButton*>(button);
    if (push && color.isValid()) {
        ///Dr::AddToColorHistory( QColor::fromRgba( button->property(User_Property::Color).toUInt() ));
        Dr::AddToColorHistory( color );
        this->updateColorButton(push, color);
        this->updateSettingsFromNewValue(push->property(User_Property::Key).toInt(), color.rgba());
    }
}

void TreeInspector::updateColorButton(QPushButton *button, QColor color) {
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















