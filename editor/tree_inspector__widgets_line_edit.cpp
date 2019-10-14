//
//      Created by Stephens Nunnally on 10/14/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
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
//##    Line Edit
//####################################################################################
QLineEdit* TreeInspector::createLineEdit(DrProperty *property, QFont &font, QSizePolicy size_policy) {
    QLineEdit *edit = new QLineEdit();
    edit->setFont(font);
    edit->setSizePolicy(size_policy);
    edit->setAttribute(Qt::WA_MacShowFocusRect, 0);

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
//##    Text Edit
//####################################################################################
QTextEdit* TreeInspector::createTextEdit(DrProperty *property, QFont &font, QSizePolicy size_policy) {
    QTextEdit *edit = new QTextEdit();
    edit->setFont(font);
    edit->setSizePolicy(size_policy);
    edit->setLineWrapMode(QTextEdit::LineWrapMode::WidgetWidth);
    edit->setFixedHeight(90);

    long property_key = property->getPropertyKey();

    edit->setProperty(User_Property::Key, QVariant::fromValue( property_key ));
    edit->setText(property->getValue().toString());

    m_filter_hover->attachToHoverHandler(edit, property);
    addToWidgetList(edit);

    connect (edit,  &QTextEdit::textChanged,
             this, [this, property_key, edit] () { updateSettingsFromNewValue( property_key, edit->toPlainText() ); }); /// edit->toHtml() ); });

    return edit;
}














