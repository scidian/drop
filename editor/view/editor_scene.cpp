//
//      Created by Stephens Nunnally on 1/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Graphics Scene Definitions
//
//
#include <QKeyEvent>
#include <QUndoView>

#include "editor/debug.h"
#include "editor/interface_editor_relay.h"
#include "editor/preferences.h"
#include "editor/view/editor_item.h"
#include "editor/view/editor_scene.h"
#include "project/dr_project.h"
#include "project/entities/dr_asset.h"
#include "project/entities/dr_font.h"
#include "project/entities/dr_world.h"
#include "project/entities/dr_stage.h"
#include "project/entities/dr_thing.h"
#include "project/settings/settings.h"
#include "project/settings/settings_component.h"
#include "project/settings/settings_component_property.h"


//####################################################################################
//##    Constructor & destructor
//####################################################################################
DrScene::DrScene(QWidget *parent, DrProject *project, IEditorRelay *editor_relay) :
                 QGraphicsScene(parent),
                 m_project(project),
                 m_editor_relay(editor_relay) {
    connect(this, SIGNAL(changed(QList<QRectF>)),   this, SLOT(sceneChanged(QList<QRectF>)));
    connect(this, SIGNAL(sceneRectChanged(QRectF)), this, SLOT(sceneRectChanged(QRectF)));
    connect(this, SIGNAL(selectionChanged()),       this, SLOT(selectionChanged()));

    ///// May speed up large scenes?
    ///setBspTreeDepth(0);
    ///setItemIndexMethod(QGraphicsScene::ItemIndexMethod::NoIndex);

    m_undo = new QUndoStack(this);

    // !!!!! #DEBUG:    Opens a window showing all undo commands
    if (Dr::CheckDebugFlag(Debug_Flags::Show_Undo_View)) {
        QUndoView *undo_view = new QUndoView(m_undo);
        undo_view->setWindowTitle(tr("Command List"));
        undo_view->show();
        undo_view->setAttribute(Qt::WidgetAttribute::WA_QuitOnClose, false);
        undo_view->move(0, 150);
    }
    // !!!!! END
}

DrScene::~DrScene() { }

void DrScene::clearStageShown() {
    setCurrentStageKeyShown(c_no_key);
    setCurrentStageShown(nullptr);
    clear();
}


//####################################################################################
//##    SLOT's: sceneRectChanged, sceneChanged
//####################################################################################
// Connected from SIGNAL: QGraphicsScene::sceneRectChanged
void DrScene::sceneRectChanged(QRectF new_rect) {
    Q_UNUSED (new_rect)
    //Dr::SetLabelText(Label_Names::Label_1,
    //                 "SRect X: " + QString::number(round(new_rect.x())) + ", Y: " + QString::number(round(new_rect.y())) +
    //                     ", W: " + QString::number(round(new_rect.width())) + ", H: " + QString::number(round(new_rect.height())));
}

// Connected from SIGNAL: QGraphicsScene::changed
void DrScene::sceneChanged(QList<QRectF>) {
    QRectF my_rect = sceneRect();
    QRectF items_rect = itemsBoundingRect();
    double buffer = 300;

    double left_diff =   (items_rect.left())   - (my_rect.left() + buffer);
    double top_diff =    (items_rect.top())    - (my_rect.top() + buffer);
    double right_diff =  (items_rect.right())  - (my_rect.right() - buffer);
    double bottom_diff = (items_rect.bottom()) - (my_rect.bottom() - buffer);

    if (left_diff   > 0)   left_diff = 0;
    if (top_diff    > 0)    top_diff = 0;
    if (right_diff  < 0)  right_diff = 0;
    if (bottom_diff < 0) bottom_diff = 0;

    my_rect.adjust(left_diff, top_diff, right_diff, bottom_diff);
    if (my_rect != sceneRect()) setSceneRect(my_rect);
}


//####################################################################################
//##    setPositionByOrigin - Sets item to new_x, new_y position in scene, offset by_origin point
//####################################################################################
void DrScene::setPositionByOrigin(QGraphicsItem *item, Position_Flags by_origin, double new_x, double new_y) {
    item->setPos(new_x, new_y);

    QRectF      item_rect = item->boundingRect();
    QPointF     item_pos;

    switch (by_origin) {
        case Position_Flags::Top_Left:      item_pos = item_rect.topLeft();                  break;
        case Position_Flags::Top_Right:     item_pos = item_rect.topRight();                 break;
        case Position_Flags::Center:        item_pos = item_rect.center();                   break;
        case Position_Flags::Bottom_Left:   item_pos = item_rect.bottomLeft();               break;
        case Position_Flags::Bottom_Right:  item_pos = item_rect.bottomRight();              break;
        case Position_Flags::Top:           item_pos = QPointF( QLineF(item_rect.topLeft(),    item_rect.topRight()).pointAt(.5) );    break;
        case Position_Flags::Bottom:        item_pos = QPointF( QLineF(item_rect.bottomLeft(), item_rect.bottomRight()).pointAt(.5) ); break;
        case Position_Flags::Left:          item_pos = QPointF( QLineF(item_rect.topLeft(),    item_rect.bottomLeft()).pointAt(.5) );  break;
        case Position_Flags::Right:         item_pos = QPointF( QLineF(item_rect.topRight(),   item_rect.bottomRight()).pointAt(.5) ); break;

        // We don't want to process these options, use top left as position
        case Position_Flags::Move_Item:
        case Position_Flags::Rotate:
        case Position_Flags::No_Position:
            item_pos = item_rect.topLeft();
    }
    item_pos = item->sceneTransform().map(item_pos);

    setPositionByOrigin(item, item_pos, new_x, new_y);
}

void DrScene::setPositionByOrigin(QGraphicsItem *item, QPointF origin_point, double new_x, double new_y) {
    QPointF     new_pos;

    double x_diff = origin_point.x() - new_x;
    double y_diff = origin_point.y() - new_y;

    new_pos.setX( new_x - x_diff);
    new_pos.setY( new_y - y_diff);

    item->setPos(new_pos);
}


















