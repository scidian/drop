//
//      Created by Stephens Nunnally on 1/10/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QApplication>

#include "editor/widgets/widgets.h"


//####################################################################################
//##    Shows the QPushButton popupMenu,
//##    Disables animation while we move it to the position we desire
//####################################################################################
void DrQComboBoxDropDown::showPopup() {
    bool oldAnimationEffects = qApp->isEffectEnabled(Qt::UI_AnimateCombo);
    qApp->setEffectEnabled(Qt::UI_AnimateCombo, false);

    QComboBox::showPopup();
    QWidget *frame = findChild<QFrame*>();
    frame->move( frame->x() + 7, mapToGlobal(this->geometry().bottomLeft()).y() - 1);

    qApp->setEffectEnabled(Qt::UI_AnimateCombo, oldAnimationEffects);
}




