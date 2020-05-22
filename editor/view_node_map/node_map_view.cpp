//
//      Created by Stephens Nunnally on 4/30/2020, (c) 2020 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QDebug>
#include "editor/helper_library.h"
#include "editor/interface_editor_relay.h"
#include "editor/view_node_map/node_map_item.h"
#include "editor/view_node_map/node_map_scene.h"
#include "editor/view_node_map/node_map_view.h"
#include "editor/widgets/widgets_editor.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
NodeMapView::NodeMapView(QWidget *parent, DrProject *project, NodeMapScene *scene, IEditorRelay *editor_relay)
    : QGraphicsView(parent), m_project(project), m_editor_relay(editor_relay) {

    // ***** Initialize rubber band object used as a selection box
    m_rubber_band = new EditorViewRubberBand(QRubberBand::Shape::Rectangle, this);

    // ***** Initialize variables
    updateGrid();
    my_scene = scene;
    setScene(scene);

    // ***** Connect Signals
    connect(my_scene, SIGNAL(aboutToClear()),   this, SLOT(sceneIsAboutToClear()));
}

NodeMapView::~NodeMapView() { }

// SLOT:    Chance to reset local pointer variables when Scene is about to clear() (like when switching Editor_Mode)
//       ...After Scene is rebuilt local reference pointers will be dangling
void NodeMapView::sceneIsAboutToClear() {
    ///m_origin_item = nullptr;
}

















