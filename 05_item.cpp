//
//      Created by Stephens Nunnally on 12/7/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrObject Class Definitions
//
//

#include <QtWidgets>

#include "05_item.h"

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


QVariant DrItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    // Value is the new position
    if (change == ItemPositionHasChanged && scene()) {
        QPointF new_pos = value.toPointF();
        setData(User_Roles::Position, new_pos);
    }
    return QGraphicsItem::itemChange(change, value);
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



// Sets item to new_x, new_y position in scene, offset by_origin
void DrItem::setPositionByOrigin(Origin by_origin, double new_x, double new_y)
{
    setPos(new_x, new_y);

    QRectF      item_rect = boundingRect();
    QPointF     item_pos;

    switch (by_origin) {
    case Origin::Top_Left:      item_pos = item_rect.topLeft();                  break;
    case Origin::Top_Right:     item_pos = item_rect.topRight();                 break;
    case Origin::Center:        item_pos = item_rect.center();                   break;
    case Origin::Bottom_Left:   item_pos = item_rect.bottomLeft();               break;
    case Origin::Bottom_Right:  item_pos = item_rect.bottomRight();              break;
    case Origin::Top:           item_pos = QPointF(item_rect.topLeft().x() + (item_rect.topRight().x() - item_rect.topLeft().x()),
                                                   item_rect.topLeft().y() );    break;
    case Origin::Bottom:        item_pos = QPointF(item_rect.bottomLeft().x() + (item_rect.bottomRight().x() - item_rect.bottomLeft().x()),
                                                   item_rect.bottomLeft().y() ); break;
    case Origin::Left:          item_pos = QPointF(item_rect.topLeft().x(),
                                                   item_rect.topLeft().y() + (item_rect.bottomLeft().y() - item_rect.topLeft().y()) );       break;
    case Origin::Right:         item_pos = QPointF(item_rect.topRight().x(),
                                                   item_rect.topRight().y() + (item_rect.bottomRight().y() - item_rect.topRight().y()) );    break;
    }
    item_pos = sceneTransform().map(item_pos);

    setPositionByOrigin(item_pos, new_x, new_y);
}

void DrItem::setPositionByOrigin(QPointF origin_point, double new_x, double new_y)
{
    QPointF     new_pos;

    double x_diff = origin_point.x() - new_x;
    double y_diff = origin_point.y() - new_y;

    new_pos.setX( new_x - x_diff);
    new_pos.setY( new_y - y_diff);

    setPos(new_pos);
}


void DrItem::mousePressEvent(QGraphicsSceneMouseEvent *event) { QGraphicsItem::mousePressEvent(event); }
void DrItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) { QGraphicsItem::mouseMoveEvent(event); }
void DrItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) { QGraphicsItem::mouseReleaseEvent(event); }
void DrItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) { QGraphicsItem::hoverLeaveEvent(event);}








