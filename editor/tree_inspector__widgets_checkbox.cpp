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

#include "colors/colors.h"
#include "editor/tree_inspector.h"
#include "forms/form_color_magnifier.h"
#include "forms/form_popup.h"
#include "helper.h"
#include "project/project.h"
#include "settings/settings.h"
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
        painter.setPen( QPen(Dr::GetColor(Window_Colors::Background_Dark).lighter(200), Dr::BorderWidthAsInt()) );
        painter.setBrush( Qt::NoBrush );
        painter.drawRoundedRect(6, 1, 20, 20, 4, 4);

        // Draw main square
        QLinearGradient gradient(6, 1, 5, 20);
        gradient.setColorAt(0.00, top);
        gradient.setColorAt(0.14, top);
        gradient.setColorAt(0.18, middle);
        gradient.setColorAt(1.00, middle);
        painter.setBrush(gradient);
        painter.setPen( QPen(Dr::GetColor(Window_Colors::Background_Dark).darker(200), Dr::BorderWidthAsInt()) );
        painter.drawRoundedRect(6, 1, 20, 19, 4, 4);

        // Draw check mark
        if (checkState()) {
            painter.setPen( QPen(Dr::GetColor(Window_Colors::Text), 2, Qt::PenStyle::SolidLine, Qt::PenCapStyle::RoundCap) );
            QVector<QLineF> check;
            check.append( QLineF( 11, 13, 13, 16) );
            check.append( QLineF( 14, 16, 21,  8) );
            painter.drawLines(check);
        }
    } else {
        // Draw check mark
        if (checkState()) {
            painter.setPen( QPen(Dr::GetColor(Window_Colors::Text_Dark), 2, Qt::PenStyle::SolidLine, Qt::PenCapStyle::RoundCap) );
            QVector<QLineF> check;
            check.append( QLineF( 11, 13, 13, 16) );
            check.append( QLineF( 14, 16, 21,  8) );
            painter.drawLines(check);
        }
    }
}











