//
//      Created by Stephens Nunnally on 12/7/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrObject Class Definitions
//
//

#include <QtWidgets>

#include "editor_scene_item.h"


//####################################################################################
//##        Constructor & destructor
//####################################################################################
DrItem::DrItem(const QColor &start_color, double width, double height, double z_order)
{
    m_width = width;
    m_height = height;
    m_color = start_color;
    setZValue(z_order);

    setFlags(QGraphicsItem::GraphicsItemFlag::ItemIsSelectable |
             QGraphicsItem::GraphicsItemFlag::ItemIsMovable |
             QGraphicsItem::GraphicsItemFlag::ItemSendsScenePositionChanges |
             QGraphicsItem::GraphicsItemFlag::ItemSendsGeometryChanges);

    setAcceptHoverEvents(true);

    setData(User_Roles::Rotation, 0);
    setData(User_Roles::Scale, QPointF(1, 1));
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

    QColor fillColor = m_color;

    ///if (option->state & QStyle::State_Selected)  { fillColor = QColor(Qt::red); } //m_color.dark(150); }              // If selected
    ///if (option->state & QStyle::State_MouseOver) { fillColor = QColor(Qt::gray); } //fillColor.light(125); }          // If mouse is over
    ///if (option->state & QStyle::State_MouseOver && option->state & QStyle::State_Selected) { fillColor = QColor(Qt::red).darker(200); }

    painter->fillRect(QRectF(0, 0, m_width, m_height), fillColor);
}


//####################################################################################
//##        Input overrides
//####################################################################################
void DrItem::mousePressEvent(QGraphicsSceneMouseEvent *event) { QGraphicsItem::mousePressEvent(event); }
void DrItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) { QGraphicsItem::mouseMoveEvent(event); }
void DrItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) { QGraphicsItem::mouseReleaseEvent(event); }
void DrItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) { QGraphicsItem::hoverLeaveEvent(event);}








