//
//      Created by Stephens Nunnally on 10/14/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QApplication>
#include <QCheckBox>
#include <QColorDialog>
#include <QDebug>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QMenu>
#include <QPainter>

#include "core/colors/colors.h"
#include "core/types/dr_color.h"
#include "editor/event_filters.h"
#include "editor/forms/form_color_magnifier.h"
#include "editor/forms/form_popup.h"
#include "editor/helper_library.h"
#include "editor/preferences.h"
#include "editor/style/style.h"
#include "editor/trees/tree_inspector.h"
#include "project/dr_project.h"
#include "project/settings/settings.h"
#include "project/settings/settings_component.h"
#include "project/settings/settings_component_property.h"


//####################################################################################
//##    Colorful button used to represent a Color property
//####################################################################################
QWidget* TreeInspector::createColorBox(DrProperty *property, QFont &font, QSizePolicy size_policy) {
    QColor color = QColor::fromRgba(property->getValue().toUInt());

    QWidget *color_box = new QWidget();
    color_box->setSizePolicy(size_policy);
    getHoverHandler()->attachToHoverHandler(color_box, property);
        QHBoxLayout *color_layout = new QHBoxLayout(color_box);
        color_layout->setContentsMargins(0, 0, 0, 0);
        color_layout->setSpacing(0);

        // ***** This is the button that shows the color and color name, clicking it opens a color popup menu
        QPushButton *color_button = new QPushButton();
        color_button->setObjectName(QStringLiteral("buttonColorBox"));
        color_button->setFont(font);
        color_button->setSizePolicy(size_policy);
        color_button->setProperty(User_Property::CompKey,  QVariant::fromValue(       property->getParentComponent()->getComponentKey()) );
        color_button->setProperty(User_Property::CompName, QString::fromStdString(    property->getParentComponent()->getComponentName()) );
        color_button->setProperty(User_Property::PropKey,  QVariant::fromValue(       property->getPropertyKey()) );
        color_button->setProperty(User_Property::PropName, QString::fromStdString(    property->getPropertyName()) );

        this->updateColorButton(color_button, color);
        connect(color_button, &QPushButton::clicked, [this, color_box, color_button, color] () {
            FormPopupColor *color_popup = new FormPopupColor(getParentProject(), color_box, color_button, -18, 5);
            color_popup->buildPopupColors(color_button, DrColor(color_button->property(User_Property::Color).toUInt()) );
            connect(color_popup, SIGNAL(colorGrabbed(QWidget*, DrColor)), this, SLOT(setButtonColor(QWidget*, DrColor)) );
            color_popup->show();
        });
        getHoverHandler()->attachToHoverHandler(color_button, Advisor_Info::ColorButton);
        QString color_as_string = "R: " + QString::number(color.red()) + ", G: " + QString::number(color.green()) + ", B: " + QString::number(color.blue());
        color_button->setToolTip( color_as_string );
        color_button->setProperty(User_Property::Body, QString::fromStdString(property->getDescription()) + "<br><br><b>Selected:</b> " + color_as_string);

        addToWidgetList(color_button);
        color_layout->addWidget(color_button);

        // ***** This is the color that shows the color picker dropper, clicking it starts the color magnifier
        QPushButton *picker_button = new QPushButton();
        picker_button->setObjectName(QStringLiteral("buttonColorPicker"));
        picker_button->setToolTip( Advisor_Info::ColorPicker[0] );
        picker_button->setFixedSize(25, 20 + Dr::BorderWidthAsInt() * 2);           // Height has to include border thickness
        connect(picker_button, &QPushButton::pressed, this, [this, picker_button, color_button]() {
            FormColorMagnifier *picker = new FormColorMagnifier(color_button, QCursor::pos(), 115, 115, 8);
            connect(picker, SIGNAL(colorGrabbed(QWidget*, DrColor)), this, SLOT(setButtonColor(QWidget*, DrColor)) );
            picker->show();
            picker_button->setDown(false);
        });
        getHoverHandler()->attachToHoverHandler(picker_button, Advisor_Info::ColorPicker);
        color_layout->addWidget(picker_button);

        // ***** This is the button that shows the color wheel, clicking it opens the system color dialog
        QPushButton *dialog_button = new QPushButton();
        dialog_button->setObjectName(QStringLiteral("buttonColorDialog"));
        dialog_button->setToolTip( Advisor_Info::ColorDialog[0] );
        dialog_button->setFixedSize(25, 20 + Dr::BorderWidthAsInt() * 2);           // Height has to include border thickness
        connect(dialog_button, &QPushButton::clicked, [this, color_button] () {
            this->setButtonColorFromSystemDialog(color_button);
        });
        getHoverHandler()->attachToHoverHandler(dialog_button, Advisor_Info::ColorDialog);
        color_layout->addWidget(dialog_button);

    return color_box;
}

void TreeInspector::setButtonColorFromSystemDialog(QPushButton *button) {
    QColor old_color= QColor::fromRgba(button->property(User_Property::Color).toUInt());
    QColor color =    QColorDialog::getColor(old_color, this, "Select Color");///, QColorDialog::ColorDialogOption::ShowAlphaChannel);
    ///QColor color = QColorDialog::getColor(old_color, this, "Select Color", QColorDialog::DontUseNativeDialog);   // Qt Implementation
    setButtonColor(button, Dr::FromQColor(color));
}

// SLOT: Receives a new color and updates color button and appropriate Project Settings
void TreeInspector::setButtonColor(QWidget *button, DrColor color) {
    QPushButton *push = dynamic_cast<QPushButton*>(button);
    if (push) {
        ///Dr::AddToColorHistory( DrColor( button->property(User_Property::Color).toUInt() ));
        Dr::AddToColorHistory( color );
        this->updateColorButton(push, Dr::ToQColor(color));
        std::string component_name = push->property(User_Property::CompName).toString().toStdString();
        std::string property_name =  push->property(User_Property::PropName).toString().toStdString();
        this->updateSettingsFromNewValue(std::make_pair(component_name, property_name), color.rgba());
    }
}

void TreeInspector::updateColorButton(QPushButton *button, QColor color) {
    QColor text_color = QColor(24, 24, 24);
    QColor highlight =  QColor(0, 0, 0);
    if (color.red() < 128 && color.green() < 128 && color.blue() < 128) {
        text_color = QColor(205, 205, 205);
        highlight =  QColor(255, 255, 255);
    }
    QString color_as_string = "R: " + QString::number(color.red()) + ", G: " + QString::number(color.green()) + ", B: " + QString::number(color.blue());
    std::string color_button = Dr::StyleSheetColorButton(Dr::FromQColor(color), Dr::FromQColor(text_color), Dr::FromQColor(highlight), 4, 0, 4, 0, true, true, "");
    button->setToolTip( color_as_string );
    button->setStyleSheet( QString::fromStdString(color_button));
    button->setText( color.name().toUpper() );

    // Update advisor info with new color
    if (m_selected_keys.contains(c_no_key) == false) {
        DrSettings *settings = getParentProject()->findSettingsFromKey(m_key_shown);
        if (settings != nullptr) {
            std::string component_name = button->property(User_Property::CompName).toString().toStdString();
            std::string property_name =  button->property(User_Property::PropName).toString().toStdString();
            DrProperty *property =      settings->getComponentProperty(component_name, property_name);
            if (property != nullptr) {
                button->setProperty(User_Property::Body, QString::fromStdString(property->getDescription()) + "<br><br><b>Selected:</b> " + color_as_string);
            }
        }
    }

    int alpha = static_cast<int>(color.alphaF() * 100.0);
    if (alpha != 100) button->setText( button->text() + " - " + QString::number(alpha) + "%" );
    button->setProperty(User_Property::Color, color.rgba());
}












