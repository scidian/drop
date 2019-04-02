//
//      Created by Stephens Nunnally on 4/1/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QRadioButton>

#include "form_main.h"
#include "form_popup.h"
#include "globals.h"

void FormPopup::buildPopupGridSnap()
{
    QFont font;  font.setPointSize(Dr::FontSize());
    QFontMetrics font_metrics { font };

    // Figure out best size of popup
    QString option1 = tr("Snap center of selection group to grid");
    QString option2 = tr("Snap center of individual objects to grid");
    QRect rect1 = font_metrics.boundingRect( option1 );
    QRect rect2 = font_metrics.boundingRect( option2 );
    int width = (rect1.width() > rect2.width()) ? rect1.width() + 50 : rect2.width() + 50;
    this->setFixedWidth(width);
    this->setMinimumHeight(rect1.height() + rect2.height() + 42);

    QWidget *first_page_widget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(first_page_widget);
    layout->setContentsMargins(8, 0, 5, 0);
    layout->setAlignment(Qt::AlignVCenter);
    layout->setSpacing(10);

        QRadioButton *button1 = new QRadioButton(option1);  button1->setObjectName(QStringLiteral("popupRadio"));
        QRadioButton *button2 = new QRadioButton(option2);  button2->setObjectName(QStringLiteral("popupRadio"));

        button1->setFont(font);
        button2->setFont(font);

        button1->setChecked( Dr::GetPreference(Preferences::World_Editor_Snap_To_Center_Of_Selection_Box).toBool());
        button2->setChecked(!Dr::GetPreference(Preferences::World_Editor_Snap_To_Center_Of_Selection_Box).toBool());

        connect(button1, &QRadioButton::released, [this]() {
            Dr::SetPreference(Preferences::World_Editor_Snap_To_Center_Of_Selection_Box, true);
            this->close();
        });
        connect(button2, &QRadioButton::released, [this]() {
            Dr::SetPreference(Preferences::World_Editor_Snap_To_Center_Of_Selection_Box, false);
            this->close();
        });

        layout->addWidget(button1);
        layout->addWidget(button2);

    m_inner_stacked_widget->addWidget(first_page_widget);
}






