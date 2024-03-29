//
//      Created by Stephens Nunnally on 4/30/2020, (c) 2020 Scidian Studios, All Rights Reserved
//
//  File:
//
//
//
#include <QDebug>
#include "editor/helper_library.h"
#include "editor/interface_editor_relay.h"
#include "editor/view_node/node_item.h"
#include "editor/view_node/node_scene.h"
#include "editor/view_node/node_view.h"
#include "editor/widgets/widgets_view.h"


//####################################################################################
//##    Constructor / Destructor
//####################################################################################
NodeView::NodeView(QWidget *parent, DrProject *project, NodeScene *scene, IEditorRelay *editor_relay)
    : QGraphicsView(parent), m_project(project), m_editor_relay(editor_relay) {

    // ***** Initialize rubber band object used as a selection box
    m_rubber_band = new ViewRubberBand(QRubberBand::Shape::Rectangle, this);

    // ***** Initialize variables
    updateGrid();
    my_scene = scene;
    setScene(scene);

    // ***** Connect Signals
    connect(my_scene, SIGNAL(aboutToClear()),   this, SLOT(sceneIsAboutToClear()));
}

NodeView::~NodeView() { }

void NodeView::focusInEvent(QFocusEvent *event) {
    m_editor_relay->setActiveWidget(Editor_Widgets::View);
    QGraphicsView::focusInEvent(event);
}

// SLOT:    Chance to reset local pointer variables when Scene is about to clear() (like when switching Editor_Mode)
//       ...After Scene is rebuilt local reference pointers will be dangling
void NodeView::sceneIsAboutToClear() {
    ///m_origin_item = nullptr;
}

















