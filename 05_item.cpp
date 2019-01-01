//
//
//
//
//


#include <QtWidgets>

#include "05_item.h"

DrItem::DrItem(const QColor &start_color, int start_x, int start_y)
{
    m_x = start_x;
    m_y = start_y;
    m_color = start_color;
    setZValue((m_x + m_y) % 2);

    setFlags(QGraphicsItem::GraphicsItemFlag::ItemIsSelectable |
             QGraphicsItem::GraphicsItemFlag::ItemIsMovable |
             QGraphicsItem::GraphicsItemFlag::ItemSendsScenePositionChanges);
    setAcceptHoverEvents(true);
}

QRectF DrItem::boundingRect() const
{
    return QRectF(0, 0, 110, 70);
}

// Seems to define mouseOver events
QPainterPath DrItem::shape() const
{
    QPainterPath path;
    path.addRect(14, 14, 82, 42);
    return path;
}

int DrItem::type() const
{
    // Enable the use of qgraphicsitem_cast with this item
    return User_Types::Object;
}

void DrItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);

    QColor fillColor = m_color;

    if (option->state & QStyle::State_Selected)  { fillColor = QColor(Qt::red); } //m_color.dark(150); }             // If selected
    if (option->state & QStyle::State_MouseOver) { fillColor = QColor(Qt::gray); } //fillColor.light(125); }          // If mouse is over

    if (option->state & QStyle::State_MouseOver && option->state & QStyle::State_Selected) { fillColor = QColor(Qt::red).darker(200); }


    //painter->fillRect(QRectF(14, 14, 82, 42), fillColor);
    painter->fillRect(QRectF(0, 0, 110, 70), fillColor);



    // Draw red ink
    if (stuff.size() > 1) {
        QPen p = painter->pen();
        painter->setPen(QPen(Qt::red, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter->setBrush(Qt::NoBrush);
        QPainterPath path;
        path.moveTo(stuff.first());
        for (int i = 1; i < stuff.size(); ++i)
            path.lineTo(stuff.at(i));
        painter->drawPath(path);
        painter->setPen(p);
    }
}

void DrItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() & Qt::LeftButton) {
        // If clicked while holding Alt key, start resizing
        if (event->modifiers() & Qt::KeyboardModifier::AltModifier) {

            double radius = boundingRect().width() / 2.0;

            m_center = QPointF(boundingRect().topLeft().x() + pos().x() + radius, boundingRect().topLeft().y() + pos().y() + radius);

            //QPointF pos = event->scenePos();
            //qDebug() << boundingRect() << radius << this->pos() << pos << event->pos();
            //double dist = sqrt(pow(m_center.x()-pos.x(), 2) + pow(m_center.y()-pos.y(), 2));
            //if (dist / radius > 0.8) {
                //qDebug() << dist << radius << dist / radius;
                m_is_resizing = true;
            //} else {
            //    m_is_resizing = false;
            // }

            return;
        }
    }
    QGraphicsItem::mousePressEvent(event);
    update();
}

void DrItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() & Qt::LeftButton) {
        // If mouse moved while Shift key is held down, draw red line
        if (event->modifiers() & Qt::KeyboardModifier::ShiftModifier) {
            stuff << event->pos();
            update();
            return;

        // If mouse moved while Alt key is held down, resize
        } else if (event->modifiers() == Qt::KeyboardModifier::AltModifier && m_is_resizing) {
            QPointF pos = event->scenePos();
            double dist = sqrt(pow(m_center.x()-pos.x(), 2) + pow(m_center.y()-pos.y(), 2));

            //setRect(m_center.x() - this->pos().x() - dist, m_center.y() - this->pos().y() - dist, dist * 2, dist * 2);

            //setScale()

            QTransform itTransf = transform();
            QPointF dp = this->boundingRect().center();

            itTransf.translate( dp.x(), dp.y());
            itTransf *= QTransform::fromScale( scale() + dist / 100,
                                               scale() + dist / 100);
            itTransf.translate( -dp.x(),  -dp.y());

            setTransform(itTransf);


            update();
            return;
        }
    }
    QGraphicsItem::mouseMoveEvent(event);
    update();
}

void DrItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() & Qt::LeftButton && m_is_resizing) {
        m_is_resizing = false;
    } else {
        QGraphicsItem::mouseReleaseEvent(event);
    }
    update();
}










