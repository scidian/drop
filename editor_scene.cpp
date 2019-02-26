//
//      Created by Stephens Nunnally on 1/3/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      Graphics Scene Definitions
//
//
#include <QKeyEvent>
#include <QRandomGenerator>
#include <QUndoView>

#include "debug.h"

#include "editor_item.h"
#include "editor_scene.h"

#include "interface_relay.h"
#include "library.h"

#include "project.h"
#include "project_world.h"
#include "project_world_stage.h"
#include "project_world_stage_object.h"
#include "settings.h"
#include "settings_component.h"
#include "settings_component_property.h"


//####################################################################################
//##        Constructor & destructor
//####################################################################################
DrScene::DrScene(QWidget *parent, DrProject *project, InterfaceRelay *relay) :
                 QGraphicsScene(parent = nullptr),
                 m_project(project),
                 m_relay(relay)
{
    connect(this, SIGNAL(changed(QList<QRectF>)), this, SLOT(sceneChanged(QList<QRectF>)));
    connect(this, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));

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

DrScene::~DrScene()
{

}



//####################################################################################
//##        SLOT: sceneChanged
//####################################################################################
// Connected from scene().changed
void DrScene::sceneChanged(QList<QRectF>)
{


}



//####################################################################################
//##        Key Events
//####################################################################################
void DrScene::keyReleaseEvent(QKeyEvent *event) { QGraphicsScene::keyReleaseEvent(event); }

void DrScene::keyPressEvent(QKeyEvent *event)
{
    // Amount to move items when arrow keys are pressed
    qreal move_by = 5;

    if (getSelectionCount() < 1) {
        QGraphicsScene::keyPressEvent(event);
        return;
    }

    // Find total bounding box of all selected items
    QRectF source_rect = totalSelectionSceneRect();

    // Process movement key press
    for (auto item : getSelectionItems()) {
        switch (event->key())
        {
        case Qt::Key::Key_Up:     item->moveBy( 0, -move_by);      break;
        case Qt::Key::Key_Down:   item->moveBy( 0,  move_by);      break;
        case Qt::Key::Key_Left:   item->moveBy(-move_by,  0);      break;
        case Qt::Key::Key_Right:  item->moveBy( move_by,  0);      break;
        case Qt::Key::Key_Comma:  item->setZValue(selectedItems().first()->zValue() - 1);    break;
        case Qt::Key::Key_Period: item->setZValue(selectedItems().first()->zValue() + 1);    break;
        }
    }
    updateSelectionBox();

    // Perform key press event on all items in selection group
    if (scene_mutex.tryLock(10) == false) return;

    QList<QGraphicsItem*>  list_new_items {};

    for (auto item : getSelectionItems()) {
    //!!    DrItem *new_item;
        QColor new_color;
        qreal new_x, new_y;

        switch (event->key())
        {
        // Clone selected items
        case Qt::Key::Key_W:
        case Qt::Key::Key_A:
        case Qt::Key::Key_S:
        case Qt::Key::Key_D:
            // Send item back to scene before we copy it
            //my_scene->getSelectionGroup()->removeFromGroup(item);

            new_color = QColor::fromRgb(QRandomGenerator::global()->generate()).light(100);
            new_x = item->scenePos().x();
            new_y = item->scenePos().y();

            if (event->key() == Qt::Key::Key_W) new_y = new_y - source_rect.height();
            if (event->key() == Qt::Key::Key_A) new_x = new_x - source_rect.width();
            if (event->key() == Qt::Key::Key_S) new_y = new_y + source_rect.height();
            if (event->key() == Qt::Key::Key_D) new_x = new_x + source_rect.width();

    //!!        new_item = new DrItem(new_color, item->boundingRect().width(), item->boundingRect().height(),
    //!!                              this->items().count() + 1, item->data(User_Roles::Name).toString());
    //!!         new_item->setPos(new_x, new_y);
    //!!        new_item->setTransform(item->transform());          // Includes rotation and scaling

    //!!        new_item->setData(User_Roles::Scale, item->data(User_Roles::Scale).toPointF());
    //!!        new_item->setData(User_Roles::Rotation, item->data(User_Roles::Rotation).toDouble());

    //!!        addItem(new_item);
    //!!        list_new_items.append(new_item);
            break;

        // Delete selected items
        case Qt::Key::Key_Delete:
        case Qt::Key::Key_Backspace:
            //emptySelectionGroup(true);
            break;
        }
    }

    // If we added (copied) new items to scene, select those items
//    if (list_new_items.count() > 0) {
//        emptySelectionGroup();
//        for (auto item : list_new_items) addItemToSelectionGroup(item);
//    }

    scene_mutex.unlock();

    //QGraphicsScene::keyPressEvent(event);         // Don't pass on, if we pass on arrow key presses, it moves view sliders
    emit updateViews();                             // Custom signal to tell Views we're attached to, to update themselves
}



//####################################################################################
//##        setPositionByOrigin - Sets item to new_x, new_y position in scene, offset by_origin point
//####################################################################################
void DrScene::setPositionByOrigin(QGraphicsItem *item, Position_Flags by_origin, double new_x, double new_y)
{
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
    default:
        item_pos = item_rect.topLeft();
    }
    item_pos = item->sceneTransform().map(item_pos);

    setPositionByOrigin(item, item_pos, new_x, new_y);
}

void DrScene::setPositionByOrigin(QGraphicsItem *item, QPointF origin_point, double new_x, double new_y)
{
    QPointF     new_pos;

    double x_diff = origin_point.x() - new_x;
    double y_diff = origin_point.y() - new_y;

    new_pos.setX( new_x - x_diff);
    new_pos.setY( new_y - y_diff);

    item->setPos(new_pos);
}




//####################################################################################
//##        Called after some change has been made to items on another widget besides
//##        the QGraphicsView, updates the items in the scene
//####################################################################################
void DrScene::updateChangesInScene(QList<DrSettings*> changed_items, QList<long> property_keys)
{
    for (auto settings_item : changed_items) {
        DrType my_type = settings_item->getType();

        if (my_type == DrType::Object) {
            updateItemInScene(settings_item, property_keys);

        } else if (my_type == DrType::Stage || my_type == DrType::StartStage) {
            emit updateAlignmentGrid();
        }

        //################ !!!!!!!!!!!!!!!!!!!!!!!
        //
        //      ROOM FOR MORE TYPES
        //
        //################ !!!!!!!!!!!!!!!!!!!!!!!
    }

    // Update selection box in case item position changed
    updateSelectionBox();
}


// Updates the item in the scene based on the new property_keys
void DrScene::updateItemInScene(DrSettings* changed_item, QList<long> property_keys)
{
    DrObject *object = dynamic_cast<DrObject*>(changed_item);
    DrItem   *item =   object->getDrItem();

    // Some local item variables, prepping for update
    QTransform transform;
    QPointF position = object->getComponentPropertyValue(Components::Object_Transform, Properties::Object_Position).toPointF();
    QPointF scale    = object->getComponentPropertyValue(Components::Object_Transform, Properties::Object_Scale).toPointF();
    QPointF size     = object->getComponentPropertyValue(Components::Object_Transform, Properties::Object_Size).toPointF();
    double  angle =    object->getComponentPropertyValue(Components::Object_Transform, Properties::Object_Rotation).toDouble();

    // Turn off itemChange() signals to stop recursive calling
    item->disableItemChangeFlags();

    // Go through each property that we have been notified has changed and update as appropriately
    for (auto one_property : property_keys) {
        Properties property = static_cast<Properties>(one_property);

        QVariant new_value = object->findPropertyFromPropertyKey(one_property)->getValue();

        switch (property)
        {

        case Properties::Object_Position:
            setPositionByOrigin(item, Position_Flags::Center, position.x(), position.y());
            break;

        case Properties::Object_Size:
        case Properties::Object_Scale:
        case Properties::Object_Rotation:
            if (property == Properties::Object_Size) {
                    scale.setX( size.x() / item->getAssetWidth()  );
                    scale.setY( size.y() / item->getAssetHeight() );
            } else {
                    size.setX(  scale.x() * item->getAssetWidth() );
                    size.setY(  scale.y() * item->getAssetHeight() );
            }
            item->setData(User_Roles::Scale, scale );
            item->setData(User_Roles::Rotation, angle );
            transform = QTransform().rotate(angle).scale(scale.x(), scale.y());
            item->setTransform(transform);
            setPositionByOrigin(item, Position_Flags::Center, position.x(), position.y());

            if (property == Properties::Object_Size) {
                object->setComponentPropertyValue(Components::Object_Transform, Properties::Object_Scale, scale);
                m_relay->updateEditorWidgetsAfterItemChange(Editor_Widgets::Scene_View, { object } , { Properties::Object_Scale });
            }
            if (property == Properties::Object_Scale) {
                object->setComponentPropertyValue(Components::Object_Transform, Properties::Object_Size, size);
                m_relay->updateEditorWidgetsAfterItemChange(Editor_Widgets::Scene_View, { object } , { Properties::Object_Size });
            }

            break;

        case Properties::Object_Z_Order:
            item->setZValue(new_value.toDouble());
            break;
        default: ;
        }
    }

    // Turn back on itemChange() signals
    item->enableItemChangeFlags();
}























