//
//      Created by Stephens Nunnally on 6/16/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <QDebug>
#include <QGraphicsWidget>

#include "form_playground.h"
#include "globals.h"
#include "helper.h"
#include "playground.h"
#include "playground_toy.h"

// Local constants
constexpr int c_pen_size = 2;
constexpr int c_pen_selected_size = 4;


//####################################################################################
//##    Custom Item Painting
//####################################################################################
void DrPlaygroundLine::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    int pen_size = (this->data(User_Roles::Selected).toBool()) ? c_pen_selected_size : c_pen_size;
    setPen( QPen(pen().brush(), pen_size) );
    QGraphicsLineItem::paint(painter, option, widget);
}

void DrPlaygroundCircle::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    int pen_size = (this->data(User_Roles::Selected).toBool()) ? c_pen_selected_size : c_pen_size;
    setPen( QPen( QBrush(data(User_Roles::Color).value<QColor>()), pen_size) );

    DrToy *toy = this->data(User_Roles::Toy).value<DrToy*>();
    if (toy && toy->body_type == Body_Type::Dynamic) {
        bool sleeping = cpBodyIsSleeping( toy->body );
        if (sleeping) setPen( QPen(Qt::yellow, pen_size) );
    }

    QGraphicsEllipseItem::paint(painter, option, widget);                               // Draw circle
    painter->drawLine( 0, 0, 0, -static_cast<int>(this->rect().height() / 2));          // Draw orientation line
}

void DrPlaygroundBox::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    int pen_size = (this->data(User_Roles::Selected).toBool()) ? c_pen_selected_size : c_pen_size;
    setPen( QPen( QBrush(data(User_Roles::Color).value<QColor>()), pen_size) );

    DrToy *toy = this->data(User_Roles::Toy).value<DrToy*>();
    if (toy && toy->body_type == Body_Type::Dynamic) {
        bool sleeping = cpBodyIsSleeping( toy->body );
        if (sleeping) setPen( QPen(Qt::yellow, pen_size) );
    }

    QGraphicsRectItem::paint(painter, option, widget);                                  // Draw box
    painter->drawLine( 0, 0, 0, -static_cast<int>(this->rect().height() / 2));          // Draw orientation line
}


//####################################################################################
//##    Adds QGraphicsItems to Represent Playground Toys
//####################################################################################
QGraphicsLineItem* FormPlayground::addGraphicsLine(DrToy *toy, QColor color) {
    DrPlaygroundLine *line = new DrPlaygroundLine();
    line->setPen( QPen(color, c_pen_size));
    cpVect p1 = cpSegmentShapeGetA(toy->shape);
    cpVect p2 = cpSegmentShapeGetB(toy->shape);
    line->setLine(p1.x, -p1.y, p2.x, -p2.y);
    addItemToScene(toy, line, color);
    return line;
}

QGraphicsEllipseItem* FormPlayground::addGraphicsCircle(DrToy *toy, QColor color) {
    DrPlaygroundCircle *circle = new DrPlaygroundCircle();
    circle->setPen( QPen(color, c_pen_size));
    double radius = cpCircleShapeGetRadius(toy->shape);
    circle->setPos(QPointF(toy->m_position.x(), -toy->m_position.y()));
    circle->setRect( QRectF(-radius, -radius, radius*2.0, radius*2.0) );
    addItemToScene(toy, circle, color);
    return circle;
}


QGraphicsRectItem* FormPlayground::addGraphicsBox(DrToy *toy, QColor color) {
    DrPlaygroundBox *box = new DrPlaygroundBox();
    box->setPen( QPen(color, c_pen_size));
    box->setPos(QPointF(toy->m_position.x(), -toy->m_position.y()));
    box->setRect( QRectF(-(toy->m_width/2.0), -(toy->m_height/2.0), toy->m_width, toy->m_height) );
    addItemToScene(toy, box, color);
    return box;
}

void FormPlayground::addItemToScene(DrToy *toy, QGraphicsItem *item, QColor color) {
    item->setData(User_Roles::Toy, QVariant::fromValue<DrToy*>(toy));               // Store reference to the associated DrToy in the Space
    item->setData(User_Roles::Color, color);                                        // Stores color of object
    item->setData(User_Roles::Selected, false);                                     // Used to keep track of which Item is selected in Scene
    m_play_scene->addItem(item);                                                    // Add to QGraphicsScene
}


//####################################################################################
//##    Item Change Event - Allows for updating Space when item is moved
//####################################################################################
static QVariant UpdateToyPosition(QGraphicsItem *item, const QVariant &value) {
    // Value is new scene position
    QPointF new_pos = value.toPointF();

    // Attempt to grab DrToy pointer from userData of QGraphicsItem
    DrToy *toy = item->data(User_Roles::Toy).value<DrToy*>();
    if (!toy) return new_pos;

    cpSpace *space = cpBodyGetSpace( toy->body );
    cpBodySetPosition( toy->body, cpv(new_pos.x(), -new_pos.y()) );

    // If we moved a static object, make sure any dynamic objects touching it are awake
    if (toy->body_type == Body_Type::Static) {
        cpSpaceReindexShapesForBody(space, toy->body);
        toy->m_playground->wakeAllBodies();
    }

    // Step space to process any item position changes in case Space may not be updating
    cpSpaceStep(space, 0);
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
//##    Gives Line Item a wider Shape for easier selecting
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
















