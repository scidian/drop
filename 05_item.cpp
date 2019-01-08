//
//      Created by Stephens Nunnally on 12/7/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrObject Class Definitions
//
//

#include <QtWidgets>

#include "05_item.h"

DrItem::DrItem(const QColor &start_color, double width, double height)
{
    m_width = width;
    m_height = height;
    m_color = start_color;
    setZValue(1);

    setFlags(QGraphicsItem::GraphicsItemFlag::ItemIsSelectable |
             QGraphicsItem::GraphicsItemFlag::ItemIsMovable |
             QGraphicsItem::GraphicsItemFlag::ItemSendsScenePositionChanges);
    setAcceptHoverEvents(true);
}                                     

// Outline of entire object
QRectF DrItem::boundingRect() const
{
    QRectF my_rect = QRectF(0, 0, m_width, m_height);
    return my_rect;
}

// Seems to define mouseOver events
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



void DrItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);
    Q_UNUSED(option);

    QColor fillColor = m_color;

    //if (option->state & QStyle::State_Selected)  { fillColor = QColor(Qt::red); } //m_color.dark(150); }              // If selected
    //if (option->state & QStyle::State_MouseOver) { fillColor = QColor(Qt::gray); } //fillColor.light(125); }          // If mouse is over
    //if (option->state & QStyle::State_MouseOver && option->state & QStyle::State_Selected) { fillColor = QColor(Qt::red).darker(200); }

    painter->fillRect(QRectF(0, 0, m_width, m_height), fillColor);
}



void DrItem::mousePressEvent(QGraphicsSceneMouseEvent *event) { QGraphicsItem::mousePressEvent(event); }
void DrItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) { QGraphicsItem::mouseMoveEvent(event); }
void DrItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) { QGraphicsItem::mouseReleaseEvent(event); }
void DrItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) { QGraphicsItem::hoverLeaveEvent(event);}








