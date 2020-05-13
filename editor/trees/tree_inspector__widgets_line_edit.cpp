//
//      Created by Stephens Nunnally on 10/14/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Object Inspector Line Edits
//
//
#include <QApplication>
#include <QCheckBox>
#include <QDebug>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QMenu>
#include <QPainter>

#include "core/colors/colors.h"
#include "editor/event_filters.h"
#include "editor/preferences.h"
#include "editor/style/style.h"
#include "editor/trees/tree_inspector.h"
#include "project/dr_project.h"
#include "project/settings/settings.h"
#include "project/settings/settings_component.h"
#include "project/settings/settings_component_property.h"


//####################################################################################
//##    Line Edit
//####################################################################################
QLineEdit* TreeInspector::createLineEdit(DrProperty *property, QFont &font, QSizePolicy size_policy) {
    QLineEdit *edit = new QLineEdit();
    edit->setFont(font);
    edit->setSizePolicy(size_policy);
    edit->setAttribute(Qt::WA_MacShowFocusRect, 0);

    edit->setProperty(User_Property::CompKee,  QVariant::fromValue(       property->getParentComponent()->getComponentKee()) );
    edit->setProperty(User_Property::CompName, QString::fromStdString(    property->getParentComponent()->getComponentName()) );
    edit->setProperty(User_Property::PropKee,  QVariant::fromValue(       property->getPropertyKee()) );
    edit->setProperty(User_Property::PropName, QString::fromStdString(    property->getPropertyName()) );
    edit->setText(QString::fromStdString(property->getValue().toString()));

    getHoverHandler()->attachToHoverHandler(edit, property);
    addToWidgetList(edit);

    connect (edit, &QLineEdit::editingFinished,
            this, [this, property, edit] () {
                if (property != nullptr && edit != nullptr) {
                    updateSettingsFromNewValue( property->getCompPropPair(), edit->text().toStdString() );
                }
            });

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

    edit->setProperty(User_Property::CompKee,  QVariant::fromValue(       property->getParentComponent()->getComponentKee()) );
    edit->setProperty(User_Property::CompName, QString::fromStdString(    property->getParentComponent()->getComponentName()) );
    edit->setProperty(User_Property::PropKee,  QVariant::fromValue(       property->getPropertyKee()) );
    edit->setProperty(User_Property::PropName, QString::fromStdString(    property->getPropertyName()) );
    edit->setText(QString::fromStdString(property->getValue().toString()));

    getHoverHandler()->attachToHoverHandler(edit, property);
    addToWidgetList(edit);

    connect (edit,  &QTextEdit::textChanged,
             this, [this, property, edit] () { updateSettingsFromNewValue( property->getCompPropPair(), edit->toPlainText().toStdString()); }); /// edit->toHtml() ); });

    return edit;
}














