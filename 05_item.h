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

#include "30_settings.h"

class DrItem : public QGraphicsItem
{
private:
    QVector<QPointF> stuff;

    QColor  m_color;
    QPointF m_center;

    bool    m_is_resizing;

public:
    DrItem(const QColor &start_color);

    virtual QRectF       boundingRect() const override;
    virtual QPainterPath shape() const override;
    virtual int          type() const override;

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget) override;

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

};

#endif // DRITEM_H
