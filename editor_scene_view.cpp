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
#include "editor_scene_scene.h"
#include "interface_relay.h"


//####################################################################################
//##        Constructor & destructor
//####################################################################################
SceneGraphicsView::SceneGraphicsView(QWidget *parent, DrProject *project, InterfaceRelay *relay) :
                                     QGraphicsView(parent = nullptr), m_project(project), m_relay(relay)
{
    // Initialize rubber band object used as a selection box
    m_rubber_band = new SceneViewRubberBand(QRubberBand::Rectangle, this, relay);

    m_over_handle = Position_Flags::No_Position;

    timer.start();
}
SceneGraphicsView::~SceneGraphicsView() { }



//####################################################################################
//##        Scene Change SLOTs / Events to paint view when scene changes
//####################################################################################
// Connected from scene().changed
void SceneGraphicsView::sceneChanged(QList<QRectF>)
{
    //update();             // Calls paint recursively
}

// Connected from scene().selectionChanged
void SceneGraphicsView::selectionChanged()
{
    //update();             // Calls paint recursively
}

void SceneGraphicsView::scrollContentsBy(int dx, int dy)
{
    QGraphicsView::scrollContentsBy(dx, dy);
    update();
}



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
//##        Returns a scene rect containing all the selected items
QRectF SceneGraphicsView::totalSelectedItemsSceneRect()
{
    if (scene() == nullptr) return QRectF();

    SceneGraphicsScene *my_scene = dynamic_cast<SceneGraphicsScene*>(scene());

    return my_scene->totalSelectedItemsSceneRect();
}



















