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
#include "project_world_scene.h"
#include "project_world_scene_object.h"

#include "settings.h"
#include "settings_component.h"
#include "settings_component_property.h"

#include "editor_scene_item.h"
#include "editor_scene_scene.h"

//####################################################################################
//##        Constructor & destructor
//####################################################################################
DrItem::DrItem(DrProject *project, DrScene *scene, long object_key, double z_order)
{
    DrObject *from_object = scene->getObject(object_key);
    DrAsset  *from_asset =  project->getAsset( from_object->getAssetKey() );

    QPointF start_pos = from_object->getComponentProperty(Object_Components::transform, Object_Properties::position)->getValue().toPointF();
    m_start_x = start_pos.x();
    m_start_y = start_pos.y();

    m_width =  from_asset->width();
    m_height = from_asset->height();
    setZValue(z_order);

    DrComponent *comp = from_asset->getComponent(Asset_Components::animation);
    DrProperty  *prop = comp->getProperty(Asset_Properties::animation_default);

    ///// Loads pixmap from byte array
    ///QByteArray byte_array = prop->getValue().toByteArray();
    ///QPixmap pix;
    ///pix.loadFromData(byte_array, "PNG");

    m_pixmap = prop->getValue().value<QPixmap>();

    setFlags(QGraphicsItem::GraphicsItemFlag::ItemIsSelectable |
             QGraphicsItem::GraphicsItemFlag::ItemIsMovable |
             QGraphicsItem::GraphicsItemFlag::ItemSendsScenePositionChanges |
             QGraphicsItem::GraphicsItemFlag::ItemSendsGeometryChanges);

    setAcceptHoverEvents(true);

    setData(User_Roles::Rotation, 0);
    setData(User_Roles::Scale, QPointF(1, 1));
    setData(User_Roles::Name, project->getAsset( scene->getObject(object_key)->getAssetKey() )->getAssetName() );

}                                     


//####################################################################################
//##        Item Property Overrides
//####################################################################################
// Outline of entire object
QRectF DrItem::boundingRect() const
{
    QRectF my_rect = QRectF(0, 0, m_width, m_height);
    return my_rect;
}

// Seems to define mouseOver events, and intersection events for Rubber Band Box
QPainterPath DrItem::shape() const
{
    QPainterPath path;
    path.addRect(0, 0, m_width, m_height);
    return path;
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

    return QGraphicsItem::itemChange(change, value);
}


//####################################################################################
//##        Custom Paint Handling
//####################################################################################
void DrItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);
    Q_UNUSED(option);

    ///if (option->state & QStyle::State_Selected)  { fillColor = QColor(Qt::red); } //m_color.dark(150); }              // If selected
    ///if (option->state & QStyle::State_MouseOver) { fillColor = QColor(Qt::gray); } //fillColor.light(125); }          // If mouse is over
    ///if (option->state & QStyle::State_MouseOver && option->state & QStyle::State_Selected) { fillColor = QColor(Qt::red).darker(200); }
    ///painter->fillRect(QRectF(0, 0, m_width, m_height), fillColor);
    ///painter->fillRect(QRectF(0, 0, m_width / 2, m_height / 2), fillColor.dark(250));

    painter->drawPixmap(0, 0, m_pixmap);
}


QColor DrItem::getColorAtPoint(QPointF at_point)
{
    QImage image = m_pixmap.toImage();

    if (image.rect().contains(at_point.toPoint()))
        return image.pixelColor(at_point.toPoint());
    else
        return QColor(0, 0, 0, 0);
}


//####################################################################################
//##        Input overrides
//####################################################################################
void DrItem::mousePressEvent(QGraphicsSceneMouseEvent *event) { QGraphicsItem::mousePressEvent(event); }
void DrItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) { QGraphicsItem::mouseMoveEvent(event); }
void DrItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) { QGraphicsItem::mouseReleaseEvent(event); }
void DrItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) { QGraphicsItem::hoverLeaveEvent(event);}








