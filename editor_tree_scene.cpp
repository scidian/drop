//
//      Created by Stephens Nunnally on 1/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Tree Scene Definitions
//
//

#include "project.h"
#include "project_world.h"
#include "project_world_scene.h"
#include "project_world_scene_object.h"

#include "settings.h"
#include "settings_component.h"
#include "settings_component_property.h"

#include "editor_tree_scene.h"
#include "interface_relay.h"


//####################################################################################
//##        Populates Tree Scene List based on project data
//####################################################################################
void TreeScene::populateTreeSceneList()
{
    this->clear();

    for (auto world_pair: m_project->getWorlds())
    {
        QTreeWidgetItem *world_item = new QTreeWidgetItem(this);                                            // Create new item (top level item)

        world_item->setIcon(0, QIcon(":/tree_icons/tree_world.png"));                                       // Loads icon for world
        world_item->setText(0, "World: " + world_pair.second->getComponentPropertyValue(
                               World_Components::settings, World_Properties::name).toString());             // Set text for item
        world_item->setData(0, User_Roles::Key, QVariant::fromValue(world_pair.second->getKey()));
        this->addTopLevelItem(world_item);                                                                  // Add it on our tree as the top item.

        for (auto scene_pair: world_pair.second->getSceneMap())
        {
            QTreeWidgetItem *scene_item = new QTreeWidgetItem(world_item);                                  // Create new item and add as child item
            scene_item->setIcon(0, QIcon(":/tree_icons/tree_scene.png"));                                   // Loads icon for scene
            scene_item->setText(0, "Scene: " + scene_pair.second->getComponentPropertyValue(
                                   Scene_Components::settings, Scene_Properties::name).toString());         // Set text for item
            scene_item->setData(0, User_Roles::Key, QVariant::fromValue(scene_pair.second->getKey()));


            // ***** Iterates through objects based on z-order of each object
            ObjectMap   objects = scene_pair.second->getObjectMap();
            QList<long> keys = scene_pair.second->objectKeysSortedByZOrder();
            for (auto key: keys)
            {
                DrObject *object = objects[key];

                QTreeWidgetItem *object_item = new QTreeWidgetItem(scene_item);                             // Create new item and add as child item
                switch (object->getType())
                {
                    case DrType::Object:    object_item->setIcon(0, QIcon(":/tree_icons/tree_object.png")); break;
                    case DrType::Camera:    object_item->setIcon(0, QIcon(":/tree_icons/tree_camera.png")); break;
                    case DrType::Character: object_item->setIcon(0, QIcon(":/tree_icons/tree_character.png")); break;
                    default: break;
                }

                object_item->setText(0, object->getComponentPropertyValue(
                                        Object_Components::settings, Object_Properties::name).toString());          // Set text for item
                object_item->setData(0, User_Roles::Key, QVariant::fromValue(object->getKey()));        // Store item key in user data

                scene_item->addChild(object_item);

                // Add lock box
                QString check_images = QString(" QCheckBox::indicator { width: 12px; height: 12px; }"
                                               " QCheckBox::indicator:checked { image: url(:/tree_icons/tree_lock.png); }"
                                               " QCheckBox::indicator:unchecked { image: url(:/tree_icons/tree_bullet.png); }");
                QCheckBox *lock_item = new QCheckBox();
                lock_item->setFocusPolicy(Qt::FocusPolicy::NoFocus);
                lock_item->setStyleSheet(check_images);
                this->setItemWidget(object_item, 1, lock_item);

            }
        }
    }
    this->expandAll();                                             // Expand all items
}




// Handles changing the Advisor on Mouse Enter
void TreeScene::enterEvent(QEvent *event)
{
    m_relay->setAdvisorInfo(Advisor_Info::Scene_List);
    QTreeWidget::enterEvent(event);
}




//####################################################################################
//##        Testing drag event
//####################################################################################
// This removes the item from under the mouse, sort of
void TreeScene::startDrag(Qt::DropActions supportedActions)
{
    // Partly copied from Qt 5.5.5 sources
    QModelIndexList indexes = selectedIndexes();
    if (indexes.count() > 0) {
        // Get a list of the supported MIMEs of the selected indexes
        QMimeData* data = model()->mimeData(indexes);
        if (!data) {
            return;
        }

        // Create the drag object
        QDrag* drag = new QDrag(this);
        drag->setMimeData(data);
        drag->setPixmap(QPixmap());

        // Execute the drag
        drag->exec(supportedActions, Qt::MoveAction);
    }
}


// Fires when we start dragging
void TreeScene::dragMoveEvent(QDragMoveEvent *event)
{
    m_mouse_x = event->pos().x();
    m_mouse_y = event->pos().y();

    // !!!!! #DEBUG:    Show scene tree drag event info
    if (Dr::CheckDebugFlag(Debug_Flags::Label_Scene_Tree_Drag)) {
        m_relay->setLabelText(Label_Names::Label_1, QString::fromStdString("MX: ") + QString::number(m_mouse_x) +
                                                    QString::fromStdString(", MY: ") + QString::number(m_mouse_y) );
    }
    // !!!!! END

    // Get item under mouse, if not null lets process it to see if we will allow dropping there
    QTreeWidgetItem *item_at = this->itemAt(event->pos());
    m_can_drop = false;
    if (item_at != nullptr)
    {
        long        check_key = item_at->data(0, User_Roles::Key).toLongLong();

        // !!!!! #DEBUG:    Show scene tree drag event info
        if (Dr::CheckDebugFlag(Debug_Flags::Label_Scene_Tree_Drag)) {
            m_relay->setLabelText(Label_Names::Label_Object_3, "Selected: " + QString::number(m_selected_key) +
                                                             ", Checking: " + QString::number(check_key) );
        }
        // !!!!! END

        // Check if its the same type as already selected, if so allow possible drop
        if (m_is_dragging && m_selected_key != 0 && check_key != 0) {
            DrSettings *check_settings = m_project->findChildSettingsFromKey(check_key);
            DrSettings *selected_settings = m_project->findChildSettingsFromKey(m_selected_key);

            if ( CheckTypesAreSame(check_settings->getType(), selected_settings->getType()) ) { m_can_drop = true; }
        }
    }

    setDropIndicatorShown(true);
    m_is_dragging = true;
    QTreeWidget::dragMoveEvent(event);
}

void TreeScene::dropEvent(QDropEvent* event)
{
    bool dropOK = false;
    DropIndicatorPosition dropIndicator = dropIndicatorPosition();

    switch (dropIndicator)
    {
        case QAbstractItemView::AboveItem:  dropOK = true;  break;
        case QAbstractItemView::BelowItem:  dropOK = true;  break;
        case QAbstractItemView::OnItem:     dropOK = false; break;
        case QAbstractItemView::OnViewport: dropOK = false; break;
    }

    if (dropOK && m_can_drop)
    {
        // ########## !!!!!!!!! Here we need manage the case of dropping an item


        QTreeWidget::dropEvent(event);              // Pass event through to base
    }
    else
    {
        event->ignore();
    }
    setDropIndicatorShown(false);                   // hide the drop indicator once the drop is done
    m_is_dragging = false;
}


//####################################################################################
//##        Updates selection
//##            Checks to make sure if more than one item is selected all new items
//##            not matching original type are not selected
//####################################################################################
void TreeScene::selectionChanged (const QItemSelection &selected, const QItemSelection &deselected)
{
    QList<QTreeWidgetItem*> item_list = this->selectedItems();

    // !!!!! #DEBUG:    Show Scene Tree selection data
    if (Dr::CheckDebugFlag(Debug_Flags::Label_Scene_Tree_Selection))
        m_relay->setLabelText(Label_Names::Label_Bottom, QString("Selected Items: ") + QString::number(item_list.size()));
    // !!!!! END

    // If size of list is zero, clear selected_key and exit function
    if (item_list.size() == 0) {
        this->setSelectedKey(0);
        return;
    }

    // !!!!! #DEBUG:    Show Scene Tree selection data
    // Otherwise add first item to label
    if (Dr::CheckDebugFlag(Debug_Flags::Label_Scene_Tree_Selection)) {
        m_relay->setLabelText(Label_Names::Label_Bottom, QString("Selected Items: ") +
                                                         QString::number(item_list.size()) +
                                                         ", First Item: " + item_list.first()->text(0));
    }
    // !!!!! END

    // If size is one, reset first selected item
    if (item_list.size() == 1) {
        long selected_key = item_list.first()->data(0, User_Roles::Key).toLongLong();       // grab stored key from list view user data
        this->setSelectedKey(selected_key);

        //******************************************************
        // Call to outside function to rebuild object inspector:
        m_relay->buildObjectInspector(QList<long> { selected_key });

        m_relay->populateScene(selected_key);

        //******************************************************

    } else {
        DrType selected_type = m_project->findChildTypeFromKey( this->getSelectedKey() );

        // Check if newly selected items are same type, if not, do not allow select
        for (auto check_item: item_list) {
            // Get key from each item so we can compare it to first selected item
            long    check_key = check_item->data(0, User_Roles::Key).toLongLong();
            DrType  check_type = m_project->findChildTypeFromKey(check_key);

            // If we are over item that was first selected, skip to next
            if (check_key == this->getSelectedKey()) { continue; }

            if (CheckTypesAreSame(check_type, selected_type) == false)
                check_item->setSelected(false);
        }
    }

    QTreeWidget::selectionChanged(selected, deselected);                    // pass event to parent
}




//####################################################################################
//##    SceneTreeHighlightStyle
//##        A sub classed QProxyStyle so we can overwrite events and do some custom
//##        drawing of TreeWidget list divider in Tree Scene List
//####################################################################################
SceneTreeHighlightProxy::~SceneTreeHighlightProxy()
{
    // Must include definition of a virtual destructor
}

void SceneTreeHighlightProxy::drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    if (element == QStyle::PE_IndicatorItemViewItemDrop) {
        if (!m_parent_tree->canWeDrop()) { return; }

        QPalette palette;
        QPen    pen(QColor(0, 180, 175, 200));
        QBrush  brush(QColor(0, 180, 175, 50));

        pen.setWidth(2);
        painter->setPen(pen);
        painter->setBrush(brush);

        if(option->rect.height() == 0) {

            painter->drawLine(QPoint(0, option->rect.top()), QPoint(option->rect.right() + 50, option->rect.top()));
        } else {

            painter->drawLine(QPoint(0, option->rect.top()), QPoint(option->rect.right() + 50, option->rect.top()));
        }

        // !!!!! #DEUBG:    Show custom highlight event data
        if (Dr::CheckDebugFlag(Debug_Flags::Label_Scene_Tree_Drag)) {
            m_relay->setLabelText(Label_Names::Label_2, QString::fromStdString("TLX: ") + QString::number(option->rect.topLeft().x()) +
                                                        QString::fromStdString(", TLY: ") + QString::number(option->rect.topLeft().y()));
        }
        // !!!!! END

    } else {
        QProxyStyle::drawPrimitive(element, option, painter, widget);
    }
}








