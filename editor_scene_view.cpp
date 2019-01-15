//
//      Created by Stephens Nunnally on 1/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Graphics View Definitions
//
//

#include "project.h"
#include "project_world.h"
#include "project_world_scene.h"
#include "project_world_scene_object.h"
#include "editor_scene_item.h"

#include "settings.h"
#include "settings_component.h"
#include "settings_component_property.h"

#include "editor_scene_view.h"
#include "interface_relay.h"


//####################################################################################
//##        Constructor & destructor
//####################################################################################
SceneGraphicsView::SceneGraphicsView(QWidget *parent, DrProject *project, InterfaceRelay *interface) :
        QGraphicsView(parent = nullptr), m_project(project), m_interface(interface)
{
    // Initialize rubber band object used as a selection box
    m_rubber_band = new SceneViewRubberBand(QRubberBand::Rectangle, this, interface);

    m_over_handle = Position_Flags::No_Position;
}
SceneGraphicsView::~SceneGraphicsView() { }



//####################################################################################
//##        Key Events
//####################################################################################
// Key press event
void SceneGraphicsView::keyPressEvent(QKeyEvent *event)
{
    // When space bar is down, enabled mouse press and move to translate viewable area
    if (event->key() == Qt::Key::Key_Space) {   m_flag_key_down_spacebar = true;
        setDragMode(QGraphicsView::DragMode::ScrollHandDrag);
        setInteractive(false); }

    // Store when control / alt are down
    if (event->key() == Qt::Key::Key_Control) { m_flag_key_down_control = true; }
    if (event->key() == Qt::Key::Key_Alt) {     m_flag_key_down_alt = true; }

    QGraphicsView::keyPressEvent(event);
}

// Key release event
void SceneGraphicsView::keyReleaseEvent(QKeyEvent *event)
{
    // When space bar is released, change mode back to select / move items
    if (event->key() == Qt::Key::Key_Space) {   m_flag_key_down_spacebar = false;
        setDragMode(QGraphicsView::DragMode::NoDrag);
        setInteractive(true); }

    // Store when control / alt are released
    if (event->key() == Qt::Key::Key_Control) { m_flag_key_down_control = false; }
    if (event->key() == Qt::Key::Key_Alt) {     m_flag_key_down_alt = false; }

    QGraphicsView::keyReleaseEvent(event);
}



//####################################################################################
//##        Selection Rect Update Handling - SLOTs to keep total selection area up to date
//####################################################################################

// Connected from scene().changed
void SceneGraphicsView::sceneChanged(QList<QRectF>)
{
    updateSelectionRect();
}

// Connected from scene().selectionChanged
void SceneGraphicsView::selectionChanged()
{
    updateSelectionRect();
}


// Store total size of selection rectangle
void SceneGraphicsView::updateSelectionRect()
{
    m_selection_rect = totalSelectedItemsSceneRect();
    update();

    // !!!!! TEMP:
    m_interface->setLabelText(Label_Names::Label_1, "Group Pos  X: " + QString::number(m_selection_rect.x()) +
                                                             ", Y: " + QString::number(m_selection_rect.y()) );
    m_interface->setLabelText(Label_Names::Label_2, "Group Size X: " + QString::number(m_selection_rect.width()) +
                                                             ", Y: " + QString::number(m_selection_rect.height()) );

    m_interface->setLabelText(Label_Names::Label_Object_1, "Scene X: " + QString::number(sceneRect().x()) +
                                                               ", Y: " + QString::number(sceneRect().y()) );
    m_interface->setLabelText(Label_Names::Label_Object_2, "Scene W: " + QString::number(sceneRect().width()) +
                                                               ", H: " + QString::number(sceneRect().height()) );
    // !!!!! END
}


// Returns a scene rect containing all the selected items
QRectF SceneGraphicsView::totalSelectedItemsSceneRect()
{
    QRectF total_rect;

    if (scene() == nullptr) return total_rect;
    if (scene()->selectedItems().count() < 1) return total_rect;

    // Start with size of first item
    total_rect = scene()->selectedItems().first()->sceneBoundingRect();

    // Add on each items Rect to store total size of selection rectangle
    for (auto item: scene()->selectedItems())
        total_rect = total_rect.united(item->sceneBoundingRect());

    return total_rect;
}
























