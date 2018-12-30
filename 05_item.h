//
//
//
//
//

#ifndef DRITEM_H
#define DRITEM_H

#include <QGraphicsItem>

class DrItem : public QGraphicsItem
{
private:
    QVector<QPointF> stuff;

    int     m_x;
    int     m_y;
    QColor  m_color;


public:
    DrItem(const QColor &color, int x, int y);

    virtual QRectF       boundingRect() const override;
    virtual QPainterPath shape() const override;

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget) override;

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

};

#endif // DRITEM_H
