//
//      Created by Stephens Nunnally on 12/7/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrObject Class Definitions
//
//

#include <QtWidgets>

#include "library.h"

#include "project.h"
#include "project_asset.h"
#include "project_world.h"
#include "project_world_stage.h"
#include "project_world_stage_object.h"

#include "settings.h"
#include "settings_component.h"
#include "settings_component_property.h"

#include "editor_item.h"
#include "editor_scene.h"

//####################################################################################
//##        Constructor & destructor
//####################################################################################
DrItem::DrItem(DrProject *project, DrObject *object)
{
    // Store relevant project / object data for use later
    m_project    = project;
    m_object     = object;
    m_object_key = object->getKey();
    m_asset_key  = m_object->getAssetKey();
    m_asset      = project->getAsset(m_asset_key);

    // Load starting position
    QPointF start_pos = m_object->getComponentProperty(Object_Components::transform, Object_Properties::position)->getValue().toPointF();
    m_start_x = start_pos.x();
    m_start_y = start_pos.y();

    // Dimensions of associated asset, used for boundingRect
    m_asset_width =  m_asset->getWidth();
    m_asset_height = m_asset->getHeight();

    // Store some initial user data
    setData(User_Roles::Name, m_asset->getAssetName() );
    setData(User_Roles::Type, StringFromType( m_object->getType() ));

    double angle = m_object->getComponentProperty(Object_Components::transform, Object_Properties::rotation)->getValue().toDouble();
    QPointF scale = m_object->getComponentProperty(Object_Components::transform, Object_Properties::scale)->getValue().toPointF();
    updateProperty(User_Roles::Z_Order, m_object->getComponentProperty(Object_Components::layering, Object_Properties::z_order)->getValue());
    updateProperty(User_Roles::Rotation, QVariant::fromValue(angle));
    updateProperty(User_Roles::Scale, scale);

    // Set up initial item settings
    setAcceptHoverEvents(true);                                         // Item tracks mouse movement
    setShapeMode(QGraphicsPixmapItem::MaskShape);                       // Allows for selecting while ignoring transparent pixels

    if (!Dr::CheckDebugFlag(Debug_Flags::Turn_Off_Antialiasing))
        setTransformationMode(Qt::SmoothTransformation);                // Turn on anti aliasing

    setFlags(QGraphicsItem::GraphicsItemFlag::ItemIsSelectable |
             QGraphicsItem::GraphicsItemFlag::ItemIsMovable |
             QGraphicsItem::GraphicsItemFlag::ItemSendsScenePositionChanges |
             QGraphicsItem::GraphicsItemFlag::ItemSendsGeometryChanges);

    // Load image from asset
    setPixmap(m_asset->getComponentProperty(Asset_Components::animation, Asset_Properties::animation_default)->getValue().value<QPixmap>());

    // Adjust item to proper transform
    QPointF center = boundingRect().center();
    QTransform t = QTransform().translate(center.x(), center.y()).rotate(angle).scale(scale.x(), scale.y()).translate(-center.x(), -center.y());
    setTransform(t);
}                                     


//####################################################################################
//##        Custom Property Storing
//####################################################################################
void DrItem::updateProperty(int key, const QVariant &value)
{
    double x, y;

    switch (key)
    {
    case User_Roles::Position:
        setData(User_Roles::Position, value);
        m_object->setComponentPropertyValue(Object_Components::transform, Object_Properties::position, value);
        break;
    case User_Roles::Rotation:
        setData(User_Roles::Rotation, value);
        m_object->setComponentPropertyValue(Object_Components::transform, Object_Properties::rotation, value);
        break;
    case User_Roles::Scale:
        setData(User_Roles::Scale, value);
        m_object->setComponentPropertyValue(Object_Components::transform, Object_Properties::scale, value);
        x = m_asset_width *  value.toPointF().x();
        y = m_asset_height * value.toPointF().y();
        m_object->setComponentPropertyValue(Object_Components::transform, Object_Properties::size, QPointF(x, y));
        break;
    case User_Roles::Z_Order:
        setZValue(value.toInt());
        m_object->setComponentPropertyValue(Object_Components::layering, Object_Properties::z_order, value);
        break;
    }

    if (scene()) {
        InterfaceRelay *relay = dynamic_cast<DrScene*>(scene())->getRelay();

        switch (key)
        {
        case User_Roles::Position: relay->updateObjectInspectorAfterItemChange(m_object, Object_Properties::position);      break;
        case User_Roles::Rotation: relay->updateObjectInspectorAfterItemChange(m_object, Object_Properties::rotation);      break;
        case User_Roles::Scale:    relay->updateObjectInspectorAfterItemChange(m_object, Object_Properties::scale);         break;
        case User_Roles::Z_Order:  relay->updateObjectInspectorAfterItemChange(m_object, Object_Properties::z_order);       break;
        }

    }
}



//####################################################################################
//##        Item Property Overrides
//####################################################################################
// Outline of entire object
QRectF DrItem::boundingRect() const
{
    QRectF my_rect = QRectF(0, 0, m_asset_width, m_asset_height);
    return my_rect;
}

// Seems to define mouseOver events, and intersection events for Rubber Band Box
QPainterPath DrItem::shape() const
{
    ///QPainterPath path;
    ///path.addRect(0, 0, m_width, m_height);
    ///return path;
    return QGraphicsPixmapItem::shape();
}

// Enable the use of qgraphicsitem_cast with this item
int DrItem::type() const
{
    return User_Types::Object;
}



//####################################################################################
//##        Item Change Event - Allows for auto updating on property changes
//####################################################################################
QVariant DrItem::itemChange(GraphicsItemChange change, const QVariant &value)
{

    // Value is the new position
    if (change == ItemPositionHasChanged && scene()) {
        ///QPointF new_pos = value.toPointF();
        ///return new_pos;
    }

    // Value is the new transform
    if (change == ItemTransformChange)
    {
        /// Extract angle from transform and store in item
        ///auto transform = value.value<QTransform>();
        ///double angle = qRadiansToDegrees(qAtan2(transform.m12(), transform.m11() ));
        ///setData(User_Roles::Rotation, angle);
    }

    return QGraphicsPixmapItem::itemChange(change, value);
}


//####################################################################################
//##        Custom Paint Handling
//####################################################################################
void DrItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    ///if (option->state & QStyle::State_Selected)  { fillColor = QColor(Qt::red); } //m_color.dark(150); }              // If selected
    ///if (option->state & QStyle::State_MouseOver) { fillColor = QColor(Qt::gray); } //fillColor.light(125); }          // If mouse is over
    ///if (option->state & QStyle::State_MouseOver && option->state & QStyle::State_Selected) { fillColor = QColor(Qt::red).darker(200); }

    ///painter->fillRect(QRectF(0, 0, m_width, m_height), fillColor);
    ///painter->fillRect(QRectF(0, 0, m_width / 2, m_height / 2), fillColor.dark(250));

    ///painter->drawPixmap(0, 0, pixmap());

    // Check item is not super duper tiny, this seems to crash paint function
    QPolygonF poly = ( sceneTransform().map( boundingRect() ));
    QRectF bounds = poly.boundingRect();
    if (bounds.width() < .5 || bounds.height() < .5) return;

    // Set paint option to "not selected" or paint routine will draw dotted lines around item
    QStyleOptionGraphicsItem myOption(*option);
    myOption.state &= ~QStyle::State_Selected;

    QGraphicsPixmapItem::paint(painter, &myOption, widget);
}


QColor DrItem::getColorAtPoint(QPointF at_local_point)
{
    QImage image = this->pixmap().toImage();

    if (image.rect().contains(at_local_point.toPoint()))
        return image.pixelColor(at_local_point.toPoint());
    else
        return QColor(0, 0, 0, 0);
}

QColor DrItem::getColorAtPoint(QPointF at_view_point, QGraphicsView *mouse_over_view)
{
    QPointF in_scene = mouse_over_view->mapToScene(at_view_point.toPoint());
    QPointF in_object = this->mapFromScene(in_scene);
    return this->getColorAtPoint(in_object);
}


//####################################################################################
//##        Input overrides
//####################################################################################
void DrItem::mousePressEvent(QGraphicsSceneMouseEvent *event) { QGraphicsItem::mousePressEvent(event); }
void DrItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) { QGraphicsItem::mouseMoveEvent(event); }
void DrItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) { QGraphicsItem::mouseReleaseEvent(event); }
void DrItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) { QGraphicsItem::hoverLeaveEvent(event);}








