//
//      Created by Stephens Nunnally on 1/8/2020, (c) 2020 Scidian Studios, All Rights Reserved
//
//  File:
//
//
//
#include <QContextMenuEvent>
#include <QLineEdit>
#include <QMenu>

#include "core/dr_math.h"
#include "editor/helper_library.h"
#include "editor/widgets/widgets_inspector.h"


//####################################################################################
//##    Catches event that Qt fires when value has changed,
//##    trims any excess zeros from the right side
//####################################################################################
QString InspectorTripleSpinBox::textFromValue(double value) const {
    return Dr::RemoveTrailingDecimals(value, decimals());
}


//####################################################################################
//##    Integer Spin Box
//####################################################################################
// Non signal activating setValue
void InspectorSpinSlot::updateValue(int value) {
    bool before_block = this->signalsBlocked();
    this->blockSignals(true);
    this->setValue( value );
    this->blockSignals(before_block);
}

// Context Menu Override, allows for us to show ContextMenu without "Step Up", "Step Down" for disabled boxes
void InspectorSpinSlot::contextMenuEvent(QContextMenuEvent *event) {
    if (m_show_menu_step) {
        QAbstractSpinBox::contextMenuEvent(event);
        return;
    }

    QMenu *menu = lineEdit()->createStandardContextMenu();
    if (menu == nullptr) return;
    QPoint pos = (event->reason() == QContextMenuEvent::Mouse) ? event->globalPos() : mapToGlobal(QPoint(event->pos().x(), 0)) + QPoint(width() / 2, height() / 2);
    menu->exec(pos);
    delete menu;
    event->accept();
}


//####################################################################################
//##    Floating Point Spin Box, Non signal activating setValue
//####################################################################################
void InspectorDoubleSpinSlot::updateValue(double value) {
    bool before_block = this->signalsBlocked();
    this->blockSignals(true);
    if ( Dr::IsCloseTo(value, this->value(), m_update_tolerance) == false) {
        this->setValue( value );
    }
    this->blockSignals(before_block);
}





















