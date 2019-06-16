//
//      Created by Stephens Nunnally on 6/16/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QStyleOptionGraphicsItem>

#include "form_playground.h"
#include "globals.h"
#include "playground.h"
#include "playground_toy.h"


//####################################################################################
//##        Custom Item Painting
//####################################################################################
void DrPlaygroundLine::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen start_pen = this->pen();

    // If item is selected, draw thicker lines
    if (this->isSelected()) {
        // Set paint option to "not selected" or paint routine will draw dotted lines around item
        QStyleOptionGraphicsItem my_option(*option);
        my_option.state &= ~QStyle::State_Selected;

        QPen select_pen = start_pen;
        select_pen.setWidth( start_pen.width() + 2);
        this->setPen(select_pen);
        QGraphicsLineItem::paint(painter, &my_option, widget);
    } else {
        QGraphicsLineItem::paint(painter, option, widget);
    }

    this->setPen( start_pen );
}

void DrPlaygroundCircle::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen start_pen = this->pen();

    // If item is selected, draw thicker lines
    if (this->isSelected()) {
        // Set paint option to "not selected" or paint routine will draw dotted lines around item
        QStyleOptionGraphicsItem my_option(*option);
        my_option.state &= ~QStyle::State_Selected;

        QPen select_pen = start_pen;
        select_pen.setWidth( start_pen.width() + 2);
        this->setPen(select_pen);
        QGraphicsEllipseItem::paint(painter, &my_option, widget);
    } else {
        QGraphicsEllipseItem::paint(painter, option, widget);
    }

    // Draw orientation line
    painter->setPen( this->pen() );
    painter->drawLine( 0, 0, 0, -static_cast<int>(this->rect().height() / 2));
    this->setPen( start_pen );
}

void DrPlaygroundBox::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QPen start_pen = this->pen();

    // If item is selected, draw thicker lines
    if (this->isSelected()) {
        // Set paint option to "not selected" or paint routine will draw dotted lines around item
        QStyleOptionGraphicsItem my_option(*option);
        my_option.state &= ~QStyle::State_Selected;

        QPen select_pen = start_pen;
        select_pen.setWidth( start_pen.width() + 2);
        this->setPen(select_pen);
        QGraphicsRectItem::paint(painter, &my_option, widget);
    } else {
        QGraphicsRectItem::paint(painter, option, widget);
    }

    // Draw orientation line
    painter->setPen( this->pen() );
    painter->drawLine( 0, 0, 0, -static_cast<int>(this->rect().height() / 2));
    this->setPen( start_pen );
}


//####################################################################################
//##        Adds QGraphicsItems to Represent Playground Toys
//####################################################################################
QGraphicsLineItem* FormPlayground::addGraphicsLine(DrToy *toy, QColor color) {
    // Create new LineItem
    DrPlaygroundLine *line = new DrPlaygroundLine();
    line->setPen( QPen(color, 2));

    cpVect p1 = cpSegmentShapeGetA(toy->shape);
    cpVect p2 = cpSegmentShapeGetB(toy->shape);
    line->setLine(p1.x, -p1.y, p2.x, -p2.y);
    line->setFlag(QGraphicsItem::ItemIsSelectable, true);

    // Store reference to the associated DrToy in the Space
    line->setData(User_Roles::Toy, QVariant::fromValue<DrToy*>(toy));

    // Add to QGraphicsScene
    m_play_scene->addItem(line);
    return line;
}

QGraphicsEllipseItem* FormPlayground::addGraphicsCircle(DrToy *toy, QColor color) {
    // Create new LineItem
    DrPlaygroundCircle *circle = new DrPlaygroundCircle();
    circle->setPen( QPen(color, 2));

    double radius = cpCircleShapeGetRadius(toy->shape);
    circle->setPos(QPointF(toy->m_position.x(), -toy->m_position.y()));
    circle->setRect( QRectF(-radius, -radius, radius*2.0, radius*2.0) );
    circle->setFlag(QGraphicsItem::ItemIsSelectable, true);

    // Store reference to the associated DrToy in the Space
    circle->setData(User_Roles::Toy, QVariant::fromValue<DrToy*>(toy));

    // Add to QGraphicsScene
    m_play_scene->addItem(circle);
    return circle;
}


QGraphicsRectItem* FormPlayground::addGraphicsBox(DrToy *toy, QColor color) {
    // Create new LineItem
    DrPlaygroundBox *box = new DrPlaygroundBox();
    box->setPen( QPen(color, 2));

    box->setPos(QPointF(toy->m_position.x(), -toy->m_position.y()));
    box->setRect( QRectF(-(toy->m_width/2.0), -(toy->m_height/2.0), toy->m_width, toy->m_height) );
    box->setFlag(QGraphicsItem::ItemIsSelectable, true);

    // Store reference to the associated DrToy in the Space
    box->setData(User_Roles::Toy, QVariant::fromValue<DrToy*>(toy));

    // Add to QGraphicsScene
    m_play_scene->addItem(box);
    return box;
}


//####################################################################################
//##        Item Change Event - Allows for updating Space when item is moved
//####################################################################################
static QVariant UpdateToyPosition(QGraphicsItem *item, const QVariant &value) {
    // Value is new scene position
    QPointF new_pos =    value.toPointF();
    DrToy *toy = item->data(User_Roles::Toy).value<DrToy*>();
    cpBodySetPosition( toy->body, cpv(new_pos.x(), -new_pos.y()) );

    if (toy->body_type == Body_Type::Static) {
        cpSpace *space = cpBodyGetSpace( toy->body );
        cpSpaceReindexShapesForBody(space, toy->body);
        toy->m_playground->wakeAllBodies();                         // If we moved a static object, make sure any dynamic objects touching it are awake
    }
    return new_pos;
}

QVariant DrPlaygroundLine::itemChange(GraphicsItemChange change, const QVariant &value) {
    if (change == ItemScenePositionHasChanged) {
        return UpdateToyPosition(this, value);
    }
    return QGraphicsLineItem::itemChange(change, value);
}

QVariant DrPlaygroundCircle::itemChange(GraphicsItemChange change, const QVariant &value) {
    if (change == ItemScenePositionHasChanged) {
        return UpdateToyPosition(this, value);
    }
    return QGraphicsEllipseItem::itemChange(change, value);
}

QVariant DrPlaygroundBox::itemChange(GraphicsItemChange change, const QVariant &value) {
    if (change == ItemScenePositionHasChanged) {
       return UpdateToyPosition(this, value);
    }
    return QGraphicsRectItem::itemChange(change, value);
}


//####################################################################################
//##        Gives Line Item a wider Shape for easier selecting
//####################################################################################
QPainterPath DrPlaygroundLine::shape() const {
    double radians = line().angle() * M_PI / 180;
    double dx = 20 * sin(radians);
    double dy = 20 * cos(radians);
    QPointF offset1 = QPointF( dx,  dy);
    QPointF offset2 = QPointF(-dx, -dy);
    QPolygonF polygon;
    polygon << line().p1() + offset1
            << line().p1() + offset2
            << line().p2() + offset2
            << line().p2() + offset1;
    QPainterPath ret;
    ret.addPolygon(polygon);
    return ret;
}
















