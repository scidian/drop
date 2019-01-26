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

void TreeInspector::applyHeaderBodyProperties(QWidget *widget, DrProperty *property)
{
    widget->setProperty(User_Property::Header, property->getDisplayName());
    widget->setProperty(User_Property::Body, property->getDescription());
    m_widget_hover->attach(widget);
}

QCheckBox* TreeInspector::createCheckBox(DrProperty *property, QFont &font)
{
    QSizePolicy sp_right(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sp_right.setHorizontalStretch(SIZE_RIGHT);

    QCheckBox *check = new QCheckBox();
    check->setObjectName("checkInspector");
    check->setFont(font);
    check->setSizePolicy(sp_right);
    check->setTristate(false);

    check->setChecked(property->getValue().toBool());
    applyHeaderBodyProperties(check, property);
    return check;
}

QDoubleSpinBox* TreeInspector::createDoubleSpinBox(DrProperty *property, QFont &font, Spin_Type spin_type)
{
    ///// Could also try to use a QLineEdit with a QValidator
    ///myLineEdit->setValidator( new QDoubleValidator(0, 100, 2, this) );

    QSizePolicy sp_right(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sp_right.setHorizontalStretch(SIZE_RIGHT);

    TripleSpinBox *spin = new TripleSpinBox();
    spin->setFont(font);
    spin->setSizePolicy(sp_right);
    spin->setDecimals(3);
    switch (spin_type)
    {
    case Spin_Type::Double:     spin->setRange(-100000000, 100000000);                  break;
    case Spin_Type::Percent:    spin->setRange(0, 100);     spin->setSuffix("%");       break;
    case Spin_Type::Angle:      spin->setRange(-360, 360);  spin->setSuffix("°");       break;
    }
    ///spin->setProperty("alignment", Qt::AlignRight);      // Align right?
    spin->setButtonSymbols(QAbstractSpinBox::ButtonSymbols::NoButtons);

    spin->setValue(property->getValue().toDouble());
    applyHeaderBodyProperties(spin, property);
    return spin;
}

QFrame* TreeInspector::createDoubleSpinBoxPair(DrProperty *property, QFont &font)
{
    QSizePolicy sp_right(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sp_right.setHorizontalStretch(SIZE_RIGHT);

    QFrame *spin_pair = new QFrame();
    spin_pair->setSizePolicy(sp_right);

    QBoxLayout *horizontal_split = new QHBoxLayout(spin_pair);
    horizontal_split->setSpacing(6);
    horizontal_split->setContentsMargins(0,0,0,0);

    TripleSpinBox *spin_left  = new TripleSpinBox();
    spin_left->setFont(font);
    spin_left->setMinimumWidth(50);
    spin_left->setPrefix("X: ");
    spin_left->setDecimals(3);
    spin_left->setRange(-100000000, 100000000);
    spin_left->setButtonSymbols(QAbstractSpinBox::ButtonSymbols::NoButtons);
    spin_left->setValue(property->getValue().toPointF().x());
    applyHeaderBodyProperties(spin_left, property);

    TripleSpinBox *spin_right  = new TripleSpinBox();
    spin_right->setFont(font);
    spin_right->setMinimumWidth(50);
    spin_right->setPrefix("Y: ");
    spin_right->setDecimals(3);
    spin_right->setRange(-100000000, 100000000);
    spin_right->setButtonSymbols(QAbstractSpinBox::ButtonSymbols::NoButtons);
    spin_right->setValue(property->getValue().toPointF().x());
    applyHeaderBodyProperties(spin_right, property);

    horizontal_split->addWidget(spin_left);
    horizontal_split->addWidget(spin_right);
    return spin_pair;
}

QLineEdit* TreeInspector::createLineEdit(DrProperty *property, QFont &font)
{
    QSizePolicy sp_right(QSizePolicy::Preferred, QSizePolicy::Preferred);
    sp_right.setHorizontalStretch(SIZE_RIGHT);

    QLineEdit *edit = new QLineEdit();
    edit->setFont(font);
    edit->setSizePolicy(sp_right);

    edit->setText(property->getValue().toString());
    applyHeaderBodyProperties(edit, property);
    return edit;
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










