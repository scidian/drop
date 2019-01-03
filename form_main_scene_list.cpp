//
//      Created by Stephens Nunnally on 12/13/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Handles dealing with the Scene List
//
//

#include "01_project.h"
#include "02_world.h"
#include "03_scene.h"
#include "04_object.h"

#include "form_main.h"


//####################################################################################
//##        Populates Tree Scene List based on project data
//####################################################################################
void FormMain::populateTreeSceneList()
{
    treeScene->clear();

    for (auto world_pair: project->getWorldMap())
    {
        QTreeWidgetItem *topLevelItem = new QTreeWidgetItem(treeScene);                                 // Create new item (top level item)

        topLevelItem->setIcon(0, QIcon(":/tree_icons/tree_world.png"));                                 // Loads icon for world
        topLevelItem->setText(0, "World: " + world_pair.second->getComponentPropertyValue(
                                 World_Components::settings, World_Properties::name).toString());       // Set text for item
        topLevelItem->setData(0, User_Roles::Key, QVariant::fromValue(world_pair.second->getKey()));
        treeScene->addTopLevelItem(topLevelItem);                                                       // Add it on our tree as the top item.

        for (auto scene_pair: world_pair.second->getSceneMap())
        {
            QTreeWidgetItem *sub_item = new QTreeWidgetItem(topLevelItem);                              // Create new item and add as child item
            sub_item->setIcon(0, QIcon(":/tree_icons/tree_scene.png"));                                 // Loads icon for scene
            sub_item->setText(0, "Scene: " + scene_pair.second->getComponentPropertyValue(
                                 Scene_Components::settings, Scene_Properties::name).toString());       // Set text for item
            sub_item->setData(0, User_Roles::Key, QVariant::fromValue(scene_pair.second->getKey()));

            for (auto object_pair: scene_pair.second->getObjectMap())
            {
                QTreeWidgetItem *sub_sub_item = new QTreeWidgetItem(sub_item);                                      // Create new item and add as child item
                switch (object_pair.second->getType())
                {
                    case DrTypes::Object:    sub_sub_item->setIcon(0, QIcon(":/tree_icons/tree_object.png")); break;
                    case DrTypes::Camera:    sub_sub_item->setIcon(0, QIcon(":/tree_icons/tree_camera.png")); break;
                    case DrTypes::Character: sub_sub_item->setIcon(0, QIcon(":/tree_icons/tree_character.png")); break;
                    default: break;
                }

                sub_sub_item->setText(0, object_pair.second->getComponentPropertyValue(
                                         Object_Components::settings, Object_Properties::name).toString());         // Set text for item
                sub_sub_item->setData(0, User_Roles::Key, QVariant::fromValue(object_pair.second->getKey()));       // Store item key in user data



                sub_item->addChild(sub_sub_item);

                // Add lock box
                QString check_images = QString(" QCheckBox::indicator { width: 12px; height: 12px; }"
                                               " QCheckBox::indicator:checked { image: url(:/tree_icons/tree_lock.png); }"
                                               " QCheckBox::indicator:unchecked { image: url(:/tree_icons/tree_bullet.png); }");
                QCheckBox *lock_item = new QCheckBox();
                lock_item->setStyleSheet(check_images);
                treeScene->setItemWidget(sub_sub_item, 1, lock_item);

            }
        }
    }
    treeScene->expandAll();                                             // Expand all items
}




// Handles changing the Advisor on Mouse Enter
void TreeSceneView::enterEvent(QEvent *event)
{
    getMainWindow()->setAdvisorInfo(Advisor_Info::Scene_List);
    QTreeWidget::enterEvent(event);
}




//####################################################################################
//
//  Testing drag event
//

// This removes the item from under the mouse, sort of
void TreeSceneView::startDrag(Qt::DropActions supportedActions)
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
void TreeSceneView::dragMoveEvent(QDragMoveEvent *event)
{
    m_mouse_x = event->pos().x();
    m_mouse_y = event->pos().y();
    getMainWindow()->setLabelText(Label_Names::Label1, QString::fromStdString("MX: ") + QString::number(m_mouse_x) +
                                                       QString::fromStdString(", MY: ") + QString::number(m_mouse_y) );

    // Get item under mouse, if not null lets process it to see if we will allow dropping there
    QTreeWidgetItem *item_at = this->itemAt(event->pos());
    m_can_drop = false;
    if (item_at != nullptr)
    {
        long        check_key = item_at->data(0, User_Roles::Key).toLongLong();
        getMainWindow()->setLabelText(Label_Names::LabelObject3, QString::fromStdString("Selected: " + std::to_string(m_selected_key) +
                                                                                        ", Checking: " + std::to_string(check_key)) );
        // Check if its the same type as already selected, if so allow possible drop
        if (m_is_dragging && m_selected_key != 0 && check_key != 0) {
            DrSettings *check_settings = getMainWindow()->project->findSettingsFromKey(check_key);
            DrSettings *selected_settings = getMainWindow()->project->findSettingsFromKey(m_selected_key);

            if ( CheckTypesAreSame(check_settings->getType(), selected_settings->getType()) ) { m_can_drop = true; }
        }
    }

    setDropIndicatorShown(true);
    m_is_dragging = true;
    QTreeWidget::dragMoveEvent(event);
}

void TreeSceneView::dropEvent(QDropEvent* event)
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
//
// Updates selection, checks to make sure if more than one item is selected all new items
//                    not matching original type are not selected
//
void TreeSceneView::selectionChanged (const QItemSelection &selected, const QItemSelection &deselected)
{
    getMainWindow()->listSelectionChanged(this->selectedItems());           // pass event to form main
    QTreeWidget::selectionChanged(selected, deselected);                    // pass event to parent
}

void FormMain::listSelectionChanged(QList<QTreeWidgetItem*> item_list)
{
    label_3->setText("Selected Items: " + QString::number(item_list.size()));

    // If size of list is zero, clear selected_key and exit function
    if (item_list.size() == 0) {
        treeScene->setSelectedKey(0);
        return;
    }

    // Otherwise add first item to label, if size is one, reset first selected item
    label_3->setText(label_3->text() + ", First Item: " + item_list.first()->text(0));

    if (item_list.size() == 1) {
        long selected_key = item_list.first()->data(0, User_Roles::Key).toLongLong();       // grab stored key from list view user data
        treeScene->setSelectedKey(selected_key);

        //******************************************************
        // Call to outside function to rebuild object inspector:
        buildObjectInspector(QList<long> { selected_key });
        //******************************************************

    } else {
        DrTypes selected_type = project->findTypeFromKey( treeScene->getSelectedKey() );

        // Check if newly selected items are same type, if not, do not allow select
        for (auto check_item: item_list) {
            // Get key from each item so we can compare it to first selected item
            long    check_key = check_item->data(0, User_Roles::Key).toLongLong();
            DrTypes check_type = project->findTypeFromKey(check_key);

            // If we are over item that was first selected, skip to next
            if (check_key == treeScene->getSelectedKey()) { continue; }

            if (CheckTypesAreSame(check_type, selected_type) == false)
                check_item->setSelected(false);
        }
    }
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

        painter->setRenderHint(QPainter::Antialiasing, true);

        QPalette palette;
        QPen    pen(QColor(0, 180, 175, 200));
        QBrush  brush(QColor(0, 180, 175, 50));
        pen.setWidth(2);
        painter->setPen(pen);
        painter->setBrush(brush);

        if(option->rect.height() == 0) {
            //painter->drawEllipse(option->rect.topLeft(), 3, 3);
            painter->drawLine(QPoint(0, option->rect.top()), QPoint(option->rect.right() + 50, option->rect.top()));
        } else {
            //painter->drawRoundedRect(option->rect, 5, 5);
            painter->drawLine(QPoint(0, option->rect.top()), QPoint(option->rect.right() + 50, option->rect.top()));
        }

        m_parent_tree->getMainWindow()->setLabelText(Label_Names::Label2, QString::fromStdString("TLX: ") + QString::number(option->rect.topLeft().x()) +
                                                                          QString::fromStdString(", TLY: ") + QString::number(option->rect.topLeft().y()));
    } else {
        QProxyStyle::drawPrimitive(element, option, painter, widget);
    }
}








