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

class DrScene;

//####################################################################################
//##    DrItem
//##        A QGraphicsItem for use with our custom QGraphicsScene
//############################
class DrItem : public QGraphicsItem
{
private:
    QPixmap         m_pixmap;

    double          m_width;
    double          m_height;

    double          m_start_x, m_start_y;

    Position_Flags  m_origin = Position_Flags::Center;

public:
    DrItem(DrProject *project, DrScene *scene, long object_key, double z_order);

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
    Position_Flags          getOrigin() { return m_origin; }
    QColor                  getColorAtPoint(QPointF at_point);

    double                  startX() { return m_start_x; }
    double                  startY() { return m_start_y; }

};

#endif // DRITEM_H








