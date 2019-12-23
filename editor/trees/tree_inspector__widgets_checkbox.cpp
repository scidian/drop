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
#include "editor/event_filters.h"
#include "editor/forms/form_popup.h"
#include "editor/globals_editor.h"
#include "editor/helper_library.h"
#include "editor/style/style.h"
#include "editor/trees/tree_inspector.h"
#include "project/dr_project.h"
#include "project/settings/settings.h"
#include "project/settings/settings_component_property.h"


//####################################################################################
//##    Checkbox
//####################################################################################
QCheckBox* TreeInspector::createCheckBox(DrProperty *property, QFont &font, QSizePolicy size_policy, Property_Type check_type) {
    long property_key = property->getPropertyKey();

    DrQCheckBox *check = new DrQCheckBox();
    check->setObjectName("checkInspector");
    check->setFont(font);
    check->setDrawTop(3);
    check->setDrawLeft(4);
    check->setSizePolicy(size_policy);
    check->setTristate(false);
    check->setProperty(User_Property::Mouse_Over, false);               // Initialize some mouse user data, DrFilterHoverHandler updates this info,
    check->setProperty(User_Property::Mouse_Pos, QPoint(0, 0));         // Used to track when the mouse is within the indicator area for custom paint event
    check->setProperty(User_Property::Key, QVariant::fromValue( property_key ));

    if (check_type == Property_Type::BoolEnabled) {
        std::vector<DrVariant> prop_list = property->getValue().toVector();
        if (prop_list.size() > 0) check->setChecked(prop_list[0].toBool());
    } else {
        check->setChecked(property->getValue().toBool());
    }

    getHoverHandler()->attachToHoverHandler(check, property);
    addToWidgetList(check);

    connect (check, &QCheckBox::toggled, [this, property_key](bool checked) { updateSettingsFromNewValue( property_key, checked );  });

    return check;
}


//####################################################################################
//##    Pair of Check Box / Double Spin Box, uses Property_Type::BoolDouble
//##        std::vector<DrVariant> of 6 values: bool, double value, min, max, double step size, string spinText
//####################################################################################
QFrame* TreeInspector::createCheckBoxSpinBoxPair(DrProperty *property, QFont &font, QSizePolicy size_policy) {
    long property_key = property->getPropertyKey();

    QFrame *spin_pair = new QFrame();
    spin_pair->setFixedHeight(25);
    spin_pair->setSizePolicy(size_policy);

    QHBoxLayout *horizontal_split = new QHBoxLayout(spin_pair);
    horizontal_split->setAlignment(Qt::AlignVCenter);
    horizontal_split->setSpacing(2);
    horizontal_split->setContentsMargins(0,0,0,2);

    DrQCheckBox *check_left = new DrQCheckBox();
    check_left->setFixedWidth(36);
    check_left->setObjectName("checkInspector");
    check_left->setFont(font);
    check_left->setDrawLeft(0);
    check_left->setDrawTop(3);
    check_left->setTristate(false);
    check_left->setProperty(User_Property::Mouse_Over, false);              // Initialize some mouse user data, DrFilterHoverHandler updates this info,
    check_left->setProperty(User_Property::Mouse_Pos, QPoint(0, 0));        // Used to track when the mouse is within the indicator area for custom paint event
    check_left->setProperty(User_Property::Key, QVariant::fromValue( property_key ));
    check_left->setChecked(property->getValue().toVector()[0].toBool());
    getHoverHandler()->attachToHoverHandler(check_left, property);

    DrQTripleSpinBox *spin_right = initializeEmptySpinBox(property, font, property->getValue().toVector()[1].toDouble());
    spin_right->setObjectName("spinBool");
    spin_right->setFixedHeight(22);
    spin_right->setPrefix(QString::fromStdString(property->getValue().toVector()[5].toString()));
    spin_right->setRange(property->getValue().toVector()[2].toDouble(), property->getValue().toVector()[3].toDouble());
    spin_right->setSingleStep(property->getValue().toVector()[4].toDouble());
    spin_right->setProperty(User_Property::Key, QVariant::fromValue( property_key ));
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

    connect (check_left, &QCheckBox::toggled, [this, property_key, spin_right](bool checked) {
        updateSettingsFromNewValue( property_key, checked );
        spin_right->setEnabled( checked );
    });
    connect (spin_right, QOverload<double>::of(&DrQTripleSpinBox::valueChanged),
         this, [this, property_key] (double d) { updateSettingsFromNewValue(property_key, d, 1); });

    return spin_pair;
}


//####################################################################################
//##    Pair of Check Box / In Spin Box, uses Property_Type::BoolInt
//##        std::vector<DrVariant> of 6 values: bool, int value, min, max, int step size, string spinText
//####################################################################################
QFrame* TreeInspector::createCheckBoxIntBoxPair(DrProperty *property, QFont &font, QSizePolicy size_policy) {
    long property_key = property->getPropertyKey();

    QFrame *spin_pair = new QFrame();
    spin_pair->setFixedHeight(25);
    spin_pair->setSizePolicy(size_policy);

    QHBoxLayout *horizontal_split = new QHBoxLayout(spin_pair);
    horizontal_split->setAlignment(Qt::AlignVCenter);
    horizontal_split->setSpacing(2);
    horizontal_split->setContentsMargins(0,0,0,0);

    DrQCheckBox *check_left = new DrQCheckBox();
    check_left->setFixedWidth(36);
    check_left->setFixedHeight(22);
    check_left->setObjectName("checkInspector");
    check_left->setFont(font);
    check_left->setDrawLeft(0);
    check_left->setDrawTop(2);
    check_left->setTristate(false);
    check_left->setProperty(User_Property::Mouse_Over, false);              // Initialize some mouse user data, DrFilterHoverHandler updates this info,
    check_left->setProperty(User_Property::Mouse_Pos, QPoint(0, 0));        // Used to track when the mouse is within the indicator area for custom paint event
    check_left->setProperty(User_Property::Key, QVariant::fromValue( property_key ));
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
    spin_right->setProperty(User_Property::Key, QVariant::fromValue( property_key ));
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

    connect (check_left, &QCheckBox::toggled, [this, property_key, spin_right](bool checked) {
        updateSettingsFromNewValue( property_key, checked );
        spin_right->setEnabled( checked );
    });
    connect (spin_right, QOverload<int>::of(&QSpinBox::valueChanged),
         this, [this, property_key] (int i) { updateSettingsFromNewValue(property_key, i, 1); });

    return spin_pair;
}



//####################################################################################
//##    Check Box Paint Event
//####################################################################################
void DrQCheckBox::paintEvent(QPaintEvent *) {
    /// default: m_draw_left = 6, m_draw_top = 1
    QRect  checkbox_indicator(m_draw_left - 1, m_draw_top - 1, 22, 26);
    QPoint mouse_position = property(User_Property::Mouse_Pos).toPoint();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QColor top, middle;
    bool hover = (property(User_Property::Mouse_Over).toBool() && checkbox_indicator.contains(mouse_position));
    if (hover) {
        top =    Dr::ToQColor(Dr::GetColor(Window_Colors::Background_Dark)).darker(110);
        middle = Dr::ToQColor(Dr::GetColor(Window_Colors::Background_Light));
    } else {
        top =    Dr::ToQColor(Dr::GetColor(Window_Colors::Background_Dark)).darker(150);
        middle = Dr::ToQColor(Dr::GetColor(Window_Colors::Background_Dark));
    }

    if (this->isEnabled()) {
        // Draw bottom highlight
        painter.setPen( QPen(Dr::ToQColor(Dr::GetColor(Window_Colors::Background_Dark)).lighter(200), Dr::BorderWidthAsInt()) );
        painter.setBrush( Qt::NoBrush );
        painter.drawRoundedRect(m_draw_left, m_draw_top, 21, 21, 4, 4);

        // Draw main square
        QLinearGradient gradient(m_draw_left, m_draw_top, m_draw_left, 21);
        gradient.setColorAt(0.00, top);
        gradient.setColorAt(0.14, top);
        gradient.setColorAt(0.18, middle);
        gradient.setColorAt(1.00, middle);
        painter.setBrush(gradient);
        painter.setPen( QPen(Dr::ToQColor(Dr::GetColor(Window_Colors::Background_Dark)).darker(200), Dr::BorderWidthAsInt()) );
        painter.drawRoundedRect(m_draw_left, m_draw_top, 21, 20, 4, 4);

        // Draw check mark
        if (checkState()) {
            painter.setPen( QPen(Dr::ToQColor(Dr::GetColor(Window_Colors::Text)), 2, Qt::PenStyle::SolidLine, Qt::PenCapStyle::RoundCap) );
            QVector<QLineF> check;
            ///check.append( QLineF( 11, 13, 13, 16) );
            ///check.append( QLineF( 14, 16, 21,  8) );
            check.append( QLineF( m_draw_left + 5, m_draw_top + 12, m_draw_left + 7,  m_draw_top + 15) );
            check.append( QLineF( m_draw_left + 8, m_draw_top + 15, m_draw_left + 15, m_draw_top +  7) );
            painter.drawLines(check);
        }
    } else {
        // Draw check mark
        if (checkState()) {
            painter.setPen( QPen(Dr::ToQColor(Dr::GetColor(Window_Colors::Text_Dark)), 2, Qt::PenStyle::SolidLine, Qt::PenCapStyle::RoundCap) );
            QVector<QLineF> check;
            ///check.append( QLineF( 11, 13, 13, 16) );
            ///check.append( QLineF( 14, 16, 21,  8) );
            check.append( QLineF( m_draw_left + 5, m_draw_top + 12, m_draw_left + 7,  m_draw_top + 15) );
            check.append( QLineF( m_draw_left + 8, m_draw_top + 15, m_draw_left + 15, m_draw_top +  7) );
            painter.drawLines(check);
        }
    }

    // Draws Outline of CheckBox
    ///painter.setBrush(Qt::NoBrush);
    ///painter.setPen(QPen(Qt::red, 1));
    ///painter.drawRect(this->rect());
}











