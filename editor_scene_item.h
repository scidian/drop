//
//      Created by Stephens Nunnally on 12/26/18, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//      DrItem - Class to hold one Graphics Item object within a QGraphicsScene
//
//

#ifndef DRITEM_H
#define DRITEM_H

#include <QGraphicsItem>

#include "settings.h"

#include "form_main.h"

enum class Origin {
    Top_Left,       Top,        Top_Right,
    Left,           Center,     Right,
    Bottom_Left,    Bottom,     Bottom_Right,
};

class DrItem : public QGraphicsItem
{
private:
    double      m_width;
    double      m_height;
    QColor      m_color;

    Origin      m_origin = Origin::Center;

public:
    DrItem(const QColor &start_color, double width, double height, double z_order, QString name);

    // Base Getter Overrides
    virtual QRectF          boundingRect() const override;
    virtual QPainterPath    shape() const override;
    virtual int             type() const override;

    // Event Overrides
    virtual QVariant        itemChange(GraphicsItemChange change, const QVariant &value) override;
    virtual void            paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget) override;
    virtual void            mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void            mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void            mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void            hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

    // Getters and Setters
    Origin                  getOrigin() { return m_origin; }


};

#endif // DRITEM_H








