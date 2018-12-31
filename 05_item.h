//
//
//
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

    int     m_x;
    int     m_y;
    QColor  m_color;

    QPointF m_center;

    bool    m_is_resizing;

public:
    DrItem(const QColor &color, int x, int y);

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
