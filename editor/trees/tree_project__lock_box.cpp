//
//      Created by Stephens Nunnally on 10/18/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "editor/interface_editor_relay.h"
#include "editor/preferences.h"
#include "editor/trees/tree_project.h"
#include "project/dr_project.h"
#include "project/entities/dr_world.h"
#include "project/entities/dr_stage.h"
#include "project/entities/dr_thing.h"


//####################################################################################
//##    Installs / Updates Lock box widget for an item
//####################################################################################
void TreeProject::installLockBox(DrSettings *entity, QTreeWidgetItem *thing_item) {
    if (entity->getType() != DrType::Thing) return;
    DrThing *thing = dynamic_cast<DrThing*>(entity);
    if (thing == nullptr) return;

    // Figure out style sheet
    bool forced = thing->getComponentPropertyValue(Components::Hidden_Settings, Properties::Hidden_Item_Locked).toBool();
    QString check_images = QString(" QCheckBox::indicator { width: 12px; height: 12px; } "
                                   " QCheckBox::indicator:unchecked { image: url(:/assets/tree_icons/tree_bullet.png); } ");
    if (forced) check_images +=    " QCheckBox::indicator:checked {   image: url(:/assets/tree_icons/tree_lock_disable.png); } ";
    else        check_images +=    " QCheckBox::indicator:checked {   image: url(:/assets/tree_icons/tree_lock.png); } ";

    // Find lock widget, or create new one
    QCheckBox *lock_item = dynamic_cast<QCheckBox*>(this->itemWidget(thing_item, COLUMN_LOCK));
    if (lock_item == nullptr) {
        lock_item = new QCheckBox();
        lock_item->setObjectName("projectTreeLock");
        lock_item->setProperty(User_Property::Key, QVariant::fromValue( thing->getKey() ));
        lock_item->setFocusPolicy( Qt::FocusPolicy::NoFocus );

        // Locking / unlocking function
        connect(lock_item, &QCheckBox::toggled, this, [this, lock_item]() { this->processLockClick(lock_item); });
        this->setItemWidget(thing_item, COLUMN_LOCK, lock_item);
    }

    // Apply style sheet and state
    lock_item->setStyleSheet(  check_images );
    lock_item->setCheckState(  thing->isLocked() ? Qt::CheckState::Checked : Qt::CheckState::Unchecked );
}


//####################################################################################
//##    Handles click on little Lock Button
//####################################################################################
void TreeProject::processLockClick(QCheckBox *from_lock) {
    // Check if currently locked
    bool locked = (from_lock->checkState() == Qt::CheckState::Checked) ? true : false;

    // Find entity key attached to lock box
    long lock_key = from_lock->property(User_Property::Key).toLongLong();

    // See if item is selected
    QList<QTreeWidgetItem*> item_list = this->selectedItems();
    bool selected = false;
    for (auto item : item_list) {
        if (lock_key == item->data(COLUMN_TITLE, User_Roles::Key).toLongLong()) {               // grab stored Key from QTreeWidgetItem user data
            selected = true;
            break;
        }
    }

    // Make a list of locks to process, either just the one, or all items that are selected
    QList<QCheckBox*> locks;
    if (selected == false) {
        locks.append(from_lock);
    } else {
        for (auto item : item_list) {
            QCheckBox *check_box = dynamic_cast<QCheckBox*>(this->itemWidget(item, COLUMN_LOCK));
            if (check_box) locks.append(check_box);
        }
    }

    // Process locks
    for (auto lock : locks) {
        long key = lock->property(User_Property::Key).toLongLong();
        DrSettings *entity = getParentProject()->findSettingsFromKey( key );
        if (entity == nullptr) continue;

        if (entity->getComponentPropertyValue(Components::Hidden_Settings, Properties::Hidden_Item_Locked).toBool()) {
            lock->setCheckState( Qt::CheckState::Checked );
        } else {
            entity->setLocked( locked );
        }
        lock->setCheckState( entity->isLocked() ? Qt::CheckState::Checked : Qt::CheckState::Unchecked );
    }

    // Update Object Inspector Properties and Update StageView selected items
    this->m_editor_relay->updateInspectorEnabledProperties();
    this->m_editor_relay->updateItemSelection(Editor_Widgets::Project_Tree);
}












