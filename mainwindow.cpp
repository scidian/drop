//
//
//      Loads, styles and handles events for MainWindow
//
//

#include "ui_mainwindow.h"
#include "form_coloring.h"
#include "form_styling.h"
#include "mainwindow.h"

#include "01_project.h"
#include "02_world.h"
#include "03_scene.h"
#include "04_object.h"

#include "30_settings.h"
#include "31_component.h"
#include "32_property.h"

#include <QCheckBox>
#include <QMessageBox>
#include <QDrag>


// Destructor for Main Window
MainWindow::~MainWindow()
{
    delete ui;
    delete project;
}


// Constructor for Main Window
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // Initial data loading
    loadPalettes();

    // Initialize form and customize colors and styles
    ui->setupUi(this);


//    // Create our custom TreeWidget, with custom styling
//    treeScene = new MyTreeView(ui->splitterHorizontal, this);
//    treeScene->setStyle(new SceneTreeHighlightProxy(treeScene->style(), treeScene));

    adjustMainWindowUi(this);
    applyMainWindowPalette(this, current_color_scheme);


    // Initialize new project, initialize local variables
    project = new DrProject();

    current_world = 0;





    // TEMP NEW PROJECT:
    //      Create a new project and add some stuff to it
    project->addWorld();
    project->getWorldWithName("World 2")->addScene();
    project->getWorldWithName("World 2")->addScene("asdfasdfasdfasdfasfdasdfasdfasdfasdfasdfasdfasdfasd");
    project->getWorldWithName("World 2")->addScene();
    project->getWorldWithName("World 2")->addScene();
    project->getWorldWithName("World 2")->addScene();
    project->getWorldWithName("World 2")->getSceneWithName("4")->addObject(DrTypes::Object);
    project->getWorldWithName("World 2")->getSceneWithName("4")->addObject(DrTypes::Object);
    project->addWorld();
    project->addWorld();




    // TEMP LIST POPULATE:
    //      Populates list based on project data
    for (auto world_pair: project->getWorldMap())
    {
        QTreeWidgetItem *topLevelItem = new QTreeWidgetItem(treeScene);                             // Create new item (top level item)

        topLevelItem->setIcon(0, QIcon(":/tree_icons/tree_world.png"));                                 // Loads icon for world
        topLevelItem->setText(0, "World: " + world_pair.second->getComponentPropertyValue(
                                 World_Components::settings, World_Properties::name).toString());       // Set text for item
        topLevelItem->setData(0, Qt::UserRole, QVariant::fromValue(world_pair.second->getKey()));
        treeScene->addTopLevelItem(topLevelItem);                                                   // Add it on our tree as the top item.

        for (auto scene_pair: world_pair.second->getSceneMap())
        {
            QTreeWidgetItem *sub_item = new QTreeWidgetItem(topLevelItem);                              // Create new item and add as child item
            sub_item->setIcon(0, QIcon(":/tree_icons/tree_scene.png"));                                 // Loads icon for scene
            sub_item->setText(0, "Scene: " + scene_pair.second->getComponentPropertyValue(
                                 Scene_Components::settings, Scene_Properties::name).toString());       // Set text for item
            sub_item->setData(0, Qt::UserRole, QVariant::fromValue(scene_pair.second->getKey()));

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
                sub_sub_item->setData(0, Qt::UserRole, QVariant::fromValue(object_pair.second->getKey()));          // Store item key in user data

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



    // TEMP:
    //      Test loading data out from a pair, i.e. "POINT2D", stored as QList<QVariant>
    QList<QVariant> myPoint = project->getWorld(current_world)->getComponentProperty(World_Components::physics, World_Properties::gravity)->getValue().toList();
    ui->label_1->setText("Gravity X:" + myPoint.first().toString());
    ui->label_2->setText("Gravity Y:" + myPoint.last().toString());


    // DISPLAY MESSAGEBOX TO KNOW WE MADE IT HERE
    //QMessageBox *msgBox = new QMessageBox(nullptr);
    //msgBox->setText("Made It 2");
    //msgBox->exec();
}


MyTreeView* MainWindow::getSceneListWidget()
{
    return treeScene;
}

void MainWindow::setSceneListWidget(MyTreeView *new_tree_scene)
{
    treeScene = new_tree_scene;
}




//##############################################################
//
//  Testing drag event
//

// This removes the item from under the mouse
void MyTreeView::startDrag(Qt::DropActions supportedActions)
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
void MyTreeView::dragMoveEvent(QDragMoveEvent *event)
{
    m_mouse_x = event->pos().x();
    m_mouse_y = event->pos().y();
    getMainWindow()->getUi()->label_1->setText(QString::fromStdString("MX: ") + QString::number(m_mouse_x) +
                                               QString::fromStdString(", MY: ") + QString::number(m_mouse_y) );

    // Get item under mouse, if not null lets process it to see if we will allow dropping there
    QTreeWidgetItem *item_at = this->itemAt(event->pos());
    m_can_drop = false;
    if (item_at != nullptr)
    {
        long        check_key = item_at->data(0, Qt::UserRole).toLongLong();
        getMainWindow()->getUi()->label_object_3->setText(QString::fromStdString("Selected: " + std::to_string(m_selected_key) +
                                                                                 ", Checking: " + std::to_string(check_key)) );
        // Check if its the same type as already selected, if so allow possible drop
        if (m_is_dragging && m_selected_key != 0 && check_key != 0) {
            DrSettings *check_settings = getMainWindow()->getProject()->findSettingsFromKey(check_key);
            DrSettings *selected_settings = getMainWindow()->getProject()->findSettingsFromKey(m_selected_key);

            if ( CheckTypesAreSame(check_settings->getType(), selected_settings->getType()) ) { m_can_drop = true; }
        }
    }

    setDropIndicatorShown(true);
    m_is_dragging = true;
    QTreeWidget::dragMoveEvent(event);
}

void MyTreeView::dropEvent(QDropEvent* event)
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


//##############################################################
//
// Updates selection, checks to make sure if more than one item is selected all new items
//                    not matching original type are not selected
//
void MyTreeView::selectionChanged (const QItemSelection &selected, const QItemSelection &deselected)
{
    getMainWindow()->listSelectionChanged(this->selectedItems());
    QTreeWidget::selectionChanged(selected, deselected);
}

void MainWindow::listSelectionChanged(QList<QTreeWidgetItem*> item_list)
{
    ui->label_3->setText("Selected Items: " + QString::number(item_list.size()));

    // If size of list is zero, clear selected_key and exit function
    if (item_list.size() == 0) {
        treeScene->setSelectedKey(0);
        return;
    }

    // Otherwise add first item to label, if size is one, reset first selected item
    ui->label_3->setText(ui->label_3->text() + ", First Item: " + item_list.first()->text(0));

    if (item_list.size() == 1){
        treeScene->setSelectedKey(item_list.first()->data(0, Qt::UserRole).toLongLong());           // grab stored key from list view user data
        buildObjectInspector();
    }
    else {
        DrTypes selected_type = project->findTypeFromKey( treeScene->getSelectedKey() );

        // Check if newly selected items are same type, if not, do not allow select
        for (auto check_item: item_list) {
            // Get key from each item so we can compare it to first selected item
            long    check_key = check_item->data(0, Qt::UserRole).toLongLong();
            DrTypes check_type = project->findTypeFromKey(check_key);

            // If we are over item that was first selected, skip to next
            if (check_key == treeScene->getSelectedKey()) { continue; }

            if (CheckTypesAreSame(check_type, selected_type) == false) {
                check_item->setSelected(false);
            }
        }
    }
}









//#######################################################################
//
//  Need to finish dynamically building object inspector
//
void MainWindow::buildObjectInspector()
{
    // First, retrieve unique key of item clicked in list
    DrTypes     selected_type = project->findTypeFromKey( treeScene->getSelectedKey() );
    std::string type_string = StringFromType(selected_type);
    ui->label_object->setText("KEY: " + QString::number( treeScene->getSelectedKey() ) + ", TYPE: " + QString::fromStdString(type_string));
    ui->label_object_2->setText("");
    ui->label_object_3->setText("");


    // Reset / clear tableWidget
    ui->tableWidget->clear();

    ComponentMap components = project->findSettingsFromKey( treeScene->getSelectedKey() )->getComponentList();


    //####**** Build Object Inspector!!!!! ****####


    // Build tables in Object Inspector
    int rowCount = 0;
    for (auto i: components)
    {

        for (auto j: i.second->getPropertyList())
        {
            QTableWidgetItem *new_item = new QTableWidgetItem(QString::fromStdString(j.second->getDisplayName()));
            new_item->setData(Qt::UserRole, QVariant::fromValue(j.second->getPropertyKey()));

            // If no rows yet, create some, otherwise don't
            if (ui->tableWidget->rowCount() < (rowCount + 1))
            {
                ui->tableWidget->insertRow(rowCount);
            }

            ui->tableWidget->setItem(rowCount, 0, new_item);

            rowCount++;
        }
    }
}




//##############################################################
//
//  On object inspector click show info about object and property
//
void MainWindow::on_tableWidget_itemClicked(QTableWidgetItem *item)
{
    // If no item is selected in tree view, exit function
    if (treeScene->getSelectedKey() == 0) { return; }

    // First, retrieve property key of item clicked in tableWidget list
    long        property_key = item->data(Qt::UserRole).toLongLong();

    // Grab a pointer to the component list of the first selected item from treeScene (stored in selected_list)
    DrSettings  *selected_item_settings = project->findSettingsFromKey( treeScene->getSelectedKey() );
    DrComponent *clicked_component = selected_item_settings->findComponentFromPropertyKey(property_key);
    DrProperty  *clicked_property = clicked_component->getProperty(property_key);

    std::string property_name = clicked_property->getDisplayName();
    std::string component_name = clicked_component->getDisplayName();
    long        component_key = clicked_component->getComponentKey();

    // Grab type of main selected item in selected tree list
    std::string type_string2 = StringFromType(project->findTypeFromKey( treeScene->getSelectedKey() ));
    std::string type_string = StringFromType(selected_item_settings->getType());

    ui->label_object->setText("KEY: " + QString::number( treeScene->getSelectedKey() ) + ", TYPE: " + QString::fromStdString(type_string));
    ui->label_object_2->setText("COMPONENT: " + QString::number(component_key) +   ", NAME: " + QString::fromStdString(component_name));
    ui->label_object_3->setText("PROPERTY: " + QString::number(property_key) +   ", NAME: " + QString::fromStdString(property_name));

}








