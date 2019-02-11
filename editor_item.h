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


//####################################################################################
//##    DrItem
//##        A QGraphicsItem for use with our custom QGraphicsScene
//############################
class DrItem : public QGraphicsPixmapItem
{
private:
    DrProject      *m_project;                              // Stores a pointer to the parent project

    DrObject       *m_object = nullptr;                     // Stores the pointer to the object for this item
    long            m_object_key;                           // Stores the object project key this item represents

    DrAsset        *m_asset = nullptr;                      // Stores pointer to the asset for this item
    long            m_asset_key;                            // Stores the asset project key this item is drawn from
    double          m_asset_width;                          // Width of asset this item is drawn from
    double          m_asset_height;                         // Height of asset this item is drawn from

    double          m_start_x;                              // Stores the item position the first time it was loaded
    double          m_start_y;                              // Stores the item position the first time it was loaded

    bool            m_temp_only = false;                    // If this is set to true when object is created, changes to this item are ignored
                                                            // and not processed into the undo stack, nor do changes have an effect on the associated
                                                            // object in the project data model

    Position_Flags  m_origin = Position_Flags::Center;

public:
    DrItem(DrProject *project, DrObject *object, bool is_temp_only = false);

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
    DrObject*               getObject()    { return m_object; }
    long                    getObjectKey() { return m_object_key; }
    DrAsset*                getAsset()     { return m_asset; }
    long                    getAssetKey()  { return m_asset_key; }

    Position_Flags          getOrigin() { return m_origin; }
    QColor                  getColorAtPoint(QPointF at_local_point);
    QColor                  getColorAtPoint(QPointF at_view_point, QGraphicsView *mouse_over_view);

    double                  startX() { return m_start_x; }
    double                  startY() { return m_start_y; }

};

#endif // DRITEM_H








