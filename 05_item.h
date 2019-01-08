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

#include "form_main.h"

class DrItem : public QGraphicsItem
{
private:
    double      m_width;
    double      m_height;
    QColor      m_color;

public:
    DrItem(const QColor &start_color, double width, double height);

    virtual QRectF       boundingRect() const override;
    virtual QPainterPath shape() const override;
    virtual int          type() const override;

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget) override;

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

};

#endif // DRITEM_H
