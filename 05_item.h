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
    int x;
    int y;
    QColor color;
    QVector<QPointF> stuff;

public:
    DrItem(const QColor &color, int x, int y);

    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget) override;

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

};

#endif // DRITEM_H
