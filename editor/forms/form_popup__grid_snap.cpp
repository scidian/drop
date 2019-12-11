//
//      Created by Stephens Nunnally on 4/1/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QRadioButton>

#include "editor/forms/form_main.h"
#include "editor/forms/form_popup.h"
#include "editor/globals_editor.h"
#include "editor/helper_library.h"


//####################################################################################
//##    Builds out FormPop's inner widget to show options for grid snapping
//####################################################################################
void FormPopup::buildPopupGridSnap() {
    QFont font = Dr::CustomFont();
    QFontMetrics font_metrics { font };

    // Figure out best size of popup
    QString option1 = tr("Snap center of selection group to grid");
    QString option2 = tr("Snap center of individual objects to grid");
    QRect rect1 = font_metrics.boundingRect( option1 );
    QRect rect2 = font_metrics.boundingRect( option2 );
    int width = (rect1.width() > rect2.width()) ? rect1.width() + 50 : rect2.width() + 50;
    this->setMinimumWidth(width);
    this->setMinimumHeight( (rect1.height() * 2) + 42);

    QVBoxLayout *layout = new QVBoxLayout(m_inner_widget);
    layout->setContentsMargins(8, 0, 5, 10);
    layout->setAlignment(Qt::AlignVCenter);
    layout->setSpacing(6);

        QRadioButton *button1 = new QRadioButton(option1);  button1->setObjectName(QStringLiteral("popupRadio"));
        QRadioButton *button2 = new QRadioButton(option2);  button2->setObjectName(QStringLiteral("popupRadio"));

        button1->setFont(font); button1->setFixedHeight( rect1.height() + 4 );
        button2->setFont(font); button2->setFixedHeight( rect1.height() + 4 );

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
}






