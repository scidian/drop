//
//      Created by Stephens Nunnally on 4/9/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include "editor/tree_project.h"
#include "interface_editor_relay.h"
#include "project/project.h"
#include "project/project_world.h"
#include "project/project_world_stage.h"
#include "project/project_world_stage_thing.h"


//####################################################################################
//##    Updates item names if they have been changed
//####################################################################################
void TreeProject::updateItems(QList<DrSettings*> changed_entities, QList<long> property_keys) {
    if (changed_entities.isEmpty()) return;

    setAllowSelectionEvent(false);

    for (auto entity : changed_entities) {
        long item_key = entity->getKey();
        for (auto row : getListOfAllTreeWidgetItems()) {
            long row_key = row->data(0, User_Roles::Key).toLongLong();

            if (item_key == row_key) {
                for (auto property : property_keys) {
                    Properties check_property = static_cast<Properties>(property);

                    if (check_property == Properties::Entity_Name) {
                        switch (entity->getType()) {
                            case DrType::World:
                                row->setText(0, "World: " + entity->getName());
                                break;
                            case DrType::Stage:
                                row->setText(0, "Stage: " + entity->getName());
                                break;
                            case DrType::Thing:
                                row->setText(0, entity->getName());
                                break;
                            default: ;
                        }
                    }

                    if (check_property == Properties::Hidden_Item_Locked ||
                        check_property == Properties::Hidden_Hide_From_Trees) {
                        if (entity->getType() == DrType::Thing) installLockBox(entity, row);
                    }
                }
            }
        }
    }
    update();
    setAllowSelectionEvent(true);
}


//####################################################################################
//##    Installs / Updates Lock box widget for an item
//####################################################################################
void TreeProject::installLockBox(DrSettings *entity, QTreeWidgetItem *thing_item) {
    if (entity->getType() != DrType::Thing) return;
    DrThing *thing = dynamic_cast<DrThing*>(entity);

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
















