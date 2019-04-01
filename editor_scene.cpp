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
#include "globals.h"
#include "interface_editor_relay.h"
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
DrScene::DrScene(QWidget *parent, DrProject *project, IEditorRelay *editor_relay) :
                 QGraphicsScene(parent),
                 m_project(project),
                 m_editor_relay(editor_relay)
{
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

DrScene::~DrScene() {}



//####################################################################################
//##        SLOT's: sceneRectChanged, sceneChanged
//####################################################################################
// Connected from SIGNAL: QGraphicsScene::sceneRectChanged
void DrScene::sceneRectChanged(QRectF new_rect)
{
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
            emit updateGrid();
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

    // ***** Some local item variables, prepping for update
    QTransform transform;
    QPointF position  = object->getComponentPropertyValue(Components::Object_Transform, Properties::Object_Position).toPointF();
    QPointF scale     = object->getComponentPropertyValue(Components::Object_Transform, Properties::Object_Scale).toPointF();
    QPointF size      = object->getComponentPropertyValue(Components::Object_Transform, Properties::Object_Size).toPointF();
    double  angle     = object->getComponentPropertyValue(Components::Object_Transform, Properties::Object_Rotation).toDouble();
    double  pre_angle = item->data(User_Roles::Rotation).toDouble();
    double  transform_scale_x, transform_scale_y;


    // ***** Turn off itemChange() signals to stop recursive calling
    item->disableItemChangeFlags();

    // ***** Go through each property that we have been notified has changed and update as appropriately
    for (auto one_property : property_keys) {
        Properties property = static_cast<Properties>(one_property);

        QVariant new_value  = object->findPropertyFromPropertyKey(one_property)->getValue();

        switch (property) {

        case Properties::Object_Position:
            setPositionByOrigin(item, Position_Flags::Center, position.x(), position.y());
            break;

        case Properties::Object_Size:
        case Properties::Object_Scale:
        case Properties::Object_Rotation:

            // If property that changed was size, calculate the proper scale based on size
            if (property == Properties::Object_Size) {
                    scale.setX( size.x() / item->getAssetWidth()  );
                    scale.setY( size.y() / item->getAssetHeight() );
            // Otherwise calculate the size based on the scale
            } else {
                    size.setX(  scale.x() * item->getAssetWidth() );
                    size.setY(  scale.y() * item->getAssetHeight() );
            }

            // Store the item transform data, one of which will have been new. Then recalculate the transform and move the object

            item->setData(User_Roles::Scale, scale );
            item->setData(User_Roles::Rotation, angle );
            transform_scale_x = Dr::CheckScaleNotZero(scale.x());
            transform_scale_y = Dr::CheckScaleNotZero(scale.y());
            transform = QTransform().rotate(angle).scale(transform_scale_x, transform_scale_y);
            item->setTransform(transform);
            setPositionByOrigin(item, Position_Flags::Center, position.x(), position.y());

            // If size or scale was changed, update the other and update the widgets in the object inspector
            if (property == Properties::Object_Size) {
                object->setComponentPropertyValue(Components::Object_Transform, Properties::Object_Scale, scale);
                m_editor_relay->updateEditorWidgetsAfterItemChange(Editor_Widgets::Scene_View, { object } , { Properties::Object_Scale });
            }
            if (property == Properties::Object_Scale) {
                object->setComponentPropertyValue(Components::Object_Transform, Properties::Object_Size, size);
                m_editor_relay->updateEditorWidgetsAfterItemChange(Editor_Widgets::Scene_View, { object } , { Properties::Object_Size });
            }

            break;

        case Properties::Object_Z_Order:
            item->setZValue(new_value.toDouble());
            break;


        case Properties::Object_Filter_Brightness:
        case Properties::Object_Filter_Contrast:
        case Properties::Object_Filter_Saturation:
        case Properties::Object_Filter_Hue:
        case Properties::Object_Filter_Grayscale:
        case Properties::Object_Filter_Negative:
            item->applyFilters();
            break;


        default: ;
        }
    }


    // ***** If angle has changed, check if we should update selection box angle as well
    if (Dr::IsCloseTo(pre_angle, angle, .001) == false) {
        if (checkAllSelectedItemsHaveSameAngle()) {
            setSelectionAngle( angle );
        }
    }


    // ***** Turn back on itemChange() signals
    item->enableItemChangeFlags();
}























